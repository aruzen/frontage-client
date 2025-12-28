//
// Created by morimoto_hibiki on 2025/12/19.
//

#include "mvp.hpp"
#include <AHO/pipeline/layouts.hpp>
#include <utility>

namespace gfx = vsl::graphic_resource;

MVPMatrix::Context MVPMatrix::make_context(std::optional<aho::StandardEngine> e) {
    if (not e.has_value())
        throw std::runtime_error("error: You must init MVPMatrix::make_context.");
    auto engine = e.value();
    return MVPMatrix::Context{
            .engine = engine,
            .mvp_layout = gfx::BindingLayout(
                    engine._data->logical_device, {
                            aho::pipeline::getBindingPoint(aho::pipeline::ResourceName::MVPMatrixUBO)
                    }),
            .mvp_pool = engine._data->graphic_resource_manager->make(
                    std::map<gfx::Type, size_t>{
                            {gfx::Type::UniformBuffer, 10}
                    })
    };
}

MVPMatrix::Context &MVPMatrix::get_context(std::optional<aho::StandardEngine> engine) {
    static MVPMatrix::Context context = make_context(engine);
    return context;
}

MVPMatrix::MVPMatrix(Data data) : _data(data) {
    using namespace aho::pipeline;

    auto &context = get_context();
    auto &engine_data = *context.engine._data;
    auto size = context.engine.boot_window->_data2->swapchain.getSwapImageSize();
    for (size_t i = 0; i < size; i++) {
        MVPMatrix::UboBuffer ubo(engine_data.logical_device, this->_data);
        this->buffers.push_back(ubo);
    }
    vsl::graphic_resource::BindingLayout l(engine_data.logical_device, {getBindingPoint(ResourceName::MVPMatrixUBO)});
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

MVPMatrix::MVPMatrix(aho::Mat4x4<float> model, aho::Mat4x4<float> view, aho::Mat4x4<float> proj)
        : MVPMatrix::MVPMatrix(MVPMatrix::Data{
        .model = model,
        .view = view,
        .proj = proj,
}) {}

void MVPMatrix::set_index(size_t index) {
    this->now_index = index;
}

MVPMatrix::Data MVPMatrix::data() {
    return _data;
}

aho::Mat4x4F MVPMatrix::model() const {
    return _data.model;
}

aho::Mat4x4F MVPMatrix::view() const {
    return _data.view;
}

aho::Mat4x4F MVPMatrix::proj() const {
    return _data.proj;
}

void MVPMatrix::set(MVPMatrix::Data data) {
    _data = data;
    generation++;
}

void MVPMatrix::set_model(aho::Mat4x4<float> model) {
    _data.model = model;
    generation++;
}

void MVPMatrix::set_view(aho::Mat4x4<float> view) {
    _data.view = view;
    generation++;
}

void MVPMatrix::set_proj(aho::Mat4x4<float> proj) {
    _data.proj = proj;
    generation++;
}

void MVPMatrix::invoke(vsl::DefaultPhaseStream &pst, const vsl::CommandPool &pool, const vsl::CommandBuffer &buffer,
                       const vsl::CommandManager &manager) {
    if (generations[this->now_index] < generation) {
        buffers[this->now_index].copy(_data);
    }
    pst << vsl::command::BindGraphicResource(resource[this->now_index], gfx::BindingDestination::Graphics);
}
