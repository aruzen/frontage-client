//
// Created by morimoto_hibiki on 2025/12/19.
//

#include <AHO/aho.hpp>

#include "game.hpp"
#include "../../utils/pipeline_helper.hpp"
#include "../../drawer/texture.hpp"
#include "../../utils/operators.hpp"
#include "../../resource/standard_buffers.hpp"
#include "../../define.hpp"

#include <tuple>
#include <span>

void GameScene::load(aho::StandardEngine e, GlobalContext &gctx) {
    using namespace vsl;
    using namespace aho;

    auto &device = e._data->logical_device;
    auto &command_manager = e._data->command_manager;
    auto &swapchain = e.boot_window->_data2->swapchain;

    X x(1.0f / (board_size + 1));
    Y y(1.0f / (board_size + 1));
    std::array<d2::PointF, 4> vertices = {
            d2::PointF{-x - y},
            {-x + y},
            {x + y},
            {x - y},
    };

    for (auto v: vertices)
        std::cout << v.value << std::endl;

    IDPickingRenderPass picking_render_pass{swapchain};
    auto a = (RenderPassAccessor) picking_render_pass;

    auto [tile_texture_layout, tile_texture] = ::utils::build_reflected_pipeline(
            device, RenderPassAccessor{picking_render_pass._data}/*FIXME 禁じ手*/,
            gctx.base_layout.add(pipeline_layout::IDPicking()),
            PATH_NORMALIZE("shaders/specialize/tile_texture.vert.spv"),
            PATH_NORMALIZE("shaders/specialize/tile_texture.frag.spv"),
            [](auto &generated) {
                if (generated.vertex_input->definitions.size() < 3) {
                    loggingln("error: tile_texture vertex input");
                    return;
                }
                auto &instance_input = generated.vertex_input->definitions[0];
                for (auto &a: std::span(generated.vertex_input->definitions).subspan(2,
                                                                                     generated.vertex_input->definitions.size() -
                                                                                     2)) {
                    loggingln("before : ", instance_input.layouts.size());
                    instance_input.layouts.insert(instance_input.layouts.begin(), a.layouts.begin(), a.layouts.end());
                    loggingln("after  : ", instance_input.layouts.size());
                }
                instance_input.updateTiming = pipeline_layout::VertexInputShapeDefinition::UpdateTiming::NextInstance;
                generated.vertex_input->definitions.resize(2);
                std::ranges::reverse(generated.vertex_input->definitions);
            }
    );

    data.emplace(Data{
            .engine = e,
            .gctx = &gctx,
            .vpmat = VPMatrix(VPMatrix::Data{}),
            .vert_buffer = {device, command_manager, vertices},
            .instance_buffer = {device, sizeof(TileData) * board_size * board_size},
            .tile_images = {"tiles",
                            {PATH_NORMALIZE("resource/image/maptile_sabaku.png"),
                             PATH_NORMALIZE("resource/image/maptile_sogen_hana.png")}},
            .tile_texture_layout = tile_texture_layout,
            .tile_texture = tile_texture,
            .picking_render_pass = picking_render_pass,
            .picking_frame_buffer = {swapchain, picking_render_pass}
    });
}

void GameScene::unload() {}

std::uint32_t makeState(GameScene::tile_state state, GameScene::tile_texture tile, std::uint16_t id) {
    return ((std::uint32_t) state << 28) + ((std::uint32_t) tile << 20) + id;
}

GameScene::tile_state extractTileState(std::uint32_t state) {
    return (GameScene::tile_state)(state >> 28);
}

GameScene::tile_texture extractTileTexture(std::uint32_t state) {
    return (GameScene::tile_texture)((state >> 20) & 0xFF);
}

std::uint16_t extractID(std::uint32_t state) {
    return state & 0xFFFF;
}

SceneID GameScene::transfer() {
    using namespace aho;
    using namespace vsl;
    using namespace aho::literals;

    auto &[vulkan_instance, physical_device, device, command_manager, graphic_resource_manager, synchro_manager]
            = *data->engine._data;
    auto &main_window = data->engine.boot_window.value();
    auto &[surface, swapchain, render_pass,
            frame_buffer, image_available, render_finish, in_flight]
            = *data->engine.boot_window.value()._data2;

    d2::VectorI size = main_window.frame_size();
    d2::VectorI center = size / 2;
    d2::VectorD window_per_frame_buffer, frame_buffer_per_ndc;
    {
        auto window = main_window.window_size().cast<double>();
        window_per_frame_buffer = d2::VectorD{
            size.value.x.value / window.value.x.value,
            size.value.y.value / window.value.y.value
        };
        frame_buffer_per_ndc = d2::VectorD{
            1.0 / window.value.x.value,
            1.0 / window.value.y.value
        };
    };
    data->vpmat.set(VPMatrix::Data{
            .view = matrix::make_view(d3::PointF(0.0f, 2.0f, 2.0f),
                                      d3::PointF(0.0f, 0.0f, 0.0f),
                                      d3::VectorF(0.0f, 0.0f, 1.0f)),
            .proj = matrix::make_perspective(45.0_deg,
                                             data->gctx->viewport.width / (float) data->gctx->viewport.height,
                                             0.1f, 10.0f),
    });
    std::vector tiles(board_size * board_size, TileData{});
    for (size_t i = 0; i < board_size; i++) {
        for (size_t j = 0; j < board_size; j++) {
            tiles[i * board_size + j].state = makeState(tile_state::Normal,
                                                        i < board_size / 2 ? tile_texture::Sabaku :
                                                        board_size / 2 < i ? tile_texture::Sougenn :
                                                        j % 2 == 0 ? tile_texture::Sabaku : tile_texture::Sougenn,
                                                        board_size * i + j + 1);

            tiles[i * board_size + j].model = matrix::make_move(d3::VectorF(
                    (j - ((float) board_size - 1) / 2) * 2.0f / (board_size + 1),
                    (i - ((float) board_size - 1) / 2) * 2.0f / (board_size + 1),
                    0));
        }
    }
    data->instance_buffer.copy(tiles.data(), tiles.size());

    main_window.add_plugin<window::WindowResizeHookPlugin>([&](aho::window::Window *w) {
        size = w->frame_size();
        center = size / 2;
        data->vpmat.set_proj(
              matrix::make_perspective(45.0_deg, size.value.x.value / (float) size.value.y.value, 0.1f, 10.0f));
        data->picking_render_pass.recreate_buffer(size.value.x.value, size.value.y.value);
        data->picking_frame_buffer = FrameBuffer(w->_data2->swapchain, data->picking_render_pass);
    });


    InputManager input_manager(main_window);
    auto mouse = input_manager.get<input::Mouse>();
    auto [move_front,
        move_back,
        move_left,
        move_right,
        move_up,
        move_down,
        mode_key,
        restart_key] = input_manager.get<input::Keys>(
            input::KeyCode::W,
            input::KeyCode::S,
            input::KeyCode::A,
            input::KeyCode::D,
            input::KeyCode::R,
            input::KeyCode::F,
            input::KeyCode::I,
            input::KeyCode::Enter)->keys;

    d3::PointF position, before_position;
    size_t frame = 0;
    size_t clicked = 0, overed = 0;
    while (aho::Window::Update() && main_window && input_manager) {
        using namespace product::hadamard;
        float delta = 0;
        std::optional<std::uint32_t> id;
        bool update_instance = false, update_view = false;

        auto cursor_pos = (_Vector(mouse->cursor()->state().value) * window_per_frame_buffer).cast<uint32_t>();
        {
            static auto prev = std::chrono::steady_clock::now();
            auto current = std::chrono::steady_clock::now();
            delta = std::chrono::duration<float>(current - prev).count();
            prev = current;
        }

        if (in(Vector(0, 0), main_window.frame_size(), cursor_pos.cast<int>())) {
            id = data->picking_render_pass.read(command_manager, in_flight, cursor_pos.value.x.value, cursor_pos.value.y.value);
        }

        // =============================================================

        if (id && 0 < *id && *id <= board_size*board_size) {
            if (mouse->leftClick()->up() && clicked != *id) {
                if (clicked != 0)
                    tiles[clicked - 1].state &= ~(0xF << 28);
                if (*id != 0)
                    tiles[*id - 1].state = (tiles[*id - 1].state & ~(0xF << 28)) | (std::uint32_t)tile_state::Slected << 28;
                clicked = *id;
                update_instance = true;
            } else if (overed != *id) {
                if (overed != 0 && extractTileState(tiles[overed - 1].state) == tile_state::MouseOver)
                    tiles[overed - 1].state &= ~(0xF << 28);
                if (*id != 0 && extractTileState(tiles[*id - 1].state) == tile_state::Normal)
                    tiles[*id - 1].state = (tiles[*id - 1].state & ~(0xF << 28)) | (std::uint32_t)tile_state::MouseOver << 28;
                overed = *id;
                update_instance = true;
            }
        }

        std::cout << mouse->wheel()->state().value.y << std::endl;

        if (mouse->leftClick()->pressed())
            position.value = position.value + (_Vector(mouse->cursor()->delta().value) * frame_buffer_per_ndc * 2.2).cast<float>().value;
        if (move_front->pressed())
            position.value -= 1.0_f_y * Y(delta);
        if (move_back->pressed())
            position.value += 1.0_f_y * Y(delta);
        if (move_left->pressed())
            position.value -= 1.0_f_x * X(delta);
        if (move_right->pressed())
            position.value += 1.0_f_x * X(delta);
        if (move_up->pressed())
            position.value -= 0.5_f_z * Z(delta);
        if (move_down->pressed())
            position.value += 0.5_f_z * Z(delta);
        if (0.0 < mouse->wheel()->state().value.y.value)
            position.value -= 0.1_f_z;
        if (mouse->wheel()->state().value.y.value < 0.0)
            position.value += 0.1_f_z;
        if (not equales(before_position, position)) {
            // この X, Y, Zオブジェクトは設計見直す必要がありそう
            auto z = abs(position.value.z.value);
            if (position.value.z != before_position.value.z) {
                auto zdiff = (before_position.value.z - position.value.z).value;
                position.value.x.value *= 1 + zdiff;
                position.value.y.value *= 1 + zdiff;
            }
            if (position.value.z < -1.5_f_z)
                position.value.z = -1.5_f_z;
            if (0.0_f_z < position.value.z)
                position.value.z = 0;
            if (z / 2 < std::abs(position.value.x.value))
                position.value.x.value = position.value.x.value < 0 ? -z / 2 : z / 2;
            if (position.value.y.value < -2*z)
                position.value.y.value = -2*z;
            if (-0.5*z < position.value.y.value)
                position.value.y.value = -0.5 * z;
            std::cout << delta << " " << position.value << std::endl;
            data->vpmat.set_view(matrix::make_view(d3::PointF(0.0f, 2.0f, 2.0f) + _Vector(position.value),
                                                   position,
                                                   d3::VectorF(0.0f, 0.0f, 1.0f)));
            before_position = position;
        }

        if (update_instance) {
            data->instance_buffer.copy(tiles.data(), tiles.size());
        }

        // =============================================================

        {
            DrawPhase phase(data->engine, [&](auto &s) {
                s << command::IDPickingRenderPassBegin(data->picking_render_pass, data->picking_frame_buffer);
            });
            phase << data->gctx->viewport << data->gctx->scissor;
            phase << data->tile_texture
                  << data->vpmat
                  << vsl::command::BindGraphicResource(data->tile_images.resource,
                                                       graphic_resource::BindingDestination::Graphics,
                                                       data->tile_texture,
                                                       1)
                  << command::BindVertexBuffer(data->vert_buffer, data->instance_buffer)
                  << command::BindIndexBuffer(bufs::rect_indexes())
                  << command::DrawInstance(6, tiles.size());
        }
        frame++;
    }
    return SceneID::Stop;
}
