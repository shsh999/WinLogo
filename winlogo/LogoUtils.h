#pragma once

#include "Common.h"
#include <unordered_set>

namespace winlogo {

/**
 * Find the HWND of the main Start button.
 */
HWND findStartButton() noexcept;

/**
 * Find all the HWND values for the Start buttons (supports multiple screens).
 */
std::unordered_set<HWND> findAllStartButtons();

/**
 * Cause explorer.exe to reload the Start button logo theme.
 */
void refreshLogo();

}  // namespace winlogo
