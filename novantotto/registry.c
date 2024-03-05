#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "novantotto.h"
#include "handler.h"
#include "winreg.h"
#include "winerror.h"
#include "debug.h"

static LPCSTR ROOT_KEYS[] = {
    "HKEY_CLASSES_ROOT",
    "HKEY_CURRENT_USER",
    "HKEY_LOCAL_MACHINE",
    "HKEY_USERS",
    "HKEY_PERFORMANCE_DATA",
    "HKEY_CURRENT_CONFIG",
    "HKEY_DYN_DATA",
};

static inline BOOL IsEmpty(LPCSTR s)
{
    return s == NULL || strlen(s) == 0;
}

static inline BOOL IsValidPathname(LPCSTR s)
{
    return IsEmpty(s) || s[0] != '\\';
}

static LPSTR ConcatenateKeys(LPCSTR k1, LPCSTR k2)
{
    if (IsEmpty(k1) && IsEmpty(k2))
    {
        return NULL;
    }
    else if (IsEmpty(k2))
    {
        return strdup(k1);
    }
    else if (IsEmpty(k1))
    {
        return strdup(k2);
    }
    else
    {
        LPSTR result;
        asprintf(&result, "%s\\%s", k1, k2);
        return result;
    }
}

LPSTR LocalStorageKey(HKEY hKey, LPCSTR lpSubKey, LSTATUS *pStatus)
{
    if (hKey >= HKEY_CLASSES_ROOT && hKey <= HKEY_DYN_DATA)
    {
        *pStatus = ERROR_SUCCESS;
        return ConcatenateKeys(ROOT_KEYS[hKey - HKEY_CLASSES_ROOT], lpSubKey);
    }
    else
    {
        HANDLE_ENTRY *handle = GetHandle((int)hKey);
        if (handle == NULL)
        {
            *pStatus = ERROR_INVALID_HANDLE;
            return NULL;
        }
        *pStatus = ERROR_SUCCESS;
        return ConcatenateKeys(handle->regKey.lpSubKey, lpSubKey);
    }
}

//*******************************************************************
// Open or create a registry key
//*******************************************************************

LSTATUS RegCreateKeyEx(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass,
                       DWORD dwOptions, REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                       PHKEY phkResult, LPDWORD lpdwDisposition)
{
    LSTATUS status;
    if (!IsValidPathname(lpSubKey))
    {
        return ERROR_BAD_PATHNAME;
    }
    LPSTR localStorageKey = LocalStorageKey(hKey, lpSubKey, &status);
    if (status != ERROR_SUCCESS)
    {
        return status;
    }
    // clang-format off
    EM_ASM({
        let item = localStorage.getItem(UTF8ToString($0));
        if (item == null) {
            localStorage.setItem(UTF8ToString($0), "{}");
        }
    }, localStorageKey);
    // clang-format on
    HANDLE_ENTRY *handle = AllocateHandle(RegKey, NULL);
    handle->regKey.lpSubKey = localStorageKey;
    *phkResult = (HKEY)handle->id;
    return status;
}

LSTATUS RegCreateKey(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult)
{
    LSTATUS status = RegCreateKeyEx(hKey, lpSubKey, 0, NULL, 0, 0, NULL, phkResult, NULL);
    if (status == ERROR_INVALID_HANDLE)
    {
        status = ERROR_BADKEY;
    }
    return status;
}

//*******************************************************************
// Open a registry key
//*******************************************************************

LSTATUS RegOpenKeyEx(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
    LSTATUS status;
    if (!IsValidPathname(lpSubKey))
    {
        return ERROR_BAD_PATHNAME;
    }
    LPSTR localStorageKey = LocalStorageKey(hKey, lpSubKey, &status);
    if (status != ERROR_SUCCESS)
    {
        return status;
    }
    // clang-format off
    status = EM_ASM_INT({
        let item = localStorage.getItem(UTF8ToString($0));
        if (item == null) {
            return 2; // ERROR_FILE_NOT_FOUND
        } else {
            return 0; // ERROR_SUCCESS
        }
    }, localStorageKey);
    // clang-format on
    if (status != ERROR_SUCCESS)
    {
        return status;
    }
    HANDLE_ENTRY *handle = AllocateHandle(RegKey, NULL);
    handle->regKey.lpSubKey = localStorageKey;
    *phkResult = (HKEY)handle->id;
    return status;
}

LSTATUS RegOpenKey(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
    return RegOpenKey(hKey, lpSubKey, 0, 0, phkResult);
}

//*******************************************************************
// Close a registry key handle
//*******************************************************************

LSTATUS RegCloseKey(HKEY hKey)
{
    if (hKey >= HKEY_CLASSES_ROOT)
    {
        return ERROR_SUCCESS;
    }
    HANDLE_ENTRY *handle = GetHandle((int)hKey);
    if (handle == NULL)
    {
        return ERROR_INVALID_HANDLE;
    }
    free(handle->regKey.lpSubKey);
    ReleaseHandle(handle);
    return ERROR_SUCCESS;
}

//*******************************************************************
// Set the data for a registry key
//*******************************************************************

LSTATUS RegSetValue(HKEY hKey, LPCSTR lpSubKey, DWORD dwType, LPCSTR lpData, DWORD cbData)
{
    LSTATUS status;
    if (dwType != REG_SZ)
    {
        return ERROR_INVALID_PARAMETER;
    }
    LPSTR localStorageKey = LocalStorageKey(hKey, lpSubKey, &status);
    if (localStorageKey == NULL)
    {
        return status;
    }
    // clang-format off
    EM_ASM({
        let item = localStorage.getItem(UTF8ToString($0));
        let dict = JSON.parse(item || "{}");
        dict[""] = UTF8ToString($1);
        item = JSON.stringify(dict);
        localStorage.setItem(UTF8ToString($0), item);
    }, localStorageKey, lpData);
    // clang-format on
    free(localStorageKey);
    return ERROR_SUCCESS;
}

//*******************************************************************
// Set data of a specified value under a registry key
//*******************************************************************

LSTATUS RegSetValueEx(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, CONST BYTE *lpData, DWORD cbData)
{
    LSTATUS status;
    LPSTR   localStorageKey = LocalStorageKey(hKey, "", &status);
    if (localStorageKey == NULL)
    {
        return status;
    }
    // clang-format off
    EM_ASM({
        let item = localStorage.getItem(UTF8ToString($0));
        let dict = JSON.parse(item || "{}");
        const key = UTF8ToString($1).toLowerCase();
        dict[key] = {};
        dict[key].type = $3;
        switch(dict[key].type) {
        case 4: //REG_DWORD
        case 5: //REG_DWORD_BIG_ENDIAN
            dict[key].value = getValue($2, "i32");
            break;
        case 11: //REG_QWORD_LITTLE_ENDIAN
            dict[key].value = getValue($2, "i64");
            break;
        default:
            dict[key].value = UTF8ToString($2);
            break;
        }
        item = JSON.stringify(dict);
        localStorage.setItem(UTF8ToString($0), item);
    }, localStorageKey, lpValueName, lpData, dwType);
    // clang-format on
    free(localStorageKey);
    return ERROR_SUCCESS;
}

//*******************************************************************
// Remove a key (and the subvalues) from the registry
//*******************************************************************

LSTATUS RegDeleteKey(HKEY hKey, LPCSTR lpSubKey)
{
    LSTATUS status;
    LPSTR   localStorageKey = LocalStorageKey(hKey, lpSubKey, &status);
    if (localStorageKey == NULL)
    {
        return status;
    }
    EM_ASM(localStorage.removeItem(UTF8ToString($0)), localStorageKey);
    free(localStorageKey);
    return ERROR_SUCCESS;
}

//*******************************************************************
// Remove a value from the registry
//*******************************************************************

LSTATUS RegDeleteValue(HKEY hKey, LPCSTR lpValueName)
{
    LSTATUS status;
    LPSTR   localStorageKey = LocalStorageKey(hKey, "", &status);
    if (localStorageKey == NULL)
    {
        return status;
    }
    // clang-format off
    status = EM_ASM_INT({
        let item = localStorage.getItem(UTF8ToString($0));
        if (item == null) {
            return 2; // ERROR_FILE_NOT_FOUND
        }
        let dict = JSON.parse(item);
        let value = dict[UTF8ToString($1).toLowerCase()];
        if (value == null) {
            return 2; // ERROR_FILE_NOT_FOUND
        }
        delete dict[UTF8ToString($1).toLowerCase()];
        item = JSON.stringify(dict);
        localStorage.setItem(UTF8ToString($0), item);
        return 0; // ERROR_SUCCESS
    }, localStorageKey, lpValueName);
    // clang-format on
    free(localStorageKey);
    return status;
}

//*******************************************************************
// Retrieves the registry key value
//*******************************************************************

LSTATUS RegQueryValue(HKEY hKey, LPCSTR lpSubKey, LPSTR lpData, PLONG lpcbData)
{
    LSTATUS status;
    if (!IsValidPathname(lpSubKey))
    {
        return ERROR_BAD_PATHNAME;
    }
    LPSTR localStorageKey = LocalStorageKey(hKey, lpSubKey, &status);
    if (localStorageKey == NULL)
    {
        return status;
    }
    // clang-format off
    status = EM_ASM_INT({
        let item = localStorage.getItem(UTF8ToString($0));
        if (item == null) {
            return 2; // ERROR_FILE_NOT_FOUND
        }
        let dict = JSON.parse(item);
        let value = dict[""];
        if (value == null) {
            return 2; // ERROR_FILE_NOT_FOUND
        }
        if ($2 != 0) {
            stringToUTF8(value, $1, $2);
        }
        return 0; // ERROR_SUCCESS
    }, localStorageKey, lpData, *lpcbData);
    // clang-format on
    if (status == ERROR_SUCCESS && lpData != NULL && lpcbData != NULL)
    {
        *lpcbData = strlen(lpData);
    }
    free(localStorageKey);
    return status;
}

//*******************************************************************
// Retrieves the data for a registry key value
//*******************************************************************

LSTATUS RegQueryValueEx(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
    LSTATUS status;
    LPSTR   localStorageKey = LocalStorageKey(hKey, "", &status);
    if (localStorageKey == NULL)
    {
        return status;
    }
    // clang-format off
    status = EM_ASM_INT({
        let item = localStorage.getItem(UTF8ToString($0));
        if (item == null) {
            return 2; // ERROR_FILE_NOT_FOUND
        }
        let dict = JSON.parse(item);
        const key = UTF8ToString($1).toLowerCase();
        if (dict[key] == null) {
            return 2; // ERROR_FILE_NOT_FOUND
        }
        if ($2 != 0) { // $2=lpData
            switch(dict[key].type) {
            case 4: //REG_DWORD
            case 5: //REG_DWORD_BIG_ENDIAN
                setValue($2, dict[key].value, "i32");
                break;
            case 11: //REG_QWORD_LITTLE_ENDIAN
                setValue($2, dict[key].value, "i64");
                break;
            default:
                stringToUTF8(dict[key].value, $2, $3);
                break;
            }
        }
        if ($4 != 0) { // $4=lpType
            setValue($4, dict[key].type, "i16");
        }
        return 0; // ERROR_SUCCESS
    }, localStorageKey, lpValueName, lpData, *lpcbData, lpType);
    if (status == ERROR_SUCCESS) {
        // clang-format on
        if (lpData != NULL && lpcbData != NULL)
        {
            switch (*lpType)
            {
            case REG_DWORD:
            case REG_DWORD_BIG_ENDIAN:
                *lpcbData = 4;
                break;
            case REG_QWORD_LITTLE_ENDIAN:
                *lpcbData = 8;
                break;
            default:
                *lpcbData = strlen((char *)lpData) + 1;
                break;
            }
        }
    }
    free(localStorageKey);
    return status;
}

//*******************************************************************
// No nothing
//*******************************************************************

LSTATUS RegConnectRegistry(LPCSTR lpMachineName, HKEY hKey, PHKEY phkResult)
{
    if (strlen(lpMachineName) == 0)
    {
        return ERROR_SUCCESS;
    }
    else
    {
        return ERROR_BAD_NETPATH;
    }
}

//*******************************************************************
// No nothing
//*******************************************************************

LSTATUS RegFlushKey(HKEY hKey)
{
    return ERROR_SUCCESS;
}
