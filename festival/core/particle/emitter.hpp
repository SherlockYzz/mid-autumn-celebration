#pragma once

#include "particle.hpp"
#include <vector>
#include <random>
#include <cmath>

namespace festival::core {

enum class EmitterType {
    Directional,
    Radial,
    Gravity,
    Drift,
    Upward,
    Burst
};

struct EmitterConfig {
    EmitterType type = EmitterType::Radial;
    Vec2 origin{0.0f, 0.0f};
    Vec2 area{0.0f, 0.0f}; // If > 0, spawn within box [origin, origin + area]

    float emissionRate = 10.0f; // particles per second
    float minLife = 1.0f;
    float maxLife = 2.0f;

    float minSpeed = 1.0f;
    float maxSpeed = 5.0f;

    float angle = 0.0f;       // Radians
    float spread = TWO_PI;    // Angular spread in radians

    Vec2 gravity{0.0f, 0.0f};
    float driftAmplitude = 0.0f;
    float driftFrequency = 1.0f;

    std::vector<std::string> characters{"*"};
    std::vector<Color> colors{Color(255, 255, 255)};

    bool active = true;
};

class ParticleEmitter {
public:
    explicit ParticleEmitter(EmitterConfig config)
        : m_config(std::move(config)), m_rng(std::random_device{}()) {}

    void setOrigin(const Vec2& origin) { m_config.origin = origin; }
    void setArea(const Vec2& area) { m_config.area = area; }
    void setActive(bool active) { m_config.active = active; }
    bool isActive() const { return m_config.active; }

    EmitterConfig& config() { return m_config; }
    const EmitterConfig& config() const { return m_config; }

    void update(float dt, std::vector<Particle>& particles, int maxParticles = 500) {
        if (!m_config.active || m_config.type == EmitterType::Burst) return;

        m_accumulator += dt * m_config.emissionRate;
        while (m_accumulator >= 1.0f) {
            m_accumulator -= 1.0f;
            if (static_cast<int>(particles.size()) < maxParticles) {
                particles.push_back(createParticle());
            }
        }
    }

    void burst(int count, std::vector<Particle>& particles, int maxParticles = 500) {
        for (int i = 0; i < count; ++i) {
            if (static_cast<int>(particles.size()) >= maxParticles) break;
            particles.push_back(createParticle());
        }
    }

private:
    Particle createParticle() {
        Particle p;

        std::uniform_real_distribution<float> dist01(0.0f, 1.0f);
        std::uniform_real_distribution<float> lifeDist(m_config.minLife, m_config.maxLife);
        std::uniform_real_distribution<float> speedDist(m_config.minSpeed, m_config.maxSpeed);

        // Spawn position
        p.position = m_config.origin;
        if (m_config.area.x > 0.0f) {
            p.position.x += dist01(m_rng) * m_config.area.x;
        }
        if (m_config.area.y > 0.0f) {
            p.position.y += dist01(m_rng) * m_config.area.y;
        }

        // Velocity by emitter type
        float speed = speedDist(m_rng);
        switch (m_config.type) {
            case EmitterType::Radial: {
                float a = dist01(m_rng) * TWO_PI;
                p.velocity = {std::cos(a) * speed, std::sin(a) * speed * 0.5f}; // Aspect ratio adjustment
                break;
            }
            case EmitterType::Directional: {
                float a = m_config.angle + (dist01(m_rng) - 0.5f) * m_config.spread;
                p.velocity = {std::cos(a) * speed, std::sin(a) * speed * 0.5f};
                break;
            }
            case EmitterType::Gravity:
            case EmitterType::Drift: {
                p.velocity = {(dist01(m_rng) - 0.5f) * 1.5f, speed};
                break;
            }
            case EmitterType::Upward: {
                float a = -HALF_PI + (dist01(m_rng) - 0.5f) * m_config.spread;
                p.velocity = {std::cos(a) * speed * 0.5f, -std::abs(std::sin(a) * speed * 0.5f)};
                break;
            }
            case EmitterType::Burst: {
                float a = dist01(m_rng) * TWO_PI;
                p.velocity = {std::cos(a) * speed, std::sin(a) * speed * 0.55f};
                break;
            }
        }

        p.acceleration = m_config.gravity;
        p.life = 0.0f;
        p.maxLife = lifeDist(m_rng);
        p.opacity = 1.0f;

        // Drift oscillation
        p.phase = dist01(m_rng) * TWO_PI;
        p.frequency = m_config.driftFrequency * (0.8f + dist01(m_rng) * 0.4f);
        p.amplitude = m_config.driftAmplitude;

        // Character selection
        if (!m_config.characters.empty()) {
            std::uniform_int_distribution<size_t> charDist(0, m_config.characters.size() - 1);
            p.character = m_config.characters[charDist(m_rng)];
        }

        // Color selection
        if (!m_config.colors.empty()) {
            std::uniform_int_distribution<size_t> colDist(0, m_config.colors.size() - 1);
            p.color = m_config.colors[colDist(m_rng)];
        }

        return p;
    }

    EmitterConfig m_config;
    std::mt19937 m_rng;
    float m_accumulator = 0.0f;
};

} // namespace festival::core
