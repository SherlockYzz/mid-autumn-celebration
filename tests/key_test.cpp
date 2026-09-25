#include "festival/core/platform/input_event.hpp"
#include "festival/themes/mid_autumn/scenes/mid_autumn_scene.hpp"
#include <iostream>
#include <cassert>

using namespace festival::core;
using namespace festival::themes::mid_autumn;

int main() {
    MidAutumnScene scene;
    scene.enter();

    std::cout << "Testing SPACE key..." << std::endl;
    InputEvent evSpace;
    evSpace.code = KeyCode::Space;
    scene.handleInput(evSpace);

    std::cout << "Testing R key..." << std::endl;
    InputEvent evR;
    evR.code = KeyCode::Char;
    evR.ch = 'r';
    scene.handleInput(evR);

    std::cout << "Testing G key..." << std::endl;
    InputEvent evG;
    evG.code = KeyCode::Char;
    evG.ch = 'g';
    scene.handleInput(evG);

    std::cout << "Testing L key..." << std::endl;
    InputEvent evL;
    evL.code = KeyCode::Char;
    evL.ch = 'l';
    scene.handleInput(evL);

    std::cout << "Testing P key..." << std::endl;
    InputEvent evP;
    evP.code = KeyCode::Char;
    evP.ch = 'p';
    scene.handleInput(evP);

    std::cout << "Testing W key..." << std::endl;
    InputEvent evW;
    evW.code = KeyCode::Char;
    evW.ch = 'w';
    scene.handleInput(evW);

    std::cout << "Testing N key..." << std::endl;
    InputEvent evN;
    evN.code = KeyCode::Char;
    evN.ch = 'n';
    scene.handleInput(evN);

    std::cout << "Testing C key..." << std::endl;
    InputEvent evC;
    evC.code = KeyCode::Char;
    evC.ch = 'c';
    scene.handleInput(evC);

    std::cout << "Testing H key..." << std::endl;
    InputEvent evH;
    evH.code = KeyCode::Char;
    evH.ch = 'h';
    scene.handleInput(evH);

    std::cout << "All direct key handlers executed without crash!" << std::endl;
    return 0;
}
