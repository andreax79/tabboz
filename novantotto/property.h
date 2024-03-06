/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef PROPERTY_H
#define PROPERTY_H

#include "novantotto.h"

#define PROPERTIES_DEFAULT_ENTRIES 8

typedef struct
{
    LPCSTR key;   // key
    HANDLE hData; // value
} PROPERTY;

typedef struct
{
    int       len;      // current length
    int       capacity; // capacity
    PROPERTY *entries;
} PROPERTIES;

extern int         FindPropertyIndex(PROPERTIES *props, LPCSTR key);
extern int         FindEmptyPropertyIndex(PROPERTIES *props);
extern HANDLE      GetProperty(PROPERTIES *props, LPCSTR key);
extern HANDLE      DelProperty(PROPERTIES *props, LPCSTR key);
extern HANDLE      SetProperty(PROPERTIES *props, LPCSTR key, HANDLE hData);
extern PROPERTIES *AllocateProperties(void);
extern void        FreeProperties(PROPERTIES *props);

#endif
