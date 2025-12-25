#pragma once

#include <VSL/vsl.hpp>
#include <VSL/utils/spir_reflector.hpp>
#include <AHO/aho.hpp>

#include <filesystem>
#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>

namespace utils {
    struct BuildResult {
        vsl::PipelineLayout layout;
        vsl::GraphicsPipeline pipeline;
    };

    inline BuildResult build_reflected_pipeline(aho::engine::GraphicalEngine *e,
                                                const vsl::PipelineLayout &base_layout,
                                                std::string vert_path, std::string frag_path) {
        namespace pl = vsl::pipeline_layout;

        auto &[vulkan_instance, physical_device, device, command_manager, graphic_resource_manager, synchro_manager]
                = *e->_data;
        auto &[surface, swapchain, image_view, render_pass,
                frame_buffer, image_available, render_finish, in_flight]
                = *e->getWindow()._data2;
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
                = vsl::utils::SPIRVReflector(device, std::filesystem::path(
                        vsl::expand_environments(frag_path))).generated;
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

}
