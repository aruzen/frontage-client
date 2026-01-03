//
// Created by morimoto_hibiki on 2025/12/19.
//

#include <AHO/aho.hpp>

#include "lobby.hpp"
#include "../../drawer/texture.hpp"
#include "../../utils/operators.hpp"

#include <tuple>

void LobbyScene::load(aho::StandardEngine e, GlobalContext &gctx) {
    std::string err;
    ::utils::Bitmap buffer;
    if (::utils::RenderTextBitmap(
            "../resource/font/VT323-Regular.ttf", "frontage", 64.0f * 5, buffer, 4, &err)) { // HannariMincho-Regular.ttf
    } else {
        std::print("[bitmap] render failed: {}\n", err);
    }
    auto title_logo = Image("title_logo", buffer);
    if (::utils::RenderTextBitmap(
            "../resource/font/VT323-Regular.ttf", "START", 64.0f, buffer, 4, &err)) { // HannariMincho-Regular.ttf
    } else {
        std::print("[bitmap] render failed: {}\n", err);
    }
    auto start_font = Image("start_font", buffer);
    data.emplace(Data{
            .engine = e,
            .gctx = &gctx,
            .title_back = Image("title_frame", "../resource/image/title_back.png"),
            .title_logo = title_logo,
            .title_frame = Image("title_frame", "../resource/image/stone_frame_short.png"),
            .start_frame = Image("start_frame", "../resource/image/wood_frame_short.png"),
            .start_font = start_font,
    });
}

void LobbyScene::unload() {}

SceneID LobbyScene::transfer() {
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
    d2::VectorD window_per_frame_buffer = [&main_window, &size]() {
        auto r = main_window.window_size().cast<double>();
        r.value.x.value = size.value.x.value / r.value.x.value;
        r.value.y.value = size.value.y.value / r.value.y.value;
        return r;
    }();

    auto title_back = std::make_tuple(data->title_back, d2::PointF{-1, -1}, d2::VectorF{2, 2});
    constexpr auto title_frame_size_rate = 4;
    auto title_frame_size = data->title_frame.height * title_frame_size_rate < size.value.y.value
                            ? data->title_frame.size()
                            : (data->title_frame.size() * (float) size.value.y.value /
                               (data->title_frame.height * title_frame_size_rate)).cast<int>();
    auto title_frame = std::make_tuple(data->title_frame,
                                       d2::PointI{center.value.x + Y(size.value.y.value / 5) -
                                                  (title_frame_size / 2).value},
                                       title_frame_size);
    constexpr auto start_frame_size_rate = 7;
    auto start_frame_size = (data->start_frame.height * start_frame_size_rate < size.value.y.value
                             ? data->start_frame.size()
                             : (data->start_frame.size() * (float) size.value.y.value /
                                (data->start_frame.height * start_frame_size_rate)).cast<int>()) + Vector(100, 0);
    auto start_frame_position =
            d2::PointI{center.value.x + 50_y + Y(size.value.y.value - 2 * size.value.y.value / 7) -
                       (start_frame_size / 2).value};

    main_window.add_plugin<window::WindowResizeHookPlugin>([&](aho::window::Window *w) {
        size = w->frame_size();
        center = size / 2;
        title_frame_size = data->title_frame.height * title_frame_size_rate < size.value.y.value
                           ? data->title_frame.size()
                           : (data->title_frame.size() * (float) size.value.y.value /
                              (data->title_frame.height * title_frame_size_rate)).cast<int>();
        title_frame = std::make_tuple(data->title_frame,
                                      d2::PointI{center.value.x + Y(size.value.y.value / 5) -
                                                 (title_frame_size / 2).value},
                                      title_frame_size);

        start_frame_size = (data->start_frame.height * start_frame_size_rate < size.value.y.value
                            ? data->start_frame.size()
                            : (data->start_frame.size() * (float) size.value.y.value /
                               (data->start_frame.height * start_frame_size_rate)).cast<int>()) + Vector(100, 0);
        start_frame_position =
                d2::PointI{center.value.x + 50_y + Y(size.value.y.value - 2 * size.value.y.value / 7) -
                           (start_frame_size / 2).value};
    });

    InputManager input_manager(main_window);
    auto mouse = input_manager.get<input::Mouse>();
    auto next_scene = SceneID::Stop;
    std::chrono::steady_clock::time_point click_time;
    while (aho::Window::Update() && main_window && input_manager) {
        using namespace product::hadamard;
        if (mouse->leftClick()->up()) {
            auto click_pos = (_Vector(mouse->cursor()->state().value) * window_per_frame_buffer).cast<int>();
            if (in(_Vector(start_frame_position.value), _Vector{start_frame_position.value} + start_frame_size,
                   click_pos)) {
                next_scene = SceneID::LocalGame;
                click_time = std::chrono::steady_clock::now();
            } else if (in(_Vector(start_frame_position.value) + Vector(0, size.value.y.value / 7),
                          _Vector{start_frame_position.value} + Vector(0, size.value.y.value / 7) + start_frame_size,
                          click_pos)) {
                std::println("オンラインは未実装です..");
                // next_scene = SceneID::OnlineGame;
            }
        }

        if (0 != mouse->wheel()->state().value.x) {
            loggingln(mouse->wheel()->state().value.x);
        }

        {
            DrawPhase phase(data->engine);
            phase << data->gctx->viewport << data->gctx->scissor;
            if (next_scene == SceneID::Stop) {
                phase << title_back
                      << title_frame
                      << pack(data->start_frame, start_frame_position, start_frame_size)
                      << pack(data->start_frame, start_frame_position + Vector(0, size.value.y.value / 7),
                              start_frame_size);
            } else {
                auto d = std::chrono::steady_clock::now() - click_time;
                auto r = (std::chrono::duration<double>(d).count()/0.25+0.2);
                r = r*r;
                phase << title_back
                      << title_frame
                      << pack(data->start_frame, start_frame_position + Vector((int)(center.value.x.value * -r), 0), start_frame_size)
                      << pack(data->start_frame, start_frame_position + Vector((int)(center.value.x.value * -r), size.value.y.value / 7),
                              start_frame_size);
                if (std::chrono::milliseconds(250) < d)
                    break;
                // UIやっぱ嫌いや ; ;
            }
        }
    }
    return next_scene;
}
