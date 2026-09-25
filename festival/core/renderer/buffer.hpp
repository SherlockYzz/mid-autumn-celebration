#pragma once

#include "cell.hpp"
#include <vector>
#include <algorithm>

namespace festival::core {

class Buffer {
public:
    Buffer() : m_width(0), m_height(0) {}
    Buffer(int w, int h) { resize(w, h); }

    void resize(int w, int h) {
        if (w == m_width && h == m_height) return;
        m_width = std::max(1, w);
        m_height = std::max(1, h);
        m_cells.assign(m_width * m_height, Cell(" ", 1, Color(255, 255, 255)));
    }

    int width() const { return m_width; }
    int height() const { return m_height; }

    void clear(const Color& bgColor = Color(0, 0, 0, 255)) {
        Cell clearCell(" ", 1, Color(255, 255, 255), bgColor);
        std::fill(m_cells.begin(), m_cells.end(), clearCell);
    }

    bool inBounds(int x, int y) const {
        return x >= 0 && x < m_width && y >= 0 && y < m_height;
    }

    const Cell& get(int x, int y) const {
        static Cell s_dummy;
        if (!inBounds(x, y)) return s_dummy;
        return m_cells[y * m_width + x];
    }

    Cell& get(int x, int y) {
        static Cell s_dummy;
        if (!inBounds(x, y)) return s_dummy;
        return m_cells[y * m_width + x];
    }

    void set(int x, int y, const Cell& cell) {
        if (!inBounds(x, y)) return;

        int idx = y * m_width + x;

        // If target cell was a continuation of a wide char on its left, clear the left cell
        if (m_cells[idx].isContinuation && x > 0) {
            m_cells[idx - 1] = Cell(" ", 1, cell.fg, cell.bg);
        }

        // If target cell was a wide character, clear its continuation on the right
        if (m_cells[idx].width == 2 && x + 1 < m_width) {
            m_cells[idx + 1] = Cell(" ", 1, cell.fg, cell.bg);
        }

        if (cell.width == 2) {
            if (x + 1 < m_width) {
                // If the right cell was also part of another wide char, clear its companion
                if (m_cells[idx + 1].width == 2 && x + 2 < m_width) {
                    m_cells[idx + 2] = Cell(" ", 1, cell.fg, cell.bg);
                }
                m_cells[idx] = cell;
                m_cells[idx + 1] = Cell::continuation(cell.fg, cell.bg);
            } else {
                // Cannot place 2-column char at the rightmost column; write space instead
                m_cells[idx] = Cell(" ", 1, cell.fg, cell.bg);
            }
        } else {
            // Normal 1-column cell or continuation
            m_cells[idx] = cell;
        }
    }

    void fillRect(int x, int y, int w, int h, const Cell& cell) {
        int x0 = std::max(0, x);
        int y0 = std::max(0, y);
        int x1 = std::min(m_width, x + w);
        int y1 = std::min(m_height, y + h);

        for (int cy = y0; cy < y1; ++cy) {
            for (int cx = x0; cx < x1; ++cx) {
                set(cx, cy, cell);
            }
        }
    }

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<Cell> m_cells;
};

} // namespace festival::core
