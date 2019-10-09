#include "Common.h"
#include "ModuleHandle.h"

#include <string>

using namespace std::string_literals;

namespace winlogo::utils {

ModuleHandle::ModuleHandle(std::string_view moduleName)
    : m_module(LoadLibraryA(moduleName.data())) {
    if (m_module == nullptr) {
        throw ModuleNotFoundError(moduleName);
    }
}

ModuleHandle::ModuleHandle(std::string_view moduleName, FailIfNotLoaded) {
    if (!GetModuleHandleExA(0, moduleName.data(), &m_module)) {
        throw ModuleNotFoundError(moduleName);
    }
}

ModuleHandle::ModuleHandle(ModuleHandle&& other) noexcept
    : m_module(std::exchange(other.m_module, nullptr)) {
    // Intentionally left blank.
}

ModuleHandle& ModuleHandle::operator=(ModuleHandle&& other) noexcept {
    ModuleHandle(std::move(other)).swap(*this);
    return *this;
}

ModuleHandle::~ModuleHandle() noexcept {
    if (m_module != nullptr) {
        FreeLibrary(m_module);
    }
}

void ModuleHandle::swap(ModuleHandle& other) noexcept {
    std::swap(m_module, other.m_module);
}

void* ModuleHandle::baseAddress() const noexcept {
    return m_module;
}

ModuleNotFoundError::ModuleNotFoundError(std::string_view moduleName)
    : std::runtime_error("Module: "s + moduleName.data() + " was not found!") {
    // Intentionally left blank.
}

void swap(ModuleHandle& first, ModuleHandle& second) noexcept {
    first.swap(second);
}

}  // namespace winlogo::utils
