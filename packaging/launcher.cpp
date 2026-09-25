#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>

// "宵月良宵 · 中秋交互庆典"
static const wchar_t* kTitle = L"\u5BB5\u6708\u826F\u5BB5 \u00B7 \u4E2D\u79CB\u4EA4\u4E92\u5E86\u5178";

static std::wstring ExePath() {
    wchar_t buf[32768];
    DWORD n = GetModuleFileNameW(nullptr, buf, 32768);
    if (n == 0) return std::wstring();
    return std::wstring(buf, n);
}

static std::wstring DirOf(const std::wstring& p) {
    size_t pos = p.find_last_of(L"\\/");
    if (pos == std::wstring::npos) return std::wstring();
    return p.substr(0, pos);
}

static bool FileExists(const std::wstring& p) {
    DWORD a = GetFileAttributesW(p.c_str());
    return a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

// 把传统控制台窗口调整到推荐尺寸（110 x 32）
static void EnsureConsoleSize(int cols, int rows) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE || h == nullptr) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(h, &csbi)) return;

    int curCols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int curRows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    if (curCols >= cols && curRows >= rows) return;

    int bufCols = (cols > curCols) ? cols : curCols;
    int bufRows = (rows > curRows) ? rows : curRows;

    SMALL_RECT tiny = {0, 0, 0, 0};
    SetConsoleWindowInfo(h, TRUE, &tiny);

    COORD buf = {(SHORT)bufCols, (SHORT)bufRows};
    SetConsoleScreenBufferSize(h, buf);

    SMALL_RECT fit = {0, 0, (SHORT)(cols - 1), (SHORT)(rows - 1)};
    SetConsoleWindowInfo(h, TRUE, &fit);
}

static void Pause() {
    std::system("pause >nul");
}

int wmain() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    SetConsoleTitleW(kTitle);

    std::wstring self = ExePath();
    std::wstring dir  = DirOf(self);
    std::wstring game = dir + L"\\festival_app.exe";

    bool inWindowsTerminal = (GetEnvironmentVariableW(L"WT_SESSION", nullptr, 0) > 0);
    if (!inWindowsTerminal) {
        EnsureConsoleSize(110, 32);
    }

    if (!FileExists(game)) {
        std::wprintf(L"\n\u627E\u4E0D\u5230 festival_app.exe\uFF0C\u8BF7\u91CD\u65B0\u5B89\u88C5\u3002\n");
        Pause();
        return 1;
    }

    std::wstring cmd = L"\"" + game + L"\"";
    std::vector<wchar_t> mut(cmd.begin(), cmd.end());
    mut.push_back(0);

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessW(nullptr, mut.data(), nullptr, nullptr, FALSE, 0, nullptr, dir.c_str(), &si, &pi)) {
        std::wprintf(L"\n\u542F\u52A8\u5931\u8D25\u3002\n");
        Pause();
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    std::wprintf(L"\n\u6309\u4EFB\u610F\u952E\u5173\u95ED\u7A97\u53E3\u2026\n");
    Pause();
    return 0;
}