#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include <string>
#include <algorithm>

namespace festival::themes::mid_autumn {

class FestivalGuideModal {
public:
    FestivalGuideModal() = default;

    bool isActive() const { return m_active; }

    void toggle() {
        m_active = !m_active;
    }

    void open() {
        m_active = true;
    }

    void close() {
        m_active = false;
    }

    void render(Renderer& renderer) {
        if (!m_active) return;

        int w = std::min(74, renderer.width() - 2);
        int h = std::min(22, renderer.height() - 2);
        int x = (renderer.width() - w) / 2;
        int y = (renderer.height() - h) / 2;

        Color bg        = Palette::NightWater;
        Color border    = Palette::FireworkGold;
        Color titleCol  = Palette::LanternLight;
        Color subCol    = Palette::PoetrySub;
        Color textCol   = Palette::PoetryText;
        Color keyCol    = Palette::OsmanthusGold;
        Color headCol   = Palette::LanternLight;

        renderer.fillRect(x, y, w, h, bg);
        renderer.drawBorder(x, y, w, h, border);

        // Title
        renderer.drawCenterText(y + 1, "🏮【宵月会游园锦囊 · 节庆全览】🏮", titleCol, bg, true);
        renderer.drawCenterText(y + 2, "✦ 月满中秋 · 游园踏月 · 祈福万家 · 岁岁常安 ✦", subCol, bg);

        // Section 1: 民俗游乐
        renderer.drawText(x + 3, y + 4, "【民俗游乐 · 宵月集会】", headCol, bg, true);
        renderer.drawText(x + 5, y + 5, "[T] 宵月猜灯谜 ── 挑灯对谜，考校才思，连对赢取游园豪礼与热度", textCol, bg);
        renderer.drawText(x + 5, y + 6, "[J] 追月求灵签 ── 摇筒卜月，求取中秋上上吉签与全景祥瑞Buff", textCol, bg);
        renderer.drawText(x + 5, y + 7, "[B] 玉兔品名饼 ── 烹献五方名饼，得玉兔咀嚼萌态、灵露与欢欣", textCol, bg);
        renderer.drawText(x + 5, y + 8, "[K] 赏灯筑阁   ── 阶梯点亮长街楼阁与水岸万家灯火(四级市井繁华)", textCol, bg);

        // Section 2: 中秋雅趣
        renderer.drawText(x + 3, y + 10, "【中秋雅趣 · 月华流光】", headCol, bg, true);
        renderer.drawText(x + 5, y + 11, "[N] 月下题名   ── 自定义祝福对象“祝【XXX】中秋快乐”与飘墨动画", textCol, bg);
        renderer.drawText(x + 5, y + 12, "[C] 复制祝福   ── 一键复制题名祝福语至系统剪贴板，赠予远方亲友", textCol, bg);
        renderer.drawText(x + 5, y + 13, "[W] 祈愿天灯   ── 亲撰心愿寄语，随宵月天灯破晓乘风而起", textCol, bg);
        renderer.drawText(x + 5, y + 14, "[R] 灵兔捣仙芝 ── 观玉兔起落捣药、晃耳摆首、臼中喷涌仙草星芒", textCol, bg);
        renderer.drawText(x + 5, y + 15, "[G] 摇落金桂   ── 摇动月桂古树，沐浴金色桂花香雨漫天纷飞", textCol, bg);
        renderer.drawText(x + 5, y + 16, "[P] 赏月吟诗   ── 轮换历代经典中秋绝句与朱砂落款印章", textCol, bg);
        renderer.drawText(x + 5, y + 17, "[L] 挑灯弄影   ── 切换朱红画舫灯笼明灭微光", textCol, bg);

        // Section 3: 庆典特效
        renderer.drawText(x + 3, y + 19, "【终极庆典】[ENTER]/[A] 盛典齐鸣(烟花+赏月+摇金桂+灵兔捣药+万家灯火全开)", keyCol, bg, true);

        // Footer
        renderer.drawCenterText(y + h - 1, " 按 [ESC] 或 [H] 收起游园锦囊，尽享中秋良夜 ", subCol, bg);
    }

private:
    bool m_active = false;
};

} // namespace festival::themes::mid_autumn
