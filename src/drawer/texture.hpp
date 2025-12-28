//
// Created by morimoto_hibiki on 2025/12/19.
//

#ifndef FRONTAGE_CLIENT_TEXTURE_HPP
#define FRONTAGE_CLIENT_TEXTURE_HPP

#include "../resource/image.hpp"
#include <VSL/utils/spir_reflector.hpp>
#include <AHO/drawer.hpp>
#include <AHO/core/point.hpp>

#include <string>
#include <optional>
#include <vector>

struct TextureDrawerContext {
    aho::engine::StandardEngine engine;
    vsl::PipelineLayout float_push_texture_layout;
    vsl::PipelineAccessor float_push_texture;

    vsl::PipelineLayout int_push_texture_layout;
    vsl::PipelineAccessor int_push_texture;
};

extern TextureDrawerContext &get_texture_drawer_controller(std::optional<aho::engine::StandardEngine> engine = std::nullopt,
                                                    std::optional<vsl::PipelineLayout> layout = std::nullopt);

namespace aho {
    template<>
    struct Drawer<Image, aho::d2::PointI, aho::d2::VectorI> {
        static void draw(const Image &image, const aho::d2::VectorI &pos, const aho::d2::VectorI &size) {

        };
    };
}

#endif //FRONTAGE_CLIENT_TEXTURE_HPP
