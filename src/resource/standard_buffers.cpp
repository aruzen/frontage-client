//
// Created by morimoto_hibiki on 2025/12/27.
//

#include "standard_buffers.hpp"

vsl::DeviceLocalBuffer<vsl::MemoryType::VertexBuffer>
make_normal_texcoord(std::optional<aho::StandardEngine> e) {
    using namespace aho;
    auto d = e->_data;
    std::array<d2::PointF, 4> texcoords = {
            d2::PointF{0.0f, 0.0f},
            d2::PointF{0.0f, 1.0f},
            d2::PointF{1.0f, 1.0f},
            d2::PointF{1.0f, 0.0f},
    };
    return {d->logical_device, d->command_manager, texcoords};
}

static vsl::DeviceLocalBuffer<vsl::MemoryType::VertexBuffer>
bufs::normal_texcoord(std::optional<aho::StandardEngine> e) {
    static auto buf = make_normal_texcoord(e);
    return buf;
}

vsl::DeviceLocalBuffer<vsl::MemoryType::VertexBuffer>
make_rect_indexes(std::optional<aho::StandardEngine> e) {
    using namespace aho;
    auto d = e->_data;
    const std::array<uint32_t, 6> indexes = {
            0, 1, 2, 2, 3, 0
    };
    return {d->logical_device, d->command_manager, indexes};
}

static vsl::DeviceLocalBuffer<vsl::MemoryType::VertexBuffer>
bufs::rect_indexes(std::optional<aho::StandardEngine> e) {
    static auto buf = make_rect_indexes(e);
    return buf;
}