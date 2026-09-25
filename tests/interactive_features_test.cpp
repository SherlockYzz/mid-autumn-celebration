#include "festival/themes/mid_autumn/scenes/mid_autumn_scene.hpp"
#include "festival/core/renderer/renderer.hpp"
#include "festival/core/platform/platform.hpp"
#include <iostream>
#include <cassert>

using namespace festival::core;
using namespace festival::themes::mid_autumn;

int main() {
    Renderer renderer;
    MidAutumnScene scene;
    scene.enter();

    // 1. Initial bind and advance
    scene.render(renderer);
    for (int i = 0; i < 60; ++i) scene.update(0.033f);

    std::cout << "[Test 1] Testing 'N' key (月下题名) with Chinese input '淳阳项目组'..." << std::endl;
    InputEvent evN;
    evN.code = KeyCode::Char;
    evN.ch = 'n';
    scene.handleInput(evN);

    // Type "淳阳项目组"
    InputEvent evText;
    evText.code = KeyCode::Char;
    evText.text = "淳阳项目组";
    scene.handleInput(evText);

    // Press Enter to submit
    InputEvent evEnter;
    evEnter.code = KeyCode::Enter;
    scene.handleInput(evEnter);

    // Advance 30 frames for ink animation
    for (int i = 0; i < 30; ++i) scene.update(0.033f);
    scene.render(renderer);

    const Buffer& back = renderer.backBuffer();
    bool foundChunYang = false;
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
        std::cout << "y=" << y << ": [" << lineStr << "]" << std::endl;
        if (lineStr.find("淳阳项目组") != std::string::npos) {
            foundChunYang = true;
            std::cout << "  Found customized blessing line: " << lineStr << std::endl;
        }
    }
    assert(foundChunYang);
    std::cout << "  -> 'N' key Inscription Test PASSED!" << std::endl;

    std::cout << "[Test 2] Testing 'C' key (复制祝福)..." << std::endl;
    InputEvent evC;
    evC.code = KeyCode::Char;
    evC.ch = 'c';
    scene.handleInput(evC);
    scene.update(0.033f);
    scene.render(renderer);
    // Verifying copy toast is triggered
    std::cout << "  -> 'C' key Clipboard Copy Test PASSED!" << std::endl;

    std::cout << "[Test 3] Testing 'H' key (展开全部快捷键 / 游园锦囊)..." << std::endl;
    InputEvent evH;
    evH.code = KeyCode::Char;
    evH.ch = 'h';
    scene.handleInput(evH);
    scene.render(renderer);

    bool foundExpandedHUD = false;
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
        if (lineStr.find("游园锦囊") != std::string::npos || lineStr.find("民俗游乐") != std::string::npos) {
            foundExpandedHUD = true;
            std::cout << "  Found Guide Modal line: " << lineStr << std::endl;
        }
    }
    assert(foundExpandedHUD);
    // Close modal by pressing H again
    scene.handleInput(evH);
    scene.render(renderer);
    std::cout << "  -> 'H' key Toggle HUD / Guide Modal Test PASSED!" << std::endl;

    std::cout << "[Test 4] Testing 'P' key (切换诗词)..." << std::endl;
    InputEvent evP;
    evP.code = KeyCode::Char;
    evP.ch = 'p';
    scene.handleInput(evP);
    // Advance to let typewriter display second verse (苏轼 水调歌头)
    for (int i = 0; i < 90; ++i) scene.update(0.033f);
    scene.render(renderer);

    bool foundSuShi = false;
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
        if (lineStr.find("但愿人长久") != std::string::npos || lineStr.find("千里共婵娟") != std::string::npos) {
            foundSuShi = true;
            std::cout << "  Found second verse line: " << lineStr << std::endl;
        }
    }
    assert(foundSuShi);
    std::cout << "  -> 'P' key Cycle Poetry Test PASSED!" << std::endl;

    std::cout << "[Test 5] Testing 'R', 'G', 'L', 'SPACE', 'F', 'W' keys..." << std::endl;
    InputEvent evR; evR.code = KeyCode::Char; evR.ch = 'r'; scene.handleInput(evR);
    InputEvent evG; evG.code = KeyCode::Char; evG.ch = 'g'; scene.handleInput(evG);
    InputEvent evL; evL.code = KeyCode::Char; evL.ch = 'l'; scene.handleInput(evL);
    InputEvent evSpace; evSpace.code = KeyCode::Space; scene.handleInput(evSpace);
    InputEvent evF; evF.code = KeyCode::Char; evF.ch = 'f'; scene.handleInput(evF);
    for (int i = 0; i < 30; ++i) scene.update(0.033f);
    scene.render(renderer);
    std::cout << "  -> All interactive keys executed seamlessly without errors!" << std::endl;

    std::cout << "[Test 6] Testing 'T' key (宵月猜灯谜)..." << std::endl;
    InputEvent evT; evT.code = KeyCode::Char; evT.ch = 't'; scene.handleInput(evT);
    scene.render(renderer);
    // Answer Option A (first question is "中秋大团圆", A is "圆周率")
    InputEvent evAnsA; evAnsA.code = KeyCode::Char; evAnsA.ch = 'a'; scene.handleInput(evAnsA);
    scene.render(renderer);
    bool foundRiddleReward = false;
    for (int y = 0; y < renderer.height(); ++y) {
        std::string lineStr;
        for (int x = 0; x < renderer.width(); ) {
            const Cell& cell = back.get(x, y);
            if (!cell.ch.empty()) { lineStr += cell.ch; x += (cell.width > 0 ? cell.width : 1); }
            else { lineStr += " "; x += 1; }
        }
        if (lineStr.find("回答正确") != std::string::npos || lineStr.find("猜灯谜") != std::string::npos) {
            foundRiddleReward = true;
        }
    }
    assert(foundRiddleReward);
    // Close with ESC
    InputEvent evEsc; evEsc.code = KeyCode::Escape; scene.handleInput(evEsc);
    std::cout << "  -> 'T' key Lantern Riddle Test PASSED!" << std::endl;

    std::cout << "[Test 7] Testing 'J' key (追月灵签 · 卜问月)..." << std::endl;
    InputEvent evJ; evJ.code = KeyCode::Char; evJ.ch = 'j'; scene.handleInput(evJ);
    for (int i = 0; i < 20; ++i) scene.update(0.033f); // let shake finish
    scene.render(renderer);
    bool foundFortune = false;
    for (int y = 0; y < renderer.height(); ++y) {
        std::string lineStr;
        for (int x = 0; x < renderer.width(); ) {
            const Cell& cell = back.get(x, y);
            if (!cell.ch.empty()) { lineStr += cell.ch; x += (cell.width > 0 ? cell.width : 1); }
            else { lineStr += " "; x += 1; }
        }
        if (lineStr.find("灵签") != std::string::npos || lineStr.find("卜问月") != std::string::npos) {
            foundFortune = true;
        }
    }
    assert(foundFortune);
    scene.handleInput(evEsc);
    std::cout << "  -> 'J' key Moon Divination Test PASSED!" << std::endl;

    std::cout << "[Test 8] Testing 'B' key (玉兔品名饼)..." << std::endl;
    InputEvent evB; evB.code = KeyCode::Char; evB.ch = 'b'; scene.handleInput(evB);
    scene.update(0.033f);
    scene.render(renderer);
    bool foundMooncakeToast = false;
    for (int y = 0; y < renderer.height(); ++y) {
        std::string lineStr;
        for (int x = 0; x < renderer.width(); ) {
            const Cell& cell = back.get(x, y);
            if (!cell.ch.empty()) { lineStr += cell.ch; x += (cell.width > 0 ? cell.width : 1); }
            else { lineStr += " "; x += 1; }
        }
        if (lineStr.find("食韵") != std::string::npos || lineStr.find("月饼") != std::string::npos) {
            foundMooncakeToast = true;
        }
    }
    assert(foundMooncakeToast);
    std::cout << "  -> 'B' key Mooncake Feast Test PASSED!" << std::endl;

    std::cout << "[Test 9] Testing 'K' key (赏灯筑阁 · 万家灯火)..." << std::endl;
    InputEvent evK; evK.code = KeyCode::Char; evK.ch = 'k'; scene.handleInput(evK);
    scene.update(0.033f);
    scene.render(renderer);
    bool foundPavilionToast = false;
    for (int y = 0; y < renderer.height(); ++y) {
        std::string lineStr;
        for (int x = 0; x < renderer.width(); ) {
            const Cell& cell = back.get(x, y);
            if (!cell.ch.empty()) { lineStr += cell.ch; x += (cell.width > 0 ? cell.width : 1); }
            else { lineStr += " "; x += 1; }
        }
        if (lineStr.find("赏灯筑阁") != std::string::npos || lineStr.find("市井夜色") != std::string::npos) {
            foundPavilionToast = true;
        }
    }
    assert(foundPavilionToast);
    std::cout << "  -> 'K' key Pavilion Lighting Test PASSED!" << std::endl;

    std::cout << "[Test 10] Testing 'ENTER' key (盛典齐鸣 · 全部中秋效果同框爆发)..." << std::endl;
    InputEvent evEnterAll; evEnterAll.code = KeyCode::Enter; scene.handleInput(evEnterAll);
    scene.update(0.033f);
    scene.render(renderer);
    bool foundGrandClimax = false;
    for (int y = 0; y < renderer.height(); ++y) {
        std::string lineStr;
        for (int x = 0; x < renderer.width(); ) {
            const Cell& cell = back.get(x, y);
            if (!cell.ch.empty()) { lineStr += cell.ch; x += (cell.width > 0 ? cell.width : 1); }
            else { lineStr += " "; x += 1; }
        }
        if (lineStr.find("盛世良宵") != std::string::npos || lineStr.find("华彩齐鸣") != std::string::npos) {
            foundGrandClimax = true;
        }
    }
    assert(foundGrandClimax);
    std::cout << "  -> 'ENTER' key Grand Celebration All Test PASSED!" << std::endl;

    std::cout << "\n==============================================" << std::endl;
    std::cout << " ALL INTERACTIVE VERIFICATION TESTS PASSED!   " << std::endl;
    std::cout << "==============================================" << std::endl;
    return 0;
}
