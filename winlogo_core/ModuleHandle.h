#pragma once
#include "Common.h"

#include <stdexcept>

namespace winlogo::utils {

/**
 * An execption raised when failing to initialize a ModuleHandle.
 */
class ModuleNotFoundError : public std::runtime_error {
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
    explicit ModuleHandle(std::string_view moduleName);

    ModuleHandle(ModuleHandle&& other) noexcept;
    ModuleHandle& operator=(ModuleHandle&& other) noexcept;

    /**
     * Initialize a module with the given name.
     * If the module is not loaded, an exception is thrown.
     */
    ModuleHandle(std::string_view moduleName, FailIfNotLoaded);

    ~ModuleHandle() noexcept;

    NO_COPY(ModuleHandle);

    void swap(ModuleHandle& other) noexcept;

    /**
     * Get the module's base address.
     */
    void* baseAddress() const noexcept;

private:
    /// The underlying Windows module handle.
    HMODULE m_module;
};

void swap(ModuleHandle& first, ModuleHandle& second) noexcept;

}  // namespace winlogo::utils
