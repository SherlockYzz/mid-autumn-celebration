#pragma once

#include "../color/color.hpp"
#include <string>

namespace festival::core {

struct Cell {
    std::string ch = " ";
    int width = 1;
    Color fg = Color(255, 255, 255);
    Color bg = Color(0, 0, 0, 0); // alpha = 0 means transparent
    bool bold = false;
    bool isContinuation = false; // Right half of a wide (CJK) character

    Cell() = default;

    Cell(std::string inCh, int inW, const Color& inFg, const Color& inBg = Color(0, 0, 0, 0), bool inBold = false)
        : ch(std::move(inCh)), width(inW), fg(inFg), bg(inBg), bold(inBold), isContinuation(false) {}

    static Cell continuation(const Color& inFg, const Color& inBg = Color(0, 0, 0, 0)) {
        Cell c("", 0, inFg, inBg);
        c.isContinuation = true;
        return c;
    }

    bool operator==(const Cell& o) const {
        if (isContinuation != o.isContinuation) return false;
        if (isContinuation && o.isContinuation) return true;
        return width == o.width &&
               ch == o.ch &&
               fg == o.fg &&
               bg == o.bg &&
               bold == o.bold;
    }

    bool operator!=(const Cell& o) const {
        return !(*this == o);
    }
};

} // namespace festival::core
