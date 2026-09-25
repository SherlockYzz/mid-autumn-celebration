#pragma once

#include "effect.hpp"
#include "../color/color.hpp"
#include "../types.hpp"
#include <cmath>
#include <algorithm>

namespace festival::core {

class Glow {
public:
    static float calculateIntensity(float x, float y, float cx, float cy, float radius, float aspect = 2.0f, float exponent = 1.5f) {
        float dx = x - cx;
        float dy = (y - cy) * aspect;
        float dist = std::sqrt(dx * dx + dy * dy);
        if (dist >= radius) return 0.0f;

        float norm = 1.0f - (dist / radius);
        return std::pow(std::clamp(norm, 0.0f, 1.0f), exponent);
    }

    static void renderRadialGlow(
        Renderer& renderer,
        float cx, float cy,
        float innerRadius, float outerRadius,
        const Color& centerColor, const Color& edgeColor,
        float intensityMultiplier = 1.0f,
        float aspect = 2.0f
    ) {
        int xMin = std::max(0, static_cast<int>(std::floor(cx - outerRadius)));
        int xMax = std::min(renderer.width() - 1, static_cast<int>(std::ceil(cx + outerRadius)));
        int yMin = std::max(0, static_cast<int>(std::floor(cy - outerRadius / aspect)));
        int yMax = std::min(renderer.height() - 1, static_cast<int>(std::ceil(cy + outerRadius / aspect)));

        for (int y = yMin; y <= yMax; ++y) {
            for (int x = xMin; x <= xMax; ++x) {
                float dx = static_cast<float>(x) - cx;
                float dy = (static_cast<float>(y) - cy) * aspect;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist > outerRadius || dist < innerRadius) continue;

                float t = (dist - innerRadius) / (outerRadius - innerRadius);
                float intensity = (1.0f - t) * intensityMultiplier;
                if (intensity <= 0.03f) continue;

                Color glowColor = Color::lerp(centerColor, edgeColor, t).withOpacity(intensity);
                
                // For soft halo, draw subtle shading dots
                const Cell& existing = renderer.backBuffer().get(x, y);
                if (existing.ch == " " || existing.ch.empty()) {
                    std::string dot = (intensity > 0.4f) ? "·" : " ";
                    renderer.drawChar(x, y, dot, glowColor, existing.bg);
                }
            }
        }
    }
};

} // namespace festival::core
