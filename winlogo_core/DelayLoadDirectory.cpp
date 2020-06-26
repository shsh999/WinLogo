#include "Common.h"
#include "DelayLoadDirectory.h"

namespace winlogo::pe_parser {

/**
 * Get a span containing the delay-load descriptors from a PE file.
 */
static gsl::span<IMAGE_DELAYLOAD_DESCRIPTOR> getDelayLoadDescriptors(PEFile peFile) {
    auto& dataDirectory = peFile.optionalHeader().DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT];
    auto startAddress = peFile.rva<IMAGE_DELAYLOAD_DESCRIPTOR*>(dataDirectory.VirtualAddress);
    auto numberOfDescriptors =
        (dataDirectory.Size / sizeof(IMAGE_DELAYLOAD_DESCRIPTOR)) - 1;  // remove 1 for the null
                                                                        // termination.
    return {startAddress, gsl::narrow_cast<ptrdiff_t>(numberOfDescriptors)};
}

DelayLoadDirectory::DelayLoadDirectory(PEFile peFile)
    : m_peFile(peFile), m_delayLoadDescriptors(getDelayLoadDescriptors(peFile)) {
    // Intentionally left blank.
}

gsl::span<IMAGE_DELAYLOAD_DESCRIPTOR> DelayLoadDirectory::rawDelayLoadDescriptors() const noexcept {
    return m_delayLoadDescriptors;
}

details::DelayLoadDirectoryIterator DelayLoadDirectory::begin() const noexcept {
    return details::DelayLoadDirectoryIterator(m_peFile, m_delayLoadDescriptors.begin());
}

details::DelayLoadDirectoryIterator DelayLoadDirectory::end() const noexcept {
    return details::DelayLoadDirectoryIterator(m_peFile, m_delayLoadDescriptors.end());
}

namespace details {

DelayLoadDirectoryIterator::DelayLoadDirectoryIterator(
    PEFile file, gsl::span<IMAGE_DELAYLOAD_DESCRIPTOR>::iterator iterator)
    : m_peFile(file), m_current(iterator) {
    // Intentionally left blank.
}

bool DelayLoadDirectoryIterator::operator==(
    const DelayLoadDirectoryIterator& other) const noexcept {
    return m_current == other.m_current;
}

bool DelayLoadDirectoryIterator::operator!=(
    const DelayLoadDirectoryIterator& other) const noexcept {
    return m_current != other.m_current;
}

DelayLoadedModule DelayLoadDirectoryIterator::operator*() const noexcept {
    return DelayLoadedModule(m_peFile, *m_current);
}

DelayLoadDirectoryIterator& DelayLoadDirectoryIterator::operator++() noexcept {
    ++m_current;
    return *this;
}

}  // namespace details
}  // namespace winlogo::pe_parser
