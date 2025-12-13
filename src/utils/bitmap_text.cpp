#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include "bitmap_text.hpp"

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

namespace utils {

namespace {

std::vector<unsigned char> ReadFile(const std::string &path) {
    std::ifstream ifs(path, std::ios::binary);
    return std::vector<unsigned char>((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
}

// ごく簡単な UTF-8 -> codepoint 変換。無効なバイトは '?' に置換。
std::vector<uint32_t> Utf8ToCodepoints(const std::string &s) {
    std::vector<uint32_t> cps;
    for (size_t i = 0; i < s.size();) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) {
            cps.push_back(c);
            ++i;
        } else if ((c >> 5) == 0x6 && i + 1 < s.size()) {
            uint32_t cp = ((c & 0x1F) << 6) | (static_cast<unsigned char>(s[i + 1]) & 0x3F);
            cps.push_back(cp);
            i += 2;
        } else if ((c >> 4) == 0xE && i + 2 < s.size()) {
            uint32_t cp = ((c & 0x0F) << 12) |
                          ((static_cast<unsigned char>(s[i + 1]) & 0x3F) << 6) |
                          (static_cast<unsigned char>(s[i + 2]) & 0x3F);
            cps.push_back(cp);
            i += 3;
        } else if ((c >> 3) == 0x1E && i + 3 < s.size()) {
            uint32_t cp = ((c & 0x07) << 18) |
                          ((static_cast<unsigned char>(s[i + 1]) & 0x3F) << 12) |
                          ((static_cast<unsigned char>(s[i + 2]) & 0x3F) << 6) |
                          (static_cast<unsigned char>(s[i + 3]) & 0x3F);
            cps.push_back(cp);
            i += 4;
        } else {
            cps.push_back(static_cast<uint32_t>('?'));
            ++i;
        }
    }
    return cps;
}

} // namespace

bool RenderTextBitmap(const std::string &font_path, const std::string &text, float pixel_height,
                      Bitmap &out, int padding, std::string *err) {
    out = Bitmap{};
    auto font_data = ReadFile(font_path);
    if (font_data.empty()) {
        if (err) *err = "failed to read font file";
        return false;
    }

    stbtt_fontinfo font{};
    if (!stbtt_InitFont(&font, font_data.data(), stbtt_GetFontOffsetForIndex(font_data.data(), 0))) {
        if (err) *err = "stbtt_InitFont failed";
        return false;
    }

    const float scale = stbtt_ScaleForPixelHeight(&font, pixel_height);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    const int baseline = static_cast<int>(ascent * scale + 0.5f);

    auto cps = Utf8ToCodepoints(text);

    // 幅計測
    int total_width = padding * 2;
    for (size_t i = 0; i < cps.size(); ++i) {
        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font, static_cast<int>(cps[i]), &advance, &lsb);
        total_width += static_cast<int>(advance * scale);
        if (i + 1 < cps.size()) {
            total_width += static_cast<int>(stbtt_GetCodepointKernAdvance(&font, static_cast<int>(cps[i]),
                                                                          static_cast<int>(cps[i + 1])) * scale);
        }
    }
    total_width += padding;

    const int total_height = static_cast<int>((ascent - descent + lineGap) * scale) + padding * 2;

    if (total_width <= 0 || total_height <= 0) {
        if (err) *err = "text results in empty bitmap";
        return false;
    }

    out.width = total_width;
    out.height = total_height;
    out.pixels.assign(static_cast<size_t>(out.width * out.height), 0);

    int x = padding;
    for (size_t i = 0; i < cps.size(); ++i) {
        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font, static_cast<int>(cps[i]), &advance, &lsb);

        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBoxSubpixel(&font, static_cast<int>(cps[i]), scale, scale, 0.0f, 0.0f,
                                            &x0, &y0, &x1, &y1);

        const int gw = x1 - x0;
        const int gh = y1 - y0;
        std::vector<unsigned char> glyph(static_cast<size_t>(gw * gh));

        stbtt_MakeCodepointBitmapSubpixel(&font, glyph.data(), gw, gh, gw, scale, scale, 0.0f, 0.0f,
                                          static_cast<int>(cps[i]));

        const int y_off = baseline + y0 + padding;
        for (int gy = 0; gy < gh; ++gy) {
            int dy = y_off + gy;
            if (dy < 0 || dy >= out.height) continue;
            for (int gx = 0; gx < gw; ++gx) {
                int dx = x + x0 + gx;
                if (dx < 0 || dx >= out.width) continue;
                out.pixels[static_cast<size_t>(dy * out.width + dx)] = glyph[static_cast<size_t>(gy * gw + gx)];
            }
        }

        x += static_cast<int>(advance * scale);
        if (i + 1 < cps.size()) {
            x += static_cast<int>(stbtt_GetCodepointKernAdvance(&font, static_cast<int>(cps[i]),
                                                                static_cast<int>(cps[i + 1])) * scale);
        }
    }

    return true;
}

} // namespace utils
