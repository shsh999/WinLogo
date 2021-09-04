#include "LogoHooks.h"
#include "LogoUtils.h"
#include "Resource.h"
#include "HookedFunction.h"

#include <Uxtheme.h>
#include <optional>
#include <unordered_set>

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace winlogo {

using hooks::HookedFunction;
using pe_parser::PEFile;

inline std::unordered_set<HTHEME> g_logoThemeHandle{};
inline HBITMAP g_logoDisabledImage = nullptr;
inline HBITMAP g_logoEnabledImage = nullptr;
inline bool g_isLogoEnabled = true;

static constexpr auto UXTHEME = "uxtheme.dll";

static constexpr auto LOGO_THEME_NAME = L"TaskbarPearl";


HBITMAP loadImage(int resource) {
    return reinterpret_cast<HBITMAP>(LoadImageW(reinterpret_cast<HMODULE>(&__ImageBase),
                                                MAKEINTRESOURCEW(resource), IMAGE_BITMAP, 0, 0,
                                                LR_CREATEDIBSECTION));
}

HTHEME newOpenThemeDataForDpi(decltype(OpenThemeDataForDpi) original, HWND hwnd,
                              LPCWSTR pszClassList, UINT dpi) {
    // If opening the handle for the start logo, save it in the global state
    auto newHandle = original(hwnd, pszClassList, dpi);
    if (_wcsicmp(pszClassList, LOGO_THEME_NAME) == 0) {
        (void)g_logoThemeHandle.insert(newHandle);
    }
    return newHandle;
}

HRESULT newCloseThemeData(decltype(CloseThemeData) original, HTHEME hTheme) {
    (void)g_logoThemeHandle.erase(hTheme);
    return original(hTheme);
}

std::pair<LONG, LONG> getImageDimensions(HBITMAP hBmp) {
    BITMAP bmp;
    GetObject(hBmp, sizeof(bmp), &bmp);
    return std::pair(bmp.bmWidth, bmp.bmHeight);
}

HRESULT newDrawThemeBackground(decltype(DrawThemeBackground) original, HTHEME hTheme, HDC hdc,
                               int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect) {
    if (g_logoThemeHandle.find(hTheme) != g_logoThemeHandle.cend()) {
        // Skip the drawing of the disabled logo...
        g_isLogoEnabled = false;
        return S_OK;
    }
    return original(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

void drawImage(HDC hdc, HWND targetWindow, HBITMAP image) {
    RECT rect{};
    GetClientRect(targetWindow, &rect);

    HDC mem = CreateCompatibleDC(hdc);
    auto old = SelectObject(mem, image);
    auto [width, height] = getImageDimensions(image);
    auto startX = ((rect.right + rect.left) - width) / 2;
    auto startY = ((rect.bottom + rect.top) - height) / 2;
    BitBlt(hdc, startX, startY, width, height, mem, 0, 0, SRCPAINT);
    SelectObject(mem, old);
    DeleteDC(mem);
}

HRESULT newEndBufferedPaint(decltype(EndBufferedPaint) original, HPAINTBUFFER hBufferedPaint,
                            BOOL fUpdateTarget) {
    auto targetDC = GetBufferedPaintTargetDC(hBufferedPaint);
    auto targetWindow = WindowFromDC(targetDC);

   const auto allStartButtons = findAllStartButtons();

    if (!g_logoThemeHandle.empty() && allStartButtons.find(targetWindow) != allStartButtons.cend()) {
        auto hdc = GetBufferedPaintDC(hBufferedPaint);
        HBITMAP image = g_isLogoEnabled ? g_logoEnabledImage : g_logoDisabledImage;
        drawImage(hdc, targetWindow, image);
        g_isLogoEnabled = true;
    }

    return original(hBufferedPaint, fUpdateTarget);
}

#define UXTHEME_HOOK(name) \
    PEFile::currentExecutable(), #name, HookedFunction<name, new##name>, UXTHEME

namespace details {

LogoHooks::LogoHooks()
    : m_uxTheme(UXTHEME),
      m_openThemeDataForDpi(UXTHEME_HOOK(OpenThemeDataForDpi)),
      m_closeThemeData(UXTHEME_HOOK(CloseThemeData)),
      m_drawThemeBackground(UXTHEME_HOOK(DrawThemeBackground)),
      m_endBufferedPaint(UXTHEME_HOOK(EndBufferedPaint)) {
    refreshLogo();
}

}  // namespace details

#undef UXTHEME_HOOK

HooksGuard::HooksGuard() {
    if (s_hooks.has_value()) {
        throw HooksAlreadyInstalledError();
    }
    initImages();
    s_hooks.emplace();
}

void HooksGuard::destroyImages() noexcept {
    if (g_logoDisabledImage) {
        DeleteObject(g_logoDisabledImage);
        g_logoDisabledImage = nullptr;
    }

    if (g_logoEnabledImage) {
        DeleteObject(g_logoEnabledImage);
        g_logoEnabledImage = nullptr;
    }
}

void HooksGuard::initImages() {
    g_logoDisabledImage = loadImage(IDB_LOGO_DISABLED);
    if (!g_logoDisabledImage) {
        throw std::exception("Bad logo disabled image!");
    }

    g_logoEnabledImage = loadImage(IDB_LOGO_ENABLED);
    if (!g_logoEnabledImage) {
        destroyImages();
        throw std::exception("Bad logo enabled image!");
    }
}


HooksGuard::~HooksGuard() {
    // Remove the hooks and tell explorer that it should refresh (and then redraw)
    s_hooks.reset();
    refreshLogo();
    
    // Make sure there is no thread running in the hooks context, and then destroy the images
    Sleep(1000);
    destroyImages();
}

std::optional<details::LogoHooks> HooksGuard::s_hooks = std::nullopt;

}  // namespace winlogo
