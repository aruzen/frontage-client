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

std::uint32_t makeState(GameScene::tile_state state, GameScene::tile_id tile, std::uint16_t id) {
    return ((std::uint32_t) state << 28) + ((std::uint32_t) tile << 20) + id;
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
        auto r = main_window.window_size().cast<double>();
        r.value.x.value = size.value.x.value / r.value.x.value;
        r.value.y.value = size.value.y.value / r.value.y.value;
        window_per_frame_buffer = r;
        r = d2::VectorD(1, 1);
        r.value.x.value /= size.value.x.value;
        r.value.y.value /= size.value.y.value;
        frame_buffer_per_ndc = r;
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
            tiles[i + board_size * j].state = makeState(tile_state::Normal,
                                                        i < board_size / 2 ? tile_id::Sabaku :
                                                        board_size / 2 < i ? tile_id::Sougenn :
                                                        j % 2 == 0 ? tile_id::Sabaku : tile_id::Sougenn,
                                                        board_size * i + j);

            tiles[i + board_size * j].model = matrix::make_move(d3::VectorF(
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
    d3::VectorF move;
    auto mouse = input_manager.get<input::Mouse>();
    while (aho::Window::Update() && main_window && input_manager) {
        using namespace product::hadamard;
        float delta = 0;
        {
            static auto prev = std::chrono::steady_clock::now();
            auto current = std::chrono::steady_clock::now();
            delta = std::chrono::duration<float>(current - prev).count();
            prev = current;
        }

        if (mouse->leftClick()->up()) {
            auto tmp = command_manager.getCurrentBufferIdx();
            auto pos = (_Vector(mouse->cursor()->state().value) * window_per_frame_buffer).cast<uint32_t>();
            auto id = data->picking_render_pass.read(command_manager, in_flight, pos.value.x.value, pos.value.y.value);
            loggingln("clicked : ", id);
        }

        if (mouse->leftClick()->pressed()) {
            move.value = move.value + (_Vector(mouse->cursor()->state().value) * window_per_frame_buffer *
                                       frame_buffer_per_ndc).cast<float>().value;
        }


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
    }
    return SceneID::Stop;
}
