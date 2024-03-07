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
#include "handler.h"
#include "debug.h"

//*******************************************************************
// Load a string resource into a buffer
//*******************************************************************

int LoadString(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax)
{
    return JS_CALL_INT("loadString", uID, lpBuffer, cchBufferMax);
}

//*******************************************************************
// Display a modal dialog box
//*******************************************************************

INT_PTR messageBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_COMMAND)
    {
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
        case IDABORT:
        case IDRETRY:
        case IDIGNORE:
        case IDYES:
        case IDNO:
        case IDTRYAGAIN:
        case IDCONTINUE:
            EndDialog(hWnd, wParam);
            return TRUE;
        }
        return FALSE;
    }
    else if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
    {
        EndDialog(hWnd, IDCANCEL);
        return TRUE;
    }
    return FALSE;
}

int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    int           parentWindowId = -1;
    MSG           msg;
    HANDLE_ENTRY *handle = AllocateHandle(Window, hWnd);

    handle->lpDialogFunc = &messageBoxProc;
    // Get parent window number
    if (hWnd != NULL)
    {
        parentWindowId = (int)hWnd;
    }
    JS_ASYNC_CALL_INT("messageBox", handle, lpText, lpCaption, uType, parentWindowId);
    // Message loop
    while ((!handle->window.fEnd) && GetMessage(&msg, (HWND)handle, 0, 0) > 0)
    {
        printf("message: %d, wParam: %d\n", msg.message, msg.wParam);
        DispatchMessage(&msg);
    }
    // Destroy the window
    DestroyWindow((HWND)handle);
    return msg.wParam; // the value to be returned to the function that created the message box
}

int MessageBoxEx(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, WORD wLanguageId)
{
    return MessageBox(hWnd, lpText, lpCaption, uType);
}

//*******************************************************************
//
//*******************************************************************

HWND CreateWindowEx(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    int           parentWindowId = -1;
    WNDCLASS      wndClass;
    HANDLE_ENTRY *handle = AllocateHandle(Window, hWndParent);

    handle->lpDialogFunc = &messageBoxProc;
    // Get parent window number
    if (hWndParent != NULL)
    {
        parentWindowId = (int)hWndParent;
    }
    // Get class
    if (GetClassInfo(hInstance, lpClassName, &wndClass))
    {
        handle->lpDialogFunc = (DLGPROC)wndClass.lpfnWndProc;
        if (wndClass.lpszMenuName != NULL && hMenu == NULL)
        {
            hMenu = LoadMenu(hInstance, wndClass.lpszMenuName);
        }
        // TODO - wndClass.lpszMenuName
        // TODO - wndClass.hbrBackground
        // TODO - wndClass.hIcon
    }
    JS_CALL("createWindow", NULL, handle, X, Y, nWidth, nHeight, lpWindowName, dwStyle, dwExStyle, parentWindowId);
    // Main menu
    if (hMenu != NULL)
    {
        JS_ASYNC_CALL("addMenuToWindow", handle, hMenu);
    }
    // Show window
    JS_CALL("showWindow", handle, 1);
    return handle;
}

HWND CreateWindow(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    return CreateWindowEx(0, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

//*******************************************************************
// Create a modal dialog box from a dialog box template resource
//*******************************************************************

INT_PTR DialogBox(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc)
{
    int           controlId;
    int           dialog = (int)lpTemplateName;
    int           parentWindowId = -1;
    MSG           msg;
    HANDLE_ENTRY *handle = AllocateHandle(Window, hWndParent);

    if (lpDialogFunc == NULL)
    {
        return 0;
    }
    handle->lpDialogFunc = lpDialogFunc;
    // Get parent window number
    if (hWndParent != NULL)
    {
        parentWindowId = (int)hWndParent;
    }
    // Show dialog
    JS_ASYNC_CALL("dialogBox", handle, dialog, parentWindowId);
    // Dispatch Init
    PostMessage((HWND)handle, WM_INITDIALOG, 0, 0);
    // Dispatch Repaint
    PostMessage((HWND)handle, WM_PAINT, 0, 0);
    // Message loop
    while ((!handle->window.fEnd) && GetMessage(&msg, (HWND)handle, 0, 0) > 0)
    {
        printf("message: %d, wParam: %d\n", msg.message, msg.wParam);
        DispatchMessage(&msg);
    }
    // Destroy the window
    DestroyWindow((HWND)handle);
    return handle->window.dialogResult; // the value to be returned to the function that created the dialog box
}

//*******************************************************************
// Destroy a modal dialog box
//*******************************************************************

BOOL EndDialog(HWND hWnd, INT_PTR retval)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    // Set end flag and result
    handle->window.dialogResult = retval;
    handle->window.fEnd = TRUE;
    return TRUE;
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
// Set the title or text of a control in a dialog box
//*******************************************************************

BOOL SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCSTR lpString)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hDlg;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return JS_CALL_INT("setDlgItemText", handle, nIDDlgItem, lpString);
}

//*******************************************************************
// Retrieve the specified system metric
//*******************************************************************

int GetSystemMetrics(int nIndex)
{
    return JS_CALL_INT("getSystemMetrics", nIndex);
}

//*******************************************************************
// Retrieve the title or text associated with a control in a dialog box
//*******************************************************************

UINT GetDlgItemText(HWND hDlg, int nIDDlgItem, LPSTR lpString, int nMaxCount)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hDlg;
    if (handle == NULL)
    {
        // Invalid window handle
        return 0;
    }
    return JS_CALL_INT("getDlgItemText", handle, nIDDlgItem, lpString, nMaxCount);
}

//*******************************************************************
// Get an entry from the property list of the specified window
//*******************************************************************

HANDLE GetProp(HWND hWnd, LPCSTR lpString)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return GetProperty(handle->window.props, lpString);
}

//*******************************************************************
// Add/change an entry in the property list of the specified window
//*******************************************************************

BOOL SetProp(HWND hWnd, LPCSTR lpString, HANDLE hData)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    SetProperty(handle->window.props, lpString, hData);
    return TRUE;
}

//*******************************************************************
// Remove an entry from the property list of the specified window
//*******************************************************************

HANDLE RemoveProp(HWND hWnd, LPCSTR lpString)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return DelProperty(handle->window.props, lpString);
}

//*******************************************************************
// Return cursor id
//*******************************************************************

HCURSOR LoadCursor(HINSTANCE hInstance, LPCSTR lpCursorName)
{
    if (HIWORD((unsigned long)lpCursorName) != 0)
    {
        return NULL;
    }
    return (HCURSOR)lpCursorName;
}

//*******************************************************************
// Return icon id
//*******************************************************************

HICON LoadIcon(HINSTANCE hInstance, LPCSTR lpIconName)
{
    if (HIWORD((unsigned long)lpIconName) != 0)
    {
        return NULL;
    }
    return (HICON)lpIconName;
}

//*******************************************************************
// Return menu id
//*******************************************************************

HMENU LoadMenu(HINSTANCE hInstance, LPCSTR lpMenuName)
{
    if (HIWORD((unsigned long)lpMenuName) != 0)
    {
        return NULL;
    }
    return (HMENU)lpMenuName;
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL DestroyIcon(HICON hIcon)
{
    return TRUE;
}

//*******************************************************************
// Return bitmap id
//*******************************************************************

HBITMAP LoadBitmap(HINSTANCE hInstance, LPCSTR lpBitmapName)
{
    if (HIWORD((unsigned long)lpBitmapName) != 0)
    {
        return NULL;
    }
    return (HBITMAP)lpBitmapName;
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL DeleteObject(void *ho)
{
    return TRUE;
}

//*******************************************************************
// Return accellerator table id
//*******************************************************************

HACCEL LoadAccelerators(HINSTANCE hInstance, LPCSTR lpTableName)
{
    if (HIWORD((unsigned long)lpTableName) != 0)
    {
        return NULL;
    }
    return (HACCEL)lpTableName;
}

//*******************************************************************
// Creates a timer
//*******************************************************************

void TimerCb(TIMER *timer)
{
    DEBUG_PRINTF("Timer callback %d\n", timer->timeoutId);
    if (timer->lpTimerFunc != NULL)
    {
        // Call timer function
        timer->lpTimerFunc(timer->hwnd, WM_TIMER, timer->nIDEvent, 0);
    }
    else
    {
        // Dispatch WM_TIMER
        PostMessage(timer->hwnd, WM_TIMER, timer->nIDEvent, 0);
        // Stop waiting for an event
        JS_CALL("stopWaiting");
    }
}

UINT_PTR SetTimer(HWND hWnd, UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc)
{
    DEBUG_PRINTF("SetTimer %d\n", nIDEvent);
    KillTimer(hWnd, nIDEvent);
    char timerPropertyName[20];
    snprintf(timerPropertyName, sizeof(timerPropertyName), "timer%d", nIDEvent);
    TIMER *timer = malloc(sizeof(TIMER));
    timer->hwnd = hWnd;
    timer->nIDEvent = nIDEvent;
    timer->lpTimerFunc = lpTimerFunc;
    timer->timeoutId = emscripten_set_timeout((void (*)(void *))TimerCb, uElapse, timer);
    return TRUE;
}

//*******************************************************************
// Destroy the specified timer
//*******************************************************************

BOOL KillTimer(HWND hWnd, UINT_PTR uIDEvent)
{
    char timerPropertyName[20];
    snprintf(timerPropertyName, sizeof(timerPropertyName), "timer%d", uIDEvent);
    TIMER *timer = (TIMER *)GetProp(hWnd, timerPropertyName);
    if (timer != NULL)
    {
        DEBUG_PRINTF("KillTimer %d\n", uIDEvent);
        emscripten_clear_timeout(timer->timeoutId);
        free(timer);
        RemoveProp(hWnd, timerPropertyName);
    }
    return TRUE;
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

BOOL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
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
// Call the default window procedure
//*******************************************************************

LRESULT DefWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;
    }
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
        HANDLE_ENTRY *handle = AllocateHandle(Window, NULL);
        WinMain((HANDLE)handle, NULL, "", SW_SHOWNORMAL);
        ReleaseHandle(handle);
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
