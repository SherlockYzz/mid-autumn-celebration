#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/particle/particle_system.hpp"
#include "../../../core/renderer/renderer.hpp"
#include "../../../core/platform/platform.hpp"
#include <memory>
#include <vector>

namespace festival::themes::mid_autumn {

class Osmanthus {
public:
    Osmanthus() = default;

    void initialize(int screenWidth, int /*screenHeight*/) {
        m_particleSystem = std::make_unique<ParticleSystem>(350); // 容纳漫天流转金粟与狂澜花雨
 
        bool unicode = Platform::instance().supportsUnicode();
        std::vector<std::string> glyphs = unicode ?
            std::vector<std::string>{"✽", "·", "*", "❀", "°"} :
            std::vector<std::string>{"*", "·", "."};

        EmitterConfig cfg;
        cfg.type = EmitterType::Drift;
        cfg.origin = {0.0f, -2.0f};
        cfg.area = {static_cast<float>(screenWidth), 3.0f};
        cfg.emissionRate = 4.0f;
        cfg.minLife = 16.0f;
        cfg.maxLife = 26.0f; // 悠然飘落长达 26 秒
        cfg.minSpeed = 0.5f;
        cfg.maxSpeed = 1.2f;
        cfg.gravity = {0.0f, 0.25f};
        cfg.driftAmplitude = 1.8f;
        cfg.driftFrequency = 0.6f;
        cfg.characters = glyphs;
        cfg.colors = {Palette::OsmanthusGold, Palette::OsmanthusLight, Palette::OsmanthusDark};
        cfg.active = false;

        m_emitter = std::make_shared<ParticleEmitter>(cfg);
        m_particleSystem->addEmitter(m_emitter);
    }

    void startDrifting() {
        if (m_emitter) {
            m_emitter->setActive(true);
        }
        m_branchVisible = true;
    }

    void stopDrifting() {
        if (m_emitter) {
            m_emitter->setActive(false);
        }
    }

    void shakeShower(int screenWidth = 80) {
        if (!m_particleSystem) return;
        m_showerNoticeTimer = 10.0f; // 维持 10 秒提示与高潮

        std::vector<Particle> burstParticles;
        EmitterConfig cfg;
        cfg.type = EmitterType::Drift;
        cfg.origin = {0.0f, 0.0f};
        cfg.area = {static_cast<float>(screenWidth > 0 ? screenWidth : 80), 4.0f};
        cfg.minLife = 18.0f;
        cfg.maxLife = 28.0f; // 摇树后桂花花瓣飘落长达 28 秒！
        cfg.minSpeed = 0.6f;
        cfg.maxSpeed = 1.6f;
        cfg.gravity = {0.0f, 0.28f};
        cfg.driftAmplitude = 2.8f;
        cfg.driftFrequency = 0.75f;
        cfg.characters = Platform::instance().supportsUnicode() ?
            std::vector<std::string>{"✽", "❀", "·", "*", "°"} :
            std::vector<std::string>{"*", "·", "."};
        cfg.colors = {Palette::OsmanthusGold, Palette::OsmanthusLight, Palette::OsmanthusDark};

        ParticleEmitter showerEmitter(cfg);
        showerEmitter.burst(96, burstParticles, 350);
        for (const auto& p : burstParticles) {
            m_particleSystem->addParticle(p);
        }
    }

    void update(float dt, int screenWidth, int screenHeight) {
        if (m_showerNoticeTimer > 0.0f) {
            m_showerNoticeTimer -= dt;
        }
        if (m_particleSystem) {
            m_particleSystem->update(dt, screenWidth, screenHeight);
        }
    }

    void render(Renderer& renderer) {
        // Draw delicate framing osmanthus branch silhouette in top-left
        if (m_branchVisible) {
            renderBranch(renderer);
        }

        // Render drifting osmanthus petal particles
        if (m_particleSystem) {
            m_particleSystem->render(renderer);
        }

        if (m_showerNoticeTimer > 0.0f) {
            float alpha = std::clamp(m_showerNoticeTimer / 1.5f, 0.0f, 1.0f);
            renderer.drawText(14, 2, " 🍃 ✨ 金粟摇落 · 暗香盈袖 ✨ 🍃 ", Palette::OsmanthusLight.withOpacity(alpha), Palette::NightDeep, true);
        }
    }

private:
    void renderBranch(Renderer& renderer) {
        Color branchCol = Palette::Mountain;
        Color flowerCol = Palette::OsmanthusGold;
        Color flowerLight = Palette::OsmanthusLight;

        // Elegant minimal branch strokes hugging top-left edge (rows 0..2)
        renderer.drawText(0, 0, "╭───────\\_____", branchCol);
        renderer.drawText(8, 1, "\\____  ╭──", branchCol);
        renderer.drawText(16, 2, "\\___", branchCol);

        // Clustered fragrant blossoms along branch
        renderer.drawChar(4, 0, "✽", flowerLight);
        renderer.drawChar(9, 0, "·", flowerCol);
        renderer.drawChar(10, 1, "✽", flowerLight);
        renderer.drawChar(14, 1, "✽", flowerCol);
        renderer.drawChar(18, 2, "·", flowerLight);
    }

    std::unique_ptr<ParticleSystem> m_particleSystem;
    std::shared_ptr<ParticleEmitter> m_emitter;
    bool m_branchVisible = false;
    float m_showerNoticeTimer = 0.0f;
};

} // namespace festival::themes::mid_autumn
