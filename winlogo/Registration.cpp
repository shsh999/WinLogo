#include "Common.h"
#include "WindowsError.h"
#include "Registry.h"
#include "Transaction.h"
#include "ModuleHandle.h"
#include "Registration.h"

#include <stdexcept>
#include <ShlObj.h>
#include <filesystem>

// The GUID used in the registry for WinLogo. A macro is used for convenient string concatenation.
#define LOGO_GUID L"{55D087CC-5D80-46C7-BBE2-B73D98328FA5}"

extern "C" IMAGE_DOS_HEADER __ImageBase;

using winlogo::utils::RegistryKey;
using winlogo::utils::RegistryOpenType;
using winlogo::utils::Transaction;
using winlogo::utils::WindowsError;
using winlogo::utils::ModuleHandle;

namespace winlogo::registration {

/**
 * The registry operations required to create / destroy the WinLogo registration.
 */
// clang-format off
static RegistrationEntry g_registrationTable[] = {
    // General COM registration
    {L"Software\\Classes\\CLSID\\" LOGO_GUID, EntryOperation::Delete, nullptr, L"WinLogo"},
    {L"Software\\Classes\\CLSID\\" LOGO_GUID L"\\InProcServer32", EntryOperation::FileName, nullptr, nullptr},
    {L"Software\\Classes\\CLSID\\" LOGO_GUID L"\\InProcServer32", EntryOperation::Default, L"ThreadingModel", L"Apartment"},
    {L"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved", EntryOperation::Delete, LOGO_GUID, L"WinLogo"},
    {L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\   WinLogo", EntryOperation::Delete, nullptr, LOGO_GUID}
};
// clang-format on

/**
 * Try loading the registered WinLogo by triggering an IconOverlay reload.
 */
void tryLoadLogo() {
    SHLoadNonloadedIconOverlayIdentifiers();
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST | SHCNF_FLUSH, nullptr, nullptr);
}

static void deleteTree(const Transaction& transaction, const wchar_t* entryPath) {
    auto asPath = std::filesystem::path(entryPath);
    std::wstring subkey = asPath.filename().wstring();
    std::wstring keyPath = asPath.parent_path().wstring();
    auto key = RegistryKey(HKEY_LOCAL_MACHINE, keyPath.c_str(), transaction, KEY_ENUMERATE_SUB_KEYS,
                           RegistryOpenType::Open);
    key.deleteTree(subkey);
}

static void unregisterServer(const Transaction& transaction) {
    for (const auto& entry : g_registrationTable) {
        if (entry.operation != EntryOperation::Delete) {
            continue;
        }

        try {
            if (entry.name == nullptr) {
                deleteTree(transaction, entry.keyPath);
            } else {
                auto key = RegistryKey(HKEY_LOCAL_MACHINE, entry.keyPath, transaction,
                                       KEY_SET_VALUE, RegistryOpenType::Open);
                key.deleteValue(entry.name);
            }
        } catch (const WindowsError& error) {
            if (error.code().value() == ERROR_FILE_NOT_FOUND) {
                continue;
            }
            throw;
        }
    }
}

static bool registerServer(const Transaction& transaction) {
    unregisterServer(transaction);

    auto filePath = ModuleHandle::getModulePath(reinterpret_cast<HMODULE>(&__ImageBase));

    for (const auto& entry : g_registrationTable) {
        auto key = RegistryKey(HKEY_LOCAL_MACHINE, entry.keyPath, transaction, KEY_WRITE,
                               RegistryOpenType::Create);

        if (entry.operation != EntryOperation::FileName && !entry.value) {
            continue;
        }

        std::wstring value = entry.value != nullptr ? entry.value : filePath;
        std::wstring name = entry.name != nullptr ? entry.name : L"";
        key.setStringValue(name, value);
    }

    return true;
}

}  // namespace winlogo::registration

/**
 * Register the DLL as an Icon Overlay and trigger Explorer to load it.
 */
extern "C" HRESULT __stdcall DllRegisterServer() {
    try {
        Transaction transaction;
        winlogo::registration::registerServer(transaction);
        transaction.commit();
        winlogo::registration::tryLoadLogo();
    } catch (WindowsError& error) {
        return HRESULT_FROM_WIN32(error.code().value());
    } catch (...) {
        return S_FALSE;
    }

    return S_OK;
}

/**
 * Unregister the DLL as an Icon Overlay and trigger and stop the logo changes, if they are applied.
 */
extern "C" HRESULT __stdcall DllUnregisterServer() {
    try {
        Transaction transaction;
        winlogo::registration::unregisterServer(transaction);
        transaction.commit();
    } catch (WindowsError& error) {
        return HRESULT_FROM_WIN32(error.code().value());
    } catch (...) {
        return S_FALSE;
    }

    return S_OK;
}

/**
 * The dll doesn't actually export any COM object.
 */
extern "C" HRESULT __stdcall DllGetClassObject(_In_ REFCLSID, _In_ REFIID, _Outptr_ LPVOID FAR*) {
    return CLASS_E_CLASSNOTAVAILABLE;
}

/**
 * We can never be removed safely as the hook is installed.
 */
extern "C" HRESULT __stdcall DllCanUnloadNow() {
    return S_FALSE;
}
