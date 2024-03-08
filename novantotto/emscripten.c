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
#include <emscripten/html5.h>
#include "novantotto.h"
#include "property.h"
#include "handle.h"
#include "debug.h"

//*******************************************************************
// Load a string resource into a buffer
//*******************************************************************

int LoadString(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax)
{
    return JS_CALL_INT("loadString", uID, lpBuffer, cchBufferMax);
}

//*******************************************************************
//
//*******************************************************************

HMENU GetSystemMenu(HWND hWnd, BOOL bRevert)
{
    return NULL; // TODO
}

//*******************************************************************
//
//*******************************************************************

BOOL InsertMenu(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, LPCSTR lpNewItem)
{
    return TRUE; // TODO
}

//*******************************************************************
// Retrieve the specified system metric
//*******************************************************************

int GetSystemMetrics(int nIndex)
{
    return JS_CALL_INT("getSystemMetrics", nIndex);
}

//*******************************************************************
// Return window handle
//*******************************************************************

HDC BeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
{
    return (HDC)hWnd;
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL EndPaint(HWND hWnd, const PAINTSTRUCT *lpPaint)
{
    return TRUE;
}

//*******************************************************************
// No nothing
//*******************************************************************

int TranslateAccelerator(HWND hWnd, HACCEL hAccTable, LPMSG lpMsg)
{
    return 0;
}

//*******************************************************************
// Show shutdown screen
//*******************************************************************

void ExitWindows(int dwReserved, int code)
{
    JS_CALL("shutdown");
}

//*******************************************************************
// Get the number of milliseconds that have elapsed since the app was started
//*******************************************************************

DWORD GetTickCount()
{
    return (int)emscripten_get_now();
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL MessageBeep(UINT uType)
{
    return TRUE;
}

//*******************************************************************
// Play a sound specified by the given file name
//*******************************************************************

BOOL PlaySound(LPCTSTR lpszSound, HMODULE hmod, DWORD fuSound)
{
    if (lpszSound != NULL)
    {
        EM_ASM(new Audio('resources/wavs/' + UTF8ToString($0)).play(), lpszSound);
    }
    return TRUE;
}

//*******************************************************************
// Play a sound specified by the given file name
//*******************************************************************

BOOL sndPlaySound(LPCTSTR lpszSound, UINT fuSound)
{
    return PlaySound(lpszSound, NULL, fuSound);
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL LockWorkStation()
{
    return FALSE;
}

//*******************************************************************
// No nothing
//*******************************************************************

HWND SetFocus(HWND hWnd)
{
    return NULL; // TODO
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL GetOpenFileName(LPOPENFILENAME unnamedParam1)
{
    return FALSE; // TODO
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL GetSaveFileName(LPOPENFILENAME unnamedParam1)
{
    return FALSE; // TODO
}

//*******************************************************************
// No nothing
//*******************************************************************

void randomize()
{
}

//*******************************************************************
// Start the application
//*******************************************************************

static BOOL bIsOpen = FALSE;

int WinMainStartup()
{
    if (!bIsOpen)
    {
        bIsOpen = TRUE;
        RESOURCE *res = AllocateHandle(HANDLE_WINDOW, NULL);
        WinMain(res->handle, NULL, "", SW_SHOWNORMAL);
        ReleaseHandle(res);
        bIsOpen = FALSE;
    }
    else
    {
        JS_CALL("showApp", 1);
    }
    return 0;
}

//*******************************************************************
// Main
//*******************************************************************

int main()
{
    // Message queue setup
    SetMessageQueue(MESSAGE_LIMIT);
    // Load string resources
    JS_ASYNC_CALL("loadStringResources");
    // Preload images
    JS_ASYNC_CALL("preload");
    // Add event listener
    JS_CALL("eventListenerSetup");
    /* printf("-->%d<--\n", MessageBox(0, "Test", "Test", MB_YESNO | MB_ICONINFORMATION)); */
    return 0;
}
