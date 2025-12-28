//
// Created by morimoto_hibiki on 2025/12/19.
//

#include "texture.hpp"

#include "../utils/pipeline_helper.hpp"

#include <VSL/vulkan/shader.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>

TextureDrawerContext make_texture_drawer_context(std::optional<aho::engine::StandardEngine> engine,
                                                    std::optional<vsl::PipelineLayout> layout) {
    auto [float_push_texture_layout, float_push_texture] = ::utils::build_reflected_pipeline(
            &(*engine), *layout,
            "../shaders/float/push_texture.vert.spv",
            "../shaders/float/push_texture.frag.spv"
    );

    auto [int_push_texture_layout, int_push_texture] = ::utils::build_reflected_pipeline(
            &(*engine), *layout,
            "../shaders/int/push_texture.vert.spv",
            "../shaders/int/push_texture.frag.spv"
    );
}

TextureDrawerContext &get_texture_drawer_controller(std::optional<aho::engine::StandardEngine> engine,
                                                    std::optional<vsl::PipelineLayout> layout) {
    static TextureDrawerContext controller = make_texture_drawer_context(engine.value(), layout.value());
    return controller;
}
