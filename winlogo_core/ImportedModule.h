#pragma once
#include "Common.h"
#include "PEFile.h"
#include "ImportedFunction.h"
#include "CaseInsensitiveString.h"

namespace winlogo::pe_parser {

namespace details {

/**
 * This is a sentinel struct to be used as an end iterator for the imported module iterator.
 */
struct ImportedModuleEndIterator {
    // Intentionally left blank.
};

/**
 * This is an iterator class for iterating over the imported functions of an imported module.
 */
class ImportedModuleIterator {
public:
    ImportedModuleIterator(PEFile peFile, IMAGE_THUNK_DATA* originalFirstThunk,
                           IMAGE_THUNK_DATA* firstThunk);

    bool operator==(const ImportedModuleIterator& other) const noexcept;
    bool operator!=(const ImportedModuleIterator& other) const noexcept;

    bool operator==(const ImportedModuleEndIterator&) const noexcept;
    bool operator!=(const ImportedModuleEndIterator&) const noexcept;

    ImportedFunction operator*() const noexcept;
    ImportedModuleIterator& operator++() noexcept;

private:
    PEFile m_peFile;
    IMAGE_THUNK_DATA* m_nameTableEntry;
    IMAGE_THUNK_DATA* m_addressTableEntry;
};

}  // namespace details

/**
 * This class represents an imported module in a PE import directory.
 * The class allows enumeration over the functions imported from this module.
 */
class ImportedModule {
public:
    ImportedModule(PEFile peFile, IMAGE_IMPORT_DESCRIPTOR& descriptor) noexcept;

    /**
     * Get the module's name.
     */
    utils::ci_string_view name() const noexcept;

    // Iterators for iterating over the functions imported from this module.

    details::ImportedModuleIterator begin() const noexcept;
    details::ImportedModuleEndIterator end() const noexcept;

    /**
     * Get the underlying raw IMAGE_IMPORT_DESCRIPTOR.
     */
    IMAGE_IMPORT_DESCRIPTOR& rawImportDescriptor() const noexcept;

private:
    /// The owning PE file.
    PEFile m_peFile;
    IMAGE_IMPORT_DESCRIPTOR& m_importDescriptor;
};

}  // namespace winlogo::pe_parser
