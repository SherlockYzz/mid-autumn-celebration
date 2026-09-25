#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

namespace festival::core {

// Decode next UTF-8 codepoint from s starting at index pos.
// Returns {codepoint, bytesConsumed}.
inline std::pair<char32_t, size_t> decodeNextUtf8(std::string_view s, size_t pos) {
    if (pos >= s.size()) {
        return {0, 0};
    }

    uint8_t c = static_cast<uint8_t>(s[pos]);
    if (c < 0x80) {
        return {static_cast<char32_t>(c), 1};
    } else if ((c & 0xE0) == 0xC0) {
        if (pos + 1 < s.size()) {
            char32_t cp = ((c & 0x1F) << 6) | (static_cast<uint8_t>(s[pos + 1]) & 0x3F);
            return {cp, 2};
        }
    } else if ((c & 0xF0) == 0xE0) {
        if (pos + 2 < s.size()) {
            char32_t cp = ((c & 0x0F) << 12) |
                          ((static_cast<uint8_t>(s[pos + 1]) & 0x3F) << 6) |
                          (static_cast<uint8_t>(s[pos + 2]) & 0x3F);
            return {cp, 3};
        }
    } else if ((c & 0xF8) == 0xF0) {
        if (pos + 3 < s.size()) {
            char32_t cp = ((c & 0x07) << 18) |
                          ((static_cast<uint8_t>(s[pos + 1]) & 0x3F) << 12) |
                          ((static_cast<uint8_t>(s[pos + 2]) & 0x3F) << 6) |
                          (static_cast<uint8_t>(s[pos + 3]) & 0x3F);
            return {cp, 4};
        }
    }

    // Fallback on invalid sequence
    return {'?', 1};
}

// Compute terminal column display width for a Unicode codepoint.
// Returns 2 for CJK / Fullwidth characters, 0 for zero-width / control, 1 for normal characters.
inline int codepointVisualWidth(char32_t cp) {
    if (cp == 0) return 0;
    if (cp < 0x20 || (cp >= 0x7F && cp < 0xA0)) return 0;

    // Combining diacritical marks & zero-width characters
    if ((cp >= 0x0300 && cp <= 0x036F) ||
        (cp >= 0x200B && cp <= 0x200F) ||
        (cp >= 0xFE00 && cp <= 0xFE0F)) {
        return 0;
    }

    // East Asian Wide / Fullwidth ranges
    if ((cp >= 0x1100 && cp <= 0x115F) || // Hangul Jamo
        (cp >= 0x2E80 && cp <= 0xA4CF && cp != 0x303F) || // CJK Radicals, Kangxi, CJK Unified Ideographs, Yi
        (cp >= 0xAC00 && cp <= 0xD7A3) || // Hangul Syllables
        (cp >= 0xF900 && cp <= 0xFAFF) || // CJK Compatibility Ideographs
        (cp >= 0xFE10 && cp <= 0xFE19) || // Vertical forms
        (cp >= 0xFE30 && cp <= 0xFE6F) || // CJK compatibility forms
        (cp >= 0xFF00 && cp <= 0xFF60) || // Fullwidth ASCII variants
        (cp >= 0xFFE0 && cp <= 0xFFE6) || // Fullwidth symbol variants
        (cp >= 0x1F000 && cp <= 0x1FAFF) || // Pictographic and emoji symbols
        (cp >= 0x20000 && cp <= 0x2FA1F) || // CJK Extension B/C/D/E/F
        (cp >= 0x30000 && cp <= 0x3134F)) { // CJK Extension G
        return 2;
    }

    return 1;
}

// Visual width of a UTF-8 string when rendered in a monospace terminal
inline int stringVisualWidth(std::string_view s) {
    int totalWidth = 0;
    size_t pos = 0;
    while (pos < s.size()) {
        auto [cp, len] = decodeNextUtf8(s, pos);
        if (len == 0) break;
        totalWidth += codepointVisualWidth(cp);
        pos += len;
    }
    return totalWidth;
}

struct Glyph {
    std::string text;
    int visualWidth = 1;
};

// Split UTF-8 string into individual glyphs with calculated visual widths
inline std::vector<Glyph> splitGlyphs(std::string_view s) {
    std::vector<Glyph> result;
    size_t pos = 0;
    while (pos < s.size()) {
        auto [cp, len] = decodeNextUtf8(s, pos);
        if (len == 0) break;
        int w = codepointVisualWidth(cp);
        result.push_back({std::string(s.substr(pos, len)), w});
        pos += len;
    }
    return result;
}

} // namespace festival::core
