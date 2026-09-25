#include "../festival/core/types.hpp"
#include "../festival/core/color/color.hpp"
#include "../festival/core/platform/platform.hpp"
#include "../festival/core/renderer/renderer.hpp"
#include "../festival/core/animation/easing.hpp"
#include "../festival/core/animation/tween.hpp"
#include "../festival/core/particle/particle_system.hpp"
#include "../festival/core/effects/firework.hpp"
#include "../festival/core/effects/starfield.hpp"
#include "../festival/core/effects/glow.hpp"
#include "../festival/core/effects/spark.hpp"
#include "../festival/core/effects/floating_particles.hpp"
#include "../festival/core/text/text_layout.hpp"
#include "../festival/core/scene/scene_manager.hpp"
#include <iostream>

using namespace festival::core;

class GenericTestScene : public Scene {
public:
    void enter() override {
        m_firework = std::make_unique<Firework>(FireworkConfig{});
    }

    void update(float dt) override {
        if (m_firework) {
            m_firework->update(dt);
        }
    }

    void render(Renderer& renderer) override {
        renderer.clear(Color(10, 10, 25));
        renderer.drawCenterText(2, "Festival Framework Core Standalone Test", Color(255, 255, 255));
        if (m_firework) {
            m_firework->render(renderer);
        }
    }

private:
    std::unique_ptr<Firework> m_firework;
};

int main() {
    std::cout << "Compiling & Initializing Core test..." << std::endl;
    GenericTestScene scene;
    scene.enter();
    scene.update(0.016f);
    std::cout << "Core compiled and instantiated successfully!" << std::endl;
    return 0;
}
