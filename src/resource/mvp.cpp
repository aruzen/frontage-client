//
// Created by morimoto_hibiki on 2025/12/19.
//

#include "mvp.hpp"
#include <AHO/pipeline/layouts.hpp>
#include <utility>

namespace gfx = vsl::graphic_resource;

VPMatrix::Context VPMatrix::make_context(std::optional<aho::StandardEngine> e) {
    if (not e.has_value())
        throw std::runtime_error("error: You must init VPMatrix::make_context.");
    auto engine = e.value();
    return VPMatrix::Context{
            .engine = engine,
            .mvp_layout = gfx::BindingLayout(
                    engine._data->logical_device, {
                            aho::pipeline::getBindingPoint(aho::pipeline::ResourceName::VPMatrixUBO)
                    }),
            .mvp_pool = engine._data->graphic_resource_manager->make(
                    std::map<gfx::Type, size_t>{
                            {gfx::Type::UniformBuffer, 10}
                    })
    };
}

VPMatrix::Context &VPMatrix::get_context(std::optional<aho::StandardEngine> engine, bool clean) {
    static std::optional<VPMatrix::Context> context = make_context(engine);
    if (clean)
        context.reset();
    return *context;
}

VPMatrix::VPMatrix(Data data, std::uint32_t binding) : _data(data), binding(binding) {
    using namespace aho::pipeline;

    auto &context = get_context();
    auto &engine_data = *context.engine._data;
    auto size = context.engine.boot_window->_data2->swapchain.getSwapImageSize();
    for (size_t i = 0; i < size; i++) {
        VPMatrix::UboBuffer ubo(engine_data.logical_device, this->_data);
        this->buffers.push_back(ubo);
    }
    vsl::graphic_resource::BindingLayout l(engine_data.logical_device, {getBindingPoint(ResourceName::VPMatrixUBO)});
    const auto& [ok, bind] = context.mvp_pool.bind(size, l);
    if (not ok)
        return;
    resource = bind;
    for (size_t i = 0; i < size; i++) {
        resource[i].update(buffers[i], (size_t) 0,
                           vsl::graphic_resource::Type::UniformBuffer);
    }
    generations = std::vector<size_t>(resource.size(), 0);
}

VPMatrix::VPMatrix(aho::Mat4x4<float> view, aho::Mat4x4<float> proj, std::uint32_t binding)
        : VPMatrix::VPMatrix(VPMatrix::Data{
        .view = view,
        .proj = proj,
}, binding) {}

void VPMatrix::set_index(size_t index) {
    this->now_index = index;
}

VPMatrix::Data VPMatrix::data() {
    return _data;
}

aho::Mat4x4F VPMatrix::view() const {
    return _data.view;
}

aho::Mat4x4F VPMatrix::proj() const {
    return _data.proj;
}

void VPMatrix::set(VPMatrix::Data data) {
    _data = data;
    generation++;
}

void VPMatrix::set_view(aho::Mat4x4<float> view) {
    _data.view = view;
    generation++;
}

void VPMatrix::set_proj(aho::Mat4x4<float> proj) {
    _data.proj = proj;
    generation++;
}

void VPMatrix::invoke(vsl::DefaultPhaseStream &pst, const vsl::CommandPool &pool, const vsl::CommandBuffer &buffer,
                       const vsl::CommandManager &manager) {
    if (generations[this->now_index] < generation) {
        buffers[this->now_index].copy(_data);
    }
    auto cmd = vsl::command::BindGraphicResource(resource[this->now_index], gfx::BindingDestination::Graphics);
    cmd.first_binding = this->binding;
    pst << cmd;
}
