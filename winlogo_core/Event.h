#pragma once

#include "Common.h"
#include "WindowsError.h"
#include "Handle.h"

namespace winlogo::utils {

namespace details {

/**
 * Windows Event handle traits.
 */
struct EventHandleTraits {
    using HandleType = HANDLE;

    static constexpr const HandleType INVALID_HANDLE = INVALID_HANDLE_VALUE;

    static void close(HandleType handle) noexcept {
        CloseHandle(handle);
    }
};

using EventHandle = Handle<EventHandleTraits>;

}  // namespace details

/**
 * An exception thrown when CreateEvent fails (or the event already exists).
 */
class EventAlreadyExistsError : public WindowsError {
public:
    using WindowsError::WindowsError;
};

/**
 * An exception thrown when waiting for an event fails.
 */
class WaitEventFailedError : public WindowsError {
public:
    using WindowsError::WindowsError;
};

/**
 * An exception thrown when setting an event fails.
 */
class SetEventFailedError : public WindowsError {
public:
    using WindowsError::WindowsError;
};

/**
 * A wrapper class for Win32 event objects.
 */
class Event final {
public:
    /**
     * Creates a new manual reset event with the given name.
     * By default, throws an exception if the event already exists.
     */
    explicit Event(const std::wstring& name, bool failIfExists = true);

    /**
     * Wait indefinitely for the event to be signaled.
     */
    void wait();

    /**
     * Signal the event.
     */
    void set();

private:
    details::EventHandle m_event;
};

}  // namespace winlogo::utils
