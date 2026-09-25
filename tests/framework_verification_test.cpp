#include "../festival/core/types.hpp"
#include "../festival/core/color/color.hpp"
#include "../festival/core/text/unicode_width.hpp"
#include "../festival/core/renderer/buffer.hpp"
#include "../festival/core/renderer/renderer.hpp"
#include "../festival/core/animation/easing.hpp"
#include "../festival/core/animation/tween.hpp"
#include "../festival/core/particle/particle_system.hpp"
#include "../festival/core/effects/firework.hpp"
#include "../festival/core/effects/starfield.hpp"
#include "../festival/core/effects/glow.hpp"
#include "../festival/themes/mid_autumn/scenes/mid_autumn_scene.hpp"
#include <iostream>
#include <cassert>
#include <vector>

using namespace festival::core;
using namespace festival::themes::mid_autumn;

void testUnicodeWidth() {
    std::cout << "[Test 1] Testing Unicode & CJK display width..." << std::endl;
    assert(stringVisualWidth("Hello") == 5);
    assert(stringVisualWidth("海上生明月") == 10);
    assert(stringVisualWidth("2026年丙午中秋") == 14); // 4 + 5*2 = 14
    assert(stringVisualWidth("举杯邀明月，对影成三人。") == 24);
    assert(stringVisualWidth("但愿人长久，千里共婵娟。") == 24);
    assert(stringVisualWidth("愿望：希望今年顺利保研") == 22);
    assert(stringVisualWidth("·") == 1 || stringVisualWidth("·") == 2);
    auto glyphs = splitGlyphs("中秋2026");
    assert(glyphs.size() == 6);
    assert(glyphs[0].visualWidth == 2);
    assert(glyphs[1].visualWidth == 2);
    assert(glyphs[2].visualWidth == 1);
    std::cout << "  -> Unicode width tests PASSED." << std::endl;
}

void testColorDegradation() {
    std::cout << "[Test 2] Testing Color system & degradation..." << std::endl;
    Color c1(255, 0, 0);
    assert(c1.toAnsi(ColorMode::TrueColor, false) == "\033[38;2;255;0;0m");
    assert(c1.to256Index() > 0);
    assert(c1.to16Index() == 1 || c1.to16Index() == 9);

    Color mid = Color::lerp(Color(0, 0, 0), Color(100, 200, 50), 0.5f);
    assert(mid.r == 50 && mid.g == 100 && mid.b == 25);

    Color blended = Color::blend(Color(0, 0, 0), Color(255, 255, 255), 0.5f);
    assert(blended.r > 120 && blended.r < 135);
    std::cout << "  -> Color degradation tests PASSED." << std::endl;
}

void testDoubleBuffer() {
    std::cout << "[Test 3] Testing Buffer & Wide character placement..." << std::endl;
    Buffer buf(40, 20);
    Cell cjkCell("月", 2, Color(255, 255, 255));
    buf.set(10, 5, cjkCell);

    const Cell& placed = buf.get(10, 5);
    assert(placed.ch == "月");
    assert(placed.width == 2);

    const Cell& cont = buf.get(11, 5);
    assert(cont.isContinuation == true);

    // Boundary clipping
    buf.set(-5, 0, cjkCell);
    buf.set(100, 100, cjkCell);
    std::cout << "  -> Buffer & Wide character tests PASSED." << std::endl;
}

void testTweenAndEasing() {
    std::cout << "[Test 4] Testing Animation & Tween..." << std::endl;
    bool callbackCalled = false;
    Tween<float> tween(0.0f, 100.0f, 1.0f, EasingType::EaseInOut);
    tween.setOnComplete([&]() { callbackCalled = true; });

    assert(tween.getValue() == 0.0f);
    tween.update(0.5f);
    assert(tween.getValue() > 40.0f && tween.getValue() < 60.0f);
    tween.update(0.6f);
    assert(tween.isDone());
    assert(tween.getValue() == 100.0f);
    assert(callbackCalled);
    std::cout << "  -> Tween and Easing tests PASSED." << std::endl;
}

void testParticleSystem() {
    std::cout << "[Test 5] Testing Particle System..." << std::endl;
    ParticleSystem ps(50);
    EmitterConfig cfg;
    cfg.type = EmitterType::Radial;
    cfg.emissionRate = 100.0f;
    cfg.minLife = 0.5f;
    cfg.maxLife = 1.0f;
    auto emitter = std::make_shared<ParticleEmitter>(cfg);
    ps.addEmitter(emitter);

    for (int i = 0; i < 30; ++i) {
        ps.update(0.05f, 80, 24);
        assert(ps.getParticleCount() <= 50); // Bound strictly enforced
    }
    std::cout << "  -> Particle System tests PASSED." << std::endl;
}

void testFireworkEffect() {
    std::cout << "[Test 6] Testing Firework Lifecycle..." << std::endl;
    FireworkConfig cfg;
    cfg.startPos = {40.0f, 20.0f};
    cfg.targetHeight = 5.0f;
    cfg.riseSpeed = 30.0f;
    cfg.particleLife = 0.5f;
    Firework fw(cfg);

    assert(fw.phase() == FireworkPhase::Launching);
    // Ascend until explode
    while (fw.phase() == FireworkPhase::Launching) {
        fw.update(0.05f);
    }
    assert(fw.phase() == FireworkPhase::Exploding);

    // Fade until finished
    int safety = 0;
    while (!fw.isFinished() && safety++ < 200) {
        fw.update(0.05f);
    }
    assert(fw.isFinished());
    std::cout << "  -> Firework Lifecycle tests PASSED." << std::endl;
}

void testFullSceneSimulation() {
    std::cout << "[Test 7] Testing Full Mid-Autumn Scene 300-frame simulation..." << std::endl;
    MidAutumnScene scene;
    scene.enter();

    Renderer renderer;
    // Simulate 300 frames (~10 seconds of festival timeline)
    for (int f = 0; f < 300; ++f) {
        scene.update(0.033f);
        scene.render(renderer);
    }

    // Test input handling
    InputEvent evM; evM.code = KeyCode::Char; evM.ch = 'm';
    scene.handleInput(evM);

    InputEvent evR; evR.code = KeyCode::Char; evR.ch = 'r';
    scene.handleInput(evR);

    InputEvent evL; evL.code = KeyCode::Char; evL.ch = 'l';
    scene.handleInput(evL);

    InputEvent evP; evP.code = KeyCode::Char; evP.ch = 'p';
    scene.handleInput(evP);

    InputEvent evF; evF.code = KeyCode::Char; evF.ch = 'f';
    scene.handleInput(evF);

    for (int f = 0; f < 30; ++f) {
        scene.update(0.033f);
        scene.render(renderer);
    }

    scene.exit();
    std::cout << "  -> Full Scene Simulation tests PASSED." << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " Running Framework Verification Tests   " << std::endl;
    std::cout << "========================================" << std::endl;

    testUnicodeWidth();
    testColorDegradation();
    testDoubleBuffer();
    testTweenAndEasing();
    testParticleSystem();
    testFireworkEffect();
    testFullSceneSimulation();

    std::cout << "\nALL 7 VERIFICATION SUITES PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
