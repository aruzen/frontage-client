#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "c_api.hpp"

namespace scene::game {

// C++ 側で扱いやすいスナップショット表現
struct Snapshot {
    int width = 0;
    int height = 0;
    int turn = 0;
    uint8_t active_player = 0;
    std::vector<CPPiece> pieces;
    std::vector<uint64_t> structures;
    std::string log_text;

    [[nodiscard]] bool hasBoard() const { return width > 0 && height > 0; }
    [[nodiscard]] std::size_t expectedCellCount() const {
        return hasBoard() ? static_cast<std::size_t>(width) * static_cast<std::size_t>(height) : 0U;
    }
    [[nodiscard]] bool isConsistent() const { return !hasBoard() || pieces.size() == expectedCellCount(); }
};

// CPSnapshot を安全にコピーして Snapshot へ変換
Snapshot CopySnapshot(const CPSnapshot &src);

} // namespace scene::game

