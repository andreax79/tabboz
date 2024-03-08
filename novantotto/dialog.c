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
#include "handle.h"
#include "debug.h"

//*******************************************************************
// Create a modal dialog box from a dialog box template resource
//*******************************************************************

INT_PTR DialogBox(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc)
{
    int       controlId;
    int       dialog = (int)lpTemplateName;
    int       parentWindowId = -1;
    MSG       msg;
    RESOURCE *res = AllocateHandle(HANDLE_WINDOW, hWndParent);

    if (lpDialogFunc == NULL)
    {
        return 0;
    }
    res->lpfnWndProc = (WNDPROC)lpDialogFunc;
    // Get parent window number
    if (hWndParent != NULL)
    {
        parentWindowId = (int)hWndParent;
    }
    // Show dialog
    JS_ASYNC_CALL("dialogBox", res->handle, dialog, parentWindowId, hInstance);
    // Dispatch Init
    PostMessage(res->handle, WM_INITDIALOG, 0, 0);
    // Dispatch Repaint
    PostMessage(res->handle, WM_PAINT, 0, 0);
    // Message loop
    while ((!res->window.fEnd) && GetMessage(&msg, res->handle, 0, 0) > 0)
    {
        printf("message: %d, wParam: %d\n", msg.message, msg.wParam);
        DispatchMessage(&msg);
    }
    // Destroy the window
    DestroyWindow(res->handle);
    return res->window.dialogResult; // the value to be returned to the function that created the dialog box
}

//*******************************************************************
// Destroy a modal dialog box
//*******************************************************************

BOOL EndDialog(HWND hWnd, INT_PTR retval)
{
    RESOURCE *res = GetHandle(hWnd, HANDLE_WINDOW);
    if (res == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    // Set end flag and result
    res->window.dialogResult = retval;
    res->window.fEnd = TRUE;
    return TRUE;
}

//*******************************************************************
// Dialog Box control proc
//*******************************************************************

LRESULT dlgItemProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    RESOURCE *res = GetHandle(hWnd, HANDLE_ANY);
    if (res == NULL)
    {
        return FALSE;
    }
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
        printf("Left button down\n");
        // Left button down
        return SendMessage(res->hwndParent, WM_COMMAND, res->control.nIDDlgItem, lParam);
    case BM_GETCHECK:
        // Get the check state of a radio button or check box
        return JS_CALL_INT("getCheck", res->hwndParent, res->control.nIDDlgItem);
    case BM_SETCHECK:
        // Set the check state of a radio button or check box
        return JS_CALL_INT("setCheck", res->hwndParent, res->control.nIDDlgItem, wParam);
    case CB_ADDSTRING:
        // Add a string to the list box of a combo box
        return JS_CALL_INT("comboBoxAddString", res->hwndParent, res->control.nIDDlgItem, lParam);
    case CB_SETCURSEL:
        // Select a string in the list of a combo box
        return JS_CALL_INT("comboBoxSelect", res->hwndParent, res->control.nIDDlgItem, wParam);
    case WM_PAINT:
        return TRUE;
    default:
        DEBUG_PRINTF("dlgItemProc uMsg=%d wParam=%d lParam=%ld\n", uMsg, wParam, lParam);
    }
    return FALSE;
}

//*******************************************************************
// Allocate a dialog control
//*******************************************************************

RESOURCE *AllocateControl(HINSTANCE hInstance, LPCTSTR lpClassName, HWND hwndParent, HMENU hMenu)
{
    RESOURCE *res = AllocateHandle(HANDLE_CONTROL, hwndParent);
    if (res == NULL)
    {
        return NULL;
    }
    res->control.nIDDlgItem = (int)hMenu;
    res->lpClassName = strdup(lpClassName);
    // Get the dialog control class
    WNDCLASS wndClass;
    if (GetClassInfo(hInstance, lpClassName, &wndClass))
    {
        res->lpfnWndProc = wndClass.lpfnWndProc;
    }
    else
    {
        // Class not found - use the default dialog control class
        res->lpfnWndProc = &dlgItemProc;
    }
    return res;
}

//*******************************************************************
// Retrieve the title or text associated with a control in a dialog box
//*******************************************************************

UINT GetDlgItemText(HWND hDlg, int nIDDlgItem, LPSTR lpString, int nMaxCount)
{
    RESOURCE *res = GetHandle(hDlg, HANDLE_ANY);
    if (res == NULL)
    {
        // Invalid window handle
        return 0;
    }
    return JS_CALL_INT("getDlgItemText", res->handle, nIDDlgItem, lpString, nMaxCount);
}

//*******************************************************************
// Set the title or text of a control in a dialog box
//*******************************************************************

BOOL SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCSTR lpString)
{
    RESOURCE *res = GetHandle(hDlg, HANDLE_ANY);
    if (res == NULL)
    {
        // Invalid window handle
        return 0;
    }
    return JS_CALL_INT("setDlgItemText", res->handle, nIDDlgItem, lpString);
}
