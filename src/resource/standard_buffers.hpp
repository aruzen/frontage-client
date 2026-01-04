//
// Created by morimoto_hibiki on 2025/12/27.
//

#ifndef FRONTAGE_CLIENT_STANDERD_BUFFERS_HPP
#define FRONTAGE_CLIENT_STANDERD_BUFFERS_HPP

#include <AHO/aho.hpp>

namespace bufs {
	std::optional<vsl::DeviceLocalBuffer<vsl::MemoryType::VertexBuffer>>
		init_normal_texcoord(std::optional<aho::StandardEngine> e = std::nullopt, bool clean = false);

	vsl::DeviceLocalBuffer<vsl::MemoryType::VertexBuffer>
		normal_texcoord();

	std::optional<vsl::DeviceLocalBuffer<vsl::MemoryType::IndexBuffer>>
		init_rect_indexes(std::optional<aho::StandardEngine> e = std::nullopt, bool clean = false);

	vsl::DeviceLocalBuffer<vsl::MemoryType::IndexBuffer>
		rect_indexes();
}

#endif //FRONTAGE_CLIENT_STANDERD_BUFFERS_HPP
