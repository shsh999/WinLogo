#include "Event.h"

namespace winlogo::utils {

Event::Event(const std::wstring& name) : m_event(CreateEventW(nullptr, true, false, name.c_str())) {
    if (m_event == nullptr) {
        throw CreateEventFailedError();
    } else if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(m_event);
        throw CreateEventFailedError();
    }
}

Event::~Event() {
    if (m_event != nullptr) {
        CloseHandle(m_event);
    }
}

void Event::wait() {
    auto result = WaitForSingleObject(m_event, INFINITE);
    if (result != WAIT_OBJECT_0) {
        throw WaitEventFailedError();
    }
}

}