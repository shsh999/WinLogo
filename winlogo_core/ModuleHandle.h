#pragma once
#include "Common.h"
#include "Handle.h"

#include <stdexcept>

namespace winlogo::utils {

namespace details {

/**
 * Windows Module handle traits.
 */
struct ModuleHandleTraits {
    using HandleType = HMODULE;

    static constexpr const HandleType INVALID_HANDLE = nullptr;

    static void close(HandleType handle) noexcept {
        FreeLibrary(handle);
    }
};

}  // namespace details

/**
 * An execption raised when failing to initialize a ModuleHandle.
 */
class ModuleNotFoundError : public WindowsError {
public:
    explicit ModuleNotFoundError(std::string_view moduleName);
};

/**
 * A tag used to select a ModuleHandle constructor that requires an already loaded module.
 */
struct FailIfNotLoaded {
    // Intentionally left blank.
};
inline constexpr const FailIfNotLoaded failIfNotLoaded{};

/**
 * This class is a wrapper for windows loaded PE modules (dll/exe).
 * Instances are responsible for keeping the module from being freed, by increasing the module's
 * reference-count (and decreasing it on destruction).
 */
class ModuleHandle {
public:
    /**
     * Initialize a module with the given name.
     * The module is loaded to memory, if necessary.
     */
    explicit ModuleHandle(const char* moduleName);
    explicit ModuleHandle(std::string_view moduleName);

    /**
     * Initialize a module with the given name.
     * If the module is not loaded, an exception is thrown.
     */
    ModuleHandle(const char* moduleName, FailIfNotLoaded);
    ModuleHandle(std::string_view moduleName, FailIfNotLoaded);

    /**
     * Get the module's base address.
     */
    void* baseAddress() const noexcept;

private:
    /// The underlying Windows module handle.
    Handle<details::ModuleHandleTraits> m_module;

    /**
     * Gets a new handle to an already loaded module.
     */
    static HMODULE getLoadedModuleHandle(const char* moduleName);
};

}  // namespace winlogo::utils
