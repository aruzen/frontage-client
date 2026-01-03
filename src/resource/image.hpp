//
// Created by morimoto_hibiki on 2025/12/15.
//

#ifndef FRONTAGE_CLIENT_IMAGE_HPP
#define FRONTAGE_CLIENT_IMAGE_HPP


#include <VSL/vsl.hpp>
#include <AHO/engine.hpp>
#include <AHO/engine/standard_engine.hpp>

#include "../utils/bitmap_text.hpp"

struct Image {
    struct Context {
        aho::StandardEngine engine;
        vsl::graphic_resource::BindingLayout texture_binding_layout;
        vsl::graphic_resource::Pool texture_pool;
    };

    static Context make_context(std::optional<aho::StandardEngine> engine);

    static Context &get_context(std::optional<aho::StandardEngine> engine = std::nullopt, bool clean = false);

    std::string name;
    uint32_t channel, width, height;
    vsl::graphic_resource::Resource resource;
    vsl::ImageAccessor image;
    vsl::SamplerAccessor sampler;

    Image(const std::string &name, const std::string &file);

    Image(const std::string &name, utils::Bitmap bitmap);

    Image(const std::string &name, size_t width, size_t height, size_t channel, unsigned char *buf);

    void update(const std::string& file_name);

    void update(const utils::Bitmap& bitmap);

    void update(size_t width, size_t height, size_t channel, unsigned char *buf);

    aho::d2::VectorI size();
};

struct ImageArray {
    std::string name;
    uint32_t channel, width, height;
    vsl::graphic_resource::Resource resource;
    vsl::ImageAccessor image;
    vsl::SamplerAccessor sampler;

    ImageArray(const std::string &name, std::initializer_list<std::string> file);

    ImageArray(const std::string &name, std::initializer_list<utils::Bitmap> bitmap);

    ImageArray(const std::string &name, size_t width, size_t height, size_t channel, size_t count,
               std::initializer_list<unsigned char *> bufs);

    void update(std::initializer_list<std::string> file_name);

    void update(std::vector<utils::Bitmap> bitmap);

    void update(size_t width, size_t height, size_t channel, size_t count, std::vector<unsigned char *> buf);

    void update(const std::string &file_name);

    void update(const utils::Bitmap &bitmap);

    void update(size_t width, size_t height, size_t channel, size_t target, unsigned char *buf);

    aho::d2::VectorI size();
};



#endif //FRONTAGE_CLIENT_IMAGE_HPP
