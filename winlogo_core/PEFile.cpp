#include "Common.h"
#include "PEFile.h"

namespace winlogo::pe_parser {

PEFile::PEFile(std::string_view loadedModuleName)
    : m_baseAddress(GetModuleHandleA(loadedModuleName.data())) {
    if (m_baseAddress == nullptr) {
        throw utils::ModuleNotFoundError(loadedModuleName);
    }
}

PEFile::PEFile(HMODULE moduleHandle) noexcept : m_baseAddress(moduleHandle) {
    // Intentionally left blank.
}

PEFile::PEFile(void* baseAddress) noexcept : m_baseAddress(baseAddress) {
    // Intentionally left blank.
}

PEFile::PEFile(const utils::ModuleHandle& moduleHandle) noexcept
    : m_baseAddress(moduleHandle.baseAddress()) {
    // Intentionally left blank.
}

void* PEFile::baseAddress() const noexcept {
    return m_baseAddress;
}

IMAGE_DOS_HEADER& PEFile::dosHeader() const noexcept {
    return *reinterpret_cast<IMAGE_DOS_HEADER*>(m_baseAddress);
}

IMAGE_NT_HEADERS& PEFile::ntHeaders() const noexcept {
    return rva<IMAGE_NT_HEADERS&>(dosHeader().e_lfanew);
}

IMAGE_OPTIONAL_HEADER& PEFile::optionalHeader() const noexcept {
    return ntHeaders().OptionalHeader;
}

IMAGE_FILE_HEADER& PEFile::fileHeader() const noexcept {
    return ntHeaders().FileHeader;
}

}  // namespace winlogo::pe_parser
