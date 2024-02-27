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

#include <string.h>
#include <stdio.h>
#include "novantotto.h"
#include "handler.h"
#include "debug.h"

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
    return JS_CALL_INT("showWindow", handle, nCmdShow && SW_SHOWNORMAL);
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
    return JS_CALL_INT("moveWindow", handle, X, Y, nWidth, nHeight);
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
        lpRect->left = JS_CALL_INT("getWindowRectDimension", handle, 0);
        lpRect->top = JS_CALL_INT("getWindowRectDimension", handle, 1);
        lpRect->right = JS_CALL_INT("getWindowRectDimension", handle, 2);
        lpRect->bottom = JS_CALL_INT("getWindowRectDimension", handle, 3);
        return TRUE;
    }
}

//*******************************************************************

BOOL RedrawWindow(HWND hWnd, const RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags)
{
    // Redraw children
    DispatchToChildren(hWnd, WM_PAINT, 0, 0);
    // Redraw Window
    DLGPROC lpDialogFunc = ((HANDLE_ENTRY *)hWnd)->lpDialogFunc;
    if (lpDialogFunc == NULL)
    {
        return FALSE;
    }
    return lpDialogFunc(hWnd, WM_PAINT, 0, 0);
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
