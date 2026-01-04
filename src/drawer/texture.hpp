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

#include "../resource/standard_buffers.hpp"

struct TextureDrawerContext {
    aho::engine::StandardEngine engine;
    vsl::GraphicsPipeline float_push_texture;
    vsl::PipelineLayout float_push_texture_layout;
    vsl::GraphicsPipeline int_push_texture;
    vsl::PipelineLayout int_push_texture_layout;
};

extern std::optional<TextureDrawerContext> &init_texture_drawer_controller(std::optional<aho::engine::StandardEngine> engine = std::nullopt,
                                                           std::optional<vsl::PipelineLayout> layout = std::nullopt,
                                                           bool clean = false);


extern TextureDrawerContext& get_texture_drawer_controller();

void draw(const Image &image, const aho::d2::PointI &pos, const aho::d2::VectorI &size,
          aho::DrawPhase *phase = aho::THREAD_LOCAL_DRAW_PHASE);

void draw(const Image &image, const aho::d2::PointF &pos, const aho::d2::VectorF &size,
          aho::DrawPhase *phase = aho::THREAD_LOCAL_DRAW_PHASE);

#endif //FRONTAGE_CLIENT_TEXTURE_HPP
