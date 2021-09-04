#include "LogoHooks.h"
#include "LogoUtils.h"

#include "CaseInsensitiveString.h"
#include "ModuleHandle.h"

#include <filesystem>

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace winlogo {

/**
 * Check if the dll is loaded into the correct process.
 */
bool shouldHookCurrentProcess() noexcept {
    auto startButton = findStartButton();
    DWORD processId = 0;
    (void)GetWindowThreadProcessId(startButton, &processId);
    return processId == GetCurrentProcessId();
}

static std::optional<HooksGuard> g_guards = std::nullopt;

void hook() {
    try {
        g_guards.emplace();
    } catch (std::exception& err) {
        OutputDebugStringA("WinLogo: Got std::exception!");
        OutputDebugStringA(err.what());
    } catch (...) {
        OutputDebugStringA("WinLogo: Got an unknown exception!");
    }
}

/**
 * The entry point for the hooker thread of the dll.
 */
DWORD __stdcall entry(void*) {
    hook();
    return 0;
}

utils::ci_wstring_view VALID_PROCESS_NAMES[] = {L"regsvr32.exe", L"explorer.exe"};

bool isValidProcess() {
    try {
        auto name = std::filesystem::path(utils::ModuleHandle::getModulePath(nullptr)).filename().wstring();
        for (auto validName : VALID_PROCESS_NAMES) {
            if (winlogo::utils::ci_wstring_view(name.c_str(), name.size()) == validName) {
                return true;
            }
        }
    } catch (...) {
        // Intentionally left blank.
    }

    return false;
}


}  // namespace winlogo

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, [[maybe_unused]] void* lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hInstance);
        if (!winlogo::isValidProcess()) {
            return FALSE;
        }

        if (!winlogo::shouldHookCurrentProcess()) {
            // We are loaded but don't need to hook - regsvr32, for example.
            return TRUE;
        }

        // Hook in another thread...
        HANDLE thread = CreateThread(nullptr, 0, winlogo::entry, nullptr, 0, nullptr);
        if (thread == nullptr) {
            return FALSE;
        }

        CloseHandle(thread);
    }

    return TRUE;
}
