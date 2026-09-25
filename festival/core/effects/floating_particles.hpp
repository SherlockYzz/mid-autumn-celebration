#pragma once

#include "effect.hpp"
#include "../color/color.hpp"
#include "../types.hpp"
#include <vector>
#include <random>
#include <cmath>
#include <string>

namespace festival::core {

struct FloatingMote {
    Vec2 position;
    Vec2 baseVelocity;
    float phase = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.5f;
    float brightness = 0.5f;
    std::string character = "·";
    Color color;
};

class FloatingParticles : public Effect {
public:
    FloatingParticles(int count, Color color, float speedMin = 0.5f, float speedMax = 1.5f)
        : m_count(count), m_color(color), m_speedMin(speedMin), m_speedMax(speedMax),
          m_rng(std::random_device{}()) {}

    void initialize(int width, int height) {
        m_motes.clear();
        m_motes.reserve(m_count);

        std::uniform_real_distribution<float> distX(0.0f, static_cast<float>(width));
        std::uniform_real_distribution<float> distY(0.0f, static_cast<float>(height));
        std::uniform_real_distribution<float> distSpeed(m_speedMin, m_speedMax);
        std::uniform_real_distribution<float> distPhase(0.0f, TWO_PI);
        std::uniform_real_distribution<float> distFreq(0.5f, 2.0f);
        std::uniform_real_distribution<float> distAmp(0.5f, 2.5f);

        for (int i = 0; i < m_count; ++i) {
            FloatingMote m;
            m.position = {distX(m_rng), distY(m_rng)};
            m.baseVelocity = {0.0f, -distSpeed(m_rng) * 0.4f};
            m.phase = distPhase(m_rng);
            m.frequency = distFreq(m_rng);
            m.amplitude = distAmp(m_rng);
            m.brightness = 0.3f + (i % 5) * 0.15f;
            m.character = (i % 3 == 0) ? "·" : ((i % 3 == 1) ? "°" : ".");
            m.color = m_color;
            m_motes.push_back(m);
        }
    }

    void update(float dt) override {
        m_time += dt;
        for (auto& m : m_motes) {
            m.position.y += m.baseVelocity.y * dt;
            m.position.x += std::sin(m.phase + m_time * m.frequency) * m.amplitude * dt;

            // Wrap around screen
            if (m.position.y < -2.0f) {
                m.position.y = 40.0f;
            }
        }
    }

    void render(Renderer& renderer) override {
        if (m_motes.empty()) {
            initialize(renderer.width(), renderer.height());
        }

        for (const auto& m : m_motes) {
            int x = static_cast<int>(std::round(m.position.x));
            int y = static_cast<int>(std::round(m.position.y));

            if (x < 0 || x >= renderer.width() || y < 0 || y >= renderer.height()) continue;

            float pulse = 0.6f + 0.4f * std::sin(m.phase + m_time * 2.0f);
            Color c = m.color.scaled(m.brightness * pulse);
            renderer.drawChar(x, y, m.character, c);
        }
    }

    bool isFinished() const override { return false; }

private:
    int m_count;
    Color m_color;
    float m_speedMin;
    float m_speedMax;
    std::vector<FloatingMote> m_motes;
    std::mt19937 m_rng;
    float m_time = 0.0f;
};

} // namespace festival::core
