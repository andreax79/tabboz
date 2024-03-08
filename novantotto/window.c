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
// Create a window
//*******************************************************************

HWND CreateWindowEx(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    int       parentWindowId = -1;
    WNDCLASS  wndClass;
    RESOURCE *res = AllocateHandle(HANDLE_WINDOW, hWndParent);

    res->lpfnWndProc = &DefWindowProc;
    // Get parent window number
    if (hWndParent != NULL)
    {
        parentWindowId = (int)hWndParent;
    }
    // Get class
    if (GetClassInfo(hInstance, lpClassName, &wndClass))
    {
        res->lpfnWndProc = wndClass.lpfnWndProc;
        if (wndClass.lpszMenuName != NULL && hMenu == NULL)
        {
            hMenu = LoadMenu(hInstance, wndClass.lpszMenuName);
        }
        // TODO - wndClass.lpszMenuName
        // TODO - wndClass.hbrBackground
        // TODO - wndClass.hIcon
    }
    JS_CALL("createWindow", NULL, res->handle, X, Y, nWidth, nHeight, lpWindowName, dwStyle, dwExStyle, parentWindowId);
    // Main menu
    if (hMenu != NULL)
    {
        RESOURCE *menuHandle = GetHandle(hMenu, HANDLE_MENU);
        if (menuHandle != NULL)
        {
            JS_ASYNC_CALL("addMenuToWindow", res->handle, menuHandle->lpResourceName);
        }
    }
    // Show window
    JS_CALL("showWindow", res->handle, 1);
    return res->handle;
}

HWND CreateWindow(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    return CreateWindowEx(0, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

//*******************************************************************
// Show/hide a window
//*******************************************************************

BOOL ShowWindow(HWND hWnd, int nCmdShow)
{
    return JS_CALL_INT("showWindow", hWnd, nCmdShow && SW_SHOWNORMAL);
}

//*******************************************************************
// Change the position and dimensions of the specified window
//*******************************************************************

BOOL MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
{
    return JS_CALL_INT("moveWindow", hWnd, X, Y, nWidth, nHeight);
}

//*******************************************************************
// Retrieve the dimensions of a specified window
//*******************************************************************

BOOL GetWindowRect(HWND hWnd, LPRECT lpRect)
{
    RESOURCE *res = GetHandle(hWnd, HANDLE_WINDOW);
    if (res == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    else
    {
        lpRect->left = JS_CALL_INT("getWindowRectDimension", res->handle, 0);
        lpRect->top = JS_CALL_INT("getWindowRectDimension", res->handle, 1);
        lpRect->right = JS_CALL_INT("getWindowRectDimension", res->handle, 2);
        lpRect->bottom = JS_CALL_INT("getWindowRectDimension", res->handle, 3);
        return TRUE;
    }
}

//*******************************************************************
// Calculates the required size of the window rectangle
//*******************************************************************

void AdjustWindowRect(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
{
    AdjustWindowRectEx(lpRect, dwStyle, bMenu, 0);
}

BOOL AdjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle)
{

    if (lpRect == NULL)
    {
        return FALSE;
    }
    if (bMenu)
    {
        lpRect->bottom += 18;
    }
    return TRUE;
}

//*******************************************************************
// Destroy window
//*******************************************************************

BOOL DestroyWindow(HWND hWnd)
{
    RESOURCE *res = GetHandle(hWnd, HANDLE_WINDOW);
    if (res == NULL)
    {
        // Invalid window handle
        return FALSE;
    }

    // Send destroy message
    SendMessage(res->handle, WM_DESTROY, 0, 0);
    SendMessage(res->handle, WM_NCDESTROY, 0, 0);

    // Dispatch destroy to children
    DispatchToChildren(res->handle, WM_DESTROY, 0, 0);

    // Destroy the window
    JS_CALL("destroyWindow", res->handle);
    ReleaseHandle(res);

    return TRUE;
}

//*******************************************************************

BOOL RedrawWindow(HWND hWnd, const RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags)
{
    RESOURCE *res = GetHandle(hWnd, HANDLE_WINDOW);
    if (res == NULL)
    {
        return FALSE;
    }
    // Redraw children
    DispatchToChildren(res->handle, WM_PAINT, 0, 0);
    // Redraw Window
    if (res->lpfnWndProc == NULL)
    {
        return FALSE;
    }
    return res->lpfnWndProc(res->handle, WM_PAINT, 0, 0);
}

//*******************************************************************

BOOL UpdateWindow(HWND hwnd)
{
    return RedrawWindow(hwnd, NULL, 0, RDW_UPDATENOW | RDW_ALLCHILDREN);
}

//*******************************************************************

BOOL InvalidateRgn(HWND hwnd, HRGN hrgn, BOOL erase)
{
    return RedrawWindow(hwnd, NULL, hrgn, RDW_INVALIDATE | (erase ? RDW_ERASE : 0));
}

//*******************************************************************

BOOL InvalidateRect(HWND hwnd, const RECT *rect, BOOL erase)
{
    return RedrawWindow(hwnd, rect, 0, RDW_INVALIDATE | (erase ? RDW_ERASE : 0));
}

//*******************************************************************

BOOL ValidateRgn(HWND hwnd, HRGN hrgn)
{
    return RedrawWindow(hwnd, NULL, hrgn, RDW_VALIDATE);
}

//*******************************************************************

BOOL ValidateRect(HWND hwnd, const RECT *rect)
{
    return RedrawWindow(hwnd, rect, 0, RDW_VALIDATE);
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
    return TRUE; // TODO
}
