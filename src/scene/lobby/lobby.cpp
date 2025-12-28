//
// Created by morimoto_hibiki on 2025/12/19.
//

#include "lobby.hpp"

void LobbyScene::load(aho::StandardEngine e, GlobalContext &_gctx) {
    engine = e;
    gctx = &_gctx;
    std::string err;

    ::utils::Bitmap buffer;
    if (::utils::RenderTextBitmap(
            "../resource/font/VT323-Regular.ttf", "frontage", 64.0f * 5, buffer, 4, &err)) { // HannariMincho-Regular.ttf
    } else {
        std::print("[bitmap] render failed: {}\n", err);
    }
    title_logo = Image("title_logo", buffer);
    if (::utils::RenderTextBitmap(
            "../resource/font/VT323-Regular.ttf", "START", 64.0f, buffer, 4, &err)) { // HannariMincho-Regular.ttf
    } else {
        std::print("[bitmap] render failed: {}\n", err);
    }
    start_font = Image("start_font", buffer);
    title_back = Image("title_frame", "../resource/title_back.png");
    title_frame = Image("title_frame", "../resource/stone_frame_short.png");
    start_frame = Image("start_frame", "../resource/wood_frame_short.png");
}

void LobbyScene::unload() {}

void LobbyScene::transfer() {

}
