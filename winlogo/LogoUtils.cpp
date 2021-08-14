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

void refreshLogo() noexcept {
    (void)SendMessageW(findStartButton(), WM_THEMECHANGED, 0, 0);
}

}  // namespace winlogo
