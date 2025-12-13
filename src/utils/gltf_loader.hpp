#pragma once

#include <optional>
#include <string>
#include <vector>

#include <tiny_gltf.h>
#include <AHO/core/Point.hpp>
#include <AHO/core/math/coordinate.hpp>

namespace utils {
    struct CompressedMesh {
        std::vector<aho::d3::PointF> positions;
        std::vector<uint32_t> indices;
    };

// GLTF/GLB を tinygltf::Model に読み込む。成功で true。
    bool LoadGLTF(const std::string &path, tinygltf::Model &model, std::string *warn = nullptr,
                  std::string *err = nullptr);

// 成功時にモデルを返す簡易版。失敗時は std::nullopt。
    std::optional<tinygltf::Model> LoadGLTF(const std::string &path, std::string *warn = nullptr,
                                            std::string *err = nullptr);

// モデル中の TRIANGLES プリミティブから頂点を列挙し、GPU 転送用に連番インデックスを割り当てる
    CompressedMesh BuildCompressedMesh(const tinygltf::Model &model, std::string *warn = nullptr,
                                       std::string *err = nullptr);

} // namespace utils
