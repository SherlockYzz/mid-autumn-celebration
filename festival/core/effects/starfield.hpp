#pragma once

#include "effect.hpp"
#include "../color/color.hpp"
#include <vector>
#include <random>
#include <cmath>
#include <string>

namespace festival::core {

struct Star {
    float x = 0.0f;
    float y = 0.0f;
    float phase = 0.0f;
    float speed = 1.0f;
    float baseBrightness = 0.5f;
    std::string character = ".";
    Color baseColor{200, 215, 230};
};

class StarField : public Effect {
public:
    StarField(int count = 60, Color baseColor = Color(200, 215, 230))
        : m_starCount(count), m_baseColor(baseColor), m_rng(std::random_device{}()) {}

    void setBaseColor(const Color& color) { m_baseColor = color; }

    void initialize(int width, int height) {
        m_stars.clear();
        m_stars.reserve(m_starCount);

        std::uniform_real_distribution<float> distX(0.0f, static_cast<float>(width - 1));
        std::uniform_real_distribution<float> distY(0.0f, static_cast<float>(height - 1));
        std::uniform_real_distribution<float> distPhase(0.0f, TWO_PI);
        std::uniform_real_distribution<float> distSpeed(0.5f, 2.5f);
        std::uniform_real_distribution<float> distBright(0.3f, 0.9f);
        std::uniform_int_distribution<int> distChar(0, 3);

        const std::string glyphs[] = {".", "·", "*", "+"};

        for (int i = 0; i < m_starCount; ++i) {
            Star s;
            s.x = distX(m_rng);
            s.y = distY(m_rng);
            s.phase = distPhase(m_rng);
            s.speed = distSpeed(m_rng);
            s.baseBrightness = distBright(m_rng);
            s.character = glyphs[distChar(m_rng)];
            s.baseColor = m_baseColor;
            m_stars.push_back(s);
        }
    }

    void update(float dt) override {
        m_time += dt;
    }

    void render(Renderer& renderer) override {
        if (m_stars.empty()) {
            initialize(renderer.width(), renderer.height());
        }

        for (const auto& s : m_stars) {
            int sx = static_cast<int>(std::round(s.x));
            int sy = static_cast<int>(std::round(s.y));

            if (sx < 0 || sx >= renderer.width() || sy < 0 || sy >= renderer.height()) continue;

            float pulse = 0.5f + 0.5f * std::sin(s.phase + m_time * s.speed);
            float brightness = s.baseBrightness * (0.3f + 0.7f * pulse);

            Color c = s.baseColor.scaled(brightness);
            renderer.drawChar(sx, sy, s.character, c);
        }
    }

    bool isFinished() const override { return false; }

private:
    int m_starCount = 60;
    Color m_baseColor;
    std::vector<Star> m_stars;
    std::mt19937 m_rng;
    float m_time = 0.0f;
};

} // namespace festival::core
