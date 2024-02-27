#ifndef HANDLER_H
#define HANDLER_H

#include "novantotto.h"
#include "property.h"

#define MIN_HANDLE_ENTRIES 100

typedef enum HandleType
{
    Window = 1,
    DlgItem = 2
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
        } window;
        // if type == DlgItem
        struct
        {
            int nIDDlgItem; // control identifier
        } dlgItem;
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
extern HWND          GetDlgItem(HWND hDlg, int nIDDlgItem);
extern ATOM          RegisterClass(const WNDCLASS *lpWndClass);
extern BOOL          UnregisterClass(LPSTR lpClassName, HANDLE hInstance);
extern HANDLE_ENTRY *GetHandle(int index);
extern void          DispatchToChildren(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
