//
// Created by morimoto_hibiki on 2025/12/19.
//

#ifndef FRONTAGE_CLIENT_LOBBY_HPP
#define FRONTAGE_CLIENT_LOBBY_HPP

#include <AHO/aho.hpp>
#include "../scene.hpp"
#include "../../resource/image.hpp"

class LobbyScene : public Scene {
    aho::StandardEngine engine;
    GlobalContext *gctx;
    Image title_back, title_logo, title_frame, start_frame, start_font;
public:
    void load(aho::StandardEngine e, GlobalContext& gctx) override;

    void unload() override;

    void transfer() override;
};

#endif //FRONTAGE_CLIENT_LOBBY_HPP
