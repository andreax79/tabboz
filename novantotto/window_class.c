/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include <stdio.h>
#include "novantotto.h"
#include "handle.h"
#include "debug.h"

static PROPERTIES *global_classes;

//*******************************************************************
// Register a window class
//*******************************************************************

ATOM RegisterClass(const WNDCLASS *lpWndClass)
{
    if (!global_classes)
    {
        DEBUG_PRINTF("RegisterClass global_classes is null\n");
        if (!(global_classes = AllocateProperties()))
        {
            return 0;
        }
    }
    WNDCLASS *h = malloc(sizeof(WNDCLASS));
    if (h == NULL)
    {
        return 0;
    }
    memcpy(h, lpWndClass, sizeof(WNDCLASS));
    // Check if already exists
    UnregisterClass(lpWndClass->lpszClassName, NULL);
    // Register the class
    SetProperty(global_classes, lpWndClass->lpszClassName, (HANDLE)h);
    return (ATOM)h;
}

//*******************************************************************
// Unregister a window class
//*******************************************************************

BOOL UnregisterClass(LPCSTR lpClassName, HANDLE hInstance)
{
    if (!global_classes)
    {
        return FALSE;
    }
    LPWNDCLASS wndClass = (LPWNDCLASS)DelProperty(global_classes, lpClassName);
    if (wndClass == NULL)
    {
        return FALSE;
    }
    if (wndClass->hIcon)
    {
        DestroyIcon(wndClass->hInstance);
    }
    if (wndClass->hCursor)
    {
        DestroyCursor(wndClass->hCursor);
    }
    if (wndClass->hbrBackground)
    {
        DeleteObject(wndClass->hbrBackground);
    }
    free(wndClass);
    return TRUE;
}

//*******************************************************************
// Get information about a window class.
//*******************************************************************

BOOL GetClassInfo(HINSTANCE hInstance, LPCSTR lpClassName, LPWNDCLASS lpWndClass)
{
    LPWNDCLASS wndClass = (LPWNDCLASS)GetProperty(global_classes, lpClassName);
    if (wndClass == NULL)
    {
        return FALSE;
    }
    memcpy(lpWndClass, wndClass, sizeof(WNDCLASS));
    return TRUE;
}
