/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HANDLE_H
#define HANDLE_H

#include "novantotto.h"
#include "property.h"

#define MIN_HANDLE_ENTRIES 100

typedef enum HandleType
{
    HANDLE_ANY = 0,
    HANDLE_WINDOW = 1,
    HANDLE_CONTROL = 2,
    HANDLE_REGKEY = 3,
    HANDLE_ICON = 4,
    HANDLE_MENU = 5,
    HANDLE_CURSOR = 6,
    HANDLE_ACCELLERATORS = 7,
    HANDLE_OBJECT = 8 // logical pen, brush, font, bitmap, region, or palette
} HandleType;

typedef struct
{
    unsigned int refcount;
    HANDLE       handle;
    HandleType   type;
    HWND         hwndParent;     // parent window
    WNDPROC      lpfnWndProc;    // window/control procedure
    LPSTR        lpClassName;    // class name
    LPSTR        lpResourceName; // resource name or ordinal
    PROPERTIES  *props;          // window/control properties
    union {
        // if type == HANDLE_WINDOW
        struct
        {
            BOOL    fEnd; // end dialog flag
            INT_PTR dialogResult;
        } window;
        // if type == HANDLE_CONTROL
        struct
        {
            int nIDDlgItem; // control identifier
        } control;
        // if type == HANDLE_REGKEY
        struct
        {
            LPSTR lpSubKey;
        } regKey;
    };
} RESOURCE;

typedef struct
{
    int       count;
    RESOURCE *entries;
} HANDLE_TABLE;

extern HANDLE_TABLE *global_table;
extern HANDLE_TABLE *AllocateHandleTable(int count);
extern RESOURCE     *AllocateHandle(HandleType type, HWND hwndParent);
extern void          ReleaseHandle(HANDLE p);
extern RESOURCE     *GetHandle(HANDLE index, HandleType type);
extern void          DispatchToChildren(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
