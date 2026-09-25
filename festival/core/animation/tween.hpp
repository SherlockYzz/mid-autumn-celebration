#pragma once

#include "easing.hpp"
#include "../color/color.hpp"
#include "../types.hpp"
#include <functional>
#include <vector>

namespace festival::core {

namespace detail {
    inline float interpolate(float a, float b, float t) {
        return a + (b - a) * t;
    }

    inline Vec2 interpolate(const Vec2& a, const Vec2& b, float t) {
        return Vec2::lerp(a, b, t);
    }

    inline Color interpolate(const Color& a, const Color& b, float t) {
        return Color::lerp(a, b, t);
    }
}

template <typename T>
class Tween {
public:
    Tween() = default;

    Tween(T fromVal, T toVal, float durationSec, EasingType easing = EasingType::Linear, bool loop = false)
        : m_from(fromVal), m_to(toVal), m_duration(std::max(0.0001f, durationSec)),
          m_easing(easing), m_loop(loop), m_current(fromVal) {}

    void setFrom(T fromVal) { m_from = fromVal; }
    void setTo(T toVal) { m_to = toVal; }
    void setDuration(float d) { m_duration = std::max(0.0001f, d); }
    void setEasing(EasingType easing) { m_easing = easing; }
    void setLoop(bool loop) { m_loop = loop; }
    void setOnComplete(std::function<void()> callback) { m_onComplete = std::move(callback); }

    void reset() {
        m_elapsed = 0.0f;
        m_done = false;
        m_current = m_from;
    }

    void update(float dt) {
        if (m_done) return;

        m_elapsed += dt;
        float t = std::clamp(m_elapsed / m_duration, 0.0f, 1.0f);
        float easedT = applyEasing(m_easing, t);
        m_current = detail::interpolate(m_from, m_to, easedT);

        if (m_elapsed >= m_duration) {
            if (m_loop) {
                m_elapsed -= m_duration;
            } else {
                m_done = true;
                m_current = m_to;
                if (m_onComplete) {
                    m_onComplete();
                }
            }
        }
    }

    T getValue() const { return m_current; }
    bool isDone() const { return m_done; }
    float progress() const { return std::clamp(m_elapsed / m_duration, 0.0f, 1.0f); }

private:
    T m_from{};
    T m_to{};
    float m_duration = 1.0f;
    float m_elapsed = 0.0f;
    EasingType m_easing = EasingType::Linear;
    bool m_loop = false;
    bool m_done = false;
    T m_current{};
    std::function<void()> m_onComplete;
};

} // namespace festival::core
