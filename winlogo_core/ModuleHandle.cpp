#include "Common.h"
#include "ModuleHandle.h"

#include <string>

using namespace std::string_literals;

namespace winlogo::utils {

ModuleHandle::ModuleHandle(const char* moduleName) : m_module(LoadLibraryA(moduleName), false) {
    if (m_module.get() == nullptr) {
        throw ModuleNotFoundError(moduleName);
    }
}

ModuleHandle::ModuleHandle(std::string_view moduleName)
    : ModuleHandle(std::string(moduleName).c_str()) {
    // Intentionally left blank.
}

ModuleHandle::ModuleHandle(const char* moduleName, FailIfNotLoaded)
    : m_module(getLoadedModuleHandle(moduleName)) {
    // Intentionally left blank.
}

ModuleHandle::ModuleHandle(std::string_view moduleName, FailIfNotLoaded)
    : m_module(getLoadedModuleHandle(std::string(moduleName).c_str())) {
    // Intentionally left blank.
}

void* ModuleHandle::baseAddress() const noexcept {
    return m_module.get();
}

std::wstring ModuleHandle::getPath() const {
    return getModulePath(m_module.get());
}

std::wstring ModuleHandle::getModulePath(HMODULE moduleHandle) {
    std::wstring result(MAX_PATH, L'\0');

    const auto length =
        GetModuleFileNameW(moduleHandle, result.data(), static_cast<DWORD>(result.size()));
    if (length == 0 || (length == result.size() && GetLastError() == ERROR_INSUFFICIENT_BUFFER)) {
        throw WindowsError("Error getting file name");
    }

    result.resize(length);
    return result;
}

HMODULE ModuleHandle::getLoadedModuleHandle(const char* moduleName) {
    HMODULE result = nullptr;
    if (!GetModuleHandleExA(0, moduleName, &result)) {
        throw ModuleNotFoundError(moduleName);
    }

    return result;
}

ModuleNotFoundError::ModuleNotFoundError(std::string_view moduleName)
    : WindowsError("Error loading module "s + moduleName.data()) {
    // Intentionally left blank.
}

}  // namespace winlogo::utils
