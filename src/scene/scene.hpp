//
// Created by morimoto_hibiki on 2025/12/27.
//

#ifndef FRONTAGE_CLIENT_SCENE_HPP
#define FRONTAGE_CLIENT_SCENE_HPP

#include <atomic>
#include "../global.hpp"

class Scene {
    std::atomic<bool> loaded = false;
public:
    bool is_loaded() const;

    virtual void load(aho::StandardEngine e, GlobalContext& gctx) = 0;
    virtual void unload() = 0;
    virtual void transfer() = 0;
};

inline bool Scene::is_loaded() const {
    return loaded;
}

#endif //FRONTAGE_CLIENT_SCENE_HPP
