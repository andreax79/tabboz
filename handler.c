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

#define MIN_HANDLE_ENTRIES 100
#define PROPERTIES_DEFAULT_ENTRIES 8

// clang-format off
#ifdef DEBUG
#define DEBUG_PRINTF(...) printf("DEBUG: " __VA_ARGS__)
#else
#define DEBUG_PRINTF(...) do {} while (0)
#endif
// clang-format on

/* static HANDLE_TABLE *global_table; */
HANDLE_TABLE *global_table;

//*******************************************************************
// Find property index
//*******************************************************************

int FindPropertyIndex(PROPERTIES *props, LPCSTR key)
{
    for (int i = 0; i < props->capacity; i++)
    {
        if ((props->entries[i].key != NULL) && (!strcmp(props->entries[i].key, key)))
        {
            DEBUG_PRINTF("FindPropertyIndex %s idx=%d\n", key, i);
            return i;
        }
    }
    DEBUG_PRINTF("FindPropertyIndex %s not found\n", key);
    return -1;
}

//*******************************************************************
// Find empty property index
//*******************************************************************

int FindEmptyPropertyIndex(PROPERTIES *props)
{
    for (int i = 0; i < props->capacity; i++)
    {
        if (props->entries[i].key == NULL)
        {
            DEBUG_PRINTF("FindEmptyPropertyIndex idx=%d\n", i);
            return i;
        }
    }
    DEBUG_PRINTF("FindEmptyPropertyIndex not found\n");
    return -1;
}

//*******************************************************************
// Get an entry from the property list
//*******************************************************************

HANDLE GetProperty(PROPERTIES *props, LPCSTR key)
{
    DEBUG_PRINTF("GetProperty %s\n", key);
    int idx = FindPropertyIndex(props, key);
    if (idx == -1)
    {
        return NULL;
    }
    else
    {
        return props->entries[idx].hData;
    }
}

//*******************************************************************
// Remove an entry from the property list
//*******************************************************************

HANDLE DelProperty(PROPERTIES *props, LPCSTR key)
{
    DEBUG_PRINTF("DelProperty %s\n", key);
    int idx = FindPropertyIndex(props, key);
    if (idx == -1)
    {
        return NULL;
    }
    else
    {
        HANDLE prev = props->entries[idx].hData;
        props->entries[idx].hData = NULL;
        free((void *)props->entries[idx].key);
        props->entries[idx].key = NULL;
        props->len--;
        return prev;
    }
}

//*******************************************************************
// Set an entry in the property list
//*******************************************************************

HANDLE SetProperty(PROPERTIES *props, LPCSTR key, HANDLE hData)
{
    DEBUG_PRINTF("SetProperty %s\n", key);
    int idx = FindPropertyIndex(props, key);
    if (idx != -1) // Already exist
    {
        DEBUG_PRINTF("SetProperty %s (set) idx=%d\n", key, idx);
        HANDLE prev = props->entries[idx].hData;
        props->entries[idx].hData = hData;
        return prev;
    }
    if (props->len == props->capacity) // Full
    {
        DEBUG_PRINTF("SetProperty %s realloc capacity=%d\n", key, props->capacity * 2);
        PROPERTY *prev = props->entries;
        props->entries = calloc(props->capacity * 2, sizeof(PROPERTY));
        if (props->entries == NULL)
        {
            DEBUG_PRINTF("SetProperty %s calloc failed\n", key);
            props->entries = prev;
            return NULL;
        }
        memcpy(props->entries, prev, props->capacity * sizeof(PROPERTY));
        props->capacity *= 2;
        free(prev);
    }
    // Search for an unused entry
    idx = FindEmptyPropertyIndex(props);
    DEBUG_PRINTF("SetProperty %s (add) idx=%d\n", key, idx);
    props->entries[idx].key = strdup(key);
    props->entries[idx].hData = hData;
    props->len++;
    return NULL;
}

//*******************************************************************
// Allocate a new properties table
//*******************************************************************

PROPERTIES *AllocateProperties(void)
{
    DEBUG_PRINTF("AllocateProperties\n");
    PROPERTIES *props = calloc(1, sizeof(PROPERTIES));
    if (props == NULL)
    {
        DEBUG_PRINTF("AllocateProperties calloc failed\n");
        return NULL;
    }
    props->capacity = PROPERTIES_DEFAULT_ENTRIES;
    props->len = 0;
    props->entries = calloc(props->capacity, sizeof(PROPERTY));
    if (props->entries == NULL)
    {
        DEBUG_PRINTF("AllocateProperties entries calloc failed\n");
        free(props);
        return NULL;
    }
    return props;
}

//*******************************************************************
// Free properties table
//*******************************************************************

void FreeProperties(PROPERTIES *props)
{
    DEBUG_PRINTF("FreeProperties - capacity=%d len=%d\n", props->capacity, props->len);
    for (int i = 0; i < props->capacity; i++)
    {
        if (props->entries[i].key != NULL)
        {
            free((void *)props->entries[i].key);
        }
    }
    free(props->entries);
    free(props);
}

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

HANDLE_ENTRY *AllocateDlgItem(HWND hwndParent, HMENU hMenu)
{
    HANDLE_ENTRY *h = AllocateHandle(DlgItem, hwndParent);
    if (h == NULL)
    {
        return NULL;
    }
    h->dlgItem.nIDDlgItem = (int)hMenu;
    h->lpDialogFunc = &dlgItemProc;
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
    HANDLE_ENTRY *h;
    for (int i = 0; i < global_table->count; i++)
    {
        h = &global_table->entries[i];
        if ((h->refcount > 0) && (hDlg == h->hwndParent) && (nIDDlgItem == h->dlgItem.nIDDlgItem))
        {
            return (HWND)h;
        }
    }
    DEBUG_PRINTF("GetDlgItem %d - not found\n", nIDDlgItem);
    return NULL;
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

#endif
