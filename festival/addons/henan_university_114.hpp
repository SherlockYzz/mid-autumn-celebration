#pragma once

#include "../themes/mid_autumn/mid_autumn_palette.hpp"
#include "../core/renderer/renderer.hpp"

namespace festival::addons {

/**
 * 河南大学 114 周年校庆专属彩蛋 (1912 - 2026)
 *
 * 特性：
 * 1. 形式：小小的一方雅致朱印与校训，静置角落，绝不喧宾夺主。
 * 2. 独立：完全独立于 Core 与主业务模块，自包含。
 * 3. 随时可删：若删除本文件，项目通过 __has_include 自动解除关联，正常编译。
 */
class HenanUniversityAnniversary {
public:
    static void render(core::Renderer& renderer) {
        int w = renderer.width();
        int h = renderer.height();

        // 放置于右下方作为一枚精致文雅的传统金石压角印落款（避开右上方的明月与底部控制栏）
        int sealX = w - 22;
        int sealY = h - 6;

        if (sealX < 45 || h < 18) return; // 窗口过小时自动隐去，确保留白

        core::Color sealBg     = themes::mid_autumn::Palette::SealRed;
        core::Color sealBorder = themes::mid_autumn::Palette::LanternRed;
        core::Color sealFg     = themes::mid_autumn::Palette::MoonWhite;
        core::Color mottoCol   = themes::mid_autumn::Palette::FireworkGold.withOpacity(0.9f);
        core::Color blessCol   = themes::mid_autumn::Palette::LanternLight.withOpacity(0.95f);

        // 小小的一方朱印：[ 河大114周年快乐 ]
        renderer.drawText(sealX, sealY,     "╭──────────────────╮", sealBorder);
        renderer.drawText(sealX, sealY + 1, "│  河大114周年快乐  │", sealFg, sealBg, true);
        renderer.drawText(sealX, sealY + 2, "╰──────────────────╯", sealBorder);

        // 典雅微字校训：「明德新民 止于至善」
        renderer.drawText(sealX + 1, sealY + 3, "明德新民  止于至善", mottoCol);

        // 校歌颂语与双节同庆：「猗欤吾校永无疆！祝全体同学双节快乐！」
        int blessX = std::max(2, w - 38);
        renderer.drawText(blessX, sealY + 4, "猗欤吾校永无疆！祝全体同学双节快乐！", blessCol);
    }
};

} // namespace festival::addons
