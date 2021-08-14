#pragma once
#include "Common.h"
#include "ModuleHandle.h"

namespace winlogo::pe_parser {

/**
 * This class provides a view of loaded PE files.
 * The class does not ensure that the PE file remains loaded. If it possible for the PE file to get
 * freed while using the class, use a ModuleHandle to guard it.
 */
class PEFile {
public:
    explicit PEFile(std::string_view loadedModuleName);
    explicit PEFile(HMODULE moduleHandle) noexcept;
    explicit PEFile(void* baseAddress) noexcept;
    explicit PEFile(const utils::ModuleHandle& moduleHandle) noexcept;

    /**
     * Get the loaded PEFile of the current executable.
     */
    static PEFile currentExecutable() noexcept;

    /**
     * Get the PE's base address.
     */
    void* baseAddress() const noexcept;

    /**
     * Get the PE's DOS header.
     */
    IMAGE_DOS_HEADER& dosHeader() const noexcept;

    /**
     * Get the PE's NT headers.
     */
    IMAGE_NT_HEADERS& ntHeaders() const noexcept;

    /**
     * Get the PE's optional header.
     */
    IMAGE_OPTIONAL_HEADER& optionalHeader() const noexcept;

    /**
     * Get the PE's file header.
     */
    IMAGE_FILE_HEADER& fileHeader() const noexcept;

    /**
     * Get a pointer of the requested type to the specified RVA.
     */
    template<typename T>
    std::enable_if_t<std::is_pointer_v<T>, T> rva(const uintptr_t rva) const noexcept {
        return reinterpret_cast<T>(reinterpret_cast<byte*>(m_baseAddress) + rva);
    }

    /**
     * Get a reference of the requested type to the specified RVA.
     */
    template<typename T>
    std::enable_if_t<std::is_lvalue_reference_v<T>, T> rva(const uintptr_t rva) const noexcept {
        return *reinterpret_cast<std::remove_reference_t<T>*>(
            reinterpret_cast<byte*>(m_baseAddress) + rva);
    }

private:
    /// The PE's base address.
    void* m_baseAddress;
};

}  // namespace winlogo::pe_parser
