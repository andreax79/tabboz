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
