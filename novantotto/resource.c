/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <string.h>
#include "novantotto.h"
#include "handle.h"
#include "debug.h"

//*******************************************************************
// Allocate a resource
//*******************************************************************

static HICON AllocateResource(HINSTANCE hInstance, LPCSTR lpResourceName, HandleType type)
{
    // Alocate the handle
    RESOURCE *res = AllocateHandle(type, NULL);
    if (res == NULL)
    {
        return NULL;
    }
    if (IS_INT_RESOURCE(lpResourceName))
    {
        // Convert the number into a string
        asprintf(&res->lpResourceName, "%d", (int)lpResourceName);
    }
    else
    {
        // Duplicate the string
        res->lpResourceName = strdup(lpResourceName);
    }
    return res->handle;
}

//*******************************************************************
// Destroy a resource handle
//*******************************************************************

static BOOL DestroyResource(HANDLE hHandle, HandleType type)
{
    if (hHandle == NULL)
    {
        return FALSE;
    }
    RESOURCE *res = GetHandle(hHandle, type);
    if (res == NULL)
    {
        return FALSE;
    }
    free(res->lpResourceName);
    ReleaseHandle(res);
    return TRUE;
}

//*******************************************************************
// Allocate a menu resource
//*******************************************************************

HMENU LoadMenu(HINSTANCE hInstance, LPCSTR lpMenuName)
{
    return (HMENU)AllocateResource(hInstance, lpMenuName, HANDLE_MENU);
}

//*******************************************************************
// Destroy menu handle
//*******************************************************************

BOOL DestroyMenu(HMENU hMenu)
{
    return DestroyResource(hMenu, HANDLE_MENU);
}

//*******************************************************************
// Allocate a icon resource
//*******************************************************************

HICON LoadIcon(HINSTANCE hInstance, LPCSTR lpIconName)
{
    return (HICON)AllocateResource(hInstance, lpIconName, HANDLE_ICON);
}

//*******************************************************************
// Destroy icon handle
//*******************************************************************

BOOL DestroyIcon(HICON hIcon)
{
    return DestroyResource(hIcon, HANDLE_ICON);
}

//*******************************************************************
// Allocate a cursor resource
//*******************************************************************

HCURSOR LoadCursor(HINSTANCE hInstance, LPCSTR lpCursorName)
{
    return (HCURSOR)AllocateResource(hInstance, lpCursorName, HANDLE_CURSOR);
}

//*******************************************************************
// Destroy cursor handle
//*******************************************************************

BOOL DestroyCursor(HCURSOR hCursor)
{
    return DestroyResource(hCursor, HANDLE_CURSOR);
}

//*******************************************************************
// Allocate an accellerators resource
//*******************************************************************

HACCEL LoadAccelerators(HINSTANCE hInstance, LPCSTR lpTableName)
{
    return (HACCEL)AllocateResource(hInstance, lpTableName, HANDLE_ACCELLERATORS);
}

//*******************************************************************
// Destroy accellerators handle
//*******************************************************************

extern BOOL DestroyAcceleratorTable(HACCEL hAccel)
{
    return DestroyResource(hAccel, HANDLE_ACCELLERATORS);
}

//*******************************************************************
// Allocate a bitmap resource
//*******************************************************************

HBITMAP LoadBitmap(HINSTANCE hInstance, LPCSTR lpBitmapName)
{
    return (HBITMAP)AllocateResource(hInstance, lpBitmapName, HANDLE_OBJECT);
}

//*******************************************************************
// Destroy a logical pen, brush, font, bitmap, region, or palette handle
//*******************************************************************

BOOL DeleteObject(HANDLE hHandle)
{
    return DestroyResource(hHandle, HANDLE_OBJECT);
}

//*******************************************************************
// Draw a bitmap
//*******************************************************************

BOOL DrawBitmap(HDC hdc, LPCSTR lpCanvasClass, HBITMAP hBitmap, int X, int Y)
{
    if (hdc == NULL)
    {
        return FALSE;
    }
    RESOURCE *res = GetHandle(hBitmap, HANDLE_OBJECT);
    if (res == NULL)
    {
        return FALSE;
    }
    if (res->lpResourceName == NULL)
    {
        return FALSE;
    }
    JS_ASYNC_CALL("drawImage", (int)hdc, lpCanvasClass, res->lpResourceName, X, Y);
    return TRUE;
}

//*******************************************************************
// Set window icon
//*******************************************************************

BOOL SetIcon(HWND hWnd, HICON hIcon)
{
    if (hWnd == NULL)
    {
        return FALSE;
    }
    RESOURCE *res = GetHandle(hIcon, HANDLE_ICON);
    if (res == NULL)
    {
        return FALSE;
    }
    if (res->lpResourceName == NULL)
    {
        return FALSE;
    }
    JS_CALL("setIcon", hWnd, res->lpResourceName);
    return TRUE;
}
