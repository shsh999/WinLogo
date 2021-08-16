#pragma once
#include "Common.h"
#include "Handle.h"
#include "WindowsError.h"

#include <ktmw32.h>

#pragma comment(lib, "ktmw32.lib")

namespace winlogo::utils {

namespace details {

/**
 * Windows Transaction handle traits.
 */
struct TransactionTraits {
    using HandleType = HANDLE;

    static constexpr const HandleType INVALID_HANDLE = INVALID_HANDLE_VALUE;

    static void close(HandleType handle) noexcept {
        CloseHandle(handle);
    }
};

using TransactionHandle = Handle<TransactionTraits>;

}  // namespace details


/**
 * This is a wrapper class for win32 transactions.
 * Destruction without calling the "commit" function will revert the transaction.
 */
class Transaction {
public:
    Transaction()
        : m_handle(CreateTransaction(nullptr, nullptr, TRANSACTION_DO_NOT_PROMOTE, 0, 0, INFINITE,
                                     nullptr)) {
    }

    /**
     * Get the transaction handle, for use by functions such as CreateFileTransacted.
     */
    HANDLE get() const noexcept {
        return m_handle.get();
    }

    /**
     * Commit the changes done by this transaction.
     */
    void commit() {
        if (!CommitTransaction(get())) {
            throw WindowsError();
        }
    }

private:
    details::TransactionHandle m_handle;
};

}  // namespace winlogo::utils
