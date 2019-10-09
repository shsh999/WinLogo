#pragma once
#include "Common.h"
#include "PEFile.h"
#include "CaseInsensitiveString.h"

namespace winlogo::pe_parser {

/**
 * This exception is raised when trying to access the name of a function imported by ordinal, or the
 * ordinal of a function imported by name.
 */
struct BadImportedFunctionAccess : public std::exception {
    // Intentionally left blank.
};

/**
 * This class represents an imported function in a PE import table.
 */
class ImportedFunction {
public:
    ImportedFunction(PEFile peFile, IMAGE_THUNK_DATA& nameTableEntry,
                     IMAGE_THUNK_DATA& addressTableEntry) noexcept;

    /**
     * Returns whether the function is imported by name.
     */
    bool isImportedByName() const noexcept;

    /**
     * Returns whether the function is imported by ordinal.
     */
    bool isImportedByOridnal() const noexcept;

    /**
     * Returns the name of the imported function.
     * If the function is imported by ordinal, a BadImportedFunctionAccess is thrown.
     */
    utils::ci_string_view name() const;

    /**
     * Returns the ordinal of the imported function.
     * If the function is imported by name, a BadImportedFunctionAccess is thrown.
     */
    uint16_t ordinal() const;

    /**
     * Returns a reference to the resolved address pointer.
     */
    void* resolvedAddress() const noexcept;

    /**
     * Get the underlying raw IMAGE_THUNK_DATA for the INT (import name table) entry.
     */
    IMAGE_THUNK_DATA& rawINTEntry() const noexcept;

    /**
     * Get the underlying raw IMAGE_THUNK_DATA for the IAT (import address table) entry.
     */
    IMAGE_THUNK_DATA& rawIATEntry() const noexcept;

private:
    /// The owning PE file.
    PEFile m_peFile;
    IMAGE_THUNK_DATA& m_nameTableEntry;
    IMAGE_THUNK_DATA& m_addressTableEntry;
};

}  // namespace winlogo::pe_parser
