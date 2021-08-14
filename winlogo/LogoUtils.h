#pragma once

#include "Common.h"

namespace winlogo {

/**
 * Find the HWND of the Start button.
 */
HWND findStartButton() noexcept;

/**
 * Cause explorer.exe to reload the Start button logo theme.
 */
void refreshLogo() noexcept;

}  // namespace winlogo
