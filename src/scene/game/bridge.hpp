#pragma once

#include <optional>

#include "c_api.hpp"
#include "snapshot.hpp"

namespace scene::game {

// Go 側からの CPSnapshot を受信して内部キューへ積む
void PushSnapshot(const CPSnapshot &src);

// レンダラが最新のスナップショットを取得する（古いものは破棄）
std::optional<Snapshot> ConsumeLatestSnapshot();

// 入力イベントを Go へ転送（未登録ならローカルキューに退避）
bool EmitInputEvent(const CPInputEvent &ev);

// ローカルに溜まった入力（Go 未接続時のフォールバック）を取得
std::optional<CPInputEvent> PollQueuedInput();

// テストや初期化向けのクリーンアップ
void ClearQueues();

// Go 側コールバックの setter（C API を経由しない場合向け）
void SetSubmitInputCallback(SubmitInputCallback cb);

} // namespace scene::game
