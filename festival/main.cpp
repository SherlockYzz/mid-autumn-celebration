#include "core/platform/platform.hpp"
#include "core/renderer/renderer.hpp"
#include "core/scene/scene_manager.hpp"
#include "themes/mid_autumn/scenes/mid_autumn_scene.hpp"
#include "themes/national_day/scenes/national_day_scene.hpp"
#include "themes/spring_festival/scenes/spring_festival_scene.hpp"
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>

using namespace festival::core;

int main() {
    // 1. Initialize Platform (VT100 ANSI, raw input mode, UTF-8 codepage, alternate screen)
    Platform::instance().initialize();

    // 2. Initialize Core Renderer and Scene Manager
    Renderer renderer;
    SceneManager sceneManager;

    // 3. Multi-Festival Scenes Collection
    std::vector<std::shared_ptr<Scene>> festivalScenes = {
        std::make_shared<festival::themes::mid_autumn::MidAutumnScene>(),
        std::make_shared<festival::themes::national_day::NationalDayScene>(),
        std::make_shared<festival::themes::spring_festival::SpringFestivalScene>()
    };

    size_t currentFestivalIdx = 0;
    sceneManager.setScene(festivalScenes[currentFestivalIdx]);

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
            // Check for Festival Switch Keys: [TAB] or [1], [2], [3]
            if (event->code == KeyCode::Tab) {
                currentFestivalIdx = (currentFestivalIdx + 1) % festivalScenes.size();
                sceneManager.setScene(festivalScenes[currentFestivalIdx], 0.3f);
                continue;
            } else if (event->isChar('1')) {
                currentFestivalIdx = 0;
                sceneManager.setScene(festivalScenes[currentFestivalIdx], 0.3f);
                continue;
            } else if (event->isChar('2')) {
                currentFestivalIdx = 1;
                sceneManager.setScene(festivalScenes[currentFestivalIdx], 0.3f);
                continue;
            } else if (event->isChar('3')) {
                currentFestivalIdx = 2;
                sceneManager.setScene(festivalScenes[currentFestivalIdx], 0.3f);
                continue;
            }

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

    std::cout << "\n「四时佳节，万家安康。」 中华华节盛典交互系统感谢您的体验！祝您诸事顺遂，岁序常新！\n" << std::endl;
    return 0;
}
