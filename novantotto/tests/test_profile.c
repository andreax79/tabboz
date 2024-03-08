/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../windefs.h"
#include "miniunit.h"

int tests_run = 0;

void PrintFile(LPSTR lpFileName)
{
    printf("----------------------------\n");
    FILE *f = fopen(lpFileName, "r");
    if (f != NULL)
    {
        // Read contents from file
        char c = fgetc(f);
        while (c != EOF)
        {
            printf("%c", c);
            c = fgetc(f);
        }

        fclose(f);
    }
    printf("----------------------------\n");
}

static char *test_GetPrivateProfileString_existing_key()
{
    char  buffer[256];
    DWORD result;

    WritePrivateProfileString("TestSection", "AAA", "0000", "test.ini");
    PrintFile("test.ini");
    WritePrivateProfileString("TestSection", "AAA", "1111", "test.ini");
    PrintFile("test.ini");
    WritePrivateProfileString("TestSection", "BBB", "2222", "test.ini");
    PrintFile("test.ini");
    WritePrivateProfileString("TestSectionB", "CCC", "3333", "test.ini");
    PrintFile("test.ini");

    result = GetPrivateProfileString("TestSection", "AAA", "Default", buffer, sizeof(buffer), "test.ini");
    printf("%s\n", buffer);
    mu_assert("error, GetPrivateProfileString failed for existing key", result == strlen("1111"));
    mu_assert("error, GetPrivateProfileString returned incorrect value", (strcmp(buffer, "1111") == 0));

    return NULL;
}

static char *test_GetPrivateProfileString_nonexistent_key()
{
    char  buffer[256];
    DWORD result;

    result = GetPrivateProfileString("TestSection", "NonExistentKey", "Default", buffer, sizeof(buffer), "test.ini");
    mu_assert("error, GetPrivateProfileString failed for nonexistent key", result == strlen("Default"));
    mu_assert("error, GetPrivateProfileString returned incorrect value", strcmp(buffer, "Default") == 0);
    return NULL;
}

static char *test_WritePrivateProfileString_new_key_value()
{
    char  buffer[256];
    DWORD result;

    WritePrivateProfileString("TestSection", "NewKey", "NewValue", "test.ini");
    result = GetPrivateProfileString("TestSection", "NewKey", "Default", buffer, sizeof(buffer), "test.ini");
    mu_assert("WritePrivateProfileString failed to write new key-value pair", result == strlen("NewValue"));
    mu_assert("WritePrivateProfileString wrote incorrect value", strcmp(buffer, "NewValue") == 0);
    return NULL;
}

static char *test_WritePrivateProfileSection_multiple_key_values()
{
    char buffer1[256];
    char buffer2[256];
    char buffer3[256];
    WritePrivateProfileSection("MultipleKeysSection", "Key1=Value1\nKey2=Value2\nKey3=Value3", "test4.ini");
    PrintFile("test4.ini");

    GetPrivateProfileString("MultipleKeysSection", "Key1", "Default", buffer1, sizeof(buffer1), "test4.ini");
    GetPrivateProfileString("MultipleKeysSection", "Key2", "Default", buffer2, sizeof(buffer2), "test4.ini");
    GetPrivateProfileString("MultipleKeysSection", "Key3", "Default", buffer3, sizeof(buffer3), "test4.ini");

    mu_assert("WritePrivateProfileSection wrote incorrect value for Key1", strcmp(buffer1, "Value1") == 0);
    mu_assert("WritePrivateProfileSection wrote incorrect value for Key2", strcmp(buffer1, "Value1") == 0);
    mu_assert("WritePrivateProfileSection wrote incorrect value for Key3", strcmp(buffer3, "Value3") == 0);
    return NULL;
}

static char *all_tests()
{
    mu_run_test(test_GetPrivateProfileString_existing_key);
    mu_run_test(test_GetPrivateProfileString_nonexistent_key);
    mu_run_test(test_WritePrivateProfileString_new_key_value);
    mu_run_test(test_WritePrivateProfileSection_multiple_key_values);
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
