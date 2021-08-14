#include "LogoHooks.h"
#include "LogoUtils.h"

#include "Event.h"

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace winlogo {

/**
 * Check if the dll is loaded into the correct process.
 */
bool isCorrectProcess() noexcept {
    auto startButton = findStartButton();
    DWORD processId = 0;
    (void)GetWindowThreadProcessId(startButton, &processId);
    return processId == GetCurrentProcessId();
}

void hook() {
    try {
        HooksGuard hooks;
        utils::Event event(L"WinLogoEvent");
        event.wait();
        OutputDebugStringA("Stopping...");
    } catch (std::exception& err) {
        OutputDebugStringA("Got std::exception!");
        OutputDebugStringA(err.what());
    } catch (...) {
        OutputDebugStringA("Got an unknown exception!");
    }
    OutputDebugStringA("END!");
}

/**
 * The entry point for the hooker thread of the dll.
 */
DWORD __stdcall entry(void*) {
    if (isCorrectProcess()) {
        hook();
    }
    FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(&__ImageBase), 0);
}

}  // namespace winlogo

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, [[maybe_unused]] void* lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hInstance);
        HANDLE thread = CreateThread(nullptr, 0, winlogo::entry, nullptr, 0, nullptr);
        if (thread == nullptr) {
            return FALSE;
        }
        CloseHandle(thread);
    }
    return TRUE;
}
