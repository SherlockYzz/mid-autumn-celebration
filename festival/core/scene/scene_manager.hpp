#pragma once

#include "scene.hpp"
#include "../renderer/renderer.hpp"
#include "../platform/input_event.hpp"
#include "../animation/tween.hpp"
#include <memory>
#include <utility>

namespace festival::core {

enum class TransitionState {
    None,
    FadeOut,
    FadeIn
};

class SceneManager {
public:
    SceneManager() = default;

    void setScene(std::shared_ptr<Scene> newScene, float transitionDuration = 0.0f) {
        if (transitionDuration <= 0.001f || !m_currentScene) {
            // Immediate switch
            if (m_currentScene) {
                m_currentScene->exit();
            }
            m_currentScene = std::move(newScene);
            if (m_currentScene) {
                m_currentScene->enter();
            }
            m_transitionState = TransitionState::None;
        } else {
            // Animated fade transition
            m_pendingScene = std::move(newScene);
            m_halfTransitionDuration = transitionDuration * 0.5f;
            m_transitionTween = Tween<float>(0.0f, 1.0f, m_halfTransitionDuration, EasingType::EaseInOut);
            m_transitionState = TransitionState::FadeOut;
        }
    }

    std::shared_ptr<Scene> currentScene() const {
        return m_currentScene;
    }

    void handleInput(const InputEvent& event) {
        if (m_transitionState == TransitionState::FadeOut) {
            return; // Ignore input during fade out
        }
        if (m_currentScene) {
            m_currentScene->handleInput(event);
        }
    }

    void update(float dt) {
        if (m_transitionState != TransitionState::None) {
            m_transitionTween.update(dt);

            if (m_transitionTween.isDone()) {
                if (m_transitionState == TransitionState::FadeOut) {
                    if (m_currentScene) {
                        m_currentScene->exit();
                    }
                    m_currentScene = std::move(m_pendingScene);
                    if (m_currentScene) {
                        m_currentScene->enter();
                    }
                    m_transitionState = TransitionState::FadeIn;
                    m_transitionTween = Tween<float>(1.0f, 0.0f, m_halfTransitionDuration, EasingType::EaseInOut);
                } else if (m_transitionState == TransitionState::FadeIn) {
                    m_transitionState = TransitionState::None;
                }
            }
        }

        if (m_currentScene) {
            m_currentScene->update(dt);
        }
    }

    void render(Renderer& renderer) {
        if (m_currentScene) {
            m_currentScene->render(renderer);
        }

        // Apply fade overlay if in transition
        if (m_transitionState != TransitionState::None) {
            float alpha = m_transitionTween.getValue();
            if (alpha > 0.02f) {
                // Dim screen with dark overlay
                for (int y = 0; y < renderer.height(); ++y) {
                    for (int x = 0; x < renderer.width(); ++x) {
                        Cell& c = renderer.backBuffer().get(x, y);
                        c.fg = Color::blend(Color(0, 0, 0), c.fg, 1.0f - alpha);
                        c.bg = Color::blend(Color(0, 0, 0), c.bg, 1.0f - alpha);
                    }
                }
            }
        }
    }

private:
    std::shared_ptr<Scene> m_currentScene;
    std::shared_ptr<Scene> m_pendingScene;
    TransitionState m_transitionState = TransitionState::None;
    Tween<float> m_transitionTween;
    float m_halfTransitionDuration = 0.5f;
};

} // namespace festival::core
