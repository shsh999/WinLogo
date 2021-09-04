#include "LogoUtils.h"

namespace winlogo {

HWND findStartButton() noexcept {
    static HWND start = nullptr;
    if (!start) {
        HWND taskBar = FindWindowW(L"Shell_TrayWnd", nullptr);
        start = FindWindowExW(taskBar, nullptr, L"Start", nullptr);
    }
    return start;
}

std::unordered_set<HWND> findAllStartButtons() {
    std::unordered_set<HWND> result{findStartButton()};
    HWND taskBar = FindWindowExW(nullptr, nullptr, L"Shell_SecondaryTrayWnd", nullptr);
    while (taskBar != nullptr) {
        HWND start = FindWindowExW(taskBar, nullptr, L"Start", nullptr);
        if (start != nullptr) {
            (void)result.insert(start);
        }
        taskBar = FindWindowExW(nullptr, taskBar, L"Shell_SecondaryTrayWnd", nullptr);
    }
    return result;
}

void refreshLogo() {
    for (auto startWindow : findAllStartButtons()) {
        (void)SendMessageW(startWindow, WM_THEMECHANGED, 0, 0);
    }
}

}  // namespace winlogo
