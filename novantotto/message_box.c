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
// Message Box proc
//*******************************************************************

LRESULT messageBoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

//*******************************************************************
// Display a modal dialog box
//*******************************************************************

int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    int           parentWindowId = -1;
    MSG           msg;
    HANDLE_ENTRY *handle = AllocateHandle(Window, hWnd);

    handle->lpfnWndProc = &messageBoxProc;
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
