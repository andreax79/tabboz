#ifndef WINDEFS_H
#define WINDEFS_H

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
typedef int            ATOM; // typedef WORD ATOM;
typedef void          *VOID;
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
#define WM_LBUTTONUP 0x0202

#define WS_OVERLAPPED 0x00000000L
#define WS_TILED WS_OVERLAPPED
#define WS_POPUP 0x80000000L
#define WS_CHILD 0x40000000L
#define WS_CHILDWINDOW WS_CHILD
#define WS_MINIMIZE 0x20000000L
#define WS_VISIBLE 0x10000000L
#define WS_DISABLED 0x08000000L
#define WS_CLIPSIBLINGS 0x04000000L
#define WS_CLIPCHILDREN 0x02000000L
#define WS_MAXIMIZE 0x01000000L
#define WS_CAPTION 0x00C00000L
#define WS_BORDER 0x00800000L
#define WS_DLGFRAME 0x00400000L
#define WS_VSCROLL 0x00200000L
#define WS_HSCROLL 0x00100000L
#define WS_SYSMENU 0x00080000L
#define WS_THICKFRAME 0x00040000L
#define WS_SIZEBOX WS_THICKFRAME
#define WS_GROUP 0x00020000L
#define WS_TABSTOP 0x00010000L
#define WS_MINIMIZEBOX 0x00020000L
#define WS_ICONIC WS_MINIMIZE
#define WS_MAXIMIZEBOX 0x00010000L
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_TILEDWINDOW WS_OVERLAPPEDWINDOW
#define WS_POPUPWINDOW (WS_POPUP | WS_BORDER | WS_SYSMENU)

#define MF_ENABLED 0x0000
#define MF_GRAYED 0x0001
#define MF_DISABLED 0x0002
#define MF_STRING 0x0000
#define MF_BITMAP 0x0004
#define MF_UNCHECKED 0x0000
#define MF_CHECKED 0x0008
#define MF_POPUP 0x0010
#define MF_MENUBARBREAK 0x0020
#define MF_MENUBREAK 0x0040
#define MF_UNHILITE 0x0000
#define MF_HILITE 0x0080
#define MF_OWNERDRAW 0x0100
#define MF_USECHECKBITMAPS 0x0200
#define MF_BYCOMMAND 0x0000
#define MF_BYPOSITION 0x0400
#define MF_SEPARATOR 0x0800
#define MF_DEFAULT 0x1000
#define MF_SYSMENU 0x2000
#define MF_HELP 0x4000
#define MF_RIGHTJUSTIFY 0x4000
#define MF_MOUSESELECT 0x8000

#define VK_ESCAPE 0x1b

#define BM_GETCHECK 0xf0f0
#define BM_SETCHECK 0xf0f1

#define CB_ADDSTRING 0x0143
#define CB_SETCURSEL 0x014e

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

#define COLOR_SCROLLBAR 0
#define COLOR_BACKGROUND 1
#define COLOR_DESKTOP 1
#define COLOR_ACTIVECAPTION 2
#define COLOR_INACTIVECAPTION 3
#define COLOR_MENU 4
#define COLOR_WINDOW 5
#define COLOR_WINDOWFRAME 6
#define COLOR_MENUTEXT 7
#define COLOR_WINDOWTEXT 8
#define COLOR_CAPTIONTEXT 9
#define COLOR_ACTIVEBORDER 10
#define COLOR_INACTIVEBORDER 11
#define COLOR_APPWORKSPACE 12
#define COLOR_HIGHLIGHT 13
#define COLOR_HIGHLIGHTTEXT 14
#define COLOR_BTNFACE 15
#define COLOR_3DFACE 15
#define COLOR_BTNSHADOW 16
#define COLOR_3DSHADOW 16
#define COLOR_GRAYTEXT 17
#define COLOR_BTNTEXT 18
#define COLOR_INACTIVECAPTIONTEXT 19
#define COLOR_BTNHIGHLIGHT 20
#define COLOR_3DHIGHLIGHT 20
#define COLOR_3DHILIGHT 20
#define COLOR_BTNHILIGHT 20
#define COLOR_3DDKSHADOW 21
#define COLOR_3DLIGHT 22
#define COLOR_INFOTEXT 23
#define COLOR_INFOBK 24
#define COLOR_HOTLIGHT 26
#define COLOR_GRADIENTACTIVECAPTION 27
#define COLOR_GRADIENTINACTIVECAPTION 28

#define random(x) ((int)(floor(emscripten_random() * x)))

#define MakeProcInstance(p, i) (p)
#define FreeProcInstance(p) (void)(p)
#define MAKEINTRESOURCE(i) ((LPSTR)(ULONG_PTR)LOWORD(i))
#define RGB(r, g, b) ((COLORREF)(((BYTE)(r) | ((WORD)((BYTE)(g)) << 8)) | (((DWORD)(BYTE)(b)) << 16)))

#define IDC_ARROW MAKEINTRESOURCE(32512)
#define IDC_IBEAM MAKEINTRESOURCE(32513)
#define IDC_WAIT MAKEINTRESOURCE(32514)
#define IDC_CROSS MAKEINTRESOURCE(32515)
#define IDC_UPARROW MAKEINTRESOURCE(32516)
#define IDC_SIZE MAKEINTRESOURCE(32640)
#define IDC_ICON MAKEINTRESOURCE(32641)
#define IDC_SIZENWSE MAKEINTRESOURCE(32642)
#define IDC_SIZENESW MAKEINTRESOURCE(32643)
#define IDC_SIZEWE MAKEINTRESOURCE(32644)
#define IDC_SIZENS MAKEINTRESOURCE(32645)
#define IDC_SIZEALL MAKEINTRESOURCE(32646)
#define IDC_NO MAKEINTRESOURCE(32648)
#define IDC_HAND MAKEINTRESOURCE(32649)
#define IDC_APPSTARTING MAKEINTRESOURCE(32650)
#define IDC_HELP MAKEINTRESOURCE(32651)
#define IDC_PIN MAKEINTRESOURCE(32671)
#define IDC_PERSON MAKEINTRESOURCE(32672)

#define RDW_INVALIDATE 0x0001
#define RDW_INTERNALPAINT 0x0002
#define RDW_ERASE 0x0004
#define RDW_VALIDATE 0x0008
#define RDW_NOINTERNALPAINT 0x0010
#define RDW_NOERASE 0x0020
#define RDW_NOCHILDREN 0x0040
#define RDW_ALLCHILDREN 0x0080
#define RDW_UPDATENOW 0x0100
#define RDW_ERASENOW 0x0200
#define RDW_FRAME 0x0400
#define RDW_NOFRAME 0x0800

extern HWND          GetDlgItem(HWND DhDlg, int nIDDlgItem);
extern HWND          SetFocus(HWND hWnd);
extern INT_PTR       DialogBox(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc);
extern BOOL          EndDialog(HWND hwnd, INT_PTR retval);
extern BOOL          SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCSTR lpString);
extern UINT          GetDlgItemText(HWND hDlg, int nIDDlgItem, LPSTR lpString, int nMaxCount);
extern BOOL          SetDlgItemInt(HWND hDlg, int nIDDlgItem, UINT uValue, BOOL bSigned);
extern UINT          GetDlgItemInt(HWND hDlg, int nIDDlgItem, BOOL *lpTranslated, BOOL bSigned);
extern int           MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
extern BOOL          ShowWindow(HWND hWnd, int nCmdShow);
extern int           LoadString(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax);
extern void          LoadStringResources(void);
extern void          InitTabboz(void);
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
extern HCURSOR       LoadCursor(HINSTANCE hInstance, LPCSTR lpCursorName);
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
extern ATOM          RegisterClass(const WNDCLASS *lpWndClass);
extern BOOL          UnregisterClass(LPSTR lpClassName, HANDLE hInstance);
extern BOOL          RedrawWindow(HWND hWnd, const RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags);
extern BOOL          SetMessageQueue(int size);
extern void          randomize();
extern HANDLE        GetProperty(PROPERTIES *props, LPCSTR key);
extern HANDLE        SetProperty(PROPERTIES *props, LPCSTR key, HANDLE hData);
extern HANDLE        DelProperty(PROPERTIES *props, LPCSTR key);
extern HANDLE_ENTRY *AllocateHandle(HandleType type, HWND hwndParent);
extern void          ReleaseHandle(HANDLE p);
extern void          DispatchToChildren(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif // WINDEFS_H
