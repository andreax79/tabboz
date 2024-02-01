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
#define PASCAL
#define WINAPI

typedef int            BOOL;
typedef int            INT_PTR;
typedef unsigned int   UINT;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned long  LONG;
typedef BYTE           BOOLEAN;
typedef WORD           ATOM;
typedef void          *PVOID;
typedef char           CHAR;
typedef CHAR          *LPSTR;
typedef const CHAR    *LPCSTR;
typedef LPCSTR         LPCTSTR;
typedef PVOID          HANDLE;
typedef HANDLE         HWND;
typedef HANDLE         HINSTANCE;
typedef HANDLE         HICON;
typedef long           LONG_PTR;
typedef unsigned int   UINT_PTR;
typedef unsigned long  ULONG_PTR;
typedef LONG_PTR       LPARAM;
typedef UINT_PTR       WPARAM;
typedef LONG_PTR       LRESULT;

typedef void *FARPROC;
typedef INT_PTR (*DLGPROC)(HWND, UINT, WPARAM, LPARAM);
/* typedef void *DLGPROC; */

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
#define MB_ICONQUESTION 0x00000020L
#define MB_ICONINFORMATION 0x00000040L
#define MB_ICONHAND 0x00000010L
#define MB_ICONERROR 0x00000010L
#define MB_ICONSTOP 0x00000010L

#define WM_DESTROY 0x0002
#define WM_ENDSESSION 0x0016
#define WM_QUERYDRAGICON 0x0037
#define WM_INITDIALOG 0x0110
#define WM_COMMAND 0x0111
#define WM_SYSCOMMAND 0x0112
#define WM_TIMER 0x0113

#define BM_SETCHECK 0xf0f1

#define SW_HIDE 0
#define SW_SHOWNORMAL 1

#define SC_CLOSE 0xf060

#define random(x) (random() % x)
#define LOWORD(x) (x & 0xffff)
#define HIWORD(x) ((x >> 16) & 0xffff)

#define MakeProcInstance(p, i) (p)
#define FreeProcInstance(p) (void)(p)
#define MAKEINTRESOURCE(i) ((LPSTR)(ULONG_PTR)LOWORD(i))

extern HWND    GetDlgItem(HWND DhDlg, int nIDDlgItem);
extern HWND    SetFocus(HWND hWnd);
extern INT_PTR DialogBox(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc);
extern BOOL    EndDialog(HWND hwnd, INT_PTR retval);
extern BOOL    SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCSTR lpString);
extern UINT    GetDlgItemText(HWND hDlg, int nIDDlgItem, LPSTR lpString, int nMaxCount);
extern int     MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
extern BOOL    ShowWindow(HWND hWnd, int nCmdShow);
extern int     LoadString(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax);
extern void    LoadStringResources(void);
extern void    InitTabboz(void);
extern LRESULT SetCheck(HWND hDlg, int nIDDlgItem, WPARAM wParam);

#endif
