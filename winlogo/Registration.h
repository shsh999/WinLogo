#pragma once

namespace winlogo::registration {

/**
 * The different operations possible for a registration entries.
 */
enum class EntryOperation {
    /// Create the key/value pair specified by the entry.
    Default,
    /// Create the key/value pair on registration, and delete on unregistration.
    Delete,
    /// Use the dll path as the value for the entry.
    FileName,
};

/**
 * This struct is used to define Registry operations used in the registration/deregistration process
 * of WinLogo.
 */
struct RegistrationEntry {
    /// The registry key path inside HKLM
    const wchar_t* keyPath;
    /// The operation to do for the key
    EntryOperation operation;

    /// The name of the value to manipulate
    const wchar_t* name;
    /// The value to assign
    const wchar_t* value;
};

}  // namespace winlogo::registration
