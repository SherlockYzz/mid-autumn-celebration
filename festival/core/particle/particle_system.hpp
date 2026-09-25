#pragma once

#include "emitter.hpp"
#include "../renderer/renderer.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace festival::core {

class ParticleSystem {
public:
    explicit ParticleSystem(int maxParticles = 300)
        : m_maxParticles(maxParticles) {}

    void setMaxParticles(int max) {
        m_maxParticles = std::max(10, max);
    }

    int getMaxParticles() const { return m_maxParticles; }
    size_t getParticleCount() const { return m_particles.size(); }

    void addEmitter(std::shared_ptr<ParticleEmitter> emitter) {
        m_emitters.push_back(std::move(emitter));
    }

    void addParticle(const Particle& p) {
        if (static_cast<int>(m_particles.size()) < m_maxParticles) {
            m_particles.push_back(p);
        }
    }

    void clearParticles() {
        m_particles.clear();
    }

    void clearAll() {
        m_particles.clear();
        m_emitters.clear();
    }

    void update(float dt, int screenWidth, int screenHeight) {
        // Update emitters
        for (auto& emitter : m_emitters) {
            if (emitter) {
                emitter->update(dt, m_particles, m_maxParticles);
            }
        }

        // Update existing particles
        for (auto& p : m_particles) {
            p.life += dt;
            p.velocity += p.acceleration * dt;

            Vec2 step = p.velocity;
            if (p.amplitude > 0.0001f) {
                step.x += std::sin(p.phase + p.life * p.frequency) * p.amplitude;
            }

            p.position += step * dt;
            float lifeRatio = p.normalizedLife();
            p.opacity = std::max(0.0f, 1.0f - lifeRatio);
        }

        // Remove dead or out of bounds particles
        m_particles.erase(
            std::remove_if(m_particles.begin(), m_particles.end(),
                [screenWidth, screenHeight](const Particle& p) {
                    if (!p.isAlive() || p.opacity <= 0.01f) return true;
                    if (p.position.x < -5.0f || p.position.x > screenWidth + 5.0f ||
                        p.position.y < -5.0f || p.position.y > screenHeight + 5.0f) {
                        return true;
                    }
                    return false;
                }),
            m_particles.end());
    }

    void render(Renderer& renderer) const {
        for (const auto& p : m_particles) {
            if (p.opacity <= 0.05f) continue;

            int ix = static_cast<int>(std::round(p.position.x));
            int iy = static_cast<int>(std::round(p.position.y));

            Color renderColor = p.color.withOpacity(p.opacity);
            renderer.drawChar(ix, iy, p.character, renderColor);
        }
    }

private:
    int m_maxParticles = 300;
    std::vector<Particle> m_particles;
    std::vector<std::shared_ptr<ParticleEmitter>> m_emitters;
};

} // namespace festival::core
