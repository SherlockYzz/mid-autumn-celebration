#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/platform/input_event.hpp"
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>

namespace festival::themes::mid_autumn {

enum class DivinationEffect {
    None,
    MoonlightWave,
    OsmanthusFlurry,
    FireworksCelebration,
    JadeRabbitJoy,
    SkyLanternAscent
};

struct FortuneLot {
    std::string tier;
    std::string title;
    std::string poem;
    std::string interpretation;
    std::string blessing;
    DivinationEffect effect;
};

class MoonDivination {
public:
    MoonDivination() {
        m_lots = {
            {
                "【 上 上 签 】",
                "月 华 如 愿",
                "长空万里飞霜练，所愿皆随月影圆。",
                "象曰：天心月圆，祥云护佑，心中所念必有回响。",
                "月华如瀑 · 满月清辉普照天地",
                DivinationEffect::MoonlightWave
            },
            {
                "【 特 吉 签 】",
                "蟾 宫 折 桂",
                "秋风吹落丹桂子，步上青云第一枝。",
                "象曰：文星高照，才思泉涌，学业事业大吉大利。",
                "仙桂飘香 · 金花漫天纷飞如雨",
                DivinationEffect::OsmanthusFlurry
            },
            {
                "【 上 吉 签 】",
                "万 家 灯 火",
                "火树银花元夕夕，星桥铁锁向月开。",
                "象曰：灯火可亲，家室团聚，人间喜乐岁岁平安。",
                "盛世欢歌 · 连珠金火映彻九霄",
                DivinationEffect::FireworksCelebration
            },
            {
                "【 安 康 签 】",
                "岁 岁 常 宁",
                "山中何事无拘束，松花酿酒春水煎。",
                "象曰：身轻体健，岁岁无虞，清吉安康最是难得。",
                "仙芝益寿 · 灵兔捣药洒降祥雾",
                DivinationEffect::JadeRabbitJoy
            },
            {
                "【 如 意 签 】",
                "千 里 婵 娟",
                "但愿人长久，千里共婵娟。",
                "象曰：同舟共济，天涯知己，遥寄相思终得眷顾。",
                "乘风寄语 · 祈愿天灯徐徐升空",
                DivinationEffect::SkyLanternAscent
            },
            {
                "【 吉 庆 签 】",
                "灵 兔 纳 福",
                "白兔捣药秋复春，玉杵金杯迎嘉宾。",
                "象曰：仙童送吉，福泽延绵，常有贵人暗中相助。",
                "灵气充盈 · 瑞兽欢歌福至心灵",
                DivinationEffect::JadeRabbitJoy
            }
        };
    }

    bool isActive() const { return m_active; }

    DivinationEffect open() {
        m_active = true;
        m_shakeTimer = 0.5f;
        m_currentIndex = rand() % m_lots.size();
        return m_lots[m_currentIndex].effect;
    }

    void close() {
        m_active = false;
    }

    DivinationEffect drawNext() {
        m_shakeTimer = 0.45f;
        m_currentIndex = (m_currentIndex + 1 + (rand() % (m_lots.size() - 1))) % m_lots.size();
        return m_lots[m_currentIndex].effect;
    }

    void update(float dt) {
        if (m_shakeTimer > 0.0f) {
            m_shakeTimer -= dt;
        }
    }

    bool handleInput(const core::InputEvent& event, DivinationEffect& outEffect) {
        if (!m_active) return false;

        outEffect = DivinationEffect::None;

        if (event.code == core::KeyCode::Escape) {
            close();
            return true;
        }

        if (event.code == core::KeyCode::Space || event.code == core::KeyCode::Enter || event.isChar('j')) {
            outEffect = drawNext();
            return true;
        }

        return false;
    }

    void render(Renderer& renderer) {
        if (!m_active) return;

        int w = std::min(62, renderer.width() - 4);
        int h = 11;
        int x = (renderer.width() - w) / 2;
        int y = (renderer.height() - h) / 2;

        Color cardBg    = Palette::NightWater;
        Color borderCol = Palette::FireworkGold;
        Color titleCol  = Palette::LanternLight;
        Color poemCol   = Palette::PoetryText;
        Color tierCol   = Palette::SealRed;
        Color descCol   = Palette::PoetrySub;
        Color buffCol   = Palette::OsmanthusGold;

        // Card frame
        renderer.fillRect(x, y, w, h, cardBg);
        renderer.drawBorder(x, y, w, h, borderCol);

        // Header Title
        std::string header = " ❖ 🏮【宵月会 · 追月灵签 · 卜问月】🏮 ❖ ";
        renderer.drawCenterText(y, header, titleCol, cardBg, true);

        if (m_shakeTimer > 0.0f) {
            // Shaking animation
            std::string shaking = "✦ 🎋 签筒摇动 · 灵光内蕴 · 金签跃出中 ··· ✦";
            renderer.drawCenterText(y + 5, shaking, Palette::FireworkGold, cardBg, true);
        } else {
            const auto& lot = m_lots[m_currentIndex];

            // Tier & Lot Name
            std::string tierLine = lot.tier + "   " + lot.title;
            renderer.drawCenterText(y + 2, tierLine, Palette::LanternLight, cardBg, true);

            // Poem
            std::string poemLine = "“ " + lot.poem + " ”";
            renderer.drawCenterText(y + 4, poemLine, poemCol, cardBg, true);

            // Interpretation
            renderer.drawCenterText(y + 6, lot.interpretation, descCol, cardBg);

            // Blessing Buff
            std::string buffLine = "✨ 宵月赐福：" + lot.blessing;
            renderer.drawCenterText(y + 8, buffLine, buffCol, cardBg, true);
        }

        // Footer Hint
        std::string footer = " [SPACE] 摇取新签  │  [ESC] 收起灵签并领受赐福 ";
        renderer.drawCenterText(y + h - 1, footer, descCol, cardBg);
    }

private:
    std::vector<FortuneLot> m_lots;
    size_t m_currentIndex = 0;
    bool m_active = false;
    float m_shakeTimer = 0.0f;
};

} // namespace festival::themes::mid_autumn
