#pragma once
#include "Common.h"
#include "Handle.h"
#include "Transaction.h"

namespace winlogo::utils {

namespace details {

/**
 * Windows Registry key handle traits.
 */
struct RegistryHandleTraits {
    using HandleType = HKEY;

    static constexpr const HandleType INVALID_HANDLE = nullptr;

    static void close(HandleType handle) noexcept {
        RegCloseKey(handle);
    }
};

using RegistryKeyHandle = Handle<RegistryHandleTraits>;

}  // namespace details

/**
 * A flag used to determine whether a registry key should be opened or created.
 */
enum class RegistryOpenType { Open, Create };

/**
 * A wrapper class for transacted registry keys.
 */
class RegistryKey {
public:
    RegistryKey(HKEY baseKey, const std::wstring& path, const Transaction& transaction,
                REGSAM access, RegistryOpenType openType);

    /**
     * Delete the value with the given name.
     * The operation is considered successful if the key doesn't exist in the first place.
     */
    void deleteValue(const std::wstring& value);

    /**
     * Delete all sub-keys and values of the key.
     */
    void deleteTree();

    /**
     * Set a string value in the registry.
     */
    void setStringValue(const std::wstring& name, const std::wstring& value);

private:
    details::RegistryKeyHandle m_handle;

    static HKEY openRegistryKey(HKEY key, const std::wstring& path, const Transaction& transaction,
                                REGSAM access);

    static HKEY createRegistryKey(HKEY key, const std::wstring& path,
                                  const Transaction& transaction, REGSAM access);
};

}  // namespace winlogo::utils
