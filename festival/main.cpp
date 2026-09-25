#include "core/platform/platform.hpp"
#include "core/renderer/renderer.hpp"
#include "core/scene/scene_manager.hpp"
#include "themes/mid_autumn/scenes/mid_autumn_scene.hpp"
#include <chrono>
#include <thread>
#include <iostream>

using namespace festival::core;
using namespace festival::themes::mid_autumn;

int main() {
    // 1. Initialize Platform (VT100 ANSI, raw input mode, UTF-8 codepage, alternate screen)
    Platform::instance().initialize();

    // 2. Initialize Core Renderer and Scene Manager
    Renderer renderer;
    SceneManager sceneManager;

    // 3. Load 2026 Mid-Autumn Theme Scene
    auto midAutumnScene = std::make_shared<MidAutumnScene>();
    sceneManager.setScene(midAutumnScene);

    // Target ~30 FPS frame timing
    constexpr int targetFps = 30;
    constexpr auto targetFrameDuration = std::chrono::milliseconds(1000 / targetFps);

    auto lastTime = std::chrono::high_resolution_clock::now();

    // 4. Main Event & Animation Loop
    while (!Platform::instance().isInterrupted()) {
        auto frameStart = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(frameStart - lastTime).count();
        lastTime = frameStart;

        // Cap dt to prevent physics explosion during window drags/resizes
        if (dt > 0.1f) dt = 0.1f;

        // Poll non-blocking keyboard input
        while (auto event = Platform::instance().pollInput()) {
            sceneManager.handleInput(*event);
            if (Platform::instance().isInterrupted()) break;
        }

        if (Platform::instance().isInterrupted()) break;

        // Update active scene
        sceneManager.update(dt);

        // Render scene to back buffer
        sceneManager.render(renderer);

        // Present double buffer to terminal (flicker-free diff output)
        renderer.present();

        // Regulate frame rate to ~30 FPS to minimize CPU usage
        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
        if (frameElapsed < targetFrameDuration) {
            std::this_thread::sleep_for(targetFrameDuration - frameElapsed);
        }
    }

    // 5. Restore terminal state gracefully on exit
    Platform::instance().shutdown();

    std::cout << "\n「但愿人长久，千里共婵娟。」 岁次丙午中秋，祝君清辉常伴，顺遂安康。\n" << std::endl;
    return 0;
}
