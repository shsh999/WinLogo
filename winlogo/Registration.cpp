#include "Common.h"
#include "WindowsError.h"
#include "Registry.h"
#include "Transaction.h"
#include "Event.h"
#include <stdexcept>
#include <ShlObj.h>

static const auto LOGO_GUID = L"{55D087CC-5D80-46C7-BBE2-B73D98328FA5}";
static const std::wstring ICON_OVERLAY_KEY =
    LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\ShellIconOverlayIdentifiers\   WinLogo\)" + std::wstring(LOGO_GUID);
    /*static const GUID LOGO_GUID
    = {0x55d087cc, 0x5d80, 0x46c7, {0xbb, 0xe2, 0xb7, 0x3d, 0x98, 0x32, 0x8f, 0xa5}};*/


extern "C" IMAGE_DOS_HEADER __ImageBase;

void refreshIconOverlays() {
    SHLoadNonloadedIconOverlayIdentifiers();
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST | SHCNF_FLUSH, nullptr, nullptr);
}

void tryStopLogo() {
    try {
        winlogo::utils::Event event(L"WinLogoEvent", false);
        event.set();
    } catch (const winlogo::utils::WindowsError&) {
        // Intentionally left blank.
    }
}

enum class EntryOption {
    None,
    Delete,
    FileName,
};

struct Entry {
    const wchar_t* Path;
    EntryOption Option;

    const wchar_t* Name;
    const wchar_t* Value;
};

static Entry g_registrationTable[] = {
    // General COM registration
    {L"Software\\Classes\\CLSID\\{55D087CC-5D80-46C7-BBE2-B73D98328FA5}", EntryOption::Delete,
     nullptr, L"WinLogo"},
    {
        L"Software\\Classes\\CLSID\\{55D087CC-5D80-46C7-BBE2-B73D98328FA5}\\InProcServer32",
        EntryOption::FileName,
    },
    {L"Software\\Classes\\CLSID\\{55D087CC-5D80-46C7-BBE2-B73D98328FA5}\\InProcServer32",
     EntryOption::None, L"ThreadingModel", L"Apartment"},

    {L"Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",
     EntryOption::Delete, L"{55D087CC-5D80-46C7-BBE2-B73D98328FA5}", L"WinLogo"},
    {L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\   WinLogo",
     EntryOption::Delete, nullptr, L"{55D087CC-5D80-46C7-BBE2-B73D98328FA5}"}};


static void unregisterServer(const winlogo::utils::Transaction& transaction) {
    for (const auto& entry : g_registrationTable) {
        if (entry.Option != EntryOption::Delete) {
            continue;
        }

        try {
            auto key = 
                winlogo::utils::RegistryKey(HKEY_LOCAL_MACHINE, entry.Path, transaction,
                              DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_SET_VALUE,
                              winlogo::utils::RegistryOpenType::Open);

            if (entry.Name == nullptr) {
                key.deleteTree();
            } else {
                key.deleteValue(entry.Name);
            }
        } catch (const winlogo::utils::WindowsError& error) {
            if (error.code().value() == ERROR_FILE_NOT_FOUND) {
                continue;
            }
            throw;
        }
    }
}

std::wstring getCurrentModulePath() {
    std::wstring result(MAX_PATH, L'\0');

    const auto length =
        GetModuleFileNameW(reinterpret_cast<HMODULE>(&__ImageBase), result.data(), static_cast<DWORD>(result.size()));
    if (length == 0 || length == result.size()) {
        throw winlogo::utils::WindowsError();
    }

    result.resize(length);

    return result;
}

static bool registerServer(const winlogo::utils::Transaction& transaction) {
    unregisterServer(transaction);

    auto filePath = getCurrentModulePath();

    for (const auto& entry : g_registrationTable) {
        auto key = winlogo::utils::RegistryKey(HKEY_LOCAL_MACHINE, entry.Path, transaction, KEY_WRITE,
                                    winlogo::utils::RegistryOpenType::Create);
        OutputDebugStringW(entry.Path);

        if (entry.Option != EntryOption::FileName && !entry.Value) {
            continue;
        }
        std::wstring value = entry.Value != nullptr ? entry.Value : filePath;
        std::wstring name = entry.Name ? entry.Name : L"";
        key.setStringValue(name, value);
    }

    return true;
}

extern "C" HRESULT __stdcall DllRegisterServer() {
    try {
        OutputDebugStringA("Register!");
        winlogo::utils::Transaction transaction;
        registerServer(transaction);
        transaction.commit();
        refreshIconOverlays();
    } catch (winlogo::utils::WindowsError& error) {
        OutputDebugStringA("Error!");
        return HRESULT_FROM_WIN32(error.code().value());
    } catch (...) {
        OutputDebugStringA("Real error!");
    }
    
    return S_OK;
}

extern "C" HRESULT __stdcall DllUnregisterServer() {
    try {
        winlogo::utils::Transaction transaction;
        unregisterServer(transaction);
        transaction.commit();
        tryStopLogo();
    } catch (winlogo::utils::WindowsError& error) {
        return HRESULT_FROM_WIN32(error.code().value());
    }

    return S_OK;
}

extern "C" HRESULT __stdcall DllGetClassObject(_In_ REFCLSID, _In_ REFIID, _Outptr_ LPVOID FAR*)
{
    return CLASS_E_CLASSNOTAVAILABLE;
}

extern "C" HRESULT __stdcall DllCanUnloadNow() {
    return S_FALSE;
}
