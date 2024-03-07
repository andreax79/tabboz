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
                h->window.fEnd = FALSE;
                h->window.dialogResult = 0;
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
    LPWNDCLASS prev = (LPWNDCLASS)GetProperty(global_classes, lpWndClass->lpszClassName);
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

BOOL UnregisterClass(LPCSTR lpClassName, HANDLE hInstance)
{
    if (!global_classes)
    {
        return FALSE;
    }
    LPWNDCLASS wndClass = (LPWNDCLASS)DelProperty(global_classes, lpClassName);
    if (wndClass == NULL)
    {
        return FALSE;
    }
    free(wndClass);
    return TRUE;
}

//*******************************************************************
// Get information about a window class.
//*******************************************************************

BOOL GetClassInfo(HINSTANCE hInstance, LPCSTR lpClassName, LPWNDCLASS lpWndClass)
{
    LPWNDCLASS wndClass = (LPWNDCLASS)GetProperty(global_classes, lpClassName);
    if (wndClass == NULL)
    {
        return FALSE;
    }
    memcpy(lpWndClass, wndClass, sizeof(WNDCLASS));
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
        if ((child->refcount > 0) && (hWnd == child->hwndParent) && (child->lpfnWndProc != NULL))
        {
            child->lpfnWndProc(hWnd, uMsg, wParam, lParam);
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
