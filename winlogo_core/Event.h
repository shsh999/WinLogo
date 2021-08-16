#pragma once

#include "Common.h"
#include "WindowsError.h"
#include "Handle.h"

namespace winlogo::utils {

    namespace details {

/**
 * Windows Transaction handle traits.
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
 * A wrapper class for Win32 event objects.
 */
class Event final {
public:
    /**
     * Creates a new event with the given name.
     * Throws an exception if the event already exists or if the event creation fails.
     */
    explicit Event(const std::wstring& name);

    /**
     * Wait indefinitely for the event to be signaled.
     */
    void wait();

private:
    details::EventHandle m_event;
};

}