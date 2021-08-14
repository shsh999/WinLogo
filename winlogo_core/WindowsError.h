#pragma once
#include "Common.h"
#include <system_error>

namespace winlogo::utils {

/**
 * An exception holding a GetLastError() error code.
 */
class WindowsError : public std::system_error {
public:
    WindowsError(DWORD lastError = GetLastError())
        : std::system_error(lastError, std::system_category()) {
        // Intentionally left blank.
    }
};

}  // namespace winlogo::utils
