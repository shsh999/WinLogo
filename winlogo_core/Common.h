#pragma once
#define NOMINMAX
#include <Windows.h>
#include <string_view>

/// Disallow copy for the given class.
#define NO_COPY(cls)          \
    cls(const cls&) = delete; \
    cls& operator=(const cls&) = delete

/// Disallow move for the given class
#define NO_MOVE(cls)     \
    cls(cls&&) = delete; \
    cls& operator=(cls&&) = delete
