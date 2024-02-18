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

#include "os.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "zarrosim.h"
#ifdef TABBOZ_EM

#include <emscripten/html5.h>

//*******************************************************************
// Load a string resource into a buffer
//*******************************************************************

int LoadString(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax)
{
    return JS_CALL_INT("loadString", uID, lpBuffer, cchBufferMax);
}

//*******************************************************************
// Show/hide a window
//*******************************************************************

BOOL ShowWindow(HWND hWnd, int nCmdShow)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return JS_CALL_INT("showWindow", handle->id, nCmdShow && SW_SHOWNORMAL);
}

//*******************************************************************
// Display a modal dialog box
//*******************************************************************

int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    int           result;
    int           parentWindowId = -1;
    HANDLE_ENTRY *handle = AllocateHandle();

    // Get parent window number
    if (hWnd != NULL)
    {
        parentWindowId = ((HANDLE_ENTRY *)hWnd)->id;
    }

    result = JS_ASYNC_CALL_INT("messageBox", handle->id, lpText, lpCaption, uType, parentWindowId);
    ReleaseHandle(handle);
    return result;
}

//*******************************************************************
// Create a modal dialog box from a dialog box template resource
//*******************************************************************

INT_PTR DialogBox(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc)
{
    INT_PTR       retval;
    int           controlId;
    int           dialog = (int)lpTemplateName;
    int           parentWindowId = -1;
    HANDLE_ENTRY *handle = AllocateHandle();
    MSG           msg;

    if (lpDialogFunc == NULL)
    {
        return 0;
    }
    handle->lpDialogFunc = lpDialogFunc;
    // Get parent window number
    if (hWndParent != NULL)
    {
        parentWindowId = ((HANDLE_ENTRY *)hWndParent)->id;
    }
    // Show dialog
    JS_ASYNC_CALL("dialogBox", handle->id, dialog, parentWindowId);
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
    retval = msg.wParam; // the value to be returned from the function that created the dialog box
    // Destroy the dialog box
    JS_CALL("destroyDialogBox", handle->id);
    ReleaseHandle(handle);
    return retval;
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
    return JS_CALL_INT("setDlgItemText", handle->id, nIDDlgItem, lpString);
}

//*******************************************************************
// Set the check state of a radio button or check box
//*******************************************************************

LRESULT SetCheck(HWND hDlg, int nIDDlgItem, WPARAM wParam)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hDlg;
    if (handle == NULL)
    {
        // Invalid window handle
        return 0;
    }
    return JS_CALL_INT("setCheck", handle->id, nIDDlgItem, wParam);
}

//*******************************************************************
// Retrieve the specified system metric
//*******************************************************************

int GetSystemMetrics(int nIndex)
{
    return JS_CALL_INT("getSystemMetrics", nIndex);
}

//*******************************************************************
// Retrieve the dimensions of a specified window
//*******************************************************************

BOOL GetWindowRect(HWND hWnd, LPRECT lpRect)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    else
    {
        lpRect->left = JS_CALL_INT("getWindowRectDimension", handle->id, 0);
        lpRect->top = JS_CALL_INT("getWindowRectDimension", handle->id, 1);
        lpRect->right = JS_CALL_INT("getWindowRectDimension", handle->id, 2);
        lpRect->bottom = JS_CALL_INT("getWindowRectDimension", handle->id, 3);
        return TRUE;
    }
}

//*******************************************************************
// Change the position and dimensions of the specified window
//*******************************************************************

BOOL MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return JS_CALL_INT("moveWindow", handle->id, X, Y, nWidth, nHeight);
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
    return JS_CALL_INT("getDlgItemText", handle->id, nIDDlgItem, lpString, nMaxCount);
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
    return GetProperty(handle->props, lpString);
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
    SetProperty(handle->props, lpString, hData);
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
    return DelProperty(handle->props, lpString);
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

HWND GetDlgItem(HWND DhDlg, int nIDDlgItem)
{
    return NULL; // TODO
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
    HANDLE_ENTRY *handle = AllocateHandle();
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
    // Register icon handler
    emscripten_set_click_callback("#zarrosim_icon", NULL, TRUE, &IconClickCb);
    return 0;
}

#endif
