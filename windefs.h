#include <emscripten.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>

#define WINVER 0x0502
#define MAX_PATH 260
#define MESSAGE_LIMIT 1000
#define FALSE 0
#define TRUE 1
#define IN
#define OUT
#define OPTIONAL

#define far
#define near
#define pascal
#define cdecl
#define _export

#define FAR
#define NEAR
#define PASCAL
#define CDECL
#define CALLBACK
#define WINAPI
#define WINAPIV
#define APIENTRY WINAPI
#define APIPRIVATE
#define CONST const

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
typedef long           LONG_PTR;
typedef unsigned int   UINT_PTR;
typedef unsigned long  ULONG_PTR;
typedef LONG_PTR       LPARAM;
typedef UINT_PTR       WPARAM;
typedef LONG_PTR       LRESULT;
typedef DWORD          COLORREF;
typedef unsigned long  DWORD_PTR;
typedef PVOID          HANDLE;
typedef void          *FARPROC;

#define MAKEWORD(bLow, bHigh) ((WORD)(((BYTE)((DWORD_PTR)(bLow)&0xff)) | (((WORD)((BYTE)((DWORD_PTR)(bHigh)&0xff))) << 8)))
#define MAKELONG(wLow, wHigh) ((LONG)(((WORD)((DWORD_PTR)(wLow)&0xffff)) | (((DWORD)((WORD)((DWORD_PTR)(wHigh)&0xffff))) << 16)))
#define LOWORD(l) ((WORD)((DWORD_PTR)(l)&0xffff))
#define HIWORD(l) ((WORD)(((DWORD_PTR)(l) >> 16) & 0xffff))
#define LOBYTE(w) ((BYTE)((DWORD_PTR)(w)&0xff))
#define HIBYTE(w) ((BYTE)(((DWORD_PTR)(w) >> 8) & 0xff))
#define DECLARE_HANDLE(n) typedef HANDLE n

DECLARE_HANDLE(HKEY);

DECLARE_HANDLE(HACCEL);
DECLARE_HANDLE(HBITMAP);
DECLARE_HANDLE(HBRUSH);
DECLARE_HANDLE(HCOLORSPACE);
DECLARE_HANDLE(HDC);
DECLARE_HANDLE(HGLRC);
DECLARE_HANDLE(HCURSOR);
DECLARE_HANDLE(HDESK);
DECLARE_HANDLE(HENHMETAFILE);
DECLARE_HANDLE(HFONT);
DECLARE_HANDLE(HICON);
DECLARE_HANDLE(HMENU);
DECLARE_HANDLE(HMETAFILE);
DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HPALETTE);
DECLARE_HANDLE(HPEN);
DECLARE_HANDLE(HRGN);
DECLARE_HANDLE(HRSRC);
DECLARE_HANDLE(HSTR);
DECLARE_HANDLE(HTASK);
DECLARE_HANDLE(HWINSTA);
DECLARE_HANDLE(HKL);
DECLARE_HANDLE(HMONITOR);
DECLARE_HANDLE(HWINEVENTHOOK);
DECLARE_HANDLE(HUMPD);
DECLARE_HANDLE(HWND);

typedef HKEY     *PHKEY;
typedef HINSTANCE HMODULE;
typedef INT_PTR (*DLGPROC)(HWND, UINT, WPARAM, LPARAM);
typedef INT_PTR (*WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef INT_PTR (*LPOFNHOOKPROC)(HWND, UINT, WPARAM, LPARAM);
typedef INT_PTR (*TIMERPROC)(HWND, UINT, WPARAM, LPARAM);

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

typedef struct
{
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

typedef struct
{
    HDC  hdc;
    BOOL fErase;
    RECT rcPaint;
    BOOL fRestore;
    BOOL fIncUpdate;
    BYTE rgbReserved[32];
} PAINTSTRUCT, *PPAINTSTRUCT, *NPPAINTSTRUCT, *LPPAINTSTRUCT;

typedef struct
{
    DWORD         lStructSize;
    HWND          hwndOwner;
    HINSTANCE     hInstance;
    LPCSTR        lpstrFilter;
    LPSTR         lpstrCustomFilter;
    DWORD         nMaxCustFilter;
    DWORD         nFilterIndex;
    LPSTR         lpstrFile;
    DWORD         nMaxFile;
    LPSTR         lpstrFileTitle;
    DWORD         nMaxFileTitle;
    LPCSTR        lpstrInitialDir;
    LPCSTR        lpstrTitle;
    DWORD         Flags;
    WORD          nFileOffset;
    WORD          nFileExtension;
    LPCSTR        lpstrDefExt;
    LPARAM        lCustData;
    LPOFNHOOKPROC lpfnHook;
    LPCSTR        lpTemplateName;
    void         *pvReserved;
    DWORD         dwReserved;
    DWORD         FlagsEx;
} OPENFILENAME, *LPOPENFILENAME;

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

typedef struct
{
    unsigned int refcount;
    unsigned int id;
    DLGPROC      lpDialogFunc;
    PROPERTIES  *props; // window properties
} HANDLE_ENTRY;

typedef struct
{
    int           count;
    HANDLE_ENTRY *entries;
} HANDLE_TABLE;

typedef struct
{
    int count;
    int next;
    int free;
    int capacity;
    MSG messages[1];
} MESAGES_QUEUE;

typedef struct
{
    HWND      hwnd;
    UINT_PTR  nIDEvent;
    TIMERPROC lpTimerFunc;
    long      timeoutId;
} TIMER;

#define NOMENU

#define IDOK 1
#define IDCANCEL 2
#define IDABORT 3
#define IDRETRY 4
#define IDIGNORE 5
#define IDYES 6
#define IDNO 7
#define IDCLOSE 8
#define IDHELP 9
#define IDTRYAGAIN 10
#define IDCONTINUE 11
#define IDTIMEOUT 32000

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
#define WM_QUIT 0x0012
#define WM_ENDSESSION 0x0016
#define WM_QUERYDRAGICON 0x0037
#define WM_SETICON 0x0080
#define WM_KEYDOWN 0x0100
#define WM_INITDIALOG 0x0110
#define WM_COMMAND 0x0111
#define WM_SYSCOMMAND 0x0112
#define WM_TIMER 0x0113
#define WM_LBUTTONDOWN 0x0201

#define VK_ESCAPE 0x1b

#define BM_SETCHECK 0xf0f1

#define SW_HIDE 0
#define SW_SHOWNORMAL 1

#define SC_MINIMIZE 0xf020
#define SC_MAXIMIZE 0xf030
#define SC_CLOSE 0xf060
#define SC_KEYMENU 0xf100
#define SC_CONTEXTHELP 0xf180

#define SM_CXSCREEN 0
#define SM_CYSCREEN 1

#define SWP_NOMOVE 0x0002
#define SWP_NOZORDER 0x0004

#define ICON_SMALL 0
#define ICON_BIG 1

#define PM_NOREMOVE 0
#define PM_REMOVE 1

#define OFN_HIDEREADONLY 0x00000004
#define OFN_LONGNAMES 0x00200000
#define OFN_OVERWRITEPROMPT 0x00000002
#define OFN_NOTESTFILECREATE 0x00010000
#define OFN_FILEMUSTEXIST 0x00001000

#define random(x) ((int)(floor(emscripten_random() * x)))

#define MakeProcInstance(p, i) (p)
#define FreeProcInstance(p) (void)(p)
#define MAKEINTRESOURCE(i) ((LPSTR)(ULONG_PTR)LOWORD(i))
#define RGB(r, g, b) ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)))

extern HWND          GetDlgItem(HWND DhDlg, int nIDDlgItem);
extern HWND          SetFocus(HWND hWnd);
extern INT_PTR       DialogBox(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc);
extern BOOL          EndDialog(HWND hwnd, INT_PTR retval);
extern BOOL          SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCSTR lpString);
extern UINT          GetDlgItemText(HWND hDlg, int nIDDlgItem, LPSTR lpString, int nMaxCount);
extern int           MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
extern BOOL          ShowWindow(HWND hWnd, int nCmdShow);
extern int           LoadString(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax);
extern void          LoadStringResources(void);
extern void          InitTabboz(void);
extern LRESULT       SetCheck(HWND hDlg, int nIDDlgItem, WPARAM wParam);
extern int           GetSystemMetrics(int nIndex);
extern BOOL          GetWindowRect(HWND hWnd, LPRECT lpRect);
extern BOOL          MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint);
extern HANDLE        GetProp(HWND hWnd, LPCSTR lpString);
extern BOOL          SetProp(HWND hWnd, LPCSTR lpString, HANDLE hData);
extern HANDLE        RemoveProp(HWND hWnd, LPCSTR lpString);
extern void          ExitWindows(int dwReserved, int code);
extern int           WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpszCmdLine, int cmdShow);
extern BOOL          GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
extern LRESULT       DispatchMessage(const MSG *lpMsg);
extern LRESULT       SendMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern BOOL          PostMessage(HWND hwnd, WORD message, WORD wParam, LONG lParam);
extern HICON         LoadIcon(HINSTANCE hInstance, LPCSTR lpIconName);
extern HBITMAP       LoadBitmap(HINSTANCE hInstance, LPCSTR lpBitmapName);
extern BOOL          DestroyIcon(HICON hIcon);
extern BOOL          DeleteObject(void *ho);
extern HDC           BeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint);
extern BOOL          EndPaint(HWND hWnd, const PAINTSTRUCT *lpPaint);
extern BOOL          SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
extern LRESULT       DefWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
extern BOOL          GetOpenFileName(LPOPENFILENAME unnamedParam1);
extern BOOL          GetSaveFileName(LPOPENFILENAME unnamedParam1);
extern UINT_PTR      SetTimer(HWND hWnd, UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc);
extern BOOL          KillTimer(HWND hWnd, UINT_PTR uIDEvent);
extern BOOL          SetMessageQueue(int size);
extern void          randomize();
extern HANDLE        GetProperty(PROPERTIES *props, LPCSTR key);
extern HANDLE        SetProperty(PROPERTIES *props, LPCSTR key, HANDLE hData);
extern HANDLE        DelProperty(PROPERTIES *props, LPCSTR key);
extern HANDLE_ENTRY *AllocateHandle();
extern void          ReleaseHandle(HANDLE p);
