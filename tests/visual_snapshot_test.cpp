#include "festival/themes/mid_autumn/scenes/mid_autumn_scene.hpp"
#include "festival/core/renderer/renderer.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

using namespace festival::core;
using namespace festival::themes::mid_autumn;

int main() {
    Renderer renderer;
    // Test with standard 80x24 terminal
    MidAutumnScene scene;
    scene.enter();

    // Initial render call to bind renderer dimensions
    scene.render(renderer);

    // Fast-forward at 30 FPS for 5 seconds into interactive stage
    for (int i = 0; i < 150; ++i) {
        scene.update(0.033f);
    }
    scene.render(renderer);

    std::cout << "Terminal size: " << renderer.width() << "x" << renderer.height() << std::endl;

    std::ofstream out("tests/snapshot_output.txt");
    const Buffer& back = renderer.backBuffer();
    bool foundBlessing = false;
    bool foundRabbit = false;
    bool foundPoetry = false;

    for (int y = 0; y < renderer.height(); ++y) {
        std::string lineStr;
        for (int x = 0; x < renderer.width(); ) {
            const Cell& cell = back.get(x, y);
            if (!cell.ch.empty()) {
                lineStr += cell.ch;
                x += (cell.width > 0 ? cell.width : 1);
            } else {
                lineStr += " ";
                x += 1;
            }
        }
        out << lineStr << "\n";

        if (lineStr.find("中秋快乐") != std::string::npos) {
            foundBlessing = true;
        }
        if (lineStr.find("海上生明月") != std::string::npos || lineStr.find("天涯共此时") != std::string::npos) {
            foundPoetry = true;
        }
    }
    out.close();

    std::cout << "Snapshot saved to tests/snapshot_output.txt" << std::endl;
    std::cout << "Blessing found: " << (foundBlessing ? "YES" : "NO") << std::endl;
    std::cout << "Poetry found: " << (foundPoetry ? "YES" : "NO") << std::endl;

    assert(foundBlessing);
    assert(foundPoetry);

    std::cout << "Visual snapshot test PASSED!" << std::endl;
    return 0;
}
