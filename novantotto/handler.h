/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef HANDLER_H
#define HANDLER_H

#include "novantotto.h"
#include "property.h"

#define MIN_HANDLE_ENTRIES 100

typedef enum HandleType
{
    Window = 1,
    DlgItem = 2,
    RegKey = 3
} HandleType;

typedef struct
{
    unsigned int refcount;
    unsigned int id;
    HandleType   type;
    HWND         hwndParent;   // parent window
    DLGPROC      lpDialogFunc; // window/dialog item procedure
    LPSTR        lpClassName;
    union {
        // if type == Window
        struct
        {
            PROPERTIES *props; // window properties
            BOOL        fEnd;  // end dialog flag
            INT_PTR     dialogResult;
        } window;
        // if type == DlgItem
        struct
        {
            int nIDDlgItem; // control identifier
        } dlgItem;
        // if type == RegKey
        struct
        {
            LPSTR lpSubKey;
        } regKey;
    };
} HANDLE_ENTRY;

typedef struct
{
    int           count;
    HANDLE_ENTRY *entries;
} HANDLE_TABLE;

extern HANDLE_TABLE *global_table;
extern HANDLE_TABLE *AllocateHandleTable(int count);
extern HANDLE_ENTRY *AllocateHandle(HandleType type, HWND hwndParent);
extern HANDLE_ENTRY *AllocateDlgItem(LPCTSTR lpClassName, HWND hwndParent, HMENU hMenu);
extern void          ReleaseHandle(HANDLE p);
extern HANDLE_ENTRY *GetHandle(int index);
extern void          DispatchToChildren(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
