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
#include "novantotto.h"
#include "property.h"
#include "handler.h"
#include "debug.h"

#define MIN_HANDLE_ENTRIES 100

HANDLE_TABLE      *global_table;
static PROPERTIES *global_classes;

//*******************************************************************
// Allocate a new handle table
//*******************************************************************

HANDLE_TABLE *AllocateHandleTable(int count)
{
    HANDLE_TABLE *table;

    if (count < MIN_HANDLE_ENTRIES)
    {
        count = MIN_HANDLE_ENTRIES;
    }
    if (!(table = calloc(1, sizeof(HANDLE_TABLE))))
    {
        DEBUG_PRINTF("AllocateHandleTable calloc failed\n");
        return NULL;
    }
    table->count = count;
    if ((table->entries = calloc(count, sizeof(*table->entries))))
    {
        return table;
    }
    else
    {
        DEBUG_PRINTF("AllocateHandleTable calloc failed\n");
        free(table);
        return NULL;
    }
}

//*******************************************************************
// Allocate an handle
//*******************************************************************

HANDLE_ENTRY *AllocateHandle(HandleType type, HWND hwndParent)
{
    if (!global_table)
    {
        DEBUG_PRINTF("AllocateHandle global_table is null\n");
        if (!(global_table = AllocateHandleTable(0)))
        {
            return NULL;
        }
    }
    for (int i = 0; i < global_table->count; i++)
    {
        HANDLE_ENTRY *h = &global_table->entries[i];
        if (h->refcount == 0)
        {
            h->type = type;
            h->hwndParent = hwndParent;
            h->window.props = NULL;
            if (type == Window)
            {
                h->window.props = AllocateProperties();
                if (h->window.props == NULL)
                {
                    DEBUG_PRINTF("AllocateHandle failed\n");
                    free(h);
                    return NULL;
                }
            }
            h->refcount++;
            h->id = i + 1;
            DEBUG_PRINTF("AllocateHandle id=%d\n", h->id);
            return (HANDLE)h;
        }
    }
    DEBUG_PRINTF("AllocateHandle table full\n");
    return NULL;
}

//*******************************************************************
// Allocate a dialog control
//*******************************************************************

INT_PTR dlgItemProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

HANDLE_ENTRY *AllocateDlgItem(LPCTSTR lpClassName, HWND hwndParent, HMENU hMenu)
{
    HANDLE_ENTRY *h = AllocateHandle(DlgItem, hwndParent);
    if (h == NULL)
    {
        return NULL;
    }
    h->dlgItem.nIDDlgItem = (int)hMenu;
    h->lpClassName = strdup(lpClassName);
    WNDCLASS *wndClass = (WNDCLASS *)GetProperty(global_classes, lpClassName);
    if (wndClass != NULL)
    {
        h->lpDialogFunc = wndClass->lpfnWndProc;
    }
    else
    {
        h->lpDialogFunc = &dlgItemProc;
    }
    return h;
}

//*******************************************************************
// Release an handle
//*******************************************************************

void ReleaseHandle(HANDLE p)
{
    HANDLE_ENTRY *h = (HANDLE_ENTRY *)p;
    if (h != NULL)
    {
        DEBUG_PRINTF("ReleaseHandle refcount=%d\n", h->refcount);
        if (h->refcount > 0)
        {
            h->refcount--;
            if (h->refcount == 0)
            {
                h->id = 0;
                free(h->lpClassName);
                if (h->type == Window)
                {
                    FreeProperties(h->window.props);
                    // Remove children
                    for (int i = 0; i < global_table->count; i++)
                    {
                        HANDLE_ENTRY *child = &global_table->entries[i];
                        if ((child->refcount > 0) && (h == child->hwndParent))
                        {
                            child->refcount = 0;
                            child->hwndParent = NULL;
                        }
                    }
                }
            }
        }
    }
}

//*******************************************************************
// Retrieve a control handle
//*******************************************************************

HWND GetDlgItem(HWND hDlg, int nIDDlgItem)
{
    for (int i = 0; i < global_table->count; i++)
    {
        HANDLE_ENTRY *h = &global_table->entries[i];
        if ((h->refcount > 0) && (hDlg == h->hwndParent) && (nIDDlgItem == h->dlgItem.nIDDlgItem))
        {
            return (HWND)h;
        }
    }
    DEBUG_PRINTF("GetDlgItem %d - not found\n", nIDDlgItem);
    return NULL;
}

//*******************************************************************
// Register a window class
//*******************************************************************

ATOM RegisterClass(const WNDCLASS *lpWndClass)
{
    if (!global_classes)
    {
        DEBUG_PRINTF("RegisterClass global_classes is null\n");
        if (!(global_classes = AllocateProperties()))
        {
            return 0;
        }
    }
    WNDCLASS *h = malloc(sizeof(WNDCLASS));
    if (h == NULL)
    {
        return 0;
    }
    memcpy(h, lpWndClass, sizeof(WNDCLASS));
    // Check if already exists
    WNDCLASS *prev = (WNDCLASS *)GetProperty(global_classes, lpWndClass->lpszClassName);
    if (prev != NULL)
    {
        free(prev);
        DelProperty(global_classes, lpWndClass->lpszClassName);
    }
    // Register the class
    SetProperty(global_classes, lpWndClass->lpszClassName, (HANDLE)h);
    return (ATOM)h;
}

//*******************************************************************
// Unregister a window class
//*******************************************************************

BOOL UnregisterClass(LPSTR lpClassName, HANDLE hInstance)
{
    if (!global_classes)
    {
        return FALSE;
    }
    WNDCLASS *prev = (WNDCLASS *)DelProperty(global_classes, lpClassName);
    if (prev == NULL)
    {
        return FALSE;
    }
    free(prev);
    return TRUE;
}

//*******************************************************************
// Send a message to the children of hWnd
//*******************************************************************

void DispatchToChildren(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Redraw children
    for (int i = 0; i < global_table->count; i++)
    {
        HANDLE_ENTRY *child = &global_table->entries[i];
        if ((child->refcount > 0) && (hWnd == child->hwndParent) && (child->lpDialogFunc != NULL))
        {
            child->lpDialogFunc(hWnd, uMsg, wParam, lParam);
        }
    }
}

//*******************************************************************
// Get an handle by index
//*******************************************************************

HANDLE_ENTRY *GetHandle(int index)
{
    HANDLE_ENTRY *h;
    if (!global_table || index < 1 || index > global_table->count)
    {
        return NULL;
    }
    h = &global_table->entries[index - 1];
    if (h->refcount > 0)
    {
        return h;
    }
    else
    {
        return NULL;
    }
}
