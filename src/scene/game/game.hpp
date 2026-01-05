//
// Created by morimoto_hibiki on 2025/12/19.
//

#ifndef FRONTAGE_CLIENT_GAME_HPP
#define FRONTAGE_CLIENT_GAME_HPP

#include <AHO/aho.hpp>
#include "../scene.hpp"
#include "../../resource/image.hpp"
#include "../../resource/mvp.hpp"

class GameScene : public Scene {
public:
    static constexpr uint32_t board_size = 7;

    enum class tile_texture : std::uint32_t {
        Sabaku = 0,
        Sougenn = 1
    };

    enum class tile_state : std::uint32_t {
        Normal = 0,
        MouseOver = 1,
        Slected = 2
    };
private:
    struct Data {
        aho::StandardEngine engine;
        GlobalContext *gctx;
        VPMatrix vpmat;

        vsl::DeviceLocalBuffer<vsl::MemoryType::VertexBuffer> vert_buffer;
        vsl::Buffer<vsl::MemoryType::VertexBuffer,
                vsl::MemoryProperty::HostCoherent | vsl::MemoryProperty::HostVisible> state_buffer;
        vsl::Buffer<vsl::MemoryType::VertexBuffer,
                vsl::MemoryProperty::HostCoherent | vsl::MemoryProperty::HostVisible> tile_model_buffer;
        vsl::Buffer<vsl::MemoryType::VertexBuffer,
                vsl::MemoryProperty::HostCoherent | vsl::MemoryProperty::HostVisible> piece_model_buffer;

        ImageArray tile_images;
        vsl::PipelineLayout tile_texture_layout;
        vsl::GraphicsPipeline tile_texture;
        ImageArray piece_images;
        vsl::PipelineLayout piece_texture_layout;
        vsl::GraphicsPipeline piece_texture;

        ImageArray structure_images;

        vsl::IDPickingRenderPass picking_render_pass;
        vsl::FrameBuffer picking_frame_buffer;
    };
    std::optional<Data> data;
public:
    void load(aho::StandardEngine e, GlobalContext &gctx) override;

    void unload() override;

    SceneID transfer() override;
};

#endif //FRONTAGE_CLIENT_GAME_HPP
