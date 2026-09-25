#pragma once

#include <string>

namespace festival::core {

enum class KeyCode {
    None,
    Unknown,
    Char,
    Enter,
    Escape,
    Space,
    Backspace,
    ArrowUp,
    ArrowDown,
    ArrowLeft,
    ArrowRight,
    Tab
};

struct InputEvent {
    KeyCode code = KeyCode::None;
    char ch = 0;
    std::string text; // UTF-8 text string (supports CJK/Chinese characters!)
    bool ctrl = false;
    bool alt = false;

    bool isChar(char c) const {
        if (code != KeyCode::Char) return false;
        char target = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if (ch != 0 && static_cast<char>(std::tolower(static_cast<unsigned char>(ch))) == target) {
            return true;
        }
        if (!text.empty() && static_cast<char>(std::tolower(static_cast<unsigned char>(text[0]))) == target) {
            return true;
        }
        return false;
    }

    bool isQuit() const {
        return code == KeyCode::Escape || isChar('q');
    }
};

} // namespace festival::core
