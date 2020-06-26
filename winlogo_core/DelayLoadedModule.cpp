#include "DelayLoadedModule.h"

namespace winlogo::pe_parser {

DelayLoadedModule::DelayLoadedModule(PEFile peFile, IMAGE_DELAYLOAD_DESCRIPTOR& descriptor) noexcept
    : m_peFile(peFile), m_delayLoadDescriptor(descriptor) {
    // Intentionally left blank.
}

utils::ci_string_view DelayLoadedModule::name() const noexcept {
    return m_peFile.rva<const char*>(m_delayLoadDescriptor.DllNameRVA);
}

details::ImportedModuleIterator DelayLoadedModule::begin() const noexcept {
    return details::ImportedModuleIterator(
        m_peFile, m_peFile.rva<IMAGE_THUNK_DATA*>(m_delayLoadDescriptor.ImportNameTableRVA),
        m_peFile.rva<IMAGE_THUNK_DATA*>(m_delayLoadDescriptor.ImportAddressTableRVA));
}

details::ImportedModuleEndIterator DelayLoadedModule::end() const noexcept {
    return details::ImportedModuleEndIterator();
}

IMAGE_DELAYLOAD_DESCRIPTOR& DelayLoadedModule::rawDelayLoadtDescriptor() const noexcept {
    return m_delayLoadDescriptor;
}

}  // namespace winlogo::pe_parser
