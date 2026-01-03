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

    return TextureDrawerContext{
            .engine = engine.value(),
            .float_push_texture = float_push_texture,
            .float_push_texture_layout = float_push_texture_layout,
            .int_push_texture = int_push_texture,
            .int_push_texture_layout = int_push_texture_layout
    };
}

TextureDrawerContext &get_texture_drawer_controller(std::optional<aho::engine::StandardEngine> engine,
                                                    std::optional<vsl::PipelineLayout> layout,
                                                    bool clean) {
    static std::optional<TextureDrawerContext> controller = make_texture_drawer_context(engine.value(), layout.value());
    if (clean)
        controller.reset();
    return *controller;
}

void draw(const Image &image, const aho::d2::PointI &pos, const aho::d2::VectorI &size,
          aho::DrawPhase *phase) {
    using namespace aho;
    using namespace vsl;

    auto &ctx = get_texture_drawer_controller();

    if (image.channel == 1) {
        throw std::runtime_error("未実装");
    } else if (image.channel == 4) {
        struct alignas(8) {
            d2::VectorI window_size;
            d2::PointI top_left;
            d2::PointI bottom_left;
            d2::PointI bottom_right;
            d2::PointI top_right;
            std::array<d2::PointF, 4> texcoords;
        } wpt{
                .window_size = ctx.engine.getWindow().frame_size(),
                .top_left     = pos,
                .bottom_left  = pos.value + size.value.y,
                .bottom_right = pos + size,
                .top_right    = pos.value + size.value.x,
                .texcoords = {
                        d2::PointF{0.0f, 0.0f},
                        d2::PointF{0.0f, 1.0f},
                        d2::PointF{1.0f, 1.0f},
                        d2::PointF{1.0f, 0.0f},
                }
        };

        *phase << ctx.int_push_texture
               << command::PushConstant(ctx.int_push_texture_layout, ShaderFlag::Vertex, sizeof wpt, 0,
                                        &wpt)
               << command::BindGraphicResource({image.resource},
                                               graphic_resource::BindingDestination::Graphics,
                                               ctx.int_push_texture)
               << command::BindIndexBuffer(bufs::rect_indexes())
               << command::DrawIndexed(6);
    }
}

void draw(const Image &image, const aho::d2::PointF &pos, const aho::d2::VectorF &size,
          aho::DrawPhase *phase) {
    using namespace aho;
    using namespace vsl;

    auto &ctx = get_texture_drawer_controller();

    if (image.channel == 1) {
        throw std::runtime_error("未実装");
    } else if (image.channel == 4) {
        struct alignas(16) {
            std::array<d2::PointF, 4> vertices;
            std::array<d2::PointF, 4> texcoords;
        } pos_with_tc{
                {
                        pos.value,
                        pos.value + size.value.y,
                        pos + size,
                        pos.value + size.value.x,
                },
                {
                        d2::PointF{0.0f, 0.0f},
                        d2::PointF{0.0f, 1.0f},
                        d2::PointF{1.0f, 1.0f},
                        d2::PointF{1.0f, 0.0f},
                }
        };

        *phase << ctx.float_push_texture
               << command::PushConstant(ctx.float_push_texture_layout, ShaderFlag::Vertex, sizeof pos_with_tc, 0,
                                        &pos_with_tc)
               << command::BindGraphicResource({image.resource},
                                               graphic_resource::BindingDestination::Graphics,
                                               ctx.float_push_texture)
               << command::BindIndexBuffer(bufs::rect_indexes())
               << command::DrawIndexed(6);
    }
}
