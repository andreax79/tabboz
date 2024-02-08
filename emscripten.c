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
// Load string resources
//*******************************************************************

EM_ASYNC_JS(void, LoadStringResourcesEm, (), {
    const response = await fetch("resources/strings/strings.json");
    window.strings = await response.json();
});

void LoadStringResources(void)
{
    LoadStringResourcesEm();
}

//*******************************************************************
// Load a string resource into a buffer
//*******************************************************************

EM_ASYNC_JS(int, LoadStringEm, (HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax), {
    const value = window.strings[uID] || "";
    stringToUTF8(value, lpBuffer, cchBufferMax);
    return value.length;
});

int LoadString(HINSTANCE hInstance, UINT uID, LPSTR lpBuffer, int cchBufferMax)
{
    return LoadStringEm(hInstance, uID, lpBuffer, cchBufferMax);
}

//*******************************************************************
// Show/hide a window
//*******************************************************************

EM_JS(BOOL, ShowWindowEm, (int windowId, BOOL show), {
    const win = document.querySelector('#win' + windowId);
    if (win != null)
    {
        win.style.display = show ? 'block' : 'none';
        return true;
    }
    else
    {
        return false;
    }
});

BOOL ShowWindow(HWND hWnd, int nCmdShow)
{
    struct handle_entry *handle = (struct handle_entry *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return ShowWindowEm(handle->id, nCmdShow && SW_SHOWNORMAL);
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
    let result = await waitListener(windowId);
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
    int                  result;
    int                  parentWindowId = -1;
    struct handle_entry *handle = alloc_handle();

    // Get parent window number
    if (hWnd != NULL)
    {
        parentWindowId = ((struct handle_entry *)hWnd)->id;
    }

    result = MessageBoxEm(handle->id, lpText, lpCaption, uType, parentWindowId);
    release_handle(handle);
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
    INT_PTR              retval;
    int                  controlId;
    int                  dialog = (int)lpTemplateName;
    int                  parentWindowId = -1;
    struct handle_entry *handle = alloc_handle();
    MSG                  msg = {.hwnd = (HWND)handle};

    if (lpDialogFunc == NULL)
    {
        return 0;
    }
    handle->lpDialogFunc = lpDialogFunc;
    // Get parent window number
    if (hWndParent != NULL)
    {
        parentWindowId = ((struct handle_entry *)hWndParent)->id;
    }
    // Show dialog
    DialogBoxEm(handle->id, dialog, parentWindowId);
    // Init
    lpDialogFunc((HWND)handle, WM_INITDIALOG, 0, 0);
    // Message loop
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        printf("%d\n", msg.wParam);
        DispatchMessage(&msg);
    }
    retval = handle->retval;
    RemoveDialogBoxEm(handle->id);
    release_handle(handle);
    return retval;
}

//*******************************************************************
// Destroy a modal dialog box
//*******************************************************************

BOOL EndDialog(HWND hWnd, INT_PTR retval)
{
    struct handle_entry *handle = (struct handle_entry *)hWnd;
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

EM_JS(BOOL, SetDlgItemTextEm, (int windowId, int nIDDlgItem, LPCSTR lpString), {
    let control = document.querySelector('#win' + windowId + ' .control' + nIDDlgItem);
    if (control == null)
    {
        return false;
    }
    else if (control.tagName == "INPUT")
    {
        if (control.type == "radio")
        {
            // Set radio label
            const label = control.parentNode.querySelector("label");
            if (label != null)
            {
                label.innerText = UTF8ToString(lpString);
            }
        }
        else
        {
            // Set text
            control.value = UTF8ToString(lpString);
        }
    }
    else
    {
        control.innerText = UTF8ToString(lpString);
    }
    return true;
});

BOOL SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCSTR lpString)
{
    struct handle_entry *handle = (struct handle_entry *)hDlg;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return SetDlgItemTextEm(handle->id, nIDDlgItem, lpString);
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
    struct handle_entry *handle = (struct handle_entry *)hDlg;
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

EM_JS(int, GetSystemMetricsEM, (int nIndex), {
    switch (nIndex)
    {
    case 0: // SM_CXSCREEN
        return parseInt(getComputedStyle(document.getElementById('screen')).width);
    case 1: // SM_CYSCREEN
        return parseInt(getComputedStyle(document.getElementById('screen')).height);
    default:
        return 0;
    }
    return 0;
});

int GetSystemMetrics(int nIndex)
{
    return GetSystemMetricsEM(nIndex);
}

//*******************************************************************
// Retrieve the dimensions of a specified window
//*******************************************************************

EM_JS(BOOL, GetWindowRectEm, (int windowId, int dimension), {
    const win = document.querySelector('#win' + windowId);
    if (win == null)
    {
        return 0;
    }
    const style = getComputedStyle(win);
    switch (dimension)
    {
    case 0: // left
        return parseInt(style.left);
    case 1: // top
        return parseInt(style.top);
    case 2: // right
        return parseInt(style.left) + parseInt(style.width);
    case 3: // bottom
        return parseInt(style.top) + parseInt(style.height);
    default:
        return 0;
    }
});

BOOL GetWindowRect(HWND hWnd, LPRECT lpRect)
{
    struct handle_entry *handle = (struct handle_entry *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    else
    {
        lpRect->left = GetWindowRectEm(handle->id, 0);
        lpRect->top = GetWindowRectEm(handle->id, 1);
        lpRect->right = GetWindowRectEm(handle->id, 2);
        lpRect->bottom = GetWindowRectEm(handle->id, 3);
        return TRUE;
    }
}

//*******************************************************************
// Change the position and dimensions of the specified window
//*******************************************************************

EM_JS(BOOL, MoveWindowEM, (int windowId, int X, int Y, int nWidth, int nHeight), {
    const win = document.querySelector('#win' + windowId);
    if (win == null)
    {
        return false;
    }
    else
    {
        win.style.left = X + 'px';
        win.style.top = Y + 'px';
        win.style.width = nWidth + 'px';
        win.style.height = nHeight + 'px';
        return true;
    }
});

BOOL MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
{
    struct handle_entry *handle = (struct handle_entry *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return MoveWindowEM(handle->id, X, Y, nWidth, nHeight);
}

//*******************************************************************
// Retrieve the title or text associated with a control in a dialog box
//*******************************************************************

EM_JS(BOOL, GetDlgItemTextEM, (int windowId, int nIDDlgItem, LPCSTR lpString, int nMaxCount), {
    let control = document.querySelector('#win' + windowId + ' input.control' + nIDDlgItem);
    if (control != null)
    {
        stringToUTF8(control.value, lpString, nMaxCount);
        return control.value.length;
    }
    control = document.querySelector('#win' + windowId + ' .control' + nIDDlgItem);
    if (control != null)
    {
        stringToUTF8(control.innerText, lpString, nMaxCount);
        return control.innerText.length;
    }
    else
    {
        return 0;
    }
});

UINT GetDlgItemText(HWND hDlg, int nIDDlgItem, LPSTR lpString, int nMaxCount)
{
    struct handle_entry *handle = (struct handle_entry *)hDlg;
    if (handle == NULL)
    {
        // Invalid window handle
        return 0;
    }
    return GetDlgItemTextEM(handle->id, nIDDlgItem, lpString, nMaxCount);
}

//*******************************************************************
// Get an entry from the property list of the specified window
//*******************************************************************

HANDLE GetProp(HWND hWnd, LPCSTR lpString)
{
    struct handle_entry *handle = (struct handle_entry *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return properties_get(handle->props, lpString);
}

//*******************************************************************
// Add/change an entry in the property list of the specified window
//*******************************************************************

BOOL SetProp(HWND hWnd, LPCSTR lpString, HANDLE hData)
{
    struct handle_entry *handle = (struct handle_entry *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    properties_set(handle->props, lpString, hData);
    return TRUE;
}

//*******************************************************************
// Remove an entry from the property list of the specified window
//*******************************************************************

HANDLE RemoveProp(HWND hWnd, LPCSTR lpString)
{
    struct handle_entry *handle = (struct handle_entry *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return properties_remove(handle->props, lpString);
}

//*******************************************************************
// Set the random number generator seed
//*******************************************************************

extern void randomize()
{
    time_t t;
    srand((unsigned)time(&t));
}

//*******************************************************************
// Retrieve a message
//*******************************************************************

EM_ASYNC_JS(int, GetMessageEM, (int windowId), {
    // Activate the window
    setActiveWindow(windowId);
    // Wait for message
    return await waitListener(windowId);
});

BOOL GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    if (hWnd != NULL)
    {
        lpMsg->hwnd = hWnd;
    }
    if (((struct handle_entry *)lpMsg->hwnd)->end)
    {
        return FALSE;
    }
    lpMsg->wParam = GetMessageEM(((struct handle_entry *)lpMsg->hwnd)->id);
    return TRUE;
}

//*******************************************************************
// Dispatch a message to a window procedure
//*******************************************************************

LRESULT DispatchMessage(const MSG *lpMsg)
{
    DLGPROC lpDialogFunc = ((struct handle_entry *)lpMsg->hwnd)->lpDialogFunc;
    if (lpMsg->wParam == SC_CLOSE)
    {
        if (!lpDialogFunc(lpMsg->hwnd, WM_SYSCOMMAND, lpMsg->wParam, 0))
        {
            EndDialog(lpMsg->hwnd, TRUE);
            // Repaint Destroy
            lpDialogFunc(lpMsg->hwnd, WM_DESTROY, lpMsg->wParam, 0);
        }
    }
    else
    {
        // Dispatch Commmand
        lpDialogFunc(lpMsg->hwnd, WM_COMMAND, lpMsg->wParam, 0);
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

HWND GetDlgItem(HWND DhDlg, int nIDDlgItem)
{
    return NULL; // TODO
}

HWND SetFocus(HWND hWnd)
{
    return NULL; // TODO
}

void ExitWindows(int dwReserved, int code)
{
    // TODO
}

// Start Tabboz Simulator
int start_zarrosim()
{
    struct handle_entry *handle = alloc_handle();
    WinMain((HANDLE)handle, NULL, "", 0);
    release_handle(handle);
    return 0;
}

// Icon click callback
int zarrosim_icon_cb(int eventType, const struct EmscriptenMouseEvent *someEvent, void *userData)
{
    return start_zarrosim();
}

int main()
{
    emscripten_set_click_callback("#zarrosim_icon", NULL, TRUE, &zarrosim_icon_cb);
    return 0;
    /* return start_zarrosim(); */
}

#endif
