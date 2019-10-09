#include "Common.h"
#include "ImportDirectory.h"

namespace winlogo::pe_parser {

/**
 * Get a span containing the import descriptors from a PE file.
 */
static gsl::span<IMAGE_IMPORT_DESCRIPTOR> getImportDescriptors(PEFile peFile) {
    auto& dataDirectory = peFile.optionalHeader().DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    auto startAddress = peFile.rva<IMAGE_IMPORT_DESCRIPTOR*>(dataDirectory.VirtualAddress);
    auto numberOfDescriptors =
        (dataDirectory.Size / sizeof(IMAGE_IMPORT_DESCRIPTOR)) - 1;  // remove 1 for the null
                                                                     // termination.
    return {startAddress, gsl::narrow_cast<ptrdiff_t>(numberOfDescriptors)};
}

ImportDirectory::ImportDirectory(PEFile peFile)
    : m_peFile(peFile), m_importDescriptors(getImportDescriptors(peFile)) {
    // Intentionally left blank.
}

gsl::span<IMAGE_IMPORT_DESCRIPTOR> ImportDirectory::rawImportDescriptors() const noexcept {
    return m_importDescriptors;
}

details::ImportDirectoryIterator ImportDirectory::begin() const noexcept {
    return details::ImportDirectoryIterator(m_peFile, m_importDescriptors.begin());
}

details::ImportDirectoryIterator ImportDirectory::end() const noexcept {
    return details::ImportDirectoryIterator(m_peFile, m_importDescriptors.end());
}

namespace details {

ImportDirectoryIterator::ImportDirectoryIterator(
    PEFile file, gsl::span<IMAGE_IMPORT_DESCRIPTOR>::iterator iterator)
    : m_peFile(file), m_current(iterator) {
    // Intentionally left blank.
}

bool ImportDirectoryIterator::operator==(const ImportDirectoryIterator& other) const noexcept {
    return m_current == other.m_current;
}

bool ImportDirectoryIterator::operator!=(const ImportDirectoryIterator& other) const noexcept {
    return m_current != other.m_current;
}

ImportedModule ImportDirectoryIterator::operator*() const noexcept {
    return ImportedModule(m_peFile, *m_current);
}

ImportDirectoryIterator& ImportDirectoryIterator::operator++() noexcept {
    ++m_current;
    return *this;
}

}  // namespace details
}  // namespace winlogo::pe_parser
