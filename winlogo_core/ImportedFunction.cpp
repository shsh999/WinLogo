#include "ImportedFunction.h"

namespace winlogo::pe_parser {

ImportedFunction::ImportedFunction(PEFile peFile, IMAGE_THUNK_DATA& nameTableEntry,
                                   IMAGE_THUNK_DATA& addressTableEntry) noexcept
    : m_peFile(peFile), m_nameTableEntry(nameTableEntry), m_addressTableEntry(addressTableEntry) {
    // Intentionally left blank.
}

bool ImportedFunction::isImportedByName() const noexcept {
    return !isImportedByOridnal();
}

bool ImportedFunction::isImportedByOridnal() const noexcept {
    return IMAGE_SNAP_BY_ORDINAL(m_nameTableEntry.u1.Function);
}

utils::ci_string_view ImportedFunction::name() const {
    if (isImportedByName()) {
        return m_peFile.rva<IMAGE_IMPORT_BY_NAME&>(m_nameTableEntry.u1.AddressOfData).Name;
    }

    throw BadImportedFunctionAccess();
}

uint16_t ImportedFunction::ordinal() const {
    if (isImportedByOridnal()) {
        return IMAGE_ORDINAL(m_nameTableEntry.u1.Ordinal);
    }

    throw BadImportedFunctionAccess();
}

void* ImportedFunction::resolvedAddress() const noexcept {
    return *reinterpret_cast<void**>(&m_addressTableEntry.u1.Function);
}

IMAGE_THUNK_DATA& ImportedFunction::rawINTEntry() const noexcept {
    return m_nameTableEntry;
}

IMAGE_THUNK_DATA& ImportedFunction::rawIATEntry() const noexcept {
    return m_addressTableEntry;
}

}  // namespace winlogo::pe_parser
