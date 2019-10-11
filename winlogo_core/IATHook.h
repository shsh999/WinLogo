#pragma once
#include "PEFile.h"
#include "CaseInsensitiveString.h"

namespace winlogo::hooks {

/**
 * This exception is thrown when failing to find a function to hook.
 */
class FunctionNotFoundError : public std::runtime_error {
public:
    explicit FunctionNotFoundError(utils::ci_string_view functionName);
};

/**
 * This class is responsible for setting up a IAT hook on the given module for the given function.
 * The hook is removed on object destruction.
 */
class IATHook {
public:
    /**
     * Set up a IAT hook.
     *
     * If a source module is defined, then the function must be imported from that module.
     * Otherwise, all of the imported modules are searched for the function.
     */
    IATHook(pe_parser::PEFile moduleToHook, utils::ci_string_view functionToHook, void* newFunction,
            utils::ci_string_view sourceModule = "");

    ~IATHook() noexcept;

    NO_COPY(IATHook);
    NO_MOVE(IATHook);

private:
    /// A pointer to the function pointer in the IAT of the hooked module.
    void** m_hookAddress;
    /// A the original hooked function.
    void* m_originalFunction;
};

}  // namespace winlogo::hooks
