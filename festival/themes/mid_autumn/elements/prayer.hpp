#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/particle/particle.hpp"
#include "../../../core/text/unicode_width.hpp"
#include <string>
#include <vector>
#include <memory>
#include <cmath>

namespace festival::themes::mid_autumn {

struct ActiveSkyLantern {
    Vec2 pos;
    Vec2 velocity;
    std::string wishText;
    float life = 0.0f;
    float maxLife = 12.0f;
    float swayPhase = 0.0f;
    std::vector<Particle> sparkTrail;
};

class PrayerSystem {
public:
    PrayerSystem() = default;

    bool isInputting() const { return m_inputting; }

    void startInput() {
        m_inputting = true;
        m_currentInput.clear();
    }

    void cancelInput() {
        m_inputting = false;
        m_currentInput.clear();
    }

    void handleInputText(const std::string& utf8Text) {
        if (!m_inputting || utf8Text.empty()) return;
        if (stringVisualWidth(m_currentInput) + stringVisualWidth(utf8Text) <= 32) {
            m_currentInput += utf8Text;
        }
    }

    void handleBackspace() {
        if (!m_inputting || m_currentInput.empty()) return;
        while (!m_currentInput.empty()) {
            uint8_t b = static_cast<uint8_t>(m_currentInput.back());
            m_currentInput.pop_back();
            if ((b & 0xC0) != 0x80) {
                break;
            }
        }
    }

    bool submitInput(float spawnX, float spawnY) {
        if (!m_inputting) return false;
        std::string wish = m_currentInput.empty() ? "平安喜乐" : m_currentInput;
        launchLantern(spawnX, spawnY, wish);
        m_lastSubmittedWish = wish;
        m_feedbackTimer = 4.0f;
        m_inputting = false;
        m_currentInput.clear();
        return true;
    }

    void launchLantern(float x, float y, const std::string& wish) {
        ActiveSkyLantern l;
        l.pos = {x, y};
        l.velocity = {0.0f, -1.8f}; // 悠缓上升
        l.wishText = wish;
        l.life = 0.0f;
        l.maxLife = 24.0f; // 升空驻留达 24 秒
        l.swayPhase = static_cast<float>(m_lanterns.size()) * 1.7f;
        m_lanterns.push_back(l);
    }

    void update(float dt) {
        if (m_feedbackTimer > 0.0f) {
            m_feedbackTimer -= dt;
        }

        for (auto& l : m_lanterns) {
            l.life += dt;
            l.pos.y += l.velocity.y * dt;
            l.pos.x += std::sin(l.swayPhase + l.life * 1.4f) * 1.8f * dt;

            // Spawn fire spark
            if (l.sparkTrail.size() < 6 && (std::sin(l.life * 10.0f) > 0.0f)) {
                Particle p;
                p.position = {l.pos.x, l.pos.y + 2.0f};
                p.velocity = {(static_cast<float>(rand() % 10) - 5.0f) * 0.1f, 1.0f};
                p.life = 0.0f;
                p.maxLife = 0.5f;
                p.character = "·";
                p.color = Palette::LanternLight;
                p.opacity = 0.8f;
                l.sparkTrail.push_back(p);
            }

            for (auto& sp : l.sparkTrail) {
                sp.life += dt;
                sp.position += sp.velocity * dt;
                sp.opacity = std::max(0.0f, 1.0f - sp.life / sp.maxLife);
            }
            l.sparkTrail.erase(
                std::remove_if(l.sparkTrail.begin(), l.sparkTrail.end(),
                    [](const Particle& p) { return !p.isAlive() || p.opacity <= 0.05f; }),
                l.sparkTrail.end());
        }

        m_lanterns.erase(
            std::remove_if(m_lanterns.begin(), m_lanterns.end(),
                [](const ActiveSkyLantern& l) { return l.life >= l.maxLife || l.pos.y < -5.0f; }),
            m_lanterns.end());
    }

    void render(Renderer& renderer) {
        // 1. Render Floating Sky Lanterns
        for (const auto& l : m_lanterns) {
            int cx = static_cast<int>(std::round(l.pos.x));
            int cy = static_cast<int>(std::round(l.pos.y));

            float progress = l.life / l.maxLife;
            float alpha = std::clamp(1.0f - progress * 0.8f, 0.2f, 1.0f);

            Color lanternGold = Palette::LanternLight.withOpacity(alpha);
            Color lanternRed  = Palette::LanternRed.withOpacity(alpha * 0.8f);

            // Draw Sky Lantern body
            renderer.drawText(cx - 1, cy - 1, "╭─╮", lanternRed);
            renderer.drawChar(cx - 1, cy + 0, "│", lanternRed);
            renderer.drawChar(cx + 0, cy + 0, "愿", lanternGold, lanternRed, true);
            renderer.drawChar(cx + 1, cy + 0, "│", lanternRed);
            renderer.drawText(cx - 1, cy + 1, "╰▲╯", lanternGold);

            // Wish text floating beside the lantern
            if (!l.wishText.empty() && cy > 3) {
                renderer.drawText(cx + 3, cy, "「" + l.wishText + "」", Palette::PoetryText.withOpacity(alpha * 0.85f));
            }

            // Sparks trailing beneath
            for (const auto& sp : l.sparkTrail) {
                int sx = static_cast<int>(std::round(sp.position.x));
                int sy = static_cast<int>(std::round(sp.position.y));
                renderer.drawChar(sx, sy, sp.character, sp.color.withOpacity(sp.opacity * alpha));
            }
        }

        // 2. Render Interactive Wish Prompt Modal (when 'W' active)
        if (m_inputting) {
            int barY = renderer.height() - 4;
            int barW = std::min(60, renderer.width() - 4);
            int barX = (renderer.width() - barW) / 2;

            renderer.fillRect(barX, barY, barW, 3, Palette::NightWater);
            renderer.drawBorder(barX, barY, barW, 3, Palette::OsmanthusGold);

            std::string promptTitle = " 【月下祈愿 · 遥寄心灯】 ";
            renderer.drawCenterText(barY, promptTitle, Palette::LanternLight, Palette::NightWater, true);

            std::string displayText = "心语: " + m_currentInput + "_  (按Enter放飞 / ESC取消)";
            renderer.drawText(barX + 2, barY + 1, displayText, Palette::PoetryText, Palette::NightWater);
        }

        // 3. Render Feedback Notification after flying
        if (m_feedbackTimer > 0.0f && !m_inputting) {
            float fade = std::clamp(m_feedbackTimer / 1.5f, 0.0f, 1.0f);
            std::string notice = "🏮 祈愿「" + m_lastSubmittedWish + "」随天灯飞升月轮，福运长伴！";
            int noticeY = renderer.height() - 3;
            renderer.drawCenterText(noticeY, notice, Palette::OsmanthusGold.withOpacity(fade), Palette::NightDeep);
        }
    }

private:
    bool m_inputting = false;
    std::string m_currentInput;
    std::string m_lastSubmittedWish;
    float m_feedbackTimer = 0.0f;
    std::vector<ActiveSkyLantern> m_lanterns;
};

} // namespace festival::themes::mid_autumn
