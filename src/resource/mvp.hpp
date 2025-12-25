//
// Created by morimoto_hibiki on 2025/12/19.
//

#ifndef FRONTAGE_CLIENT_MVP_HPP
#define FRONTAGE_CLIENT_MVP_HPP

#include <vector>
#include <VSL/vsl.hpp>
#include <AHO/engine.hpp>
#include <AHO/engine/standard_engine.hpp>

struct MVPMatrix {
    struct Context {
        aho::StandardEngine engine;
        vsl::graphic_resource::BindingLayout mvp_layout;
        vsl::graphic_resource::Pool mvp_pool; // size = 10
    };

    static Context make_context(std::optional<aho::StandardEngine> engine);

    static Context &get_context(std::optional<aho::StandardEngine> engine = std::nullopt);

    struct Data {
        aho::Mat4x4<float> model;
        aho::Mat4x4<float> view;
        aho::Mat4x4<float> proj;
    };
private:
    Data _data{};
public:
    using UboBuffer = vsl::Buffer<vsl::MemoryType::UniformBuffer,
            vsl::MemoryProperty::HostVisible | vsl::MemoryProperty::HostCoherent>;
    std::vector<UboBuffer> buffers{};
    std::vector<vsl::graphic_resource::Resource> resource{};
    size_t generation = 0;
    std::vector<size_t> generations{};

    size_t now_index{};

    MVPMatrix() = default;

    explicit MVPMatrix(Data data);

    MVPMatrix(aho::Mat4x4<float> model, aho::Mat4x4<float> view, aho::Mat4x4<float> proj);

    Data data();

    aho::Mat4x4F model() const;

    aho::Mat4x4F view() const;

    aho::Mat4x4F proj() const;

    void set_index(size_t index);

    void set(Data data);

    void set_model(aho::Mat4x4<float> model);

    void set_view(aho::Mat4x4<float> view);

    void set_proj(aho::Mat4x4<float> proj);

    void invoke(vsl::DefaultPhaseStream& pst, const vsl::CommandPool& pool, const vsl::CommandBuffer& buffer, const vsl::CommandManager& manager);
};


#endif //FRONTAGE_CLIENT_MVP_HPP
