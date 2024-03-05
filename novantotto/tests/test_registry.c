#include <stdio.h>
#include <string.h>
#include "../windefs.h"
#include "../handler.h"
#include "../winreg.h"
#include "../winerror.h"
#include "miniunit.h"

int          tests_run = 0;
extern LPSTR LocalStorageKey(HKEY hKey, LPCSTR lpSubKey, LSTATUS *pStatus);

static char *test_query_value()
{
    LSTATUS       status;
    unsigned long lType;
    unsigned long lLen = 64;
    LPSTR         sVal = malloc(lLen);

    lLen = 64;
    status = RegQueryValue((HKEY)9999, "", NULL, &lLen);
    mu_assert("error, RegQueryValue status != ERROR_INVALID_HANDLE", status == ERROR_INVALID_HANDLE);

    lLen = 64;
    status = RegQueryValue(HKEY_CURRENT_USER, "test", NULL, &lLen);
    mu_assert("error, RegQueryValue status != ERROR_FILE_NOT_FOUND", status == ERROR_FILE_NOT_FOUND);

    lLen = 64;
    status = RegQueryValue(HKEY_CURRENT_USER, "\\novantotto", NULL, &lLen);
    mu_assert("error, RegQueryValue status != ERROR_BAD_PATHNAME", status == ERROR_BAD_PATHNAME);

    lLen = 64;
    status = RegQueryValue(HKEY_CURRENT_USER, "test", sVal, &lLen);
    mu_assert("error, RegQueryValue status != ERROR_FILE_NOT_FOUND", status == ERROR_FILE_NOT_FOUND);

    status = RegQueryValueEx((HKEY)9999, "", 0, &lType, (LPBYTE)sVal, &lLen);
    mu_assert("error, RegQueryValueEx status != ERROR_INVALID_HANDLE", status == ERROR_INVALID_HANDLE);

    return NULL;
}

static char *test_set_query_delete()
{
    LSTATUS       status;
    HKEY          hkey;
    unsigned long lLen = 64;
    LPSTR         sVal = malloc(lLen);

    status = RegCreateKey(HKEY_CURRENT_USER, "novantotto", &hkey);
    mu_assert("error, RegCreateKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegSetValue(hkey, "key1", REG_SZ, NULL, 0);
    mu_assert("error, RegSetValue status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegSetValue((HKEY)9999, "", 0, NULL, 0);
    mu_assert("error, RegSetValue status != ERROR_INVALID_PARAMETER", status == ERROR_INVALID_PARAMETER);

    status = RegSetValue((HKEY)9999, "novantotto", 0, NULL, 0);
    mu_assert("error, RegSetValue status != ERROR_INVALID_PARAMETER", status == ERROR_INVALID_PARAMETER);

    status = RegSetValue((HKEY)9999, "novantotto", REG_SZ, NULL, 0);
    mu_assert("error, RegSetValue status != ERROR_INVALID_HANDLE", status == ERROR_INVALID_HANDLE);

    status = RegSetValue(HKEY_LOCAL_MACHINE, "novantotto", REG_SZ, NULL, 0);
    mu_assert("error, RegSetValue status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegSetValue(HKEY_CURRENT_USER, "novantotto", REG_SZ, "test-string", 0);
    mu_assert("error, RegSetValue status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    lLen = 64;
    status = RegQueryValue(HKEY_CURRENT_USER, "novantotto", sVal, &lLen);
    mu_assert("error, RegQueryValue status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    mu_assert("error, RegQueryValue value != test-string", strcmp(sVal, "test-string") == 0);

    status = RegDeleteKey(HKEY_CURRENT_USER, "novantotto");
    mu_assert("error, RegDeleteKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    lLen = 64;
    status = RegQueryValue(HKEY_CURRENT_USER, "novantotto", sVal, &lLen);
    mu_assert("error, RegQueryValue status != ERROR_FILE_NOT_FOUND", status == ERROR_FILE_NOT_FOUND);

    status = RegCloseKey(hkey);
    mu_assert("error, RegCloseKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    return NULL;
}

static char *test_set_query_delete_value()
{
    LSTATUS       status;
    HKEY          hkey;
    unsigned long lLen = 64;
    LPSTR         sVal = malloc(lLen);
    DWORD         lType;
    DWORD         value;

    status = RegCreateKey(HKEY_CURRENT_USER, "novantotto", &hkey);
    mu_assert("error, RegCreateKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegSetValue(hkey, NULL, REG_SZ, "test-string", 0);
    mu_assert("error, RegSetValue status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegSetValueEx(hkey, "k1", 0, REG_SZ, (const BYTE *)"1111", 0);
    mu_assert("error, RegSetValueEx status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegSetValueEx(hkey, "k2", 0, REG_SZ, (const BYTE *)"2222", 0);
    mu_assert("error, RegSetValueEx status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    value = 3333;
    status = RegSetValueEx(hkey, "k3", 0, REG_DWORD, (const BYTE *)&value, 0);
    mu_assert("error, RegSetValueEx status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    lLen = 64;
    status = RegQueryValue(hkey, "", sVal, &lLen);
    mu_assert("error, RegQueryValue status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    mu_assert("error, RegQueryValueEx type != REG_SZ", lType == REG_SZ);
    mu_assert("error, RegQueryValue value != test-string", strcmp(sVal, "test-string") == 0);

    lLen = 64;
    status = RegQueryValueEx(hkey, "k1", 0, &lType, (LPBYTE)sVal, &lLen);
    mu_assert("error, RegQueryValueEx status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    mu_assert("error, RegQueryValueEx type != REG_SZ", lType == REG_SZ);
    mu_assert("error, RegQueryValueEx value != 1111", strcmp(sVal, "1111") == 0);

    lLen = 64;
    status = RegQueryValueEx(hkey, "k2", 0, &lType, (LPBYTE)sVal, &lLen);
    mu_assert("error, RegQueryValueEx status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    mu_assert("error, RegQueryValueEx type != REG_SZ", lType == REG_SZ);
    mu_assert("error, RegQueryValueEx value != 2222", strcmp(sVal, "2222") == 0);

    lLen = 64;
    status = RegQueryValueEx(hkey, "k3", 0, &lType, (LPBYTE)sVal, &lLen);
    mu_assert("error, RegQueryValueEx status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    mu_assert("error, RegQueryValueEx type != REG_DWORD", lType == REG_DWORD);
    mu_assert("error, RegQueryValueEx value != 3333", value == 3333);

    status = RegDeleteValue(hkey, "k2");
    mu_assert("error, RegDeleteValue status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegDeleteValue(hkey, "k9");
    mu_assert("error, RegDeleteValue status != ERROR_FILE_NOT_FOUND", status == ERROR_FILE_NOT_FOUND);

    lLen = 64;
    status = RegQueryValueEx(hkey, "k1", 0, &lType, (LPBYTE)sVal, &lLen);
    mu_assert("error, ReqQueryValueEx status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    mu_assert("error, ReqQueryValueEx value != 1111", strcmp(sVal, "1111") == 0);

    lLen = 64;
    status = RegQueryValueEx(hkey, "k2", 0, &lType, (LPBYTE)sVal, &lLen);
    mu_assert("error, ReqQueryValueEx status != ERROR_FILE_NOT_FOUND", status == ERROR_FILE_NOT_FOUND);

    status = RegDeleteKey(hkey, "");
    mu_assert("error, RegDeleteKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    lLen = 64;
    status = RegQueryValueEx(hkey, "k1", 0, &lType, (LPBYTE)sVal, &lLen);
    mu_assert("error, RegQueryValueEx status != ERROR_FILE_NOT_FOUND", status == ERROR_FILE_NOT_FOUND);

    status = RegCloseKey(hkey);
    mu_assert("error, RegCloseKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    return NULL;
}

static char *test_open()
{
    LSTATUS       status;
    HKEY          hkey;
    HKEY          hkey2;
    unsigned long lLen = 64;
    LPSTR         sVal = malloc(lLen);

    status = RegCreateKey(HKEY_CURRENT_USER, "novantotto", &hkey);
    mu_assert("error, RegCreateKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegOpenKeyEx(HKEY_CURRENT_USER, "novantotto", 0, 0, &hkey2);
    mu_assert("error, RegOpenKeyEx status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegDeleteKey(HKEY_CURRENT_USER, "novantotto");
    mu_assert("error, RegDeleteKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegOpenKeyEx(HKEY_CURRENT_USER, "novantotto", 0, 0, &hkey2);
    mu_assert("error, RegOpenKeyEx status == ERROR_SUCCESS", status != ERROR_SUCCESS);

    status = RegCloseKey(hkey);
    mu_assert("error, RegCloseKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegCloseKey(hkey2);
    mu_assert("error, RegCloseKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    return NULL;
}

static char *test_close()
{
    LSTATUS status;

    status = RegCloseKey((HKEY)9999);
    mu_assert("error, RegCloseKey status != ERROR_INVALID_HANDLE", status == ERROR_INVALID_HANDLE);

    status = RegCloseKey(HKEY_LOCAL_MACHINE);
    mu_assert("error, RegCloseKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegCloseKey(HKEY_LOCAL_MACHINE);
    mu_assert("error, RegCloseKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    return NULL;
}

static char *test_connect()
{
    LSTATUS status;
    HKEY    hkey;

    status = RegConnectRegistry("", (HKEY)9999, &hkey);
    mu_assert("error, RegConnectRegistry status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegConnectRegistry("", HKEY_LOCAL_MACHINE, &hkey);
    mu_assert("error, RegConnectRegistry status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    status = RegConnectRegistry("\\\\novantotto", HKEY_LOCAL_MACHINE, &hkey);
    mu_assert("error, RegConnectRegistry status != ERROR_BAD_NETPATH", status == ERROR_BAD_NETPATH);

    return NULL;
}

static char *test_local_storage_key_default()
{
    LSTATUS status;
    HKEY    hkey;
    LPSTR   l;

    l = LocalStorageKey(HKEY_LOCAL_MACHINE, "", &status);
    mu_assert("error, LocalStorageKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    mu_assert("error, LocalStorageKey l != HKEY_LOCAL_MACHINE", strcmp(l, "HKEY_LOCAL_MACHINE") == 0);
    free(l);

    l = LocalStorageKey(HKEY_LOCAL_MACHINE, "test", &status);
    mu_assert("error, LocalStorageKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    mu_assert("error, LocalStorageKey l != HKEY_LOCAL_MACHINE\\test", strcmp(l, "HKEY_LOCAL_MACHINE\\test") == 0);
    free(l);

    return NULL;
}

static char *test_create_key()
{
    LSTATUS status;
    HKEY    hkey;
    DWORD   dwDisp;

    status = RegCreateKey((HKEY)9999, "", &hkey);
    mu_assert("error, RegCreateKey status != ERROR_BADKEY", status == ERROR_BADKEY);

    status = RegCreateKey(HKEY_LOCAL_MACHINE, "", &hkey);
    mu_assert("error, RegCreateKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    RegCloseKey(hkey);

    status = RegCreateKey(HKEY_LOCAL_MACHINE, "\\aa", &hkey);
    mu_assert("error, RegCreateKey status != ERROR_BAD_PATHNAME", status == ERROR_BAD_PATHNAME);

    status = RegCreateKey(HKEY_LOCAL_MACHINE, "\\aa\\", &hkey);
    mu_assert("error, RegCreateKey status != ERROR_BAD_PATHNAME", status == ERROR_BAD_PATHNAME);

    status = RegCreateKeyEx((HKEY)9999, "", 0, "", 0, 0, NULL, &hkey, &dwDisp);
    mu_assert("error, RegCreateKeyEx status != ERROR_INVALID_HANDLE", status == ERROR_INVALID_HANDLE);
    RegCloseKey(hkey);

    status = RegCreateKeyEx(HKEY_CURRENT_USER, "novantotto",
                            0, NULL, REG_OPTION_NON_VOLATILE,
                            KEY_ALL_ACCESS, NULL, &hkey, NULL);
    mu_assert("error, RegCreateKeyEx status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    RegCloseKey(hkey);

    return NULL;
}

static char *test_local_storage_key_custom()
{
    LSTATUS status;
    HKEY    hkey;
    LPSTR   l;

    status = RegCreateKeyEx(HKEY_CURRENT_USER, "aa\\bb\\cc",
                            0, NULL, REG_OPTION_NON_VOLATILE,
                            KEY_ALL_ACCESS, NULL, &hkey, NULL);
    mu_assert("error, RegCreateKeyEx status != ERROR_SUCCESS", status == ERROR_SUCCESS);

    l = LocalStorageKey(hkey, "", &status);
    mu_assert("error, LocalStorageKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    mu_assert("error, LocalStorageKey HKEY_CURRENT_USER\\aa\\bb\\cc", strcmp(l, "HKEY_CURRENT_USER\\aa\\bb\\cc") == 0);
    free(l);

    l = LocalStorageKey(hkey, "test", &status);
    mu_assert("error, LocalStorageKey status != ERROR_SUCCESS", status == ERROR_SUCCESS);
    mu_assert("error, LocalStorageKey HKEY_CURRENT_USER\\aa\\bb\\cc\\test", strcmp(l, "HKEY_CURRENT_USER\\aa\\bb\\cc\\test") == 0);
    free(l);

    RegCloseKey(hkey);
    return NULL;
}

static char *all_tests()
{
    mu_run_test(test_create_key);
    mu_run_test(test_open);
    mu_run_test(test_close);
    mu_run_test(test_connect);
    mu_run_test(test_local_storage_key_default);
    mu_run_test(test_local_storage_key_custom);
    mu_run_test(test_query_value);
    mu_run_test(test_set_query_delete);
    mu_run_test(test_set_query_delete_value);
    return 0;
}

int main(int argc, char **argv)
{
    char *result = all_tests();
    if (result != 0)
    {
        printf("%s\n", result);
    }
    else
    {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
