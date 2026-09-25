#pragma once

#include "effect.hpp"
#include "../particle/particle.hpp"
#include "../color/color.hpp"
#include <vector>
#include <random>
#include <cmath>

namespace festival::core {

class SparkBurst : public Effect {
public:
    SparkBurst(Vec2 center, int count, const std::vector<Color>& colors, float maxLife = 0.8f, float speed = 8.0f)
        : m_center(center), m_maxLife(maxLife) {
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> angleDist(0.0f, TWO_PI);
        std::uniform_real_distribution<float> speedDist(speed * 0.4f, speed);
        std::uniform_real_distribution<float> lifeDist(maxLife * 0.5f, maxLife);

        const std::string glyphs[] = {"*", "·", "+", "°"};

        m_particles.reserve(count);
        for (int i = 0; i < count; ++i) {
            Particle p;
            p.position = center;
            float a = angleDist(rng);
            float s = speedDist(rng);
            p.velocity = {std::cos(a) * s, std::sin(a) * s * 0.5f};
            p.acceleration = {0.0f, 3.0f};
            p.life = 0.0f;
            p.maxLife = lifeDist(rng);
            p.opacity = 1.0f;
            p.character = glyphs[i % 4];
            p.color = colors.empty() ? Color(255, 255, 200) : colors[i % colors.size()];
            m_particles.push_back(p);
        }
    }

    void update(float dt) override {
        bool anyAlive = false;
        for (auto& p : m_particles) {
            if (!p.isAlive()) continue;
            p.life += dt;
            p.velocity += p.acceleration * dt;
            p.position += p.velocity * dt;
            p.opacity = std::max(0.0f, 1.0f - p.normalizedLife());
            if (p.isAlive() && p.opacity > 0.05f) {
                anyAlive = true;
            }
        }
        m_finished = !anyAlive;
    }

    void render(Renderer& renderer) override {
        for (const auto& p : m_particles) {
            if (!p.isAlive() || p.opacity <= 0.05f) continue;
            int px = static_cast<int>(std::round(p.position.x));
            int py = static_cast<int>(std::round(p.position.y));
            renderer.drawChar(px, py, p.character, p.color.withOpacity(p.opacity));
        }
    }

    bool isFinished() const override { return m_finished; }

private:
    Vec2 m_center;
    float m_maxLife;
    bool m_finished = false;
    std::vector<Particle> m_particles;
};

} // namespace festival::core
