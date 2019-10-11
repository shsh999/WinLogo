#pragma once
#include "Common.h"

#include <stdexcept>
#include <gsl/gsl>

namespace winlogo::utils {

/**
 * This exception is raised when failing to run VirtualProtect.
 */
class VirtualProtectFailedError : public std::runtime_error {
public:
    VirtualProtectFailedError() noexcept;
};

/**
 * This is a RAII class for temporarily changing the protection of a memory region.
 */
class MemoryProtectionGuard {
public:
    MemoryProtectionGuard(void* startAddress, size_t size, DWORD protection);

    NO_COPY(MemoryProtectionGuard);
    NO_MOVE(MemoryProtectionGuard);

    ~MemoryProtectionGuard() noexcept;

private:
    /// The start address of the memory region for which the protection should be changed.
    void* m_startAddress;
    /// The size of the memory region for which the protection should be changed.
    size_t m_size;
    /// The old protection to be restored in the end.
    DWORD m_oldProtection;

    /**
     * Run VirtualProtect for the stored address and store the previous protection in
     * m_oldProtection.
     */
    bool doVirtualProtect(DWORD protection) noexcept;
};

}  // namespace winlogo::utils
