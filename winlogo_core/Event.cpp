#include "Event.h"

namespace winlogo::utils {

Event::Event(const std::wstring& name, bool failIfExists /* = true */)
    : m_event(CreateEventW(nullptr, true, false, name.c_str())) {
    if (failIfExists && GetLastError() == ERROR_ALREADY_EXISTS) {
        throw EventAlreadyExistsError();
    }
}

void Event::wait() {
    auto result = WaitForSingleObject(m_event.get(), INFINITE);
    if (result != WAIT_OBJECT_0) {
        throw WaitEventFailedError();
    }
}

void Event::set() {
    if (!SetEvent(m_event.get())) {
        throw SetEventFailedError();
    }
}

}  // namespace winlogo::utils
