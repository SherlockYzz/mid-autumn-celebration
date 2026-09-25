#pragma once

#include "input_event.hpp"
#include "../color/color.hpp"
#include <optional>
#include <string>
#include <cstdlib>
#include <atomic>
#include <iostream>

#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <conio.h>
#else
    #include <termios.h>
    #include <sys/ioctl.h>
    #include <unistd.h>
    #include <signal.h>
    #include <poll.h>
#endif

namespace festival::core {

class Platform {
public:
    static Platform& instance() {
        static Platform s_instance;
        return s_instance;
    }

    void initialize() {
        if (m_initialized) return;
        m_interrupted = false;

#if defined(_WIN32)
        m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        m_hIn = GetStdHandle(STD_INPUT_HANDLE);

        // Store original modes
        if (m_hOut != INVALID_HANDLE_VALUE) {
            GetConsoleMode(m_hOut, &m_origOutMode);
            DWORD outMode = m_origOutMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
            SetConsoleMode(m_hOut, outMode);
        }

        if (m_hIn != INVALID_HANDLE_VALUE) {
            GetConsoleMode(m_hIn, &m_origInMode);
            DWORD inMode = ENABLE_EXTENDED_FLAGS;
            SetConsoleMode(m_hIn, inMode);
        }

        m_origOutCP = GetConsoleOutputCP();
        m_origInCP = GetConsoleCP();
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        SetConsoleCtrlHandler(consoleCtrlHandler, TRUE);
#else
        tcgetattr(STDIN_FILENO, &m_origTermios);
        struct termios raw = m_origTermios;
        raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
        raw.c_iflag &= ~(IXON | ICRNL);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

        struct sigaction sa{};
        sa.sa_handler = posixSignalHandler;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGINT, &sa, nullptr);
        sigaction(SIGTERM, &sa, nullptr);
#endif

        // Enter alternate screen buffer & hide cursor
        std::cout << "\033[?1049h\033[?25l" << std::flush;

        detectCapabilities();
        m_initialized = true;
    }

    void shutdown() {
        if (!m_initialized) return;

        // Reset color, show cursor, leave alternate screen buffer
        std::cout << "\033[0m\033[?25h\033[?1049l" << std::flush;

#if defined(_WIN32)
        if (m_hOut != INVALID_HANDLE_VALUE) {
            SetConsoleMode(m_hOut, m_origOutMode);
        }
        if (m_hIn != INVALID_HANDLE_VALUE) {
            SetConsoleMode(m_hIn, m_origInMode);
        }
        if (m_origOutCP != 0) {
            SetConsoleOutputCP(m_origOutCP);
        }
        if (m_origInCP != 0) {
            SetConsoleCP(m_origInCP);
        }
#else
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &m_origTermios);
#endif

        m_initialized = false;
    }

    ~Platform() {
        shutdown();
    }

    std::pair<int, int> getTerminalSize() const {
        int width = 80;
        int height = 24;

#if defined(_WIN32)
        if (m_hOut != INVALID_HANDLE_VALUE) {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (GetConsoleScreenBufferInfo(m_hOut, &csbi)) {
                width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            }
        }
#else
        struct winsize ws{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0 && ws.ws_row > 0) {
            width = ws.ws_col;
            height = ws.ws_row;
        }
#endif

        return {std::max(20, width), std::max(10, height)};
    }

    ColorMode getColorMode() const {
        return m_colorMode;
    }

    void setColorMode(ColorMode mode) {
        m_colorMode = mode;
    }

    bool supportsUnicode() const {
        return m_supportsUnicode;
    }

    bool isInterrupted() const {
        return m_interrupted.load();
    }

    void requestStop() {
        m_interrupted.store(true);
    }

    bool copyToClipboard(const std::string& utf8Text) {
#if defined(_WIN32)
        if (!OpenClipboard(NULL)) return false;
        EmptyClipboard();

        int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(), -1, NULL, 0);
        if (wlen <= 0) {
            CloseClipboard();
            return false;
        }

        HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, wlen * sizeof(wchar_t));
        if (!hGlob) {
            CloseClipboard();
            return false;
        }

        wchar_t* wstr = static_cast<wchar_t*>(GlobalLock(hGlob));
        if (wstr) {
            MultiByteToWideChar(CP_UTF8, 0, utf8Text.c_str(), -1, wstr, wlen);
            GlobalUnlock(hGlob);
            SetClipboardData(CF_UNICODETEXT, hGlob);
        } else {
            GlobalFree(hGlob);
        }

        CloseClipboard();
        return true;
#else
        return false;
#endif
    }

    std::optional<InputEvent> pollInput() {
#if defined(_WIN32)
        if (m_hIn != INVALID_HANDLE_VALUE) {
            DWORD numEvents = 0;
            while (GetNumberOfConsoleInputEvents(m_hIn, &numEvents) && numEvents > 0) {
                INPUT_RECORD ir{};
                DWORD readCount = 0;
                if (!ReadConsoleInputW(m_hIn, &ir, 1, &readCount) || readCount == 0) {
                    break;
                }

                if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
                    WORD vk = ir.Event.KeyEvent.wVirtualKeyCode;
                    WCHAR wch = ir.Event.KeyEvent.uChar.UnicodeChar;
                    InputEvent ev;

                    if (vk == VK_ESCAPE) {
                        ev.code = KeyCode::Escape;
                        return ev;
                    } else if (vk == VK_RETURN) {
                        ev.code = KeyCode::Enter;
                        return ev;
                    } else if (vk == VK_SPACE) {
                        ev.code = KeyCode::Space;
                        return ev;
                    } else if (vk == VK_BACK) {
                        ev.code = KeyCode::Backspace;
                        return ev;
                    } else if (vk == VK_TAB) {
                        ev.code = KeyCode::Tab;
                        return ev;
                    } else if (vk == VK_UP) {
                        ev.code = KeyCode::ArrowUp;
                        return ev;
                    } else if (vk == VK_DOWN) {
                        ev.code = KeyCode::ArrowDown;
                        return ev;
                    } else if (vk == VK_LEFT) {
                        ev.code = KeyCode::ArrowLeft;
                        return ev;
                    } else if (vk == VK_RIGHT) {
                        ev.code = KeyCode::ArrowRight;
                        return ev;
                    } else if (wch != 0) {
                        if (wch == 3) { // Ctrl+C
                            m_interrupted = true;
                            ev.code = KeyCode::Escape;
                            return ev;
                        }
                        ev.code = KeyCode::Char;
                        std::string utf8;
                        if (wch < 0x80) {
                            utf8.push_back(static_cast<char>(wch));
                            ev.ch = static_cast<char>(wch);
                        } else if (wch < 0x800) {
                            utf8.push_back(static_cast<char>(0xC0 | ((wch >> 6) & 0x1F)));
                            utf8.push_back(static_cast<char>(0x80 | (wch & 0x3F)));
                            ev.ch = 0;
                        } else {
                            utf8.push_back(static_cast<char>(0xE0 | ((wch >> 12) & 0x0F)));
                            utf8.push_back(static_cast<char>(0x80 | ((wch >> 6) & 0x3F)));
                            utf8.push_back(static_cast<char>(0x80 | (wch & 0x3F)));
                            ev.ch = 0;
                        }
                        ev.text = utf8;
                        return ev;
                    } else if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9')) {
                        ev.code = KeyCode::Char;
                        ev.ch = static_cast<char>(std::tolower(vk));
                        ev.text = std::string(1, ev.ch);
                        return ev;
                    }
                }
            }

            // Fallback for CRT console buffer (e.g. various terminal emulators)
            if (_kbhit()) {
                wint_t wc = _getwch();
                InputEvent ev;
                if (wc == 27) {
                    ev.code = KeyCode::Escape;
                    return ev;
                } else if (wc == 13 || wc == 10) {
                    ev.code = KeyCode::Enter;
                    return ev;
                } else if (wc == 32) {
                    ev.code = KeyCode::Space;
                    return ev;
                } else if (wc == 8) {
                    ev.code = KeyCode::Backspace;
                    return ev;
                } else if (wc == 0 || wc == 0xE0) {
                    wint_t ext = _getwch();
                    if (ext == 72) { ev.code = KeyCode::ArrowUp; return ev; }
                    if (ext == 80) { ev.code = KeyCode::ArrowDown; return ev; }
                    if (ext == 75) { ev.code = KeyCode::ArrowLeft; return ev; }
                    if (ext == 77) { ev.code = KeyCode::ArrowRight; return ev; }
                } else {
                    ev.code = KeyCode::Char;
                    std::string utf8;
                    if (wc < 0x80) {
                        utf8.push_back(static_cast<char>(wc));
                        ev.ch = static_cast<char>(wc);
                    } else if (wc < 0x800) {
                        utf8.push_back(static_cast<char>(0xC0 | ((wc >> 6) & 0x1F)));
                        utf8.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
                        ev.ch = 0;
                    } else {
                        utf8.push_back(static_cast<char>(0xE0 | ((wc >> 12) & 0x0F)));
                        utf8.push_back(static_cast<char>(0x80 | ((wc >> 6) & 0x3F)));
                        utf8.push_back(static_cast<char>(0x80 | (wc & 0x3F)));
                        ev.ch = 0;
                    }
                    ev.text = utf8;
                    return ev;
                }
            }

            // Support piped stdin (headless runs / testing)
            DWORD bytesAvail = 0;
            if (PeekNamedPipe(m_hIn, NULL, 0, NULL, &bytesAvail, NULL) && bytesAvail > 0) {
                char ch = 0;
                DWORD bytesRead = 0;
                if (ReadFile(m_hIn, &ch, 1, &bytesRead, NULL) && bytesRead > 0) {
                    InputEvent ev;
                    if (ch == 27 || ch == 'q' || ch == 'Q') {
                        ev.code = KeyCode::Escape;
                        return ev;
                    } else if (ch == '\r' || ch == '\n') {
                        ev.code = KeyCode::Enter;
                        return ev;
                    } else if (ch == ' ') {
                        ev.code = KeyCode::Space;
                        return ev;
                    } else {
                        ev.code = KeyCode::Char;
                        ev.ch = ch;
                        return ev;
                    }
                }
            }
        }
#else
        struct pollfd pfd{};
        pfd.fd = STDIN_FILENO;
        pfd.events = POLLIN;
        if (poll(&pfd, 1, 0) > 0 && (pfd.revents & POLLIN)) {
            char ch = 0;
            if (read(STDIN_FILENO, &ch, 1) > 0) {
                InputEvent ev;
                if (ch == 27) { // Escape or escape sequence
                    // Check if more characters follow
                    if (poll(&pfd, 1, 10) > 0 && (pfd.revents & POLLIN)) {
                        char seq[2];
                        if (read(STDIN_FILENO, &seq[0], 1) > 0 && seq[0] == '[') {
                            if (read(STDIN_FILENO, &seq[1], 1) > 0) {
                                switch (seq[1]) {
                                    case 'A': ev.code = KeyCode::ArrowUp; return ev;
                                    case 'B': ev.code = KeyCode::ArrowDown; return ev;
                                    case 'C': ev.code = KeyCode::ArrowRight; return ev;
                                    case 'D': ev.code = KeyCode::ArrowLeft; return ev;
                                }
                            }
                        }
                    }
                    ev.code = KeyCode::Escape;
                    return ev;
                } else if (ch == '\n' || ch == '\r') {
                    ev.code = KeyCode::Enter;
                    return ev;
                } else if (ch == ' ') {
                    ev.code = KeyCode::Space;
                    return ev;
                } else if (ch == 127 || ch == 8) {
                    ev.code = KeyCode::Backspace;
                    return ev;
                } else if (ch == '\t') {
                    ev.code = KeyCode::Tab;
                    return ev;
                } else if (ch == 3) { // Ctrl+C
                    m_interrupted = true;
                    ev.code = KeyCode::Escape;
                    return ev;
                } else {
                    ev.code = KeyCode::Char;
                    ev.ch = ch;
                    return ev;
                }
            }
        }
#endif
        return std::nullopt;
    }

private:
    Platform() = default;

    void detectCapabilities() {
        // Detect Color capability
        const char* colorTerm = std::getenv("COLORTERM");
        const char* term = std::getenv("TERM");
        const char* wtSession = std::getenv("WT_SESSION");
        const char* termProgram = std::getenv("TERM_PROGRAM");

        bool trueColor = false;
        if (colorTerm && (std::string(colorTerm) == "truecolor" || std::string(colorTerm) == "24bit")) {
            trueColor = true;
        } else if (wtSession != nullptr || (termProgram && std::string(termProgram) == "vscode")) {
            trueColor = true;
        }

#if defined(_WIN32)
        // Modern Windows 10/11 console host supports 24-bit TrueColor
        trueColor = true;
#endif

        if (trueColor) {
            m_colorMode = ColorMode::TrueColor;
        } else if (term && std::string(term).find("256color") != std::string::npos) {
            m_colorMode = ColorMode::Ansi256;
        } else {
            m_colorMode = ColorMode::Ansi16;
        }

        // Detect Unicode capability
        m_supportsUnicode = true;
        const char* lang = std::getenv("LANG");
        const char* lcAll = std::getenv("LC_ALL");
        if (lang && std::string(lang) == "C") {
            m_supportsUnicode = false;
        }
        if (lcAll && std::string(lcAll) == "C") {
            m_supportsUnicode = false;
        }
    }

#if defined(_WIN32)
    static BOOL WINAPI consoleCtrlHandler(DWORD dwCtrlType) {
        if (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_BREAK_EVENT || dwCtrlType == CTRL_CLOSE_EVENT) {
            instance().m_interrupted = true;
            return TRUE;
        }
        return FALSE;
    }

    HANDLE m_hOut = INVALID_HANDLE_VALUE;
    HANDLE m_hIn = INVALID_HANDLE_VALUE;
    DWORD m_origOutMode = 0;
    DWORD m_origInMode = 0;
    UINT m_origOutCP = 0;
    UINT m_origInCP = 0;
#else
    static void posixSignalHandler(int sig) {
        if (sig == SIGINT || sig == SIGTERM) {
            instance().m_interrupted = true;
        }
    }

    struct termios m_origTermios{};
#endif

    bool m_initialized = false;
    std::atomic<bool> m_interrupted{false};
    ColorMode m_colorMode = ColorMode::TrueColor;
    bool m_supportsUnicode = true;
};

} // namespace festival::core
