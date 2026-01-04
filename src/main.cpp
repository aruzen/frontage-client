#include <VSL/vsl.hpp>
#include <VSL/utils/shader_compiler.hpp>
#include <VSL/utils/spir_reflector.hpp>

// #define AHO_POOP_PUBLIC_SECURITY
#pragma warning( disable : 4455 )

#include <AHO/aho.hpp>

#include <chrono>
#include <print>
#include <cstdlib>

/*
* https://vulkan-tutorial.com/en/Vertex_buffers/Staging_buffer
*/
// #include <boost/bimap.hpp>
// #include "../thirdparty/json.hpp"
#include "../thirdparty/stb_image.h"
#include "../thirdparty/stb_image_write.h"
#include "utils/bitmap_text.hpp"
#include "utils/pipeline_helper.hpp"

#include "resource/image.hpp"
#include "resource/mvp.hpp"
#include "drawer/texture.hpp"
#include "scene/lobby/lobby.hpp"
#include "scene/game/game.hpp"
#include "global.hpp"
#include "define.hpp"

#define DEBUG

int main() {

#if !defined(DEBUG) && !defined(_DEBUG)
    try {
#else
    {
#endif
        using namespace vsl;
        namespace gfx_src = vsl::graphic_resource;
        using namespace aho;
        using namespace aho::coordinate;
        using namespace aho::literals;
        namespace pl = vsl::pipeline_layout;
        vsl::utils::ShaderCompiler shader_compiler("glslc", {
                "${AHO_HOME}/built-in-resource/shaders/",
                PATH_NORMALIZE("shaders/float/"),
                PATH_NORMALIZE("shaders/int/"),
                PATH_NORMALIZE("shaders/ubo/"),
                PATH_NORMALIZE("shaders/ubo_pos/"),
                PATH_NORMALIZE("shaders/specialize/") });
        shader_compiler.load();
        shader_compiler.compile();

        aho::engine::StandardEngine engine("frontage");
        auto &[vulkan_instance, physical_device, device, command_manager, graphic_resource_manager, synchro_manager]
                = *engine._data;
        auto &main_window = engine.boot_window.value();
        auto &[surface, swapchain, render_pass,
                frame_buffer, image_available, render_finish, in_flight]
                = *engine.boot_window.value()._data2;
        main_window.resize(960, 540);

        GlobalContext gctx{
                .viewport{swapchain},
                .scissor{swapchain},
                .base_layout = {device}
        };

        gctx.base_layout.add(pl::ColorBlend(),
                       pl::InputAssembly(),
                       pl::Multisample(),
                       pl::Rasterization(),
                       pl::DepthStencil(),
                       pl::DynamicState(),
                       gctx.scissor,
                       gctx.viewport);

        ::Image::init_contxt(engine);
        ::VPMatrix::init_context(engine);
        init_texture_drawer_controller(engine, gctx.base_layout);
        bufs::init_rect_indexes(engine);
        bufs::init_normal_texcoord(engine);

        main_window.add_plugin<window::WindowResizeHookPlugin>([&gctx, &swapchain](auto w) {
            gctx.viewport = Viewport(swapchain);
            gctx.scissor = Scissor(swapchain);
        });

        std::shared_ptr<Scene> lobbyScene(new LobbyScene());
        std::shared_ptr<Scene> gameScene(new GameScene());

        std::shared_ptr<Scene> nowScene = gameScene;

        while (true) {
            if (not nowScene->is_loaded())
                nowScene->load(engine, gctx);
            auto nextScene = nowScene->transfer();
            switch (nextScene) {
                case SceneID::Stop:
                    goto main_loop_out;
                case SceneID::Lobby:
                    nowScene = lobbyScene;
                    break;
                case SceneID::LocalGame:
                    nowScene = gameScene;
                    break;
                case SceneID::OnlineGame:
                    break;
                case SceneID::None:
                default:
                    return 1;
            }
        }
        main_loop_out:

        ::Image::init_contxt(engine, true);
        ::VPMatrix::init_context(engine, true);
        init_texture_drawer_controller(engine, gctx.base_layout, true);
        bufs::init_rect_indexes(engine, true);
        bufs::init_normal_texcoord(engine, true);
#if !defined(DEBUG) && !defined(_DEBUG)
    }
    catch (std::exception &e) {
        vsl::loggingln(e.what());
        return 1;
    }
    catch (vsl::exceptions::VSLException &e) {
        vsl::loggingln(e.what());
        return 1;
    }
#else
    }
#endif
}
    /*
    VPMatrix mvp({
                          .model = matrix::make_rotation(0.0_rad,
                                                         Vector(0.0f, 0.0f, 1.0f)),
                          .view = matrix::make_view(Point(2.0f, 0.0f, 2.0f),
                                                    Point(0.0f, 0.0f, 0.0f),
                                                    Vector(0.0f, 0.0f, 1.0f)),
                          .proj = matrix::make_perspective(45.0_deg,
                                                           gctx.viewport.width / (float) gctx.viewport.height,
                                                           0.1f,
                                                           10.0f)
                  });
    main_window.add_plugin<window::WindowResizeHookPlugin>([&mvp, &swapchain](auto w) {
        auto size = w->frame_size();
        mvp.set_proj(matrix::make_perspective(45.0_deg,
                                              (float) size.value.x.value / size.value.y.value,
                                              0.1f,
                                              10.0f));
    });
        ::Image logo("ahaha_logo", "${AHO_HOME}/built-in-resource/images/ahahacraft.png");

        ::utils::Bitmap buffer;
        std::string err;
        if (::utils::RenderTextBitmap(
                "../resource/font/HannariMincho-Regular.ttf", "Frontage", 64.0f * 5, buffer, 4, &err)) {
        } else {
            std::print("[bitmap] render failed: {}\n", err);
        }
        ::Image title("title", buffer);


        auto [input_vertices_layout, input_vertices] = ::utils::build_reflected_pipeline(
                &engine, layout,
                "${AHO_HOME}/built-in-resource/shaders/vd2p_fc_umpv.vert.spv",
                "${AHO_HOME}/built-in-resource/shaders/fc.frag.spv"
        );

    auto [float_push_triangle_layout, float_push_triangle] = ::utils::build_reflected_pipeline(
                &engine, layout,
                "${AHO_HOME}/built-in-resource/shaders/2dtriangle_single_color.vert.spv",
                "${AHO_HOME}/built-in-resource/shaders/fc.frag.spv"
        );

    auto [int_push_triangle_layout, int_push_triangle] = ::utils::build_reflected_pipeline(
                &engine, layout,
                "${AHO_HOME}/built-in-resource/shaders/2dint_triangle_single_color.vert.spv",
                "${AHO_HOME}/built-in-resource/shaders/fc.frag.spv"
        );
     InputManager input_manager(main_window);
    auto [keyUp,
            keyDown,
            keyLeft,
            keyRight] = input_manager.get<input::Keys>(
            input::KeyCode::Up,
            input::KeyCode::Down,
            input::KeyCode::Left,
            input::KeyCode::Right)->keys;
    auto mouse = input_manager.get<input::Mouse>();

    while (aho::Window::Update() && main_window && input_manager) {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();

        d3::VectorF move;
        if (mouse->leftClick()->pressed()) {
            move.value = move.value - mouse->cursor()->delta().cast<float>().value;
            move *= 0.005f;
        }

        if (keyUp->pressed())
            move.value -= 0.05_f_y;
        if (keyDown->pressed())
            move.value += 0.05_f_y;
        if (keyLeft->pressed())
            move.value -= 0.05_f_x;
        if (keyRight->pressed())
            move.value += 0.05_f_x;

        {
            auto phase = DrawPhase(engine);
            mvp.set_index(phase.getImageIndex());
            mvp.set_view(mvp.view() * matrix::make_move(move));

            phase << command::RenderPassBegin(render_pass, frame_buffer);
            phase << input_vertices << viewport << scissor
                  << mvp
                  << command::BindVertexBuffer(vertBuffer, colorBuffer)
                  << command::BindIndexBuffer(indexBuffer)
                  << command::DrawIndexed(indices.size());

            /*
            struct alignas(16) {
                std::array<d2::PointF, 4> vertices;
                std::array<d2::PointF, 4> texcoords;
            } pos_with_tc{
                    {
                            d2::PointF{1.0f, 0.0f},
                            d2::PointF{0.0f, 0.0f},
                            d2::PointF{0.0f, 1.0f},
                            d2::PointF{1.0f, 1.0f}
                    },
                    texcoords
            };
            phase << float_push_texture
                  << command::PushConstant(float_push_texture_layout, ShaderFlag::Vertex, sizeof pos_with_tc, 0,
                                           &pos_with_tc)
                  << command::BindGraphicResource({logo.resource},
                                                  graphic_resource::BindingDestination::Graphics,
                                                  float_push_texture)
                  << command::BindIndexBuffer(indexBuffer)
                  << command::DrawIndexed(indices.size());


            struct alignas(8) {
                d2::VectorI window_size;
                d2::PointI top_left;
                d2::PointI bottom_left;
                d2::PointI bottom_right;
                d2::PointI top_right;
                std::array<d2::PointF, 4> texcoords;
            } wpt{
                    .window_size = main_window.frame_size(),
                    .top_left     = d2::PointI{0, 0},
                    .bottom_left  = d2::PointI{0, 200},
                    .bottom_right = d2::PointI{200, 200},
                    .top_right    = d2::PointI{200, 0},
                    .texcoords = texcoords
            };
            for (auto pos: {
                    wpt.top_left,
                    wpt.top_right,
                    wpt.bottom_right,
                    wpt.bottom_left
            }) {
                phase << int_push_triangle
                      << command::PushConstant(int_push_triangle_layout, ShaderFlag::Vertex,
                                               sizeof wpt.window_size, 0, &wpt.window_size);
                int i = 0;
                for (auto v: {d2::VectorI{0, -10}, d2::VectorI{-10, 10}, d2::VectorI{10, 10}}) {
                    auto m = pos + v;
                    phase << command::PushConstant(int_push_triangle_layout, ShaderFlag::Vertex,
                                                   sizeof wpt.top_right,
                                                   sizeof wpt.window_size + sizeof wpt.top_right * i,
                                                   &m);
                    i++;
                }
                RGB color{1.0f, 1.0f, 0.0f};
                phase << command::PushConstant(int_push_triangle_layout, ShaderFlag::Vertex,
                                               sizeof color,
                                               sizeof wpt.window_size + sizeof wpt.top_right * 3,
                                               &color)
                      << command::BindIndexBuffer(indexBuffer)
                      << command::DrawIndexed(indices.size());
            }

            RGB color{1.0f, 1.0f, 0.0f};
            phase << command::PushConstant(int_push_triangle_layout, ShaderFlag::Vertex,
                                           sizeof wpt.window_size + sizeof wpt.top_right * 3,
                                           0,
                                           &wpt)
                  << command::PushConstant(int_push_triangle_layout, ShaderFlag::Vertex,
                                           sizeof color,
                                           sizeof wpt.window_size + sizeof wpt.top_right * 3,
                                           &color)
                  << command::BindIndexBuffer(indexBuffer)
                  << command::DrawIndexed(indices.size());

            phase << int_push_texture
                  << command::PushConstant(int_push_texture_layout, ShaderFlag::Vertex, sizeof wpt, 0, &wpt)
                  << command::BindGraphicResource({logo.resource},
                                                  graphic_resource::BindingDestination::Graphics, int_push_texture)
                  << command::BindIndexBuffer(indexBuffer)
                  << command::DrawIndexed(indices.size());

            /*
            struct alignas(16) {
                color::FloatRGBA text, back;
                std::array<d2::PointF, 4> vertices;
            } color_with_pos{
                    {0.0f, 1.0f, 1.0f, 1.0f},
                    {0.0f, 0.0f, 0.0f, 0.0f},
                    {
                     d2::PointF{1.0f, 0.0f} - d2::VectorF{0.5f, 0.5f},
                           d2::PointF{0.0f, 0.0f} - d2::VectorF{0.5f, 0.5f},
                                 d2::PointF{0.0f, 1.0f} - d2::VectorF{0.5f, 0.5f},
                                       d2::PointF{1.0f, 1.0f} - d2::VectorF{0.5f, 0.5f}
                    }
            };
            phase << push_font
                  << command::PushConstant(push_font_layout, ShaderFlag::Vertex, sizeof color_with_pos, 0,
                                           &color_with_pos)
                  << command::BindGraphicResource({title.resource},
                                                  graphic_resource::BindingDestination::Graphics, push_font)
                  << command::BindIndexBuffer(indexBuffer)
                  << command::DrawIndexed(indices.size());

            Triangle<float, vsl::D2> triangle{Point(-0.5f, 0.0f), Point(0.5f, 0.5f), Point(-0.5f, -0.5f)};
            RGB rgb{1.0f, 0.0f, 0.0f};
            phase << push_triangle
                  << command::PushConstant(push_triangle_layout, ShaderFlag::Vertex, sizeof triangle, 0, &triangle)
                  << command::PushConstant(push_triangle_layout, ShaderFlag::Vertex, sizeof rgb, 44 - sizeof rgb, &rgb)
                  << command::Draw(3);

            phase << input_texture
                  << command::BindGraphicResource(
                          {input_vertices_resource[phase.getImageIndex()], texture_resource[swapchain.getSwapImageSize()]},
                          graphic_resource::BindingDestination::Graphics, input_texture)
                  << command::BindVertexBuffer(vertBuffer, colorBuffer, texCoordBuffer)
                  << command::BindIndexBuffer(indexBuffer)
                  << command::DrawIndexed(indices.size());


            phase << command::RenderPassEnd();
        }
        // auto debugMapped = debugBuffer.data();
        // debug = static_cast<struct debug_t *>(debugMapped.data);
        command_manager.next();
    }
    in_flight.wait();
    /**
}
*/
/**
}
*/
