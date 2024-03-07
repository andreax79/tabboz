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
#include "debug.h"

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
    handle->lpfnWndProc = (WNDPROC)lpDialogFunc;
    // Get parent window number
    if (hWndParent != NULL)
    {
        parentWindowId = (int)hWndParent;
    }
    // Show dialog
    JS_ASYNC_CALL("dialogBox", handle, dialog, parentWindowId, hInstance);
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
// Dialog Box control proc
//*******************************************************************

LRESULT dlgItemProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HANDLE_ENTRY *h = (HANDLE_ENTRY *)hWnd;
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
        printf("Left button down\n");
        // Left button down
        return SendMessage(h->hwndParent, WM_COMMAND, h->dlgItem.nIDDlgItem, lParam);
    case BM_GETCHECK:
        // Get the check state of a radio button or check box
        return JS_CALL_INT("getCheck", h->hwndParent, h->dlgItem.nIDDlgItem);
    case BM_SETCHECK:
        // Set the check state of a radio button or check box
        return JS_CALL_INT("setCheck", h->hwndParent, h->dlgItem.nIDDlgItem, wParam);
    case CB_ADDSTRING:
        // Add a string to the list box of a combo box
        return JS_CALL_INT("comboBoxAddString", h->hwndParent, h->dlgItem.nIDDlgItem, lParam);
    case CB_SETCURSEL:
        // Select a string in the list of a combo box
        return JS_CALL_INT("comboBoxSelect", h->hwndParent, h->dlgItem.nIDDlgItem, wParam);
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

HANDLE_ENTRY *AllocateDlgItem(HINSTANCE hInstance, LPCTSTR lpClassName, HWND hwndParent, HMENU hMenu)
{
    HANDLE_ENTRY *h = AllocateHandle(DlgItem, hwndParent);
    if (h == NULL)
    {
        return NULL;
    }
    h->dlgItem.nIDDlgItem = (int)hMenu;
    h->lpClassName = strdup(lpClassName);
    // Get the dialog control class
    WNDCLASS wndClass;
    if (GetClassInfo(hInstance, lpClassName, &wndClass))
    {
        h->lpfnWndProc = wndClass.lpfnWndProc;
    }
    else
    {
        // Class not found - use the default dialog control class
        h->lpfnWndProc = &dlgItemProc;
    }
    return h;
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
