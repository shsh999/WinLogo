#pragma once

#include "Common.h"
#include "WindowsError.h"

namespace winlogo::utils {

/**
 * An exception thrown when CreateEvent fails (or the event already exists).
 */
class CreateEventFailedError : public WindowsError {
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
    ~Event() noexcept;

    NO_COPY(Event);
    NO_MOVE(Event);

    /**
     * Wait indefinitely for the event to be signaled.
     */
    void wait();

private:
    HANDLE m_event;
};

}