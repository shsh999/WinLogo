#pragma once
#include "Common.h"
#include "PEFile.h"
#include "ImportedModule.h"

#include <gsl/gsl>

namespace winlogo::pe_parser {

namespace details {

/**
 * This is an input iterator for iterating over the imported modules of an import directory.
 */
class ImportDirectoryIterator {
public:
    ImportDirectoryIterator(PEFile file, gsl::span<IMAGE_IMPORT_DESCRIPTOR>::iterator iterator);

    bool operator==(const ImportDirectoryIterator& other) const noexcept;
    bool operator!=(const ImportDirectoryIterator& other) const noexcept;

    ImportedModule operator*() const noexcept;
    ImportDirectoryIterator& operator++() noexcept;

private:
    PEFile m_peFile;
    gsl::span<IMAGE_IMPORT_DESCRIPTOR>::iterator m_current;
};

}  // namespace details

/**
 * This class represents an iterable view of a PE file's import directory.
 */
class ImportDirectory {
public:
    /**
     * Initialize the import directory from its PE file.
     */
    explicit ImportDirectory(PEFile peFile);

    /**
     * Get the raw import descriptors.
     */
    gsl::span<IMAGE_IMPORT_DESCRIPTOR> rawImportDescriptors() const noexcept;

    // Iterators for iterating over the imported modules.

    details::ImportDirectoryIterator begin() const noexcept;
    details::ImportDirectoryIterator end() const noexcept;

private:
    /// The owning PE file.
    PEFile m_peFile;
    gsl::span<IMAGE_IMPORT_DESCRIPTOR> m_importDescriptors;
};

}  // namespace winlogo::pe_parser
