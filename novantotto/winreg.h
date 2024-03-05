#ifndef __WINREG_H
#define __WINREG_H

/* Registry value types */
#define REG_NONE 0
#define REG_SZ 1
#define REG_EXPAND_SZ 2
#define REG_BINARY 3
#define REG_DWORD 4
#define REG_DWORD_LITTLE_ENDIAN 4
#define REG_DWORD_BIG_ENDIAN 5
#define REG_LINK 6
#define REG_MULTI_SZ 7
#define REG_RESOURCE_LIST 8
#define REG_FULL_RESOURCE_DESCRIPTOR 9
#define REG_RESOURCE_REQUIREMENTS_LIST 10
#define REG_QWORD 11
#define REG_QWORD_LITTLE_ENDIAN 11

/* Predefined Keys */
#define HKEY_CLASSES_ROOT ((HKEY)0x80000000)
#define HKEY_CURRENT_USER ((HKEY)0x80000001)
#define HKEY_LOCAL_MACHINE ((HKEY)0x80000002)
#define HKEY_USERS ((HKEY)0x80000003)
#define HKEY_PERFORMANCE_DATA ((HKEY)0x80000004)
#define HKEY_CURRENT_CONFIG ((HKEY)0x80000005)
#define HKEY_DYN_DATA ((HKEY)0x80000006)

#define REG_OPTION_RESERVED 0x00000000
#define REG_OPTION_NON_VOLATILE 0x00000000
#define REG_OPTION_VOLATILE 0x00000001

#define REG_CREATED_NEW_KEY 0x00000001
#define REG_OPENED_EXISTING_KEY 0x00000002

#define KEY_READ 0
#define KEY_WRITE 0
#define KEY_EXECUTE 0
#define KEY_ALL_ACCESS 0

typedef unsigned long LSTATUS;
typedef ACCESS_MASK   REGSAM;

extern LSTATUS RegCloseKey(HKEY hKey);
extern LSTATUS RegConnectRegistry(LPCSTR lpMachineName, HKEY hKey, PHKEY phkResult);
extern LSTATUS RegCreateKey(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult);
extern LSTATUS RegCreateKeyEx(HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition);
extern LSTATUS RegDeleteKey(HKEY hKey, LPCSTR lpSubKey);
extern LSTATUS RegDeleteValue(HKEY hKey, LPCSTR lpValueName);
extern LSTATUS RegFlushKey(HKEY hKey);
extern LSTATUS RegOpenKey(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
extern LSTATUS RegOpenKeyEx(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
extern LSTATUS RegQueryValue(HKEY hKey, LPCSTR lpSubKey, LPSTR lpData, PLONG lpcbData);
extern LSTATUS RegQueryValueEx(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
extern LSTATUS RegSetValue(HKEY hKey, LPCSTR lpSubKey, DWORD dwType, LPCSTR lpData, DWORD cbData);
extern LSTATUS RegSetValueEx(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, CONST BYTE *lpData, DWORD cbData);

#endif
