#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "gltf_loader.hpp"

#include <array>
#include <cctype>
#include <filesystem>
#include <optional>
#include <vector>

namespace utils {
    inline bool hasGlbExtension(const std::filesystem::path &p) {
        auto ext = p.extension().string();
        for (auto &c: ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return ext == ".glb";
    }

    template<typename T>
    const T *getBufferPtr(const tinygltf::Model &model, const tinygltf::Accessor &accessor,
                          const tinygltf::BufferView &view, size_t index) {
        const auto &buffer = model.buffers[view.buffer];
        const auto stride = accessor.ByteStride(view);
        const size_t offset = accessor.byteOffset + view.byteOffset + index * stride;
        if (offset + sizeof(T) > buffer.data.size()) return nullptr;
        return reinterpret_cast<const T *>(&buffer.data[offset]);
    }

// POSITION を float3 で読む前提の単純な抽出
    bool readPosition(const tinygltf::Model &model, const tinygltf::Primitive &primitive,
                      std::vector<std::array<float, 3>> &out, std::string *err) {
        auto attr = primitive.attributes.find("POSITION");
        if (attr == primitive.attributes.end()) {
            if (err) *err += "POSITION attribute missing\n";
            return false;
        }

        const auto &accessor = model.accessors[attr->second];
        const auto &view = model.bufferViews[accessor.bufferView];
        if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || accessor.type != TINYGLTF_TYPE_VEC3) {
            if (err) *err += "POSITION must be VEC3 float\n";
            return false;
        }

        out.resize(accessor.count);
        for (size_t i = 0; i < accessor.count; ++i) {
            const float *ptr = getBufferPtr<float>(model, accessor, view, i);
            if (!ptr) {
                if (err) *err += "POSITION buffer overrun\n";
                return false;
            }
            out[i] = {ptr[0], ptr[1], ptr[2]};
        }
        return true;
    }

    bool readIndices(const tinygltf::Model &model, const tinygltf::Primitive &primitive,
                     std::vector<uint32_t> &out, std::string *err) {
        if (primitive.indices < 0) {
            return false; // インデックスなし
        }

        const auto &accessor = model.accessors[primitive.indices];
        const auto &view = model.bufferViews[accessor.bufferView];
        const auto &buffer = model.buffers[view.buffer];
        const auto stride = accessor.ByteStride(view);
        out.resize(accessor.count);

        auto copyIndex = [&](size_t i, uint32_t v) { out[i] = v; };

        for (size_t i = 0; i < accessor.count; ++i) {
            const unsigned char *base = buffer.data.data() + view.byteOffset + accessor.byteOffset + i * stride;
            switch (accessor.componentType) {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    copyIndex(i, *reinterpret_cast<const uint8_t *>(base));
                    break;
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    copyIndex(i, *reinterpret_cast<const uint16_t *>(base));
                    break;
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                    copyIndex(i, *reinterpret_cast<const uint32_t *>(base));
                    break;
                default:
                    if (err) *err += "Unsupported index component type\n";
                    return false;
            }
        }
        return true;
    }

    bool LoadGLTF(const std::string &path, tinygltf::Model &model, std::string *warn, std::string *err) {
        tinygltf::TinyGLTF loader;
        std::string w, e;
        bool ok = false;

        if (utils::hasGlbExtension(path)) {
            ok = loader.LoadBinaryFromFile(&model, &w, &e, path);
        } else {
            ok = loader.LoadASCIIFromFile(&model, &w, &e, path);
        }

        if (warn) *warn = std::move(w);
        if (err) *err = std::move(e);
        return ok;
    }

    std::optional<tinygltf::Model> LoadGLTF(const std::string &path, std::string *warn, std::string *err) {
        tinygltf::Model model;
        if (!LoadGLTF(path, model, warn, err)) return std::nullopt;
        return model;
    }

    CompressedMesh BuildCompressedMesh(const tinygltf::Model &model, std::string *warn, std::string *err) {
        CompressedMesh mesh;

        for (const auto &mesh_node: model.meshes) {
            for (const auto &primitive: mesh_node.primitives) {
                if (primitive.mode != TINYGLTF_MODE_TRIANGLES) {
                    if (warn) *warn += "Skip non-triangle primitive\n";
                    continue; // TODO : implement me (ライン/ストリップ対応)
                }

                std::vector<std::array<float, 3>> positions;
                if (!readPosition(model, primitive, positions, err)) {
                    continue;
                }

                std::vector<uint32_t> indices;
                const bool has_index = readIndices(model, primitive, indices, err);

                auto emitVertex = [&](uint32_t idx) {
                    const auto max_index = positions.size();
                    if (idx >= max_index) {
                        if (err) *err += "Index out of range in primitive\n";
                        return false;
                    }
                    const auto &p = positions[idx];
                    mesh.positions.emplace_back(p[0], p[1], p[2]);
                    mesh.indices.push_back(static_cast<uint32_t>(mesh.positions.size() - 1));
                    return true;
                };

                if (has_index) {
                    for (auto idx: indices) emitVertex(idx);
                } else {
                    for (uint32_t i = 0; i < positions.size(); ++i) emitVertex(i);
                }
            }
        }

        return mesh;
    }
} // namespace utils
