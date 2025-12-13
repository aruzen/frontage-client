#pragma once

#include <string>
#include <vector>

namespace utils {

struct Bitmap {
    int width = 0;
    int height = 0;
    // 1byte 灰度。0=透明、255=白。
    std::vector<unsigned char> pixels;
};

// フォントファイルと UTF-8 テキストから 1ch (gray) のビットマップを生成。
// 成功で true。pixel_height は px 単位。
bool RenderTextBitmap(const std::string &font_path, const std::string &text, float pixel_height,
                      Bitmap &out, int padding = 2, std::string *err = nullptr);

} // namespace utils
