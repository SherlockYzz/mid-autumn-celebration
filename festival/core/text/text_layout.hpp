#pragma once

#include "unicode_width.hpp"
#include "../renderer/renderer.hpp"
#include "../animation/tween.hpp"
#include <string>
#include <vector>

namespace festival::core {

class TypewriterText {
public:
    TypewriterText() = default;

    explicit TypewriterText(std::string text, float speedCharsPerSec = 8.0f)
        : m_fullText(std::move(text)), m_speed(speedCharsPerSec) {
        m_glyphs = splitGlyphs(m_fullText);
    }

    void setText(std::string text) {
        m_fullText = std::move(text);
        m_glyphs = splitGlyphs(m_fullText);
        reset();
    }

    void setSpeed(float charsPerSec) {
        m_speed = std::max(0.5f, charsPerSec);
    }

    void reset() {
        m_elapsed = 0.0f;
        m_visibleCount = 0;
        m_complete = false;
    }

    void completeImmediately() {
        m_visibleCount = m_glyphs.size();
        m_complete = true;
    }

    bool isComplete() const { return m_complete; }

    void update(float dt) {
        if (m_complete) return;
        m_elapsed += dt;
        m_visibleCount = std::min(m_glyphs.size(), static_cast<size_t>(m_elapsed * m_speed));
        if (m_visibleCount >= m_glyphs.size()) {
            m_complete = true;
        }
    }

    int visibleVisualWidth() const {
        int w = 0;
        for (size_t i = 0; i < m_visibleCount; ++i) {
            w += m_glyphs[i].visualWidth;
        }
        return w;
    }

    int totalVisualWidth() const {
        int w = 0;
        for (const auto& g : m_glyphs) {
            w += g.visualWidth;
        }
        return w;
    }

    void render(Renderer& renderer, int x, int y, const Color& color, bool centered = false) const {
        int startX = x;
        if (centered) {
            startX = (renderer.width() - totalVisualWidth()) / 2;
        }

        int curX = startX;
        for (size_t i = 0; i < m_visibleCount; ++i) {
            renderer.drawChar(curX, y, m_glyphs[i].text, color);
            curX += m_glyphs[i].visualWidth;
        }
    }

private:
    std::string m_fullText;
    float m_speed = 8.0f;
    std::vector<Glyph> m_glyphs;
    float m_elapsed = 0.0f;
    size_t m_visibleCount = 0;
    bool m_complete = false;
};

class FadingText {
public:
    FadingText() = default;

    FadingText(std::string text, Color targetColor, float duration = 1.0f)
        : m_text(std::move(text)), m_targetColor(targetColor), m_duration(duration) {
        m_visualWidth = stringVisualWidth(m_text);
    }

    void setText(std::string text) {
        m_text = std::move(text);
        m_visualWidth = stringVisualWidth(m_text);
    }

    void setTargetColor(const Color& c) { m_targetColor = c; }

    void fadeIn(float duration) {
        m_duration = duration;
        m_tween = Tween<float>(0.0f, 1.0f, duration, EasingType::EaseInOut);
        m_fading = true;
    }

    void fadeOut(float duration) {
        m_duration = duration;
        m_tween = Tween<float>(1.0f, 0.0f, duration, EasingType::EaseInOut);
        m_fading = true;
    }

    void update(float dt) {
        m_tween.update(dt);
        if (m_tween.isDone()) {
            m_fading = false;
        }
    }

    float opacity() const {
        return m_tween.getValue();
    }

    int visualWidth() const { return m_visualWidth; }

    void render(Renderer& renderer, int x, int y, bool centered = false) const {
        float alpha = opacity();
        if (alpha <= 0.02f) return;

        Color c = m_targetColor.withOpacity(alpha);
        if (centered) {
            renderer.drawCenterText(y, m_text, c);
        } else {
            renderer.drawText(x, y, m_text, c);
        }
    }

private:
    std::string m_text;
    Color m_targetColor{255, 255, 255};
    float m_duration = 1.0f;
    int m_visualWidth = 0;
    Tween<float> m_tween{1.0f, 1.0f, 0.001f};
    bool m_fading = false;
};

class Banner {
public:
    static void renderBanner(
        Renderer& renderer,
        int x, int y, int width, int height,
        std::string_view title,
        const Color& borderColor,
        const Color& titleColor,
        const Color& bgColor = Color(0, 0, 0, 200)
    ) {
        renderer.fillRect(x, y, width, height, bgColor);
        renderer.drawBorder(x, y, width, height, borderColor);

        if (!title.empty()) {
            int titleWidth = stringVisualWidth(title);
            int titleX = x + (width - titleWidth) / 2;
            renderer.drawText(titleX, y, title, titleColor, bgColor, true);
        }
    }
};

} // namespace festival::core
