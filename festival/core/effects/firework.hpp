#pragma once

#include "effect.hpp"
#include "../particle/particle.hpp"
#include "../color/color.hpp"
#include <vector>
#include <random>
#include <cmath>

namespace festival::core {

enum class FireworkPhase {
    Launching,
    Exploding,
    Finished
};

struct FireworkConfig {
    Vec2 startPos{40.0f, 30.0f};
    float targetHeight = 10.0f;
    float riseSpeed = 22.0f;
    int particleCount = 55;
    float explosionSpeedMin = 4.0f;
    float explosionSpeedMax = 12.0f;
    Vec2 gravity{0.0f, 1.8f}; // Soft weeping willow gravity
    float drag = 0.94f;
    float particleLife = 4.8f; // Lasting 4.8+ seconds!
    std::vector<Color> colors{Color(255, 230, 140), Color(255, 190, 80), Color(255, 140, 50)};
    Color trailColor{240, 200, 90};
    std::vector<std::string> characters{"*", "·", "✦", "°", "•"};
};

class Firework : public Effect {
public:
    explicit Firework(FireworkConfig config)
        : m_config(std::move(config)), m_pos(m_config.startPos), m_phase(FireworkPhase::Launching),
          m_rng(std::random_device{}()) {}

    FireworkPhase phase() const { return m_phase; }

    void update(float dt) override {
        if (m_phase == FireworkPhase::Finished) return;

        if (m_phase == FireworkPhase::Launching) {
            m_pos.y -= m_config.riseSpeed * dt;

            // Leave rocket spark trail
            if (m_sparkTrail.size() < 12) {
                Particle p;
                p.position = m_pos;
                p.velocity = {0.0f, 2.0f};
                p.life = 0.0f;
                p.maxLife = 0.25f;
                p.character = "|";
                p.color = m_config.trailColor;
                p.opacity = 0.8f;
                m_sparkTrail.push_back(p);
            }

            // Update trails
            for (auto& sp : m_sparkTrail) {
                sp.life += dt;
                sp.position += sp.velocity * dt;
                sp.opacity = std::max(0.0f, 1.0f - sp.life / sp.maxLife);
            }
            m_sparkTrail.erase(
                std::remove_if(m_sparkTrail.begin(), m_sparkTrail.end(),
                    [](const Particle& p) { return !p.isAlive() || p.opacity <= 0.05f; }),
                m_sparkTrail.end());

            if (m_pos.y <= m_config.targetHeight) {
                explode();
            }
        } else if (m_phase == FireworkPhase::Exploding) {
            bool anyAlive = false;
            for (auto& p : m_particles) {
                if (!p.isAlive()) continue;

                p.life += dt;
                p.velocity = p.velocity * std::pow(m_config.drag, dt * 30.0f);
                p.velocity += m_config.gravity * dt;
                p.position += p.velocity * dt;

                float lifeRatio = p.normalizedLife();
                p.opacity = std::max(0.0f, 1.0f - lifeRatio);

                // Twinkle / shimmer
                float flicker = 0.7f + 0.3f * std::sin(p.phase + p.life * p.frequency);
                p.opacity *= flicker;

                if (p.isAlive() && p.opacity > 0.05f) {
                    anyAlive = true;
                }
            }

            if (!anyAlive) {
                m_phase = FireworkPhase::Finished;
            }
        }
    }

    void render(Renderer& renderer) override {
        if (m_phase == FireworkPhase::Launching) {
            for (const auto& sp : m_sparkTrail) {
                int tx = static_cast<int>(std::round(sp.position.x));
                int ty = static_cast<int>(std::round(sp.position.y));
                renderer.drawChar(tx, ty, sp.character, sp.color.withOpacity(sp.opacity));
            }

            int rx = static_cast<int>(std::round(m_pos.x));
            int ry = static_cast<int>(std::round(m_pos.y));
            renderer.drawChar(rx, ry, "^", m_config.trailColor, Color(0, 0, 0, 0), true);
        } else if (m_phase == FireworkPhase::Exploding) {
            for (const auto& p : m_particles) {
                if (!p.isAlive() || p.opacity <= 0.05f) continue;

                int px = static_cast<int>(std::round(p.position.x));
                int py = static_cast<int>(std::round(p.position.y));
                renderer.drawChar(px, py, p.character, p.color.withOpacity(p.opacity));
            }
        }
    }

    bool isFinished() const override {
        return m_phase == FireworkPhase::Finished;
    }

private:
    void explode() {
        m_phase = FireworkPhase::Exploding;
        m_particles.clear();
        m_particles.reserve(m_config.particleCount);

        std::uniform_real_distribution<float> angleDist(0.0f, TWO_PI);
        std::uniform_real_distribution<float> speedDist(m_config.explosionSpeedMin, m_config.explosionSpeedMax);
        std::uniform_real_distribution<float> lifeDist(m_config.particleLife * 0.7f, m_config.particleLife * 1.3f);
        std::uniform_real_distribution<float> phaseDist(0.0f, TWO_PI);
        std::uniform_real_distribution<float> freqDist(10.0f, 25.0f);

        for (int i = 0; i < m_config.particleCount; ++i) {
            Particle p;
            p.position = m_pos;

            float angle = angleDist(m_rng);
            float speed = speedDist(m_rng);

            // Aspect ratio adjustment (terminal cells are ~2x taller than wide)
            p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed * 0.5f};
            p.acceleration = m_config.gravity;
            p.life = 0.0f;
            p.maxLife = lifeDist(m_rng);
            p.opacity = 1.0f;
            p.phase = phaseDist(m_rng);
            p.frequency = freqDist(m_rng);

            if (!m_config.characters.empty()) {
                p.character = m_config.characters[i % m_config.characters.size()];
            }

            if (!m_config.colors.empty()) {
                p.color = m_config.colors[i % m_config.colors.size()];
            }

            m_particles.push_back(p);
        }
    }

    FireworkConfig m_config;
    Vec2 m_pos;
    FireworkPhase m_phase;
    std::vector<Particle> m_particles;
    std::vector<Particle> m_sparkTrail;
    std::mt19937 m_rng;
};

} // namespace festival::core
