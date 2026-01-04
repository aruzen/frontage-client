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
	return { d->logical_device, d->command_manager, texcoords };
}

std::optional<vsl::DeviceLocalBuffer<vsl::MemoryType::VertexBuffer>>
bufs::init_normal_texcoord(std::optional<aho::StandardEngine> e, bool clean) {
	static std::optional buf = make_normal_texcoord(e);
	if (clean)
		buf.reset();
	return buf;
}


vsl::DeviceLocalBuffer<vsl::MemoryType::VertexBuffer>
bufs::normal_texcoord() {
	return init_normal_texcoord().value();
}

vsl::DeviceLocalBuffer<vsl::MemoryType::IndexBuffer>
make_rect_indexes(std::optional<aho::StandardEngine> e) {
	using namespace aho;
	auto d = e->_data;
	const std::array<uint32_t, 6> indexes = {
			0, 1, 2, 2, 3, 0
	};
	return { d->logical_device, d->command_manager, indexes };
}

std::optional<vsl::DeviceLocalBuffer<vsl::MemoryType::IndexBuffer>>
bufs::init_rect_indexes(std::optional<aho::StandardEngine> e, bool clean) {
	static std::optional buf = make_rect_indexes(e);
	if (clean)
		buf.reset();
	return buf;
}

vsl::DeviceLocalBuffer<vsl::MemoryType::IndexBuffer>
bufs::rect_indexes() {
	return init_rect_indexes().value();
}