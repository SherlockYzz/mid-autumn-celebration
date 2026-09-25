#pragma once

#include "../types.hpp"
#include "../color/color.hpp"
#include <string>

namespace festival::core {

struct Particle {
    Vec2 position{0.0f, 0.0f};
    Vec2 velocity{0.0f, 0.0f};
    Vec2 acceleration{0.0f, 0.0f};
    float life = 0.0f;
    float maxLife = 1.0f;
    float opacity = 1.0f;
    std::string character = "*";
    Color color{255, 255, 255};
    float phase = 0.0f;
    float frequency = 1.0f;
    float amplitude = 0.0f;

    bool isAlive() const {
        return life < maxLife;
    }

    float normalizedLife() const {
        if (maxLife <= 0.0001f) return 1.0f;
        return std::clamp(life / maxLife, 0.0f, 1.0f);
    }
};

} // namespace festival::core
