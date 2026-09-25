#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/text/unicode_width.hpp"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

namespace festival::themes::mid_autumn {

/**
 * 中央核心视觉中心：【祝】【大家】【中秋快乐】
 *
 * 特性：
 * 1. 严格居于屏幕正中央 (X, Y 均居中对齐)，成为全画面的第一视觉焦点。
 * 2. “祝” 与 “中秋快乐” 永久固定；受福对象（默认“大家”）支持通过 N 键动态修改。
 * 3. 采用纯正原生中文排版，杜绝 ASCII 像素字体导致的乱码与字符断裂问题。
 * 4. 融合东方金石御匾边框、朱砂红印与流动月华光泽。
 */
class CentralBlessing {
public:
    CentralBlessing() : m_targetName("大家") {}

    const std::string& targetName() const { return m_targetName; }
    void setTargetName(const std::string& name) { m_targetName = name.empty() ? "大家" : name; }

    std::string fullBlessing() const {
        return "祝【" + m_targetName + "】中秋快乐";
    }

    void triggerHighlight(float duration = 3.5f) {
        m_highlightTimer = duration;
    }

    void update(float dt) {
        m_time += dt;
        if (m_highlightTimer > 0.0f) {
            m_highlightTimer = std::max(0.0f, m_highlightTimer - dt);
        }
    }

    void render(Renderer& renderer, int screenWidth, int screenHeight) {
        // 核心文字组成
        std::string zhuText = "祝";
        std::string leftBracket = "【";
        std::string rightBracket = "】";
        std::string festivalText = "中秋快乐";

        // 计算总宽度
        // "✦ · " (4) + "祝 " (3) + "【" (2) + targetName + "】" (2) + " " (1) + "中秋快乐" (8) + " · ✦" (4)
        std::string fullContent = "✦ · 祝 【" + m_targetName + "】 " + festivalText + " · ✦";
        int contentW = core::stringVisualWidth(fullContent);

        // 匾额尺寸（保证气派端庄）
        int plaqueW = std::max(38, contentW + 8);
        int plaqueH = 3;

        // 严格放置于屏幕正中心
        int px = (screenWidth - plaqueW) / 2;
        int py = (screenHeight / 2) - 2;

        if (py < 7) py = 7; // 保留顶部月亮与诗句空间

        float pulse = 0.85f + 0.15f * std::sin(m_time * 2.2f);
        float hlRatio = std::clamp(m_highlightTimer / 1.5f, 0.0f, 1.0f);

        Color goldBorder = Palette::OsmanthusGold.withOpacity(pulse);
        Color ribbonCol  = Palette::FireworkGold;
        Color bgShadow   = Palette::NightDeep;

        // 1. 匾额背衬底色（纯黑蓝微晕，绝无大面积蓝色色块）
        renderer.fillRect(px, py, plaqueW, plaqueH, bgShadow, " ", goldBorder);

        // 2. 雕花宫廷金框：顶边融入「❖ 岁次丙午 · 月满中秋 ❖」吉祥云额
        std::string ribbon = " ❖ 岁次丙午 · 月满中秋 ❖ ";
        int ribW = core::stringVisualWidth(ribbon);
        int sidePad = std::max(1, (plaqueW - 2 - ribW) / 2);

        std::string topBorder = "╭";
        for (int i = 0; i < sidePad; ++i) topBorder += "─";
        topBorder += ribbon;
        while (core::stringVisualWidth(topBorder) < plaqueW - 1) topBorder += "─";
        topBorder += "╮";

        std::string botBorder = "╰";
        for (int i = 0; i < plaqueW - 2; ++i) botBorder += "─";
        botBorder += "╯";

        renderer.drawText(px, py + 0, topBorder, goldBorder);
        renderer.drawChar(px, py + 1, "│", goldBorder);
        renderer.drawChar(px + plaqueW - 1, py + 1, "│", goldBorder);
        renderer.drawText(px, py + 2, botBorder, goldBorder);

        // 3. 匾心文字精工居中绘制（字色冷暖对比：朱红括号 + 璨金称谓 + 月白祝词）
        int textX = px + (plaqueW - contentW) / 2;
        int textY = py + 1;

        // "✦ · "
        renderer.drawText(textX, textY, "✦ · ", Palette::FireworkGold.withOpacity(pulse));
        textX += 4;

        // "祝 "
        renderer.drawText(textX, textY, "祝 ", Palette::MoonWhite, bgShadow, true);
        textX += 3;

        // "【" (朱砂红)
        renderer.drawText(textX, textY, "【", Palette::SealRed, bgShadow, true);
        textX += 2;

        // 受福之人称呼 (璀璨赤金，若是新题名则高光流光闪烁)
        Color nameCol = (hlRatio > 0.05f) ?
            Color::fromHex(0xFFFFFF) :
            Color::fromHex(0xFFD269);
        renderer.drawText(textX, textY, m_targetName, nameCol, bgShadow, true);
        textX += core::stringVisualWidth(m_targetName);

        // "】" (朱砂红)
        renderer.drawText(textX, textY, "】", Palette::SealRed, bgShadow, true);
        textX += 2;

        // " "
        textX += 1;

        // "中秋快乐" (温润琥珀汉白)
        renderer.drawText(textX, textY, festivalText, Palette::FireworkGold, bgShadow, true);
        textX += 8;

        // " · ✦"
        renderer.drawText(textX, textY, " · ✦", Palette::FireworkGold.withOpacity(pulse));

        // 4. 匾额下方垂悬如意微光
        int knotX = px + (plaqueW - 9) / 2;
        renderer.drawText(knotX, py + 3, "· ✧ ❖ ✧ ·", Palette::OsmanthusDark.withOpacity(0.75f));

        // 5. 当处于高亮/题名落款状态时，匾额上下飘散微小金辉
        if (hlRatio > 0.05f) {
            float shineAlpha = hlRatio * 0.9f;
            renderer.drawChar(px - 1, py + 1, "✧", Palette::FireworkGold.withOpacity(shineAlpha));
            renderer.drawChar(px + plaqueW, py + 1, "✧", Palette::FireworkGold.withOpacity(shineAlpha));
        }
    }

private:
    std::string m_targetName;
    float m_time = 0.0f;
    float m_highlightTimer = 0.0f;
};

} // namespace festival::themes::mid_autumn
