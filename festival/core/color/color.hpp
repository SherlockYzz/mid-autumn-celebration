#pragma once

#include <cstdint>
#include <string>
#include <algorithm>
#include <cmath>

namespace festival::core {

enum class ColorMode {
    TrueColor,  // 24-bit RGB (16.7M colors)
    Ansi256,    // 8-bit palette (256 colors)
    Ansi16,     // 4-bit standard & bright ANSI (16 colors)
    Monochrome  // Plain text without color codes
};

struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;

    constexpr Color() = default;
    constexpr Color(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inA = 255)
        : r(inR), g(inG), b(inB), a(inA) {}

    static constexpr Color fromHex(uint32_t hex, uint8_t inA = 255) {
        return Color(
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF),
            inA
        );
    }

    constexpr bool operator==(const Color& o) const {
        return r == o.r && g == o.g && b == o.b && a == o.a;
    }

    constexpr bool operator!=(const Color& o) const {
        return !(*this == o);
    }

    Color withAlpha(uint8_t newAlpha) const {
        return Color(r, g, b, newAlpha);
    }

    Color withOpacity(float opacity) const {
        float clamped = std::clamp(opacity, 0.0f, 1.0f);
        return Color(r, g, b, static_cast<uint8_t>(a * clamped));
    }

    Color scaled(float factor) const {
        float f = std::max(0.0f, factor);
        return Color(
            static_cast<uint8_t>(std::clamp(r * f, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(g * f, 0.0f, 255.0f)),
            static_cast<uint8_t>(std::clamp(b * f, 0.0f, 255.0f)),
            a
        );
    }

    static Color lerp(const Color& c1, const Color& c2, float t) {
        float ct = std::clamp(t, 0.0f, 1.0f);
        return Color(
            static_cast<uint8_t>(c1.r + (c2.r - c1.r) * ct),
            static_cast<uint8_t>(c1.g + (c2.g - c1.g) * ct),
            static_cast<uint8_t>(c1.b + (c2.b - c1.b) * ct),
            static_cast<uint8_t>(c1.a + (c2.a - c1.a) * ct)
        );
    }

    static Color blend(const Color& bg, const Color& fg, float alpha) {
        float aNorm = std::clamp(alpha * (fg.a / 255.0f), 0.0f, 1.0f);
        return Color(
            static_cast<uint8_t>(bg.r * (1.0f - aNorm) + fg.r * aNorm),
            static_cast<uint8_t>(bg.g * (1.0f - aNorm) + fg.g * aNorm),
            static_cast<uint8_t>(bg.b * (1.0f - aNorm) + fg.b * aNorm),
            255
        );
    }

    // Convert to xterm 256-color index
    uint8_t to256Index() const {
        // Check for grayscale
        if (r == g && g == b) {
            if (r < 8) return 16;
            if (r > 248) return 231;
            return static_cast<uint8_t>(232 + std::round((r - 8) / 247.0f * 23.0f));
        }

        // 6x6x6 color cube
        int rIndex = (r * 5 + 127) / 255;
        int gIndex = (g * 5 + 127) / 255;
        int bIndex = (b * 5 + 127) / 255;
        return static_cast<uint8_t>(16 + 36 * rIndex + 6 * gIndex + bIndex);
    }

    // Convert to 16 ANSI color code (30-37 or 90-97 for fg, 40-47 or 100-107 for bg)
    uint8_t to16Index() const {
        int rBit = (r > 127) ? 1 : 0;
        int gBit = (g > 127) ? 2 : 0;
        int bBit = (b > 127) ? 4 : 0;
        int bright = (r > 192 || g > 192 || b > 192) ? 8 : 0;

        int index = rBit | gBit | bBit;
        if (index == 0 && bright) return 8; // Bright black (dark gray)
        return static_cast<uint8_t>(index | bright);
    }

    std::string toAnsi(ColorMode mode, bool isBackground) const {
        if (mode == ColorMode::Monochrome || a == 0) {
            return "";
        }

        if (mode == ColorMode::TrueColor) {
            std::string code = isBackground ? "\033[48;2;" : "\033[38;2;";
            code += std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
            return code;
        }

        if (mode == ColorMode::Ansi256) {
            std::string code = isBackground ? "\033[48;5;" : "\033[38;5;";
            code += std::to_string(to256Index()) + "m";
            return code;
        }

        // Ansi16
        uint8_t idx = to16Index();
        int base = isBackground ? (idx >= 8 ? 100 : 40) : (idx >= 8 ? 90 : 30);
        int colorCode = base + (idx % 8);
        return "\033[" + std::to_string(colorCode) + "m";
    }
};

} // namespace festival::core
