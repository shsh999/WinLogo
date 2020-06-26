#pragma once

#include "Common.h"
#include "PEFile.h"
#include "DelayLoadedModule.h"

#include <gsl/gsl>

namespace winlogo::pe_parser {

namespace details {
/**
 * This is an input iterator for iterating over the delay-loaded modules of the delay-load
 * directory.
 */
class DelayLoadDirectoryIterator {
public:
    DelayLoadDirectoryIterator(PEFile file,
                               gsl::span<IMAGE_DELAYLOAD_DESCRIPTOR>::iterator iterator);

    bool operator==(const DelayLoadDirectoryIterator& other) const noexcept;
    bool operator!=(const DelayLoadDirectoryIterator& other) const noexcept;

    DelayLoadedModule operator*() const noexcept;
    DelayLoadDirectoryIterator& operator++() noexcept;

private:
    PEFile m_peFile;
    gsl::span<IMAGE_DELAYLOAD_DESCRIPTOR>::iterator m_current;
};

}  // namespace details

/**
 * This class represents an iterable view of a PE file's delay-loaded directory.
 */
class DelayLoadDirectory {
public:
    /**
     * Initialize the delay-load directory from its PE file.
     */
    explicit DelayLoadDirectory(PEFile peFile);

    /**
     * Get the raw import descriptors.
     */
    gsl::span<IMAGE_DELAYLOAD_DESCRIPTOR> rawDelayLoadDescriptors() const noexcept;

    // Iterators for iterating over the imported modules.

    details::DelayLoadDirectoryIterator begin() const noexcept;
    details::DelayLoadDirectoryIterator end() const noexcept;

private:
    /// The owning PE file.
    PEFile m_peFile;
    gsl::span<IMAGE_DELAYLOAD_DESCRIPTOR> m_delayLoadDescriptors;
};

}  // namespace winlogo::pe_parser
