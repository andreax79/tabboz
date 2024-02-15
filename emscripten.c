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

#include <emscripten/html5.h>

//*******************************************************************
// Load a string resource into a buffer
//*******************************************************************

int LoadString(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax)
{
    return EM_ASM_INT({
        const value = window.strings[$0] || "";
        stringToUTF8(value, $1, $2);
        return value.length;
    },
                      uID, lpBuffer, cchBufferMax);
}

//*******************************************************************
// Show/hide a window
//*******************************************************************

BOOL ShowWindow(HWND hWnd, int nCmdShow)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return EM_ASM_INT({return showWindow($0, $1)}, handle->id, nCmdShow && SW_SHOWNORMAL);
}

//*******************************************************************
// Display a modal dialog box
//*******************************************************************

EM_ASYNC_JS(int, MessageBoxEm, (int windowId, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, int parentWindowId), {
    const element = document.getElementById('messagebox');
    const wall = document.getElementById('wall').cloneNode(true);
    const c = element.cloneNode(true);
    // Set window id
    wall.id = 'wall' + windowId;
    c.id = 'win' + windowId;
    // Set window position
    if (parentWindowId >= 0)
    {
        const parent = document.getElementById('win' + parentWindowId);
        if (parent != null)
        {
            const style = getComputedStyle(parent);
            c.style.left = (parseInt(style.left) + 40) + 'px';
            c.style.top = (parseInt(style.top) + 40) + 'px';
        }
    }
    // Icon
    if (uType & 0x00000030)
    { // MB_ICONEXCLAMATION
        c.querySelector('img').src = "resources/icons/101.png";
    }
    else if (uType & 0x00000020)
    { // MB_ICONQUESTION
        c.querySelector('img').src = "resources/icons/102.png";
    }
    else if (uType & 0x00000010)
    { // MB_ICONSTOP
        c.querySelector('img').src = "resources/icons/103.png";
    }
    else if (uType & 0x00000040)
    { // MB_ICONINFORMATION
        c.querySelector('img').src = "resources/icons/104.png";
    }
    // Buttons
    if (uType & 0x00000001)
    { // MB_OKCANCEL
        c.querySelector('.control1').innerText = 'OK';
        c.querySelector('.control2').innerText = 'Cancel';
        c.querySelector('.control2').style.display = 'inline';
    }
    else if (uType & 0x00000004)
    { // MB_YESNO
        c.querySelector('.control1').innerText = 'Yes';
        c.querySelector('.control2').innerText = 'No';
        c.querySelector('.control2').style.display = 'inline';
    }
    else
    { // MB_OK
        c.querySelector('.control1').innerText = 'OK';
        c.querySelector('.control2').style.display = 'none';
    }
    wall.style.zIndex = windowId;
    c.style.zIndex = windowId;
    c.style.position = 'absolute';
    // Set title and message
    c.querySelector('.title-bar-text').innerText = UTF8ToString(lpCaption);
    c.querySelector('.content').innerText = UTF8ToString(lpText);
    // Add window to screen
    const destination = document.getElementById('screen');
    destination.appendChild(wall);
    destination.appendChild(c);
    // Center the dialog
    let x = parseInt(getComputedStyle(document.getElementById('screen')).width);
    let y = parseInt(getComputedStyle(document.getElementById('screen')).height);
    let w = parseInt(getComputedStyle(c).width);
    let h = parseInt(getComputedStyle(c).height);
    x = (x - w) / 2;
    y = (y - h) / 2;
    c.style.left = x + 'px';
    c.style.top = y + 'px';
    // Activate the window
    setActiveWindow(windowId);
    // Make the window draggrable
    makeDraggable(c);
    // Wait for events
    let result = (await waitListener(windowId)).controlId;
    console.log(result);
    // Convert the result
    if (uType & 0x00000004)
    { // MB_YESNO
        switch (result)
        {
        case 1:
            result = 6; // IDYES
            break;
        case 2:
            result = 7; // IDNO
            break;
        }
    }
    // Remove the window
    destination.removeChild(wall);
    destination.removeChild(c);
    return result;
});

int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    int           result;
    int           parentWindowId = -1;
    HANDLE_ENTRY *handle = AllocateHandle();

    // Get parent window number
    if (hWnd != NULL)
    {
        parentWindowId = ((HANDLE_ENTRY *)hWnd)->id;
    }

    result = MessageBoxEm(handle->id, lpText, lpCaption, uType, parentWindowId);
    ReleaseHandle(handle);
    return result;
}

//*******************************************************************
// Create a modal dialog box from a dialog box template resource
//*******************************************************************

EM_ASYNC_JS(void, DialogBoxEm, (int windowId, int dialog, int parentWindowId), {
    // Load html
    const response = await fetch("resources/dialogs/includes/" + dialog + ".inc.html");
    const html = await     response.text();
    const wall = document.getElementById('wall').cloneNode(true);
    const c = createElementFromHTML(html);
    // Set window id
    wall.id = 'wall' + windowId;
    c.id = 'win' + windowId;
    // Set window position
    if (parentWindowId >= 0)
    {
        const parent = document.getElementById('win' + parentWindowId);
        if (parent != null)
        {
            const style = getComputedStyle(parent);
            c.style.left = (parseInt(style.left) + 40) + 'px';
            c.style.top = (parseInt(style.top) + 40) + 'px';
        }
    }
    // Add window to screen
    const destination = document.getElementById('screen');
    destination.appendChild(wall);
    destination.appendChild(c);
    // Activate the window
    setActiveWindow(windowId);
    // Add menu
    addMainMenu(c);
    // Make the window draggrable
    makeDraggable(c);
});

EM_JS(void, RemoveDialogBoxEm, (int windowId), {
    const destination = document.getElementById('screen');
    destination.removeChild(document.getElementById('wall' + windowId));
    destination.removeChild(document.getElementById('win' + windowId));
});

INT_PTR DialogBox(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc)
{
    INT_PTR       retval;
    int           controlId;
    int           dialog = (int)lpTemplateName;
    int           parentWindowId = -1;
    HANDLE_ENTRY *handle = AllocateHandle();
    MSG           msg = {.hwnd = (HWND)handle};

    if (lpDialogFunc == NULL)
    {
        return 0;
    }
    handle->lpDialogFunc = lpDialogFunc;
    // Get parent window number
    if (hWndParent != NULL)
    {
        parentWindowId = ((HANDLE_ENTRY *)hWndParent)->id;
    }
    // Show dialog
    DialogBoxEm(handle->id, dialog, parentWindowId);
    // Dispatch Init
    lpDialogFunc((HWND)handle, WM_INITDIALOG, 0, 0);
    // Dispatch Repaint
    lpDialogFunc((HWND)handle, WM_PAINT, 0, 0);
    // Message loop
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        printf("%d\n", msg.wParam);
        DispatchMessage(&msg);
    }
    retval = handle->retval;
    RemoveDialogBoxEm(handle->id);
    ReleaseHandle(handle);
    return retval;
}

//*******************************************************************
// Destroy a modal dialog box
//*******************************************************************

BOOL EndDialog(HWND hWnd, INT_PTR retval)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    handle->retval = retval;
    handle->end = TRUE;
    return TRUE;
}

//*******************************************************************
// Set the title or text of a control in a dialog box
//*******************************************************************

BOOL SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCSTR lpString)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hDlg;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return EM_ASM_INT({return setDlgItemText($0, $1, $2)}, handle->id, nIDDlgItem, lpString);
}

//*******************************************************************
// Set the check state of a radio button or check box
//*******************************************************************

EM_JS(BOOL, SetCheckEM, (int windowId, int nIDDlgItem, WPARAM wParam), {
    const control = document.querySelector('#win' + windowId + ' .control' + nIDDlgItem);
    if (control != null)
    {
        control.checked = (wParam != 0);
    }
    return 0;
});

LRESULT SetCheck(HWND hDlg, int nIDDlgItem, WPARAM wParam)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hDlg;
    if (handle == NULL)
    {
        // Invalid window handle
        return 0;
    }
    return SetCheckEM(handle->id, nIDDlgItem, wParam);
}

//*******************************************************************
// Retrieve the specified system metric
//*******************************************************************

int GetSystemMetrics(int nIndex)
{
    return EM_ASM_INT({return getSystemMetrics($0)}, nIndex);
}

//*******************************************************************
// Retrieve the dimensions of a specified window
//*******************************************************************

BOOL GetWindowRect(HWND hWnd, LPRECT lpRect)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    else
    {
        lpRect->left = EM_ASM_INT({return getWindowRectDimension($0, 0)}, handle->id);
        lpRect->top = EM_ASM_INT({return getWindowRectDimension($0, 1)}, handle->id);
        lpRect->right = EM_ASM_INT({return getWindowRectDimension($0, 2)}, handle->id);
        lpRect->bottom = EM_ASM_INT({return getWindowRectDimension($0, 3)}, handle->id);
        return TRUE;
    }
}

//*******************************************************************
// Change the position and dimensions of the specified window
//*******************************************************************

BOOL MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return EM_ASM_INT({return moveWindow($0, $1, $2, $3, $4)}, handle->id, X, Y, nWidth, nHeight);
}

//*******************************************************************
// Retrieve the title or text associated with a control in a dialog box
//*******************************************************************

UINT GetDlgItemText(HWND hDlg, int nIDDlgItem, LPSTR lpString, int nMaxCount)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hDlg;
    if (handle == NULL)
    {
        // Invalid window handle
        return 0;
    }
    return EM_ASM_INT({return getDlgItemText($0, $1, $2, $3, $4)}, handle->id, nIDDlgItem, lpString, nMaxCount);
}

//*******************************************************************
// Get an entry from the property list of the specified window
//*******************************************************************

HANDLE GetProp(HWND hWnd, LPCSTR lpString)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return GetProperty(handle->props, lpString);
}

//*******************************************************************
// Add/change an entry in the property list of the specified window
//*******************************************************************

BOOL SetProp(HWND hWnd, LPCSTR lpString, HANDLE hData)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    SetProperty(handle->props, lpString, hData);
    return TRUE;
}

//*******************************************************************
// Remove an entry from the property list of the specified window
//*******************************************************************

HANDLE RemoveProp(HWND hWnd, LPCSTR lpString)
{
    HANDLE_ENTRY *handle = (HANDLE_ENTRY *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return DelProperty(handle->props, lpString);
}

//*******************************************************************
// Retrieve a message
//*******************************************************************

EM_ASYNC_JS(int, GetMessageEM, (int windowId, int32_t *x, int32_t *y), {
    // Activate the window
    setActiveWindow(windowId);
    // Wait for message
    let msg = await waitListener(windowId);
    // Set return values
    setValue(x, msg.x, "i32");
    setValue(y, msg.y, "i32");
    return msg.controlId;
});

BOOL GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    int32_t x, y;
    if (hWnd != NULL)
    {
        lpMsg->hwnd = hWnd;
    }
    if (((HANDLE_ENTRY *)lpMsg->hwnd)->end)
    {
        return FALSE;
    }
    lpMsg->wParam = GetMessageEM(((HANDLE_ENTRY *)lpMsg->hwnd)->id, &x, &y);
    // low-order word specifies the x-coordinate, high-order word specifies the y-coordinate
    lpMsg->lParam = ((y & 0xffff) << 16) + (x & 0xffff);
    return TRUE;
}

//*******************************************************************
// Dispatch a message to a window procedure
//*******************************************************************

LRESULT DispatchMessage(const MSG *lpMsg)
{
    DLGPROC lpDialogFunc = ((HANDLE_ENTRY *)lpMsg->hwnd)->lpDialogFunc;
    if (lpMsg->wParam == SC_CLOSE)
    {
        if (!lpDialogFunc(lpMsg->hwnd, WM_SYSCOMMAND, lpMsg->wParam, 0))
        {
            EndDialog(lpMsg->hwnd, TRUE);
            // Dispatch Destroy
            lpDialogFunc(lpMsg->hwnd, WM_DESTROY, lpMsg->wParam, 0);
        }
    }
    else
    {
        // Dispatch Commmand
        lpDialogFunc(lpMsg->hwnd, WM_COMMAND, lpMsg->wParam, lpMsg->lParam);
        // Dispatch Repaint
        lpDialogFunc(lpMsg->hwnd, WM_PAINT, lpMsg->wParam, 0);
    }
    return 0;
}

//*******************************************************************
// Return icon id
//*******************************************************************

HICON LoadIcon(HINSTANCE hInstance, LPCSTR lpIconName)
{
    if (HIWORD((unsigned long)lpIconName) != 0)
    {
        return NULL;
    }
    return (HICON)lpIconName;
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL DestroyIcon(HICON hIcon)
{
    return TRUE;
}

//*******************************************************************
// Return bitmap id
//*******************************************************************

HBITMAP LoadBitmap(HINSTANCE hInstance, LPCSTR lpBitmapName)
{
    if (HIWORD((unsigned long)lpBitmapName) != 0)
    {
        return NULL;
    }
    return (HBITMAP)lpBitmapName;
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL DeleteObject(void *ho)
{
    return TRUE;
}

//*******************************************************************
// Return window handle
//*******************************************************************

HDC BeginPaint(HWND hWnd, LPPAINTSTRUCT lpPaint)
{
    return (HDC)hWnd;
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL EndPaint(HWND hWnd, const PAINTSTRUCT *lpPaint)
{
    return TRUE;
}

//*******************************************************************
// No nothing
//*******************************************************************

BOOL SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
    return TRUE;
}

//*******************************************************************
// No nothing
//*******************************************************************

LRESULT DefWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

//*******************************************************************
// Show shutdown screen
//*******************************************************************

void ExitWindows(int dwReserved, int code)
{
    EM_ASM(shutdown());
}

HWND GetDlgItem(HWND DhDlg, int nIDDlgItem)
{
    return NULL; // TODO
}

HWND SetFocus(HWND hWnd)
{
    return NULL; // TODO
}

//*******************************************************************
// Set the random number generator seed
//*******************************************************************

void randomize()
{
    time_t t;
    srand((unsigned)time(&t));
}

static BOOL is_open = FALSE;

// Start Tabboz Simulator
int start_zarrosim()
{
    is_open = TRUE;
    HANDLE_ENTRY *handle = AllocateHandle();
    WinMain((HANDLE)handle, NULL, "", 0);
    ReleaseHandle(handle);
    is_open = FALSE;
    return 0;
}

// Icon click callback
int zarrosim_icon_cb(int eventType, const struct EmscriptenMouseEvent *someEvent, void *userData)
{
    if (is_open)
    {
        return 0;
    }
    else
    {
        return start_zarrosim();
    }
}

int main()
{
    // Load string resources
    EM_ASM(loadStringResources());
    // Preload images
    EM_ASM(preload());
    // Register icon handler
    emscripten_set_click_callback("#zarrosim_icon", NULL, TRUE, &zarrosim_icon_cb);
    return 0;
    /* return start_zarrosim(); */
}

#endif
