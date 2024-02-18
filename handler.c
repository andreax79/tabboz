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

#define MIN_HANDLE_ENTRIES 32
#define PROPERTIES_DEFAULT_ENTRIES 8

static HANDLE_TABLE *global_table;

//*******************************************************************
// Find property index
//*******************************************************************

int FindPropertyIndex(PROPERTIES *props, LPCSTR key)
{
    for (int i = 0; i < props->len; i++)
    {
        if (!strcmp(props->entry[i].key, key))
        {
            return i;
        }
    }
    return -1;
}

//*******************************************************************
// Get an entry from the property list
//*******************************************************************

HANDLE GetProperty(PROPERTIES *props, LPCSTR key)
{
    int idx = FindPropertyIndex(props, key);
    if (idx == -1)
    {
        return NULL;
    }
    else
    {
        return props->entry[idx].hData;
    }
}

//*******************************************************************
// Remove an entry from the property list
//*******************************************************************

HANDLE DelProperty(PROPERTIES *props, LPCSTR key)
{
    int idx = FindPropertyIndex(props, key);
    if (idx == -1)
    {
        return NULL;
    }
    else
    {
        HANDLE prev = props->entry[idx].hData;
        props->entry[idx].hData = NULL;
        free((void *)props->entry[idx].key);
        props->entry[idx].key = NULL;
        return prev;
    }
}

//*******************************************************************
// Set an entry in the property list
//*******************************************************************

HANDLE SetProperty(PROPERTIES *props, LPCSTR key, HANDLE hData)
{
    int idx = FindPropertyIndex(props, key);
    if (idx != -1)
    {
        HANDLE prev = props->entry[idx].hData;
        props->entry[idx].hData = hData;
        return prev;
    }
    if (props->len == props->capacity)
    {
        props->capacity *= 2;
        props->entry = realloc(props->entry, props->capacity * sizeof(PROPERTY));
    }
    props->entry[props->len].key = strdup(key);
    props->entry[props->len].hData = hData;
    props->len++;
    return NULL;
}

//*******************************************************************
// Allocate a new properties table
//*******************************************************************

PROPERTIES *AllocateProperties(void)
{
    PROPERTIES  proto = {0, PROPERTIES_DEFAULT_ENTRIES, malloc(PROPERTIES_DEFAULT_ENTRIES * sizeof(PROPERTY))};
    PROPERTIES *d = malloc(sizeof(PROPERTIES));
    *d = proto;
    return d;
}

//*******************************************************************
// Free properties table
//*******************************************************************

void FreeProperties(PROPERTIES *props)
{
    for (int i = 0; i < props->len; i++)
    {
        if (props->entry[i].key != NULL)
        {
            free((void *)props->entry[i].key);
        }
    }
    free(props->entry);
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
    if (!(table = malloc(sizeof(HANDLE_TABLE))))
    {
        return NULL;
    }
    table->count = count;
    if ((table->entries = malloc(count * sizeof(*table->entries))))
    {
        bzero(table->entries, count * sizeof(*table->entries));
        return table;
    }
    else
    {
        free(table);
        return NULL;
    }
}

//*******************************************************************
// Allocate an handle
//*******************************************************************

HANDLE_ENTRY *AllocateHandle()
{
    int i;
    if (!global_table)
    {
        if (!(global_table = AllocateHandleTable(0)))
            return NULL;
    }
    for (i = 0; i < global_table->count; i++)
    {
        HANDLE_ENTRY *h = &global_table->entries[i];
        if (h->refcount == 0)
        {
            h->refcount++;
            h->id = i + 1;
            h->props = AllocateProperties();
            return (HANDLE)h;
        }
    }
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
        if (h->refcount > 0)
        {
            h->refcount--;
            if (h->refcount == 0)
            {
                h->id = 0;
                FreeProperties(h->props);
            }
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

#endif
