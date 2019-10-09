#include "ImportedModule.h"

namespace winlogo::pe_parser {

ImportedModule::ImportedModule(PEFile peFile, IMAGE_IMPORT_DESCRIPTOR& descriptor) noexcept
    : m_peFile(peFile), m_importDescriptor(descriptor) {
    // Intentionally left blank.
}

utils::ci_string_view ImportedModule::name() const noexcept {
    return m_peFile.rva<const char*>(m_importDescriptor.Name);
}

details::ImportedModuleIterator ImportedModule::begin() const noexcept {
    return details::ImportedModuleIterator(
        m_peFile, m_peFile.rva<IMAGE_THUNK_DATA*>(m_importDescriptor.OriginalFirstThunk),
        m_peFile.rva<IMAGE_THUNK_DATA*>(m_importDescriptor.FirstThunk));
}

details::ImportedModuleEndIterator ImportedModule::end() const noexcept {
    return details::ImportedModuleEndIterator();
}

IMAGE_IMPORT_DESCRIPTOR& ImportedModule::rawImportDescriptor() const noexcept {
    return m_importDescriptor;
}

namespace details {

ImportedModuleIterator::ImportedModuleIterator(PEFile peFile, IMAGE_THUNK_DATA* originalFirstThunk,
                                               IMAGE_THUNK_DATA* firstThunk)
    : m_peFile(peFile), m_nameTableEntry(originalFirstThunk), m_addressTableEntry(firstThunk) {
    // Intentionally left blank.
}

bool ImportedModuleIterator::operator==(const ImportedModuleIterator& other) const noexcept {
    return m_nameTableEntry == other.m_nameTableEntry;
}

bool ImportedModuleIterator::operator!=(const ImportedModuleIterator& other) const noexcept {
    return m_nameTableEntry == other.m_nameTableEntry;
}

bool ImportedModuleIterator::operator==(const ImportedModuleEndIterator&) const noexcept {
    return m_nameTableEntry->u1.AddressOfData == 0;
}

bool ImportedModuleIterator::operator!=(const ImportedModuleEndIterator&) const noexcept {
    return m_nameTableEntry->u1.AddressOfData != 0;
}

ImportedFunction ImportedModuleIterator::operator*() const noexcept {
    return ImportedFunction(m_peFile, *m_nameTableEntry, *m_addressTableEntry);
}

ImportedModuleIterator& ImportedModuleIterator::operator++() noexcept {
    ++m_nameTableEntry;
    ++m_addressTableEntry;
    return *this;
}

}  // namespace details
}  // namespace winlogo::pe_parser
