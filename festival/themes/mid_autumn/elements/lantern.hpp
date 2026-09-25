#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/animation/tween.hpp"
#include "../../../core/effects/glow.hpp"
#include <cmath>
#include <vector>

namespace festival::themes::mid_autumn {

struct SingleLantern {
    float anchorX = 0.0f;
    float anchorY = 0.0f;
    float stringLength = 4.0f;
    float phase = 0.0f;
    float swaySpeed = 1.2f;
    float maxSway = 1.4f;
    float lightPulseSpeed = 1.8f;
    bool lit = false;
    float lightIntensity = 0.0f;
    Tween<float> igniteTween{0.0f, 1.0f, 2.0f, EasingType::EaseInOut};
};

class LanternGroup {
public:
    LanternGroup() = default;

    void initialize(int screenWidth, int /*screenHeight*/) {
        m_lanterns.clear();

        // 3 gracefully arranged lanterns framing the scene perimeter (never blocking center or poetry)
        SingleLantern l1;
        l1.anchorX = std::max(3.0f, screenWidth * 0.05f);
        l1.anchorY = 1.0f;
        l1.stringLength = 3.5f;
        l1.phase = 0.0f;
        l1.swaySpeed = 1.1f;
        l1.maxSway = 1.2f;
        m_lanterns.push_back(l1);

        SingleLantern l2;
        l2.anchorX = screenWidth * 0.88f;
        l2.anchorY = 1.0f;
        l2.stringLength = 4.0f;
        l2.phase = 1.6f;
        l2.swaySpeed = 0.95f;
        l2.maxSway = 1.0f;
        m_lanterns.push_back(l2);

        SingleLantern l3;
        l3.anchorX = screenWidth * 0.94f;
        l3.anchorY = 2.5f;
        l3.stringLength = 3.0f;
        l3.phase = 2.8f;
        l3.swaySpeed = 1.25f;
        l3.maxSway = 1.0f;
        m_lanterns.push_back(l3);
    }

    void ignite() {
        m_active = true;
        for (auto& l : m_lanterns) {
            l.lit = true;
            l.lightIntensity = 1.0f;
            l.igniteTween = Tween<float>(1.0f, 1.0f, 0.1f, EasingType::Linear);
        }
    }

    void toggle() {
        m_active = !m_active;
        m_noticeTimer = 9.0f; // 维持 9 秒雅致通知
        m_noticeText = m_active ? "华灯初上 · 照夜未央 · 万家团聚" : "灯火渐隐 · 独对清辉 · 静待清欢";
        for (auto& l : m_lanterns) {
            l.lit = m_active;
            if (m_active) {
                l.igniteTween = Tween<float>(l.lightIntensity, 1.0f, 0.8f, EasingType::EaseInOut);
            } else {
                l.igniteTween = Tween<float>(l.lightIntensity, 0.0f, 0.8f, EasingType::EaseInOut);
            }
        }
    }

    void update(float dt) {
        m_time += dt;
        if (m_noticeTimer > 0.0f) {
            m_noticeTimer -= dt;
        }

        for (auto& l : m_lanterns) {
            l.igniteTween.update(dt);
            l.lightIntensity = l.igniteTween.getValue();
        }
    }

    void render(Renderer& renderer) {
        for (const auto& l : m_lanterns) {
            if (l.lightIntensity <= 0.01f && !m_active) continue;

            float sway = std::sin(m_time * l.swaySpeed + l.phase) * l.maxSway;
            float lanternX = l.anchorX + sway;
            float lanternY = l.anchorY + l.stringLength;

            int ix = static_cast<int>(std::round(lanternX));
            int iy = static_cast<int>(std::round(lanternY));

            // String from ceiling/branch to lantern
            Color cordCol = Palette::Mountain;
            for (int sy = static_cast<int>(l.anchorY); sy < iy - 1; ++sy) {
                float prog = (sy - l.anchorY) / (iy - 1 - l.anchorY + 0.001f);
                int sx = static_cast<int>(std::round(l.anchorX + sway * prog));
                renderer.drawChar(sx, sy, "|", cordCol);
            }

            // Warm candlelight breathing pulse
            float breath = 0.85f + 0.15f * std::sin(m_time * l.lightPulseSpeed + l.phase);
            float currentGlow = l.lightIntensity * breath;

            // Soft radial candlelight glow
            if (currentGlow > 0.05f) {
                Glow::renderRadialGlow(
                    renderer,
                    static_cast<float>(ix + 1), static_cast<float>(iy + 1),
                    1.5f, 6.0f * currentGlow,
                    Palette::LanternLight.withOpacity(currentGlow * 0.7f),
                    Palette::NightSky,
                    0.5f * currentGlow,
                    2.0f
                );
            }

            // Draw Cinnabar Red Lantern Body
            Color redBody = Palette::LanternRed.withOpacity(std::max(0.3f, l.lightIntensity));
            Color innerLight = Palette::LanternLight.scaled(currentGlow);
            Color darkFrame = Palette::LanternDarkRed;
            Color tasselCol = Palette::LanternTassel;

            // Cap
            renderer.drawText(ix - 1, iy - 1, "╭─^─╮", darkFrame);

            // Body & warm light interior with auspicious lattice
            renderer.drawChar(ix - 1, iy + 0, "│", redBody);
            renderer.drawText(ix + 0, iy + 0, "❖ ", innerLight, Palette::LanternDarkRed, true);
            renderer.drawChar(ix + 2, iy + 0, "│", redBody);

            renderer.drawChar(ix - 1, iy + 1, "│", redBody);
            renderer.drawText(ix + 0, iy + 1, "✦ ", innerLight, Palette::LanternDarkRed, true);
            renderer.drawChar(ix + 2, iy + 1, "│", redBody);

            // Base
            renderer.drawText(ix - 1, iy + 2, "╰─v─╯", darkFrame);

            // Swaying Tassel (流苏)
            int tasselSway = (sway > 0.3f) ? 1 : ((sway < -0.3f) ? -1 : 0);
            renderer.drawChar(ix + 1 + tasselSway, iy + 3, "§", tasselCol);
            renderer.drawChar(ix + 1 + tasselSway, iy + 4, "v", tasselCol);
        }

        if (m_noticeTimer > 0.0f) {
            float alpha = std::clamp(m_noticeTimer / 1.5f, 0.0f, 1.0f);
            std::string toast = " 🏮 " + m_noticeText + " 🏮 ";
            renderer.drawCenterText(renderer.height() - 3, toast, Palette::LanternLight.withOpacity(alpha), Palette::NightDeep, true);
        }
    }

private:
    std::vector<SingleLantern> m_lanterns;
    float m_time = 0.0f;
    bool m_active = false;
    float m_noticeTimer = 0.0f;
    std::string m_noticeText;
};

} // namespace festival::themes::mid_autumn
