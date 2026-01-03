#pragma once

#include <VSL/vsl.hpp>
#include <VSL/utils/spir_reflector.hpp>
#include <AHO/aho.hpp>

#include <filesystem>
#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>
#include <optional>

namespace utils {
    struct BuildResult {
        vsl::PipelineLayout layout;
        vsl::GraphicsPipeline pipeline;
    };

    inline BuildResult build_reflected_pipeline(vsl::LogicalDeviceAccessor device,
                                                vsl::RenderPassAccessor render_pass,
                                                const vsl::PipelineLayout &base_layout,
                                                std::string vert_path, std::string frag_path,
                                                std::function<void(
                                                        vsl::utils::SPIRVReflector::GenerateData &)> vert_decorator = nullptr,
                                                std::function<void(
                                                        vsl::utils::SPIRVReflector::GenerateData &)> frag_decorator = nullptr) {
        namespace pl = vsl::pipeline_layout;
        vert_path = vsl::expand_environments(vert_path);
        frag_path = vsl::expand_environments(frag_path);
        auto layout = base_layout.copy();

        auto vert_shader = vsl::Shader<vsl::ShaderType::Vertex>(device, vert_path);
        auto frag_shader = vsl::Shader<vsl::ShaderType::Fragment>(device, frag_path);
        auto shaders = pl::ShaderGroup(vert_shader.name(), {vert_shader, frag_shader});
        layout.add(shaders);

        auto vert_reflector
                = vsl::utils::SPIRVReflector(device, std::filesystem::path(vert_path)).generated;
        auto frag_reflector
                = vsl::utils::SPIRVReflector(device, std::filesystem::path(frag_path)).generated;
        if (vert_decorator)
            vert_decorator(vert_reflector);
        if (frag_decorator)
            frag_decorator(frag_reflector);

        auto layouts = vert_reflector.makeBindingLayout();
        const auto &frag_layout = frag_reflector.makeBindingLayout();

        layouts.reserve(layouts.size() + frag_layout.size());
        layouts.insert(layouts.end(), frag_layout.begin(), frag_layout.end());
        if (!layouts.empty())
            layout.add(pl::ResourceBinding(layouts));

        if (vert_reflector.vertex_input)
            layout.add(*vert_reflector.vertex_input);

        if (vert_reflector.push_constants)
            layout.add(*vert_reflector.push_constants);
        return {layout, vsl::GraphicsPipeline(layout, render_pass)};
    }

    inline BuildResult build_reflected_pipeline(aho::engine::GraphicalEngine *e,
                                                const vsl::PipelineLayout &base_layout,
                                                std::string vert_path, std::string frag_path,
                                                std::function<void(
                                                        vsl::utils::SPIRVReflector::GenerateData &)> vert_decorator = nullptr,
                                                std::function<void(
                                                        vsl::utils::SPIRVReflector::GenerateData &)> frag_decorator = nullptr) {
        auto &device = e->_data->logical_device;
        auto &render_pass = e->getWindow()._data2->render_pass;
        return build_reflected_pipeline(device, render_pass, base_layout, vert_path, frag_path, vert_decorator, frag_decorator);
    }
}
