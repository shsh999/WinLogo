#include "IATHook.h"
#include "DelayLoadDirectory.h"
#include "ImportDirectory.h"
#include "Algorithms.h"
#include "MemoryProtectionGuard.h"

namespace winlogo::hooks {

using pe_parser::ImportedFunction;
using pe_parser::ImportedModule;
using pe_parser::PEFile;
using utils::ci_string_view;

using namespace std::string_literals;

FunctionNotFoundError::FunctionNotFoundError(ci_string_view functionName)
    : std::runtime_error("Function: "s + functionName.data() + " was not found!") {
    // Intentionally left blank.
}

/**
 * Find the import (or delay-loaded) table entry for the requested function in the given module.
 * If the function does not exist in the module, nullptr is returned.
 */
template<typename ModuleType>
static void** getImportEntry(ModuleType importedModule, ci_string_view functionToHook) noexcept {
    auto importedFunction = utils::find_if(importedModule.begin(), importedModule.end(),
                                           [functionToHook](ImportedFunction importedFunction) {
                                               return importedFunction.isImportedByName() &&
                                                      importedFunction.name() == functionToHook;
                                           });

    if (importedFunction == importedModule.end()) {
        return nullptr;
    }

    auto& iatEntry = (*importedFunction).rawIATEntry();

    return reinterpret_cast<void**>(&iatEntry.u1.Function);
}

/**
 * Get the address of the import table entry that should be modified in order to hook the requested
 * function.
 */
template<typename DirectoryType>
void** findImportHookAddress(PEFile moduleToHook, ci_string_view functionToHook,
                             ci_string_view sourceModuleName = "") {
    void** hookAddress = nullptr;

    auto importDirectory = DirectoryType(moduleToHook);

    if (sourceModuleName.empty()) {
        // The module from which the function was taken doesn't matter - search for the function in
        // all the modules and return the first matching.

        // Ignore the return value, as what matters is the hookAddress, and not the module.
        (void)std::find_if(importDirectory.begin(), importDirectory.end(),
                           [&hookAddress, functionToHook](auto mod) {
                               hookAddress = getImportEntry(mod, functionToHook);
                               return hookAddress != nullptr;
                           });
    } else {
        // Find the specific module from which the function is imported.
        auto importedModule = std::find_if(importDirectory.begin(), importDirectory.end(),
                                           [sourceModuleName](auto importedModule) {
                                               return importedModule.name() == sourceModuleName;
                                           });
        if (importedModule == importDirectory.end()) {
            throw utils::ModuleNotFoundError(sourceModuleName.data());
        }

        // Find the import entry in the module.
        hookAddress = getImportEntry(*importedModule, functionToHook);
    }

    return hookAddress;
}

/**
 * Find the address that should be changed in order to hook the requested function.
 * The function first looks for imported functions and then for delay-loaded functions.
 * If the functions are not found, an exception is raised.
 */
void** findHookAddress(PEFile moduleToHook, ci_string_view functionToHook,
                       ci_string_view sourceModuleName = "") {
    void** result = findImportHookAddress<pe_parser::ImportDirectory>(moduleToHook, functionToHook,
                                                                      sourceModuleName);
    if (result == nullptr) {
        result = findImportHookAddress<pe_parser::DelayLoadDirectory>(moduleToHook, functionToHook,
                                                                      sourceModuleName);
    }

    // A function matching the parameters was not found.
    if (result == nullptr) {
        throw FunctionNotFoundError(functionToHook);
    }

    return result;
}

IATHook::IATHook(PEFile moduleToHook, ci_string_view functionToHook, void* newFunction,
                 ci_string_view sourceModule /* = "" */)
    : m_hookAddress(findHookAddress(moduleToHook, functionToHook, sourceModule)),
      m_originalFunction(*m_hookAddress) {
    utils::MemoryProtectionGuard guard(m_hookAddress, sizeof(void*), PAGE_READWRITE);
    *m_hookAddress = newFunction;
}

IATHook::~IATHook() noexcept {
    try {
        utils::MemoryProtectionGuard guard(m_hookAddress, sizeof(void*), PAGE_READWRITE);
        *m_hookAddress = m_originalFunction;
    } catch (const utils::VirtualProtectFailedError&) {
        // Intentionally left blank.
    }
}

}  // namespace winlogo::hooks
