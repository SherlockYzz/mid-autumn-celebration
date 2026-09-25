#pragma once

#include "../mid_autumn_palette.hpp"
#include "../../../core/effects/firework.hpp"
#include "../../../core/renderer/renderer.hpp"
#include <vector>
#include <memory>

namespace festival::themes::mid_autumn {

class FestivalFireworks {
public:
    FestivalFireworks() = default;

    void launchSingle(float x, float groundY, float targetY) {
        FireworkConfig cfg;
        cfg.startPos = {x, groundY};
        cfg.targetHeight = targetY;
        cfg.riseSpeed = 26.0f;
        cfg.particleCount = 85;
        cfg.explosionSpeedMin = 3.5f;
        cfg.explosionSpeedMax = 11.0f;
        cfg.gravity = {0.0f, 0.95f}; // 柔美金柳垂丝漫天飞絮
        cfg.drag = 0.96f;
        cfg.particleLife = 10.5f; // 漫天金屑持久留存 10.5 秒！
        cfg.colors = {Palette::FireworkGold, Palette::OsmanthusLight, Palette::MoonGlow};
        cfg.trailColor = Palette::OsmanthusGold;
        cfg.characters = {"✨", "✦", "*", "·", "°", "★"};

        m_fireworks.push_back(std::make_unique<Firework>(cfg));
    }

    void launchGrandCelebration(int screenWidth, int screenHeight) {
        float ground = static_cast<float>(screenHeight - 3);

        // Burst 1: Left Golden Willow
        FireworkConfig cfg1;
        cfg1.startPos = {screenWidth * 0.26f, ground};
        cfg1.targetHeight = screenHeight * 0.22f;
        cfg1.riseSpeed = 26.0f;
        cfg1.particleCount = 95;
        cfg1.gravity = {0.0f, 0.90f};
        cfg1.drag = 0.96f;
        cfg1.particleLife = 11.5f;
        cfg1.colors = {Palette::FireworkGold, Palette::OsmanthusLight, Palette::MoonGlow};
        cfg1.trailColor = Palette::OsmanthusGold;
        cfg1.characters = {"✨", "✦", "*", "·", "°"};
        m_fireworks.push_back(std::make_unique<Firework>(cfg1));

        // Burst 2: Center-Right Cinnabar & Royal Gold
        FireworkConfig cfg2;
        cfg2.startPos = {screenWidth * 0.52f, ground};
        cfg2.targetHeight = screenHeight * 0.16f;
        cfg2.riseSpeed = 28.0f;
        cfg2.particleCount = 115;
        cfg2.gravity = {0.0f, 1.0f};
        cfg2.drag = 0.96f;
        cfg2.particleLife = 12.0f;
        cfg2.colors = {Palette::LanternRed, Palette::LanternLight, Palette::FireworkGold};
        cfg2.trailColor = Palette::LanternRed;
        cfg2.characters = {"✦", "✨", "*", "•", "°", "★"};
        m_fireworks.push_back(std::make_unique<Firework>(cfg2));

        // Burst 3: Far Right Lunar White Starlight
        FireworkConfig cfg3;
        cfg3.startPos = {screenWidth * 0.78f, ground};
        cfg3.targetHeight = screenHeight * 0.24f;
        cfg3.riseSpeed = 24.0f;
        cfg3.particleCount = 95;
        cfg3.gravity = {0.0f, 0.90f};
        cfg3.drag = 0.96f;
        cfg3.particleLife = 11.0f;
        cfg3.colors = {Palette::MoonWhite, Palette::MoonGlow, Palette::Stars};
        cfg3.trailColor = Palette::MoonSoft;
        cfg3.characters = {"✧", "✨", "·", "*", "°"};
        m_fireworks.push_back(std::make_unique<Firework>(cfg3));
    }

    void update(float dt) {
        for (auto& fw : m_fireworks) {
            fw->update(dt);
        }
        m_fireworks.erase(
            std::remove_if(m_fireworks.begin(), m_fireworks.end(),
                [](const std::unique_ptr<Firework>& fw) { return fw->isFinished(); }),
            m_fireworks.end());
    }

    void render(Renderer& renderer) {
        for (const auto& fw : m_fireworks) {
            fw->render(renderer);
        }
    }

    size_t count() const { return m_fireworks.size(); }

private:
    std::vector<std::unique_ptr<Firework>> m_fireworks;
};

} // namespace festival::themes::mid_autumn
