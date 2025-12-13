#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- C互換のスナップショット構造体群 ---

typedef struct CPPoint {
    int x;
    int y;
} CPPoint;

typedef struct CPPiece {
    uint64_t id;
    int hp;
    int mp;
    int atk;
    uint8_t owner;
    uint8_t flags; // 移動済み / スタンなどのビットフラグを想定
} CPPiece;

typedef struct CPSnapshot {
    int width;
    int height;
    int turn;
    uint8_t active_player;
    // 配列長は width * height を期待。nullptr の場合は空扱い。
    const CPPiece *pieces;
    const uint64_t *structures;
    const char *log_text; // UTF-8 文字列。nullptr 可。
} CPSnapshot;

// --- C互換の入力イベント ---

typedef enum CPInputEventType {
    CP_INPUT_CLICK_TILE = 0,
    CP_INPUT_DRAG = 1,
    CP_INPUT_PLAY_CARD = 2,
    CP_INPUT_END_TURN = 3,
    CP_INPUT_UNKNOWN = 255,
} CPInputEventType;

typedef struct CPInputEvent {
    CPInputEventType type;
    CPPoint from;   // クリック位置 or ドラッグ開始
    CPPoint to;     // ドラッグ先やターゲット座標。未使用なら (0,0)
    uint64_t card_id;
    uint64_t target_id;
} CPInputEvent;

// Go -> C++ : Snapshot を受け取るエントリポイント
void RenderSnapshot(const CPSnapshot *snap);

// C++ -> Go : 入力送信用コールバックを登録（Go 側がセットする想定）
typedef void (*SubmitInputCallback)(const CPInputEvent *ev);
void RegisterSubmitInputCallback(SubmitInputCallback cb);

#ifdef __cplusplus
}
#endif
