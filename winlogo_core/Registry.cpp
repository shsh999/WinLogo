#include "Registry.h"

namespace winlogo::utils {

HKEY RegistryKey::openRegistryKey(HKEY key, const std::wstring& path,
                                  const Transaction& transaction, REGSAM access) {
    HKEY handle = nullptr;
    auto result =
        RegOpenKeyTransacted(key, path.c_str(), 0, access, &handle, transaction.get(), nullptr);

    if (result != ERROR_SUCCESS) {
        throw WindowsError(result);
    }

    return handle;
}

HKEY RegistryKey::createRegistryKey(HKEY key, const std::wstring& path,
                                    const Transaction& transaction, REGSAM access) {
    HKEY handle = nullptr;
    auto result =
        RegCreateKeyTransacted(key, path.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, access,
                               nullptr, &handle, nullptr, transaction.get(), nullptr);

    if (result != ERROR_SUCCESS) {
        throw WindowsError(result);
    }

    return handle;
}

RegistryKey::RegistryKey(HKEY baseKey, const std::wstring& path, const Transaction& transaction,
                         REGSAM access, RegistryOpenType openType)
    : m_handle(openType == RegistryOpenType::Create
                   ? createRegistryKey(baseKey, path, transaction, access)
                   : openRegistryKey(baseKey, path, transaction, access)) {
    // Intentionally left blank.
}

void RegistryKey::deleteValue(const std::wstring& value) {
    auto result = RegDeleteValueW(m_handle.get(), value.c_str());
    if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND) {
        throw WindowsError(result);
    }
}

void RegistryKey::deleteTree() {
    auto result = RegDeleteTreeW(m_handle.get(), nullptr);
    if (result != ERROR_SUCCESS) {
        throw WindowsError(result);
    }
}

void RegistryKey::setStringValue(const std::wstring& name, const std::wstring& value) {
    const wchar_t* namePtr = name.empty() ? nullptr : name.c_str();
    auto result = RegSetValueEx(m_handle.get(), namePtr, 0, REG_SZ,
                                reinterpret_cast<const BYTE*>(value.c_str()),
                                static_cast<DWORD>(sizeof(wchar_t) * (value.size() + 1)));
    if (result != ERROR_SUCCESS) {
        throw WindowsError(result);
    }
}

}  // namespace winlogo::utils
