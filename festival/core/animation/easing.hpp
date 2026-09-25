#pragma once

#include <cmath>
#include <algorithm>
#include "../types.hpp"

namespace festival::core {

enum class EasingType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    EaseInCubic,
    EaseOutCubic,
    Pulse,
    BounceOut
};

inline float easeLinear(float t) {
    return std::clamp(t, 0.0f, 1.0f);
}

inline float easeInQuad(float t) {
    float ct = std::clamp(t, 0.0f, 1.0f);
    return ct * ct;
}

inline float easeOutQuad(float t) {
    float ct = std::clamp(t, 0.0f, 1.0f);
    return ct * (2.0f - ct);
}

inline float easeInOutQuad(float t) {
    float ct = std::clamp(t, 0.0f, 1.0f);
    return ct < 0.5f ? 2.0f * ct * ct : -1.0f + (4.0f - 2.0f * ct) * ct;
}

inline float easeInCubic(float t) {
    float ct = std::clamp(t, 0.0f, 1.0f);
    return ct * ct * ct;
}

inline float easeOutCubic(float t) {
    float ct = std::clamp(t, 0.0f, 1.0f) - 1.0f;
    return ct * ct * ct + 1.0f;
}

inline float easeInOutSine(float t) {
    float ct = std::clamp(t, 0.0f, 1.0f);
    return -(std::cos(PI * ct) - 1.0f) * 0.5f;
}

// Sinusoidal pulse: smooth rise and fall 0 -> 1 -> 0 over range [0, 1]
inline float easePulse(float t) {
    float ct = std::clamp(t, 0.0f, 1.0f);
    return (1.0f - std::cos(TWO_PI * ct)) * 0.5f;
}

inline float easeBounceOut(float t) {
    float ct = std::clamp(t, 0.0f, 1.0f);
    float n1 = 7.5625f;
    float d1 = 2.75f;

    if (ct < 1.0f / d1) {
        return n1 * ct * ct;
    } else if (ct < 2.0f / d1) {
        ct -= 1.5f / d1;
        return n1 * ct * ct + 0.75f;
    } else if (ct < 2.5f / d1) {
        ct -= 2.25f / d1;
        return n1 * ct * ct + 0.9375f;
    } else {
        ct -= 2.625f / d1;
        return n1 * ct * ct + 0.984375f;
    }
}

inline float applyEasing(EasingType type, float t) {
    switch (type) {
        case EasingType::Linear: return easeLinear(t);
        case EasingType::EaseIn: return easeInQuad(t);
        case EasingType::EaseOut: return easeOutQuad(t);
        case EasingType::EaseInOut: return easeInOutQuad(t);
        case EasingType::EaseInCubic: return easeInCubic(t);
        case EasingType::EaseOutCubic: return easeOutCubic(t);
        case EasingType::Pulse: return easePulse(t);
        case EasingType::BounceOut: return easeBounceOut(t);
    }
    return t;
}

} // namespace festival::core
