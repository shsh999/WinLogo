#include "MemoryProtectionGuard.h"

namespace winlogo::utils {

VirtualProtectFailedError::VirtualProtectFailedError() noexcept
    : std::runtime_error("Virtual protect failed!") {
    // Intentionally left blank.
}

MemoryProtectionGuard::MemoryProtectionGuard(void* startAddress, size_t size, DWORD protection)
    : m_startAddress(startAddress), m_size(size) {
    if (!doVirtualProtect(protection)) {
        throw VirtualProtectFailedError();
    }
}

MemoryProtectionGuard::~MemoryProtectionGuard() noexcept {
    doVirtualProtect(m_oldProtection);
}

bool MemoryProtectionGuard::doVirtualProtect(DWORD protection) noexcept {
    return VirtualProtect(m_startAddress, m_size, protection, &m_oldProtection) != FALSE;
}

}  // namespace winlogo::utils
