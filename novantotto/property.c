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
#include "debug.h"

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
