//
// Created by morimoto_hibiki on 2025/12/15.
//

#include "image.hpp"
#include <AHO/pipeline/layouts.hpp>
#include <utility>
#include <stb_image.h>
#include <ranges>

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

Image::Context &Image::get_context(std::optional<aho::StandardEngine> engine, bool clean) {
    static std::optional<Image::Context> context = make_context(engine);
    if (clean)
        context.reset();
    return *context;
}

Image::Image(const std::string &name, const std::string &file_path) : name(name) {
    sampler = vsl::Sampler(Image::get_context().engine._data->logical_device);
    update(file_path);
}

Image::Image(const std::string &name, size_t width, size_t height, size_t channel, unsigned char *buf) : name(name) {
    sampler = vsl::Sampler(Image::get_context().engine._data->logical_device);
    update(width, height, channel, buf);
}

Image::Image(const std::string &name, utils::Bitmap bitmap) : name(name) {
    sampler = vsl::Sampler(Image::Image::get_context().engine._data->logical_device);
    update(bitmap);
}

void Image::update(const std::string &file_path) {
    using namespace vsl;
    using namespace aho;
    auto &engine_data = Image::get_context().engine._data;

    std::filesystem::path path(vsl::expand_environments(file_path));
    loggingln(path);

    int texWidth = 0, texHeight = 0, texChannels = 0;
    stbi_uc *pixels = stbi_load(path.c_str(),
                                &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    update(static_cast<size_t>(texWidth), static_cast<size_t>(texHeight),
           static_cast<size_t>(texChannels), pixels);
    stbi_image_free(pixels);
}

void Image::update(const utils::Bitmap &bitmap) {
    update(static_cast<size_t>(bitmap.width), static_cast<size_t>(bitmap.height), 1,
           const_cast<unsigned char *>(bitmap.pixels.data()));
}

void Image::update(size_t width, size_t height, size_t channel, unsigned char *buf) {
    using namespace vsl;
    using namespace aho;
    this->width = width;
    this->channel = channel;
    this->height = height;

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

aho::d2::VectorI Image::size() {
    return {(int) width, (int) height};
}

ImageArray::ImageArray(const std::string &name, std::initializer_list<std::string> file) : name(std::move(name)) {
    sampler = vsl::Sampler(Image::get_context().engine._data->logical_device);
    update(file);
}

ImageArray::ImageArray(const std::string &name, std::initializer_list<utils::Bitmap> bitmap) : name(std::move(name)) {
    sampler = vsl::Sampler(Image::get_context().engine._data->logical_device);
    update(bitmap);
}

ImageArray::ImageArray(const std::string &name, size_t width, size_t height, size_t channel, size_t count,
                       std::initializer_list<unsigned char *> bufs) : name(std::move(name)) {
    sampler = vsl::Sampler(Image::get_context().engine._data->logical_device);
    update(width, height, channel, count, bufs);
}

void ImageArray::update(std::initializer_list<std::string> file_names) {
    using namespace vsl;
    using namespace aho;
    auto &engine_data = ::Image::get_context().engine._data;

    int texWidth = 0, texHeight = 0, texChannels = 0;
    std::vector<stbi_uc *> images;
    images.reserve(file_names.size());
    for (const auto &file_path: file_names) {
        std::filesystem::path path(vsl::expand_environments(file_path));
        loggingln(path);

        int texWidthTmp = 0, texHeightTmp = 0, texChannelsTmp = 0;
        stbi_uc *pixels = stbi_load(path.c_str(),
                                    &texWidthTmp, &texHeightTmp, &texChannelsTmp, STBI_rgb_alpha);
        if (texWidth == 0) {
            texWidth = texWidthTmp;
            texHeight = texHeightTmp;
            texChannels = texChannelsTmp;
        } else if (texWidth != texWidthTmp || texHeight != texHeightTmp || texChannels != texChannelsTmp) {
            loggingln("warning : ImageArray don't support difference size image.");
        }
        images.push_back(pixels);
    }
    update(static_cast<size_t>(texWidth), static_cast<size_t>(texHeight),
           static_cast<size_t>(texChannels), file_names.size(), images);
    for (auto i: images)
        stbi_image_free(i);
}

void ImageArray::update(std::vector<utils::Bitmap> bitmaps) {
    update(static_cast<size_t>(bitmaps[0].width), static_cast<size_t>(bitmaps[0].height), 1, bitmaps.size(),
           bitmaps
           | std::views::transform([](auto a) { return a.pixels.data(); })
           | std::ranges::to<std::vector<unsigned char *>>());
}

void ImageArray::update(size_t width, size_t height, size_t channel, size_t count,
                        std::vector<unsigned char *> bufs) {
    using namespace vsl;
    using namespace aho;
    this->width = width;
    this->channel = channel;
    this->height = height;

    auto &engine_data = ::Image::Image::get_context().engine._data;

    image = vsl::Image<ImageType::TransferDst | ImageType::Sampled>(engine_data->logical_device, width, height,
                                                                    channel == 1 ? vsl::data_format::Srgb8R
                                                                                 : vsl::data_format::Srgb8RGBA,
                                                                    bufs.size());
    std::vector<StagingBuffer<>> staging_bufs;
    for (std::uint32_t i = 0; i < count; i++) {
        staging_bufs.emplace_back(engine_data->logical_device, width * height * channel);
        auto data = staging_bufs[i].data();
        memcpy(data.data, bufs[i], width * height * channel);
    }

    auto in_flight = ::Image::Image::get_context().engine.boot_window->_data2->in_flight;
    {
        auto phase = engine_data->command_manager.startPhase<ComputePhase>(std::nullopt, std::nullopt, in_flight);
        phase << command::ChangeImageBarrier(image, ImageLayout::Undefined,
                                             ImageLayout::TransferDstOptimal);
        for (std::uint32_t i = 0; i < count; i++) {
            phase << command::CopyBufferToArrayImage(image, &staging_bufs[i], ImageLayout::TransferDstOptimal, i);
        }
        phase << command::ChangeImageBarrier(image, ImageLayout::TransferDstOptimal,
                                             ImageLayout::ShaderReadOnlyOptimal);
    }
    in_flight.wait();
    const auto &[ok, resource] = ::Image::Image::get_context().texture_pool
            .bind(::Image::Image::get_context().texture_binding_layout);
    if (!ok) {
        vsl::loggingln("texture bind failed.");
        throw std::runtime_error("texture bind failed.");
    }
    this->resource = resource;
    this->resource.update(image, sampler, 0, vsl::graphic_resource::Type::CombinedImageSampler);
}

void ImageArray::update(const std::string &file_name) {
    // TODO
    throw std::runtime_error("未実装");
}

void ImageArray::update(const utils::Bitmap &bitmap) {
    // TODO
    throw std::runtime_error("未実装");
}

void ImageArray::update(size_t width, size_t height, size_t channel, size_t target, unsigned char *buf) {
    // TODO
    throw std::runtime_error("未実装");
}

aho::d2::VectorI ImageArray::size() {
    return {(int) width, (int) height};
}