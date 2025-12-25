//
// Created by morimoto_hibiki on 2025/12/15.
//

#include "image.hpp"
#include <AHO/pipeline/layouts.hpp>
#include <utility>
#include <stb_image.h>

Image::Context Image::make_context(std::optional<aho::StandardEngine> e) {
    if (not e.has_value())
        throw std::runtime_error("error: You must init Image::make_context.");
    auto engine = e.value();
    return Image::Context{
            .engine = engine,
            .texture_binding_layout = vsl::graphic_resource::BindingLayout(
                    engine._data->logical_device, {
                            aho::pipeline::getBindingPoint(aho::pipeline::ResourceName::Texture)
                    }),
            .texture_pool = engine._data->graphic_resource_manager->make(
                    std::map<vsl::graphic_resource::Type, size_t>{
                            {vsl::graphic_resource::Type::CombinedImageSampler, 100}
                    })
    };
}

Image::Context& Image::get_context(std::optional<aho::StandardEngine> engine) {
    static Image::Context context = make_context(engine);
    return context;
}

Image::Image(std::string name, const std::string &file_path) : name(std::move(name)) {
    using namespace vsl;
    using namespace aho;
    auto &engine_data = Image::get_context().engine._data;

    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(vsl::expand_environments(file_path).c_str(),
                                &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    sampler = Sampler(engine_data->logical_device);
    update(static_cast<size_t>(texWidth), static_cast<size_t>(texHeight),
           static_cast<size_t>(texChannels), pixels);
}

Image::Image(const std::string &name, size_t width, size_t height, size_t channel, unsigned char *buf) {
    sampler = vsl::Sampler(Image::get_context().engine._data->logical_device);
    update(width, height, channel, buf);
}

Image::Image(const std::string &name, utils::Bitmap bitmap) {
    sampler = vsl::Sampler(Image::Image::get_context().engine._data->logical_device);
    update(bitmap);
}

void Image::update(const utils::Bitmap &bitmap) {
    update(static_cast<size_t>(bitmap.width), static_cast<size_t>(bitmap.height), 1,
           const_cast<unsigned char *>(bitmap.pixels.data()));
}

void Image::update(size_t width, size_t height, size_t channel, unsigned char *buf) {
    using namespace vsl;
    using namespace aho;
    auto &engine_data = Image::Image::get_context().engine._data;

    image = vsl::Image<ImageType::TransferDst | ImageType::Sampled>(engine_data->logical_device, width, height,
                                                                    channel == 1 ? vsl::data_format::Srgb8R
                                                                                 : vsl::data_format::Srgb8RGBA);
    {
        auto in_flight = Image::Image::get_context().engine.boot_window->_data2->in_flight;
        StagingBuffer<> staging_buf(engine_data->logical_device, width * height * channel);
        auto data = staging_buf.data();
        memcpy(data.data, buf, width * height * channel);
        {
            auto phase = engine_data->command_manager.startPhase<ComputePhase>(std::nullopt, std::nullopt, in_flight);
            phase << command::ChangeImageBarrier(image, ImageLayout::Undefined,
                                                 ImageLayout::TransferDstOptimal);
            phase << command::CopyBufferToImage(image, &staging_buf, ImageLayout::TransferDstOptimal);
            phase << command::ChangeImageBarrier(image, ImageLayout::TransferDstOptimal,
                                                 ImageLayout::ShaderReadOnlyOptimal);
        }
        in_flight.wait();
    }
    const auto &[ok, resource] = Image::Image::get_context().texture_pool
            .bind(Image::Image::get_context().texture_binding_layout);
    if (!ok) {
        vsl::loggingln("texture bind failed.");
        throw std::runtime_error("texture bind failed.");
    }
    this->resource = resource;
    this->resource.update(image, sampler, 0, vsl::graphic_resource::Type::CombinedImageSampler);
}
