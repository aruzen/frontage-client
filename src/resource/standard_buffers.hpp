//
// Created by morimoto_hibiki on 2025/12/27.
//

#ifndef FRONTAGE_CLIENT_STANDERD_BUFFERS_HPP
#define FRONTAGE_CLIENT_STANDERD_BUFFERS_HPP

#include <AHO/aho.hpp>

namespace bufs {
    static vsl::DeviceLocalBuffer<vsl::MemoryType::VertexBuffer>
    normal_texcoord(std::optional<aho::StandardEngine> e);

    static vsl::DeviceLocalBuffer<vsl::MemoryType::VertexBuffer>
    rect_indexes(std::optional<aho::StandardEngine> e);
}

#endif //FRONTAGE_CLIENT_STANDERD_BUFFERS_HPP
