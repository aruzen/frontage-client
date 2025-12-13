#include "bridge.hpp"

#include <atomic>
#include <deque>
#include <mutex>
#include <optional>

namespace scene::game {

namespace {
constexpr std::size_t kSnapshotCapacity = 3;   // 最新優先で過去は捨てる
constexpr std::size_t kInputCapacity = 64;     // 入力キュー上限

std::mutex snapshot_mutex;
std::deque<Snapshot> snapshot_queue;

std::mutex input_mutex;
std::deque<CPInputEvent> input_queue;
std::atomic<SubmitInputCallback> submit_input_cb{nullptr};
} // namespace

void PushSnapshot(const CPSnapshot &src) {
    Snapshot copied = CopySnapshot(src);

    std::lock_guard lock(snapshot_mutex);
    if (snapshot_queue.size() >= kSnapshotCapacity) {
        snapshot_queue.pop_front(); // 古いものから破棄
    }
    snapshot_queue.push_back(std::move(copied));
}

std::optional<Snapshot> ConsumeLatestSnapshot() {
    std::lock_guard lock(snapshot_mutex);
    if (snapshot_queue.empty()) return std::nullopt;

    Snapshot latest = std::move(snapshot_queue.back());
    snapshot_queue.clear();
    return latest;
}

bool EmitInputEvent(const CPInputEvent &ev) {
    if (auto cb = submit_input_cb.load(std::memory_order_acquire)) {
        cb(&ev);
        return true;
    }

    std::lock_guard lock(input_mutex);
    if (input_queue.size() >= kInputCapacity) {
        input_queue.pop_front();
    }
    input_queue.push_back(ev);
    return false;
}

std::optional<CPInputEvent> PollQueuedInput() {
    std::lock_guard lock(input_mutex);
    if (input_queue.empty()) return std::nullopt;

    CPInputEvent ev = input_queue.front();
    input_queue.pop_front();
    return ev;
}

void ClearQueues() {
    {
        std::lock_guard lock(snapshot_mutex);
        snapshot_queue.clear();
    }
    {
        std::lock_guard lock(input_mutex);
        input_queue.clear();
    }
}

void SetSubmitInputCallback(SubmitInputCallback cb) {
    submit_input_cb.store(cb, std::memory_order_release);
}

} // namespace scene::game

// --- C API 実装（cgo から呼ばれる想定） ---

extern "C" void RenderSnapshot(const CPSnapshot *snap) {
    if (snap == nullptr) return;
    scene::game::PushSnapshot(*snap);
}

extern "C" void RegisterSubmitInputCallback(SubmitInputCallback cb) {
    scene::game::SetSubmitInputCallback(cb);
    // TODO : implement me (Go 側からの切断時の扱い)
}
