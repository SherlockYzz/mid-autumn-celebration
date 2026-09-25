#pragma once

#include "../../core/color/color.hpp"

namespace festival::themes::mid_autumn {

using namespace festival::core;

struct Palette {
    // Night sky
    static constexpr Color NightSky       = Color::fromHex(0x0A1020);
    static constexpr Color NightDeep      = Color::fromHex(0x050810);
    static constexpr Color NightWater     = Color::fromHex(0x0D172A);
    static constexpr Color Stars          = Color::fromHex(0xE0E8F0);

    // Moon & Radiance
    static constexpr Color MoonWhite      = Color::fromHex(0xF5F0E1);
    static constexpr Color MoonSoft       = Color::fromHex(0xEAE2CE);
    static constexpr Color MoonShade      = Color::fromHex(0xD4C8AF);
    static constexpr Color MoonGlow       = Color::fromHex(0xD4B87A);
    static constexpr Color MoonGlowDim    = Color::fromHex(0x604E2C);

    // Osmanthus
    static constexpr Color OsmanthusGold  = Color::fromHex(0xE6C878);
    static constexpr Color OsmanthusLight = Color::fromHex(0xF5E09E);
    static constexpr Color OsmanthusDark  = Color::fromHex(0xB89A4E);

    // Lanterns
    static constexpr Color LanternRed     = Color::fromHex(0xC23A2B);
    static constexpr Color LanternDarkRed = Color::fromHex(0x781F16);
    static constexpr Color LanternLight   = Color::fromHex(0xFFD080);
    static constexpr Color LanternTassel  = Color::fromHex(0xD4AF37);

    // Poetry & Seal
    static constexpr Color PoetryText     = Color::fromHex(0xF0EBE0);
    static constexpr Color PoetrySub      = Color::fromHex(0xA8A090);
    static constexpr Color SealRed        = Color::fromHex(0xB22222);

    // Clouds & Landscape
    static constexpr Color CloudBody      = Color::fromHex(0x1E2B3E);
    static constexpr Color CloudHighlight = Color::fromHex(0x3B506B);
    static constexpr Color Mountain       = Color::fromHex(0x0E1A29);
    static constexpr Color WaterRipple    = Color::fromHex(0x1A2A42);

    // Celebration Firework
    static constexpr Color FireworkGold   = Color::fromHex(0xFFE28A);
    static constexpr Color FireworkWarm   = Color::fromHex(0xF4A261);
};

} // namespace festival::themes::mid_autumn
