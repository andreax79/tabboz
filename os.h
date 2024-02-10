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

// -------------------------------------------------------------------
// Definizioni per i vari sistemi operativi...

#ifdef EMSCRIPTEN
#define TABBOZ_EM
#else
// #define TABBOZ_LINUX
// #define TABBOZ_AMIGA
// #define TABBOZ_WIN16
#define TABBOZ_WIN32
#endif

// Linux -------------------------------------------------------------

#ifdef TABBOZ_LINUX
#include <gtk/gtk.h>
etc...
#endif

// Amiga -------------------------------------------------------------

#ifdef TABBOZ_AMIGA
    etc...
#endif

// Windows 16bit -----------------------------------------------------

#ifdef TABBOZ_WIN16
#define TABBOZ_WIN
#include <windows.h>

#define SetCheck(hDlg, nIDDlgItem, wParam) (GetDlgItem(hDlg, nIDDlgItem), BM_SETCHECK, wParam, 0L)
#endif

// Windows 32bit -----------------------------------------------------

#ifdef TABBOZ_WIN32
#define TABBOZ_WIN
#include <windows.h>

#define SetCheck(hDlg, nIDDlgItem, wParam) (GetDlgItem(hDlg, nIDDlgItem), BM_SETCHECK, wParam, 0L)
#endif

// Emscripten --------------------------------------------------------

#ifdef TABBOZ_EM
#include <emscripten.h>
#include <stdlib.h>

#define FAR
#define NEAR
#define PASCAL
#define WINAPI

typedef int            BOOL;
typedef int            INT_PTR;
typedef int            INT;
typedef unsigned int   UINT;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned long  LONG;
typedef BYTE           BOOLEAN;
typedef WORD           ATOM;
typedef void          *PVOID;
typedef void          *LPVOID;
typedef char           CHAR;
typedef CHAR          *LPSTR;
typedef const CHAR    *LPCSTR;
typedef LPCSTR         LPCTSTR;
typedef PVOID          HANDLE;
typedef HANDLE         HWND;
typedef HANDLE         HINSTANCE;
typedef HANDLE         HICON;
typedef HANDLE         HDC;
typedef HANDLE         HBITMAP;
typedef HANDLE         HCURSOR;
typedef HANDLE         HBRUSH;
typedef HANDLE         HMENU;
typedef long           LONG_PTR;
typedef unsigned int   UINT_PTR;
typedef unsigned long  ULONG_PTR;
typedef LONG_PTR       LPARAM;
typedef UINT_PTR       WPARAM;
typedef LONG_PTR       LRESULT;
typedef DWORD          COLORREF;

typedef void *FARPROC;
typedef INT_PTR (*DLGPROC)(HWND, UINT, WPARAM, LPARAM);
typedef INT_PTR (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef struct
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT, *PRECT, *NPRECT, *LPRECT;

typedef struct
{
    UINT      style;
    WNDPROC   lpfnWndProc;
    INT       cbClsExtra;
    INT       cbWndExtra;
    HINSTANCE hInstance;
    HICON     hIcon;
    HCURSOR   hCursor;
    HBRUSH    hbrBackground;
    LPCSTR    lpszMenuName;
    LPCSTR    lpszClassName;
} WNDCLASS, *PWNDCLASS, *LPWNDCLASS;

typedef struct
{
    LPVOID    lpCreateParams;
    HINSTANCE hInstance;
    HMENU     hMenu;
    HWND      hwndParent;
    int       cy;
    int       cx;
    int       y;
    int       x;
    LONG      style;
    LPCSTR    lpszName;
    LPCSTR    lpszClass;
    DWORD     dwExStyle;
} CREATESTRUCT, *LPCREATESTRUCT;

typedef struct {
    LONG x;
    LONG y;
} POINT, *PPOINT, *NPPOINT, *LPPOINT;

typedef struct
{
    HWND   hwnd;
    UINT   message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD  time;
    POINT  pt;
    DWORD  lPrivate;
} MSG, *PMSG, *NPMSG, *LPMSG;

typedef struct {
    HDC  hdc;
    BOOL fErase;
    RECT rcPaint;
    BOOL fRestore;
    BOOL fIncUpdate;
    BYTE rgbReserved[32];
} PAINTSTRUCT, *PPAINTSTRUCT, *NPPAINTSTRUCT, *LPPAINTSTRUCT;

struct property
{
    LPCSTR key;
    HANDLE hData;
};

struct properties
{
    int              len;
    int              capacity;
    struct property *entry;
};

struct handle_entry
{
    unsigned int       refcount;
    unsigned int       id;
    DLGPROC            lpDialogFunc;
    BOOL               end;
    INT_PTR            retval; // the value to be returned from the function that created the dialog box
    struct properties *props;  // window properties
};

struct handle_table
{
    int                  count;
    struct handle_entry *entries;
};

#define NOMENU

#define FALSE 0
#define TRUE 1

#define IDOK 1
#define IDCANCEL 2
#define IDABORT 3
#define IDYES 6
#define IDNO 7

#define MB_OK 0x00000000L
#define MB_YESNO 0x00000004L
#define MB_OKCANCEL 0x00000001L
#define MB_ICONHAND 0x00000010L
#define MB_ICONERROR 0x00000010L
#define MB_ICONSTOP 0x00000010L
#define MB_ICONQUESTION 0x00000020L
#define MB_ICONEXCLAMATION 0x00000030L
#define MB_ICONWARNING 0x00000030L
#define MB_ICONINFORMATION 0x00000040L
#define MB_ICONASTERISK 0x00000040L

#define WM_CREATE 0x0001
#define WM_DESTROY 0x0002
#define WM_PAINT 0x000f
#define WM_ENDSESSION 0x0016
#define WM_QUERYDRAGICON 0x0037
#define WM_INITDIALOG 0x0110
#define WM_COMMAND 0x0111
#define WM_SYSCOMMAND 0x0112
#define WM_TIMER 0x0113
#define WM_LBUTTONDOWN 0x0201

#define BM_SETCHECK 0xf0f1

#define SW_HIDE 0
#define SW_SHOWNORMAL 1

#define SC_CLOSE 0xf060

#define SM_CXSCREEN 0
#define SM_CYSCREEN 1

#define SWP_NOMOVE 0x0002
#define SWP_NOZORDER 0x0004

#define random(x) (random() % x)
#define LOWORD(x) (x & 0xffff)
#define HIWORD(x) ((x >> 16) & 0xffff)

#define MakeProcInstance(p, i) (p)
#define FreeProcInstance(p) (void)(p)
#define MAKEINTRESOURCE(i) ((LPSTR)(ULONG_PTR)LOWORD(i))
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

extern HWND                 GetDlgItem(HWND DhDlg, int nIDDlgItem);
extern HWND                 SetFocus(HWND hWnd);
extern INT_PTR              DialogBox(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc);
extern BOOL                 EndDialog(HWND hwnd, INT_PTR retval);
extern BOOL                 SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCSTR lpString);
extern UINT                 GetDlgItemText(HWND hDlg, int nIDDlgItem, LPSTR lpString, int nMaxCount);
extern int                  MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
extern BOOL                 ShowWindow(HWND hWnd, int nCmdShow);
extern int                  LoadString(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax);
extern void                 LoadStringResources(void);
extern void                 InitTabboz(void);
extern LRESULT              SetCheck(HWND hDlg, int nIDDlgItem, WPARAM wParam);
extern int                  GetSystemMetrics(int nIndex);
extern BOOL                 GetWindowRect(HWND hWnd, LPRECT lpRect);
extern BOOL                 MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint);
extern HANDLE               GetProp(HWND hWnd, LPCSTR lpString);
extern BOOL                 SetProp(HWND hWnd, LPCSTR lpString, HANDLE hData);
extern HANDLE               RemoveProp(HWND hWnd, LPCSTR lpString);
extern void                 ExitWindows(int dwReserved, int code);
extern int                  WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int cmdShow);
extern BOOL                 GetMessage(LPMSG lpMsg, HWND  hWnd, UINT  wMsgFilterMin, UINT  wMsgFilterMax);
extern LRESULT              DispatchMessage(const MSG *lpMsg);
extern HICON                LoadIcon(HINSTANCE hInstance, LPCSTR lpIconName);
extern HBITMAP              LoadBitmap(HINSTANCE hInstance, LPCSTR lpBitmapName);
extern BOOL                 DestroyIcon(HICON hIcon);
extern BOOL                 DeleteObject(void *ho);
extern HDC                  BeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
extern BOOL                 EndPaint(HWND hWnd, const PAINTSTRUCT *lpPaint);
extern BOOL                 SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
extern LRESULT              DefWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
extern void                 randomize();
extern HANDLE               properties_get(struct properties *props, LPCSTR key);
extern HANDLE               properties_set(struct properties *props, LPCSTR key, HANDLE hData);
extern HANDLE               properties_remove(struct properties *props, LPCSTR key);
extern struct handle_entry *alloc_handle();
extern void                 release_handle(HANDLE p);

#endif
