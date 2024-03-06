/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "novantotto.h"
#include "handler.h"
#include "winreg.h"
#include "winerror.h"
#include "debug.h"

#define LINE_LEN 256
#define DEFAULT_INI_PATH "/win.ini"

//*******************************************************************
// Function to read a string value from an INI file
//*******************************************************************

DWORD GetPrivateProfileString(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
{
    char line[LINE_LEN];
    char searchKey[LINE_LEN];

    FILE *file = fopen(lpFileName, "r");
    if (file == NULL)
    {
        // Return the default value
        strncpy(lpReturnedString, lpDefault, nSize);
        return strlen(lpDefault);
    }

    snprintf(searchKey, sizeof(searchKey), "[%s]", lpAppName);

    // Find the section
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strncmp(line, searchKey, strlen(searchKey)) == 0)
        {
            break;
        }
    }

    // Find the key in the section
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == '[')
        {
            // End of section, key not found
            fclose(file);
            strncpy(lpReturnedString, lpDefault, nSize);
            return strlen(lpDefault);
        }

        char *token = strtok(line, "=");
        if (token != NULL)
        {
            if (strcmp(token, lpKeyName) == 0)
            {
                token = strtok(NULL, "\n");
                fclose(file);

                // Remove leading whitespaces
                while (*token == ' ' || *token == '\t')
                {
                    token++;
                }

                // Copy the value to the output buffer
                strncpy(lpReturnedString, token, nSize);
                return strlen(lpReturnedString);
            }
        }
    }

    // Not found
    fclose(file);
    strncpy(lpReturnedString, lpDefault, nSize);
    return strlen(lpReturnedString);
}

//*******************************************************************
// Write a string value to an INI file
//*******************************************************************

BOOL WritePrivateProfileString(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString, LPCSTR lpFileName)
{
    char  line[LINE_LEN];
    BOOL  bStatus = TRUE;
    LPSTR lpTempFileName;

    FILE *file = fopen(lpFileName, "r");
    if (file == NULL)
    {
        // File not found, create a new file
        file = fopen(lpFileName, "w");
        if (file == NULL)
        {
            return FALSE;
        }
        fprintf(file, "[%s]\n", lpAppName);
        fprintf(file, "%s=%s\n", lpKeyName, lpString);
        fclose(file);
        return TRUE;
    }

    // Create temp file
    asprintf(&lpTempFileName, "%s.tmp", lpFileName);
    FILE *tempFile = fopen(lpTempFileName, "w");
    if (tempFile == NULL)
    {
        fclose(file);
        free(lpTempFileName);
        return FALSE;
    }

    char searchKey[LINE_LEN];
    snprintf(searchKey, sizeof(searchKey), "[%s]", lpAppName);

    // Copy existing content to temporary file
    BOOL sectionFound = FALSE;
    BOOL keyFound = FALSE;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        fputs(line, tempFile);
        if (strncmp(line, searchKey, strlen(searchKey)) == 0)
        {
            // Section found, update or add the key
            sectionFound = TRUE;
            keyFound = FALSE;
            while (fgets(line, sizeof(line), file) != NULL)
            {
                if (line[0] == '[')
                {
                    // End of section, add the new key
                    fprintf(tempFile, "%s=%s\n", lpKeyName, lpString);
                    keyFound = TRUE;
                    break;
                }

                if (strncmp(line, lpKeyName, strlen(lpKeyName)) == 0 && line[strlen(lpKeyName)] == '=')
                {
                    // Update existing key
                    fprintf(tempFile, "%s=%s\n", lpKeyName, lpString);
                    keyFound = TRUE;
                    break;
                }
                fputs(line, tempFile);
            }

            if (!keyFound)
            {
                // Key not found in the section, add it
                fprintf(tempFile, "%s=%s\n", lpKeyName, lpString);
            }
        }
    }

    // If section not found, add the new section and key
    if (!sectionFound)
    {
        fprintf(tempFile, "[%s]\n", lpAppName);
        fprintf(tempFile, "%s=%s\n", lpKeyName, lpString);
    }

    // Close files
    fclose(file);
    fclose(tempFile);

    // Rename temporary file to original file
    if (rename(lpTempFileName, lpFileName) != 0)
    {
        remove(lpTempFileName);
        bStatus = FALSE;
    }

    free(lpTempFileName);
    return bStatus;
}

//*******************************************************************
// Function to retrieve a section from an INI file
//*******************************************************************

DWORD GetPrivateProfileSection(LPCSTR lpAppName, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName)
{
    char  line[LINE_LEN];
    char  searchKey[LINE_LEN];
    DWORD bytesRead = 0;

    FILE *file = fopen(lpFileName, "r");
    if (file == NULL)
    {
        lpReturnedString[0] = '\0';
        return 0;
    }

    snprintf(searchKey, sizeof(searchKey), "[%s]", lpAppName);

    // Find the section
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (strncmp(line, searchKey, strlen(searchKey)) == 0)
        {
            break;
        }
    }

    // Copy the section content to the output buffer
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == '[')
        {
            // End of section, break
            break;
        }

        // Copy the line to the output buffer
        strncpy(lpReturnedString + bytesRead, line, nSize - bytesRead);
        bytesRead += strlen(line);

        // Check if the buffer is full
        if (bytesRead >= nSize)
        {
            break;
        }
    }

    fclose(file);
    return bytesRead;
}

//*******************************************************************
// Function to write a section to an INI file
//*******************************************************************

BOOL WritePrivateProfileSection(LPCSTR lpAppName, LPCSTR lpString, LPCSTR lpFileName)
{
    char  line[LINE_LEN];
    BOOL  bStatus = TRUE;
    LPSTR lpTempFileName;
    char  searchKey[LINE_LEN];

    FILE *file = fopen(lpFileName, "r");
    if (file == NULL)
    {
        // File not found, create a new file
        file = fopen(lpFileName, "w");
        if (file == NULL)
        {
            return FALSE;
        }
        fprintf(file, "[%s]\n", lpAppName);
        fprintf(file, "%s\n", lpString);
        fclose(file);
        return TRUE;
    }

    asprintf(&lpTempFileName, "%s.tmp", lpFileName);
    FILE *tempFile = fopen(lpTempFileName, "w");
    if (tempFile == NULL)
    {
        fclose(file);
        free(lpTempFileName);
        return FALSE;
    }

    snprintf(searchKey, sizeof(searchKey), "[%s]", lpAppName);

    // Copy existing content to temporary file
    BOOL sectionFound = FALSE;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        fputs(line, tempFile);
        if (strncmp(line, searchKey, strlen(searchKey)) == 0)
        {
            // Section found, skip existing content
            sectionFound = TRUE;
            while (fgets(line, sizeof(line), file) != NULL)
            {
                if (line[0] == '[')
                {
                    // End of section, break
                    break;
                }
            }
        }
    }

    // If section not found, add the new section and content
    if (!sectionFound)
    {
        fprintf(tempFile, "[%s]\n", lpAppName);
    }

    // Add the new section content
    fprintf(tempFile, "%s\n", lpString);

    // Close files
    fclose(file);
    fclose(tempFile);

    // Rename temporary file to original file
    if (rename(lpTempFileName, lpFileName) != 0)
    {
        remove(lpTempFileName);
        bStatus = FALSE;
    }

    free(lpTempFileName);
    return bStatus;
}

//*******************************************************************
// Get an integer associated with a key from an INI file
//*******************************************************************

UINT GetPrivateProfileInt(LPSTR lpAppName, LPSTR lpKeyName, INT nDefault, LPSTR lpFileName)
{
    char  lpBuffer[LINE_LEN];
    LPSTR lpDefault;
    asprintf(&lpDefault, "%d", nDefault);
    GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpBuffer, 5, lpFileName);
    free(lpDefault);
    return atoi(lpBuffer);
}

//*******************************************************************
// Get a string associated with a key
//*******************************************************************

DWORD GetProfileString(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize)
{
    return GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, DEFAULT_INI_PATH);
}

//*******************************************************************
// Get an integer associated with a key
//*******************************************************************

UINT GetProfileInt(LPSTR lpAppName, LPSTR lpKeyName, INT nDefault)
{
    return GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, DEFAULT_INI_PATH);
}

//*******************************************************************
// Write a string value
//*******************************************************************

BOOL WriteProfileString(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString)
{
    return WritePrivateProfileString(lpAppName, lpKeyName, lpString, DEFAULT_INI_PATH);
}
