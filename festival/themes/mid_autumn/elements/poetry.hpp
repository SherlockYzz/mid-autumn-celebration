#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/text/text_layout.hpp"
#include "../../../core/animation/tween.hpp"
#include <vector>
#include <string>

namespace festival::themes::mid_autumn {

struct Verse {
    std::string line1;
    std::string line2;
    std::string author;
    std::string seal;
};

class PoetryDisplay {
public:
    PoetryDisplay() {
        m_verses = {
            {
                "海上生明月",
                "天涯共此时",
                "唐 · 张九龄 《望月怀远》",
                "曲江风度"
            },
            {
                "但愿人长久",
                "千里共婵娟",
                "宋 · 苏轼 《水调歌头》",
                "东坡居士"
            },
            {
                "中庭地白树栖鸦",
                "冷露无声湿桂花",
                "唐 · 王建 《十五夜望月》",
                "秋思谁家"
            },
            {
                "露从今夜白",
                "月是故乡明",
                "唐 · 杜甫 《月夜忆舍弟》",
                "少陵野老"
            },
            {
                "举杯邀明月",
                "对影成三人",
                "唐 · 李白 《月下独酌》",
                "青莲居士"
            },
            {
                "玉颗珊珊下月轮",
                "殿前拾得露华新",
                "唐 · 宋之问 《天竺寺夜》",
                "蟾宫折桂"
            },
            {
                "可怜今夕月",
                "向何处去悠悠",
                "宋 · 辛弃疾 《木兰花慢》",
                "稼轩长短句"
            }
        };

        m_fadeTween = Tween<float>(0.0f, 1.0f, 2.0f, EasingType::EaseInOut);
    }

    void initialize(int screenWidth, int screenHeight) {
        m_screenWidth = screenWidth;
        m_screenHeight = screenHeight;
        m_typewriter1.setSpeed(6.0f);
        m_typewriter2.setSpeed(6.0f);
        setVerse(0);
    }

    void startDisplay() {
        m_active = true;
        m_fadingOut = false;
        m_fadeTween = Tween<float>(0.0f, 1.0f, 1.8f, EasingType::EaseInOut);
        m_typewriter1.reset();
        m_typewriter2.reset();
    }

    void nextVerse() {
        m_currentIndex = (m_currentIndex + 1) % m_verses.size();
        setVerse(m_currentIndex);
        startDisplay();
    }

    void update(float dt) {
        if (!m_active) return;

        m_fadeTween.update(dt);
        m_typewriter1.update(dt);

        if (m_typewriter1.isComplete()) {
            m_typewriter2.update(dt);
        }
    }

    void render(Renderer& renderer) {
        if (!m_active) return;

        float alpha = m_fadeTween.getValue();
        if (alpha <= 0.02f) return;

        const auto& v = m_verses[m_currentIndex];

        // Position: elegant composition on the upper-left keeping negative space
        int startX = std::max(12, static_cast<int>(m_screenWidth * 0.14f));
        int startY = std::max(3, static_cast<int>(m_screenHeight * 0.12f));

        Color mainText = Palette::PoetryText.withOpacity(alpha);
        Color subText = Palette::PoetrySub.withOpacity(alpha * 0.85f);
        Color sealBg = Palette::SealRed.withOpacity(alpha * 0.95f);
        Color sealFg = Palette::MoonWhite.withOpacity(alpha);

        // Verse Line 1
        m_typewriter1.render(renderer, startX, startY, mainText);

        // Verse Line 2
        m_typewriter2.render(renderer, startX, startY + 1, mainText);

        // Author note & Inscription Seal (displayed side by side on row startY + 2)
        if (m_typewriter2.isComplete()) {
            renderer.drawText(startX + 2, startY + 2, v.author, subText);

            // Red Seal / Inscription Stamp [ 曲江风度 ]
            int authorW = stringVisualWidth(v.author);
            int sealX = startX + 2 + authorW + 2;
            int sealY = startY + 2;
            std::string sealStr = " " + v.seal + " ";
            renderer.fillRect(sealX, sealY, stringVisualWidth(sealStr), 1, sealBg, " ", sealFg);
            renderer.drawText(sealX, sealY, sealStr, sealFg, sealBg, true);
        }
    }

private:
    void setVerse(size_t index) {
        m_currentIndex = index % m_verses.size();
        const auto& v = m_verses[m_currentIndex];
        m_typewriter1.setText(v.line1);
        m_typewriter2.setText(v.line2);
    }

    std::vector<Verse> m_verses;
    size_t m_currentIndex = 0;
    int m_screenWidth = 80;
    int m_screenHeight = 24;

    bool m_active = false;
    bool m_fadingOut = false;
    Tween<float> m_fadeTween;
    TypewriterText m_typewriter1;
    TypewriterText m_typewriter2;
};

} // namespace festival::themes::mid_autumn
