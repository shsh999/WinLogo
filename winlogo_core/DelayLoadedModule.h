#pragma once
#include "Common.h"
#include "PEFile.h"
#include "ImportedModule.h"
#include "CaseInsensitiveString.h"

namespace winlogo::pe_parser {

/**
 * This class represents an delay-loaded module in a PE delay-load directory.
 * The class allows enumeration over the functions imported from this module.
 */
class DelayLoadedModule {
public:
    DelayLoadedModule(PEFile peFile, IMAGE_DELAYLOAD_DESCRIPTOR& descriptor) noexcept;

    /**
     * Get the module's name.
     */
    utils::ci_string_view name() const noexcept;

    // Iterators for iterating over the functions imported from this module.

    details::ImportedModuleIterator begin() const noexcept;
    details::ImportedModuleEndIterator end() const noexcept;

    /**
     * Get the underlying raw IMAGE_DELAYLOAD_DESCRIPTOR.
     */
    IMAGE_DELAYLOAD_DESCRIPTOR& rawDelayLoadtDescriptor() const noexcept;

private:
    /// The owning PE file.
    PEFile m_peFile;
    IMAGE_DELAYLOAD_DESCRIPTOR& m_delayLoadDescriptor;
};

}  // namespace winlogo::pe_parser
