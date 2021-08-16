#pragma once
#include "WindowsError.h"

namespace winlogo::utils {

/**
 * This template is used to generate unique handle guard classes - responsible for holding and
 * releaing windows handles.
 * 
 * The HandleTraits class should have the following static members and types:
 * - typename HandleType - the type of the handle, such as HANDLE or HKEY.
 * - HandleType INVALID_HANDLE - a value representing the invalid handle value.
 * - static void close(HandleType) - a function closing the given handle.
 * 
 * For an example, see Registry.h.
 */
template<typename HandleTraits>
class Handle {
public:
    using HandleType = typename HandleTraits::HandleType;

    explicit Handle(HandleType value) : m_handle(value) {
        if (m_handle == HandleTraits::INVALID_HANDLE) {
            throw WindowsError();
        }
    }

    NO_COPY(Handle);

    Handle(Handle&& other) noexcept
        : m_handle(std::exchange(other.m_handle, HandleTraits::INVALID_HANDLE)) {
        // Intentionally left blank.
    }

    Handle& operator=(Handle&& other) noexcept {
        Handle(std::move(other)).swap(*this);
    }

    ~Handle() noexcept {
        if (m_handle != HandleTraits::INVALID_HANDLE) {
            HandleTraits::close(m_handle);
        }
    }

    /**
     * Get the underlying handle.
     */
    HandleType get() const noexcept {
        return m_handle;
    }

private:
    /// The underlying handle
    HandleType m_handle;

    static void swap(Handle& other) noexcept {
        std::swap(m_handle, other.m_handle);
    }
};

}  // namespace winlogo::utils
