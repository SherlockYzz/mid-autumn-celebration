#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/animation/tween.hpp"
#include <cmath>
#include <vector>

namespace festival::themes::mid_autumn {

class PixelMoon {
public:
    PixelMoon() = default;

    void initialize(int screenWidth, int screenHeight) {
        int pw = screenWidth;
        int ph = screenHeight * 2; // Double vertical subpixel resolution

        m_targetX = pw * 0.62f;
        m_targetY = ph * 0.22f;
        m_currentX = m_targetX;
        m_radius = std::clamp(ph * 0.14f, 6.5f, 9.5f);

        m_startY = ph + m_radius + 4.0f;
        m_currentY = m_startY;

        // Fast, smooth moonrise: 3.2 seconds
        m_riseTween = Tween<float>(m_startY, m_targetY, 3.2f, EasingType::EaseOutCubic);
        m_pulseTween = Tween<float>(0.0f, 1.0f, 3.0f, EasingType::Pulse, true);
        m_moonriseStarted = false;
    }

    void startMoonrise() {
        m_moonriseStarted = true;
        m_riseTween.reset();
    }

    void skipToTop() {
        m_moonriseStarted = true;
        m_currentY = m_targetY;
    }

    bool isMoonriseComplete() const {
        return m_riseTween.isDone() || m_currentY <= m_targetY + 0.5f;
    }

    Vec2 position() const {
        return {m_currentX, m_currentY};
    }

    float radius() const {
        return m_radius;
    }

    void triggerMoonlightWave() {
        m_waveActive = true;
        m_waveRadius = m_radius;
        m_waveTween = Tween<float>(m_radius, m_radius * 3.8f, 1.8f, EasingType::EaseOut);
    }

    void setMoonlightBoost(float boost) {
        m_glowBoost = boost;
    }

    void update(float dt) {
        m_time += dt;

        if (m_moonriseStarted) {
            m_riseTween.update(dt);
            m_currentY = m_riseTween.getValue();
        }

        m_pulseTween.update(dt);

        if (m_waveActive) {
            m_waveTween.update(dt);
            m_waveRadius = m_waveTween.getValue();
            if (m_waveTween.isDone()) {
                m_waveActive = false;
            }
        }

        if (m_glowBoost > 0.0f) {
            m_glowBoost = std::max(0.0f, m_glowBoost - dt * 0.5f);
        }
    }

    void render(Renderer& renderer) {
        float cx = m_currentX;
        float cy = m_currentY;
        float r = m_radius;
        float r2 = r * r;

        float pulse = 0.88f + 0.12f * m_pulseTween.getValue() + m_glowBoost * 0.4f;

        // 1. Pixel Glow Halo (Radial soft falloff)
        float outerGlowR = r * 2.5f * pulse;
        int minGx = std::max(0, static_cast<int>(std::floor(cx - outerGlowR)));
        int maxGx = std::min(renderer.pixelWidth() - 1, static_cast<int>(std::ceil(cx + outerGlowR)));
        int minGy = std::max(0, static_cast<int>(std::floor(cy - outerGlowR)));
        int maxGy = std::min(renderer.pixelHeight() - 1, static_cast<int>(std::ceil(cy + outerGlowR)));

        for (int py = minGy; py <= maxGy; ++py) {
            for (int px = minGx; px <= maxGx; ++px) {
                float dx = px - cx;
                float dy = py - cy;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist > r && dist <= outerGlowR) {
                    float norm = (dist - r) / (outerGlowR - r);
                    float alpha = std::pow(1.0f - norm, 1.6f) * 0.65f * pulse;
                    if (alpha > 0.04f) {
                        Color glow = Color::lerp(Palette::MoonGlow, Palette::NightSky, norm);
                        renderer.drawPixel(px, py, glow.withOpacity(alpha));
                    }
                }
            }
        }

        // 2. Interactive Moonlight Shockwave Ring
        if (m_waveActive) {
            float waveProgress = m_waveTween.progress();
            float waveAlpha = (1.0f - waveProgress) * 0.8f;
            float wr = m_waveRadius;
            for (float angle = 0.0f; angle < TWO_PI; angle += 0.06f) {
                int wx = static_cast<int>(std::round(cx + std::cos(angle) * wr));
                int wy = static_cast<int>(std::round(cy + std::sin(angle) * wr));
                renderer.drawPixel(wx, wy, Palette::MoonGlow.withOpacity(waveAlpha));
            }
        }

        // 3. Solid Ivory Pixel Moon Disc with Procedural Lunar Mare Texture
        int minPx = std::max(0, static_cast<int>(std::floor(cx - r)));
        int maxPx = std::min(renderer.pixelWidth() - 1, static_cast<int>(std::ceil(cx + r)));
        int minPy = std::max(0, static_cast<int>(std::floor(cy - r)));
        int maxPy = std::min(renderer.pixelHeight() - 1, static_cast<int>(std::ceil(cy + r)));

        for (int py = minPy; py <= maxPy; ++py) {
            for (int px = minPx; px <= maxPx; ++px) {
                float dx = px - cx;
                float dy = py - cy;
                float d2 = dx * dx + dy * dy;

                if (d2 <= r2) {
                    float d = std::sqrt(d2);
                    float edgeDist = r - d;

                    // Procedural lunar mare (gentle shaded craters)
                    float nx = dx / r;
                    float ny = dy / r;
                    float mare = 0.5f * std::sin(nx * 4.0f + ny * 2.5f) +
                                 0.3f * std::cos(nx * 6.0f - ny * 5.0f);

                    Color discColor = (mare < -0.15f) ? Palette::MoonShade : Palette::MoonWhite;

                    // Edge antialiasing & rim light
                    if (edgeDist < 1.2f) {
                        float rimAlpha = edgeDist / 1.2f;
                        discColor = Color::lerp(Palette::MoonGlow, discColor, rimAlpha);
                    }

                    renderer.drawPixel(px, py, discColor);
                }
            }
        }
    }

private:
    float m_targetX = 60.0f;
    float m_targetY = 15.0f;
    float m_startY = 60.0f;
    float m_currentX = 60.0f;
    float m_currentY = 60.0f;
    float m_radius = 12.0f;

    bool m_moonriseStarted = false;
    Tween<float> m_riseTween;
    Tween<float> m_pulseTween;

    bool m_waveActive = false;
    float m_waveRadius = 12.0f;
    Tween<float> m_waveTween;
    float m_glowBoost = 0.0f;

    float m_time = 0.0f;
};

} // namespace festival::themes::mid_autumn
