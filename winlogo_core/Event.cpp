#include "Event.h"

namespace winlogo::utils {

Event::Event(const std::wstring& name) : m_event(CreateEventW(nullptr, true, false, name.c_str())) {
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        throw EventAlreadyExistsError();
    }
}

void Event::wait() {
    auto result = WaitForSingleObject(m_event.get(), INFINITE);
    if (result != WAIT_OBJECT_0) {
        throw WaitEventFailedError();
    }
}

}  // namespace winlogo::utils
