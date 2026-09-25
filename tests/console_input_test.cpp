#include "festival/core/platform/platform.hpp"
#include <iostream>
#include <cassert>

using namespace festival::core;

int main() {
    Platform::instance().initialize();
    std::cout << "Testing real Windows Console Input Record simulation..." << std::endl;

#if defined(_WIN32)
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    assert(hIn != INVALID_HANDLE_VALUE);

    auto testKey = [&](char c, WORD vk, const std::string& name) {
        INPUT_RECORD ir{};
        ir.EventType = KEY_EVENT;
        ir.Event.KeyEvent.bKeyDown = TRUE;
        ir.Event.KeyEvent.wVirtualKeyCode = vk;
        ir.Event.KeyEvent.uChar.UnicodeChar = static_cast<WCHAR>(c);
        DWORD written = 0;
        BOOL ok = WriteConsoleInputW(hIn, &ir, 1, &written);
        assert(ok && written == 1);

        auto ev = Platform::instance().pollInput();
        if (!ev.has_value()) {
            std::cerr << "FAILED: pollInput() returned nullopt for key " << name << "!" << std::endl;
            assert(false);
        }
        if (!ev->isChar(c)) {
            std::cerr << "FAILED: ev is not char '" << c << "'! code=" << (int)ev->code << ", ch=" << ev->ch << ", text=" << ev->text << std::endl;
            assert(false);
        }
        std::cout << "  Key '" << name << "' -> detected successfully! ch=" << ev->ch << ", text=" << ev->text << std::endl;
    };

    testKey('h', 'H', "H");
    testKey('n', 'N', "N");
    testKey('c', 'C', "C");
    testKey('r', 'R', "R");
    testKey('l', 'L', "L");
    testKey('g', 'G', "G");
    testKey('p', 'P', "P");
    testKey('w', 'W', "W");
    testKey('f', 'F', "F");
    testKey('q', 'Q', "Q");

    std::cout << "\nALL 10 CONSOLE KEYS PASSED WITH FLYING COLORS!" << std::endl;
#endif

    Platform::instance().shutdown();
    return 0;
}
