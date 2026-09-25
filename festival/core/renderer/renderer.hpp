#pragma once

#include "buffer.hpp"
#include "../color/color.hpp"
#include "../platform/platform.hpp"
#include "../text/unicode_width.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace festival::core {

class Renderer {
public:
    Renderer() {
        auto [w, h] = Platform::instance().getTerminalSize();
        m_width = w;
        m_height = h;
        m_frontBuffer.resize(w, h);
        m_backBuffer.resize(w, h);
        m_colorMode = Platform::instance().getColorMode();
        m_forceRedraw = true;
    }

    int width() const { return m_width; }
    int height() const { return m_height; }

    ColorMode getColorMode() const { return m_colorMode; }
    void setColorMode(ColorMode mode) {
        m_colorMode = mode;
        m_forceRedraw = true;
    }

    void handleResize() {
        auto [w, h] = Platform::instance().getTerminalSize();
        if (w != m_width || h != m_height) {
            m_width = w;
            m_height = h;
            m_frontBuffer.resize(w, h);
            m_backBuffer.resize(w, h);
            m_forceRedraw = true;
        }
    }

    void clear(const Color& bg = Color(0, 0, 0, 255)) {
        m_backBuffer.clear(bg);
    }

    void drawChar(int x, int y, std::string_view ch, const Color& fg, const Color& bg = Color(0, 0, 0, 0), bool bold = false) {
        if (!m_backBuffer.inBounds(x, y)) return;
        auto [cp, len] = decodeNextUtf8(ch, 0);
        int w = codepointVisualWidth(cp);
        Cell cell(std::string(ch), w, fg, bg, bold);
        m_backBuffer.set(x, y, cell);
    }

    void drawText(int x, int y, std::string_view text, const Color& fg, const Color& bg = Color(0, 0, 0, 0), bool bold = false) {
        if (y < 0 || y >= m_height) return;

        size_t pos = 0;
        int curX = x;
        while (pos < text.size() && curX < m_width) {
            auto [cp, len] = decodeNextUtf8(text, pos);
            if (len == 0) break;
            int w = codepointVisualWidth(cp);

            if (curX >= 0 && curX < m_width) {
                std::string glyphStr(text.substr(pos, len));
                Cell cell(glyphStr, w, fg, bg, bold);
                m_backBuffer.set(curX, y, cell);
            }

            curX += (w > 0 ? w : 1);
            pos += len;
        }
    }

    void drawCenterText(int y, std::string_view text, const Color& fg, const Color& bg = Color(0, 0, 0, 0), bool bold = false) {
        int textWidth = stringVisualWidth(text);
        int startX = std::max(0, (m_width - textWidth) / 2);
        drawText(startX, y, text, fg, bg, bold);
    }

    int pixelWidth() const { return m_width; }
    int pixelHeight() const { return m_height * 2; }

    void drawPixel(int px, int py, const Color& color) {
        if (px < 0 || px >= m_width || py < 0 || py >= m_height * 2) return;
        int cx = px;
        int cy = py / 2;
        const Cell& existing = m_backBuffer.get(cx, cy);

        if (existing.ch != "▀") {
            Color base = (existing.bg.a > 0) ? existing.bg : Color(0, 0, 0, 255);
            Cell newCell("▀", 1, base, base, false);
            m_backBuffer.set(cx, cy, newCell);
        }

        Cell& cell = m_backBuffer.get(cx, cy);
        if (py % 2 == 0) {
            cell.fg = (color.a < 255) ? Color::blend(cell.fg, color, color.a / 255.0f) : color;
        } else {
            cell.bg = (color.a < 255) ? Color::blend(cell.bg, color, color.a / 255.0f) : color;
        }
    }

    void drawPixelCircle(float cx, float cy, float radius, const Color& color, bool fill = true) {
        int minX = std::max(0, static_cast<int>(std::floor(cx - radius)));
        int maxX = std::min(m_width - 1, static_cast<int>(std::ceil(cx + radius)));
        int minY = std::max(0, static_cast<int>(std::floor(cy - radius)));
        int maxY = std::min(m_height * 2 - 1, static_cast<int>(std::ceil(cy + radius)));

        float r2 = radius * radius;
        for (int py = minY; py <= maxY; ++py) {
            for (int px = minX; px <= maxX; ++px) {
                float dx = px - cx;
                float dy = py - cy;
                float d2 = dx * dx + dy * dy;
                if (fill) {
                    if (d2 <= r2) {
                        drawPixel(px, py, color);
                    }
                } else {
                    if (std::abs(std::sqrt(d2) - radius) <= 0.6f) {
                        drawPixel(px, py, color);
                    }
                }
            }
        }
    }

    void drawPixelRect(int px, int py, int pw, int ph, const Color& color) {
        for (int y = py; y < py + ph; ++y) {
            for (int x = px; x < px + pw; ++x) {
                drawPixel(x, y, color);
            }
        }
    }

    void drawPixelLine(int x0, int y0, int x1, int y1, const Color& color) {
        int dx = std::abs(x1 - x0);
        int dy = -std::abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;

        while (true) {
            drawPixel(x0, y0, color);
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }

    void fillRect(int x, int y, int w, int h, const Color& bg, std::string_view fillCh = " ", const Color& fg = Color(255, 255, 255)) {
        Cell cell(std::string(fillCh), 1, fg, bg, false);
        m_backBuffer.fillRect(x, y, w, h, cell);
    }

    void drawBorder(int x, int y, int w, int h, const Color& color, bool doubleLine = false) {
        if (w < 2 || h < 2) return;

        bool unicode = Platform::instance().supportsUnicode();
        std::string horiz = unicode ? (doubleLine ? "═" : "─") : "-";
        std::string vert  = unicode ? (doubleLine ? "║" : "│") : "|";
        std::string tl    = unicode ? (doubleLine ? "╔" : "┌") : "+";
        std::string tr    = unicode ? (doubleLine ? "╗" : "┐") : "+";
        std::string bl    = unicode ? (doubleLine ? "╚" : "└") : "+";
        std::string br    = unicode ? (doubleLine ? "╝" : "┘") : "+";

        drawChar(x, y, tl, color);
        drawChar(x + w - 1, y, tr, color);
        drawChar(x, y + h - 1, bl, color);
        drawChar(x + w - 1, y + h - 1, br, color);

        for (int i = 1; i < w - 1; ++i) {
            drawChar(x + i, y, horiz, color);
            drawChar(x + i, y + h - 1, horiz, color);
        }

        for (int j = 1; j < h - 1; ++j) {
            drawChar(x, y + j, vert, color);
            drawChar(x + w - 1, y + j, vert, color);
        }
    }

    void present() {
        handleResize();

        std::string output;
        output.reserve(m_width * m_height * 8);

        if (m_forceRedraw) {
            output += "\033[2J"; // Clear entire screen on first frame or resize
            m_forceRedraw = false;
        }

        Color lastFg(0, 0, 0, 0);
        Color lastBg(0, 0, 0, 0);
        bool lastBold = false;

        for (int y = 0; y < m_height; ++y) {
            // Check if this line has any changes compared to frontBuffer
            bool lineChanged = false;
            for (int x = 0; x < m_width; ++x) {
                if (m_backBuffer.get(x, y) != m_frontBuffer.get(x, y)) {
                    lineChanged = true;
                    break;
                }
            }
            if (!lineChanged) continue;

            // Move cursor to start of this line and stream sequentially across the entire width
            // This guarantees CJK wide characters are never split or corrupted by partial cell jumps!
            output += "\033[" + std::to_string(y + 1) + ";1H";

            for (int x = 0; x < m_width; ++x) {
                const Cell& cell = m_backBuffer.get(x, y);
                if (cell.isContinuation) {
                    continue; // Already displayed by the left half of the wide character
                }

                // Update styling attributes
                if (cell.bold != lastBold) {
                    output += cell.bold ? "\033[1m" : "\033[22m";
                    lastBold = cell.bold;
                }

                // Update FG Color
                if (cell.fg != lastFg) {
                    output += cell.fg.toAnsi(m_colorMode, false);
                    lastFg = cell.fg;
                }

                // Update BG Color
                if (cell.bg != lastBg) {
                    output += cell.bg.toAnsi(m_colorMode, true);
                    lastBg = cell.bg;
                }

                output += cell.ch;
            }
        }

        if (!output.empty()) {
            output += "\033[0m"; // Reset styling at end
            outputConsole(output);
        }

        // Swap buffers
        std::swap(m_frontBuffer, m_backBuffer);
    }

    Buffer& backBuffer() { return m_backBuffer; }
    const Buffer& backBuffer() const { return m_backBuffer; }

private:
    void outputConsole(const std::string& utf8Str) {
#if defined(_WIN32)
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &mode)) {
            int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), static_cast<int>(utf8Str.size()), NULL, 0);
            if (wlen > 0) {
                std::wstring wstr(wlen, L'\0');
                MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), static_cast<int>(utf8Str.size()), wstr.data(), wlen);
                DWORD written = 0;
                WriteConsoleW(hOut, wstr.data(), static_cast<DWORD>(wlen), &written, NULL);
                return;
            }
        }
#endif
        std::cout << utf8Str << std::flush;
    }

    int m_width = 80;
    int m_height = 24;
    Buffer m_frontBuffer;
    Buffer m_backBuffer;
    ColorMode m_colorMode = ColorMode::TrueColor;
    bool m_forceRedraw = true;
};

} // namespace festival::core
