#pragma once

#include "IATHook.h"
#include "ModuleHandle.h"
#include "PEFile.h"

#include <optional>

namespace winlogo {

class HooksAlreadyInstalledError : public std::logic_error {
public:
    HooksAlreadyInstalledError() : std::logic_error("Hooks are already installed!") {
        // Intentionally left blank.
    }
};

class ImageLoadError : public std::runtime_error {

};

namespace details {
class LogoHooks final {
public:
    LogoHooks();

private:
    utils::ModuleHandle m_uxTheme;
    hooks::IATHook m_openThemeDataForDpi;
    hooks::IATHook m_closeThemeData;
    hooks::IATHook m_drawThemeBackground;
    hooks::IATHook m_endBufferedPaint;
};
}

/**
 * This class is used to initialize the WinLogo hooks.
 * Instantiating the class twice will lead to an exception!
 */
class HooksGuard {
public:
    HooksGuard();
    ~HooksGuard();

private:
    static std::optional<details::LogoHooks> s_hooks;

    static void initImages();
    static void destroyImages() noexcept;
};


}  // namespace winlogo
