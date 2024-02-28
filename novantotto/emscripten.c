// Tabboz Simulator
// Copyright (c) 1997-2000 Andrea Bonomi

/*
       This file is part of Tabboz Simulator.

       Tabboz Simulator is free software: you can redistribute it and/or modify
       it under the terms of the GNU General Public License as published by
       the Free Software Foundation, either version 3 of the License, or
       (at your option) any later version.

       Tabboz Simulator is distributed in the hope that it will be useful,
       but WITHOUT ANY WARRANTY; without even the implied warranty of
       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
       GNU General Public License for more details.

       You should have received a copy of the GNU General Public License
       along with Tabboz Simulator.  If not, see <http://www.gnu.org/licenses/>.
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
    HANDLE_ENTRY *handle = AllocateHandle(Window, hWnd);
    MSG           msg;

    handle->lpDialogFunc = &messageBoxProc;
    // Get parent window number
    if (hWnd != NULL)
    {
        parentWindowId = (int)hWnd;
    }
    JS_ASYNC_CALL_INT("messageBox", handle, lpText, lpCaption, uType, parentWindowId);
    // Message loop
    while (GetMessage(&msg, (HWND)handle, 0, 0) > 0)
    {
        printf("message: %d, wParam: %d\n", msg.message, msg.wParam);
        DispatchMessage(&msg);
    }
    // Destroy the dialog box
    JS_CALL("destroyDialogBox", handle);
    ReleaseHandle(handle);
    return msg.wParam; // the value to be returned to the function that created the message box
}

//*******************************************************************
// Create a modal dialog box from a dialog box template resource
//*******************************************************************

INT_PTR DialogBox(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc)
{
    int           controlId;
    int           dialog = (int)lpTemplateName;
    int           parentWindowId = -1;
    HANDLE_ENTRY *handle = AllocateHandle(Window, hWndParent);
    MSG           msg;

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
    while (GetMessage(&msg, (HWND)handle, 0, 0) > 0)
    {
        printf("message: %d, wParam: %d\n", msg.message, msg.wParam);
        DispatchMessage(&msg);
    }
    // Destroy the dialog box
    JS_CALL("destroyDialogBox", handle);
    ReleaseHandle(handle);
    return msg.wParam; // the value to be returned to the function that created the dialog box
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
    // Dispatch destroy to children
    DispatchToChildren(hWnd, WM_DESTROY, retval, 0);
    // Send WM_QUIT message
    PostMessage(hWnd, WM_QUIT, retval, 0);
    return TRUE;
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
// No nothing
//*******************************************************************

LRESULT DefWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
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

int WinMainStartup()
{
    HANDLE_ENTRY *handle = AllocateHandle(Window, NULL);
    WinMain((HANDLE)handle, NULL, "", 0);
    ReleaseHandle(handle);
    return 0;
}

//*******************************************************************
// Icon click callback
//*******************************************************************

static BOOL bIsOpen = FALSE;

int IconClickCb(int eventType, const struct EmscriptenMouseEvent *someEvent, void *userData)
{
    int ret = 0;
    if (!bIsOpen)
    {
        bIsOpen = TRUE;
        ret = WinMainStartup();
        bIsOpen = FALSE;
    }
    else
    {
        JS_CALL("showApp", 1);
    }
    return ret;
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
    // Register icon handler
    emscripten_set_click_callback("#zarrosim_icon", NULL, TRUE, &IconClickCb);
    return 0;
}
