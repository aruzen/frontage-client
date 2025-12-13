#include "snapshot.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>

namespace scene::game {

namespace {
constexpr std::size_t kMaxCells = 1'000'000; // 安全のための上限（約 1000x1000）

bool willOverflow(std::size_t a, std::size_t b) {
    if (a == 0 || b == 0) return false;
    return a > std::numeric_limits<std::size_t>::max() / b;
}
} // namespace

Snapshot CopySnapshot(const CPSnapshot &src) {
    Snapshot dst{};
    dst.width = src.width;
    dst.height = src.height;
    dst.turn = src.turn;
    dst.active_player = src.active_player;
    if (src.log_text != nullptr) {
        dst.log_text = src.log_text;
    }

    // 不正値を避けるために寸法を検証
    if (src.width <= 0 || src.height <= 0) {
        // TODO : implement me (負値の扱いポリシーが未決定)
        return dst;
    }

    if (willOverflow(static_cast<std::size_t>(src.width), static_cast<std::size_t>(src.height))) {
        std::cerr << "snapshot copy skipped: size overflow" << std::endl;
        return dst;
    }

    const std::size_t cell_count = static_cast<std::size_t>(src.width) * static_cast<std::size_t>(src.height);
    if (cell_count == 0) return dst;
    if (cell_count > kMaxCells) {
        std::cerr << "snapshot copy truncated: cell_count=" << cell_count << " exceeds cap " << kMaxCells
                  << std::endl;
        // TODO : implement me (巨大盤面の扱い方を決める)
        return dst;
    }

    if (src.pieces != nullptr) {
        dst.pieces.assign(src.pieces, src.pieces + cell_count);
    }

    if (src.structures != nullptr) {
        dst.structures.assign(src.structures, src.structures + cell_count);
    }

    return dst;
}

} // namespace scene::game

