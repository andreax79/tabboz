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

#define MIN_HANDLE_ENTRIES 32

struct handle_entry
{
    unsigned int refcount;
    unsigned int id;
    BOOL         end;
    INT_PTR      retval; // the value to be returned from the function that created the dialog box
};

struct handle_table
{
    int                  count;
    struct handle_entry *entries;
};

static struct handle_table *global_table;

/* allocate a new handle table */
struct handle_table *alloc_handle_table(int count)
{
    struct handle_table *table;

    if (count < MIN_HANDLE_ENTRIES)
    {
        count = MIN_HANDLE_ENTRIES;
    }
    if (!(table = malloc(sizeof(struct handle_table))))
    {
        return NULL;
    }
    table->count = count;
    if ((table->entries = malloc(count * sizeof(*table->entries))))
    {
        bzero(table->entries, count * sizeof(*table->entries));
        return table;
    }
    else
    {
        free(table);
        return NULL;
    }
}

struct handle_entry *alloc_handle()
{
    int i;
    if (!global_table)
    {
        if (!(global_table = alloc_handle_table(0)))
            return NULL;
    }
    for (i = 0; i < global_table->count; i++)
    {
        struct handle_entry *h = &global_table->entries[i];
        if (h->refcount == 0)
        {
            h->refcount++;
            h->id = i + 1;
            h->end = FALSE;
            return (HANDLE)h;
        }
    }
    return NULL;
}

void release_handle(HANDLE p)
{
    struct handle_entry *h = (struct handle_entry *)p;
    if (h != NULL)
    {
        if (h->refcount > 0)
        {
            h->refcount--;
            if (h->refcount == 0)
            {
                h->id = 0;
            }
        }
    }
}

struct handle_entry *get_handle(int index)
{
    struct handle_entry *h;
    if (!global_table || index < 1 || index > global_table->count)
    {
        return NULL;
    }
    h = &global_table->entries[index - 1];
    if (h->refcount > 0)
    {
        return h;
    }
    else
    {
        return h; // TODO - NULL
    }
}

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
    const c = element.cloneNode(true);
    // Set window id
    c.id = 'win' + windowId;
    // Set window position
    if (parentWindowId >= 0) {
        const parent = document.getElementById('win' + parentWindowId);
        if (parent != null) {
            const style = getComputedStyle(parent);
            c.style.left = (parseInt(style.left) + 40) + 'px';
            c.style.top = (parseInt(style.top) + 40) + 'px';
        }
    }
    c.id = 'win' + windowId;
    c.style.zIndex = '' + windowId;
    c.style.position = 'absolute';
    // Set title and message
    c.querySelector('.title-bar-text').innerText = UTF8ToString(lpCaption);
    c.querySelector('.content').innerText = UTF8ToString(lpText);
    // Add window to screen
    const destination = document.getElementById('screen');
    destination.appendChild(c);
    // Make the window draggrable
    makeDraggable(c);
    const result = await waitListenerS(`#win${windowId} button, #win${windowId} input, #win${windowId} .menu, #win${windowId} img`, "click");
    // Remove the window
    destination.removeChild(c);
    return result;
});

int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    int                  result;
    int                  parentWindowId = -1;
    struct handle_entry *handle = alloc_handle();

    // Get parent window number
    if (hWnd != NULL) {
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
    const html = await response.text();
    const c = createElementFromHTML(html);
    // Set window id
    c.id = 'win' + windowId;
    // Set window position
    if (parentWindowId >= 0) {
        const parent = document.getElementById('win' + parentWindowId);
        if (parent != null) {
            const style = getComputedStyle(parent);
            c.style.left = (parseInt(style.left) + 40) + 'px';
            c.style.top = (parseInt(style.top) + 40) + 'px';
        }
    }
    // Add window to screen
    const destination = document.getElementById('screen');
    destination.appendChild(c);
    // Add menu
    addMainMenu(c);
    // Make the window draggrable
    makeDraggable(c);
});

EM_ASYNC_JS(int, DialogBoxWaitEvent, (int windowId), {
    return await waitListenerS(`#win${windowId} button, #win${windowId} input, #win${windowId} .menu, #win${windowId} img`, "click");
});

EM_JS(void, RemoveDialogBoxEm, (int windowId), {
    const destination = document.getElementById('screen');
    destination.removeChild(document.getElementById('win' + windowId));
});

INT_PTR DialogBox(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc)
{
    INT_PTR              retval;
    int                  controlId;
    int                  dialog = (int)lpTemplateName;
    int                  parentWindowId = -1;
    struct handle_entry *handle = alloc_handle();

    // Get parent window number
    if (hWndParent != NULL) {
        parentWindowId = ((struct handle_entry *)hWndParent)->id;
    }

    DialogBoxEm(handle->id, dialog, parentWindowId);

    if (lpDialogFunc != NULL)
    {
        // Init
        lpDialogFunc((HWND)handle, WM_INITDIALOG, 0, 0);
        // Loop
        while (!handle->end)
        {
            controlId = DialogBoxWaitEvent(handle->id);
            printf("%d\n", controlId);
            if (controlId == SC_CLOSE)
            {
                if (!lpDialogFunc((HWND)handle, WM_SYSCOMMAND, controlId, 0))
                {
                    EndDialog((HWND)handle, TRUE);
                }
            }
            else
            {
                lpDialogFunc((HWND)handle, WM_COMMAND, controlId, 0);
            }
        }
        retval = handle->retval;
    }
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
    const control = document.querySelector('#win' + windowId + ' .control' + nIDDlgItem);
    if (control != null)
    {
        control.innerText = UTF8ToString(lpString);
        return true;
    }
    else
    {
        return false;
    }
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

LRESULT SetCheck(HWND hDlg, int nIDDlgItem, WPARAM wParam) {
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
    switch (nIndex) {
        case 0: // SM_CXSCREEN
            return parseInt(getComputedStyle(document.getElementById('screen')).width);
        case 1: // SM_CYSCREEN
            return parseInt(getComputedStyle(document.getElementById('screen')).height);
        default:
            return 0;
    }
    const control = document.querySelector('#win' + windowId + ' .control' + nIDDlgItem);
    if (control != null)
    {
        control.checked = (wParam != 0);
    }
    return 0;
});

int GetSystemMetrics(int nIndex) {
    return GetSystemMetricsEM(nIndex);
}

//*******************************************************************
// Retrieve the dimensions of a specified window
//*******************************************************************

EM_JS(BOOL, GetWindowRectEm, (int windowId, int dimension), {
    const win = document.querySelector('#win' + windowId);
    if (win == null) {
        return 0;
    }
    const style = getComputedStyle(win);
    switch(dimension) {
        case 0: // left
            return style.left;
        case 1: // top
            return style.top;
        case 2: // right
            return style.left + style.width;
        case 3: // bottom
            return style.top + style.height;
        default:
            return 0;
    }
});

BOOL GetWindowRect(HWND hWnd, LPRECT lpRect) {
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

BOOL MoveWindow(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint) {
    struct handle_entry *handle = (struct handle_entry *)hWnd;
    if (handle == NULL)
    {
        // Invalid window handle
        return FALSE;
    }
    return MoveWindowEM(handle->id, X, Y, nWidth, nHeight);
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

UINT GetDlgItemText(HWND hDlg, int nIDDlgItem, LPSTR lpString, int nMaxCount)
{
    return 0; // TODO
}

extern BOOL FAR PASCAL TabbozWndProc(HWND hWnd, WORD message, WORD wParam, LONG lParam);

int main()
{
    /* printf("Hello Tabboz %s\n", MostraSoldi(10)); */
    /* char City[100]; */

    /* LoadStringResources(); */
    /* LoadString(NULL, (400 + random(22) ), City, (sizeof(City)-1)); */
    /* printf("%s\n", City); */
    /**/
    TabbozAddKey("Debug", "1");
    /* Inizializza il programma */
    InitTabboz();

    /* Finestra principale */
    DialogBox(hInst, MAKEINTRESOURCE(1), NULL, (DLGPROC)TabbozWndProc);
    //    MessageBox( NULL, "Test", "Test", MB_OK | MB_ICONINFORMATION);

    /*     MessageBox( NULL, */
    /* "Il biglietto e' valido solo dopo la convalida.Il biglietto deve essere conservato per tutta la durata \ */
    /* del viaggio. Il diritto a viaggiare cessa al termine della tratta corrispondente al valore del biglietto. \ */
    /* Il passeggero che al controllo non fosse in grado di presentare il biglietto o lo presentasse irriconoscibile, \ */
    /* o comunque non valido, verra' abbattuto. La notifica del decesso verra' inviata ai parenti solo previo pagamento \ */
    /* delle spese postali.", "Norme di utilizzo", MB_OK | MB_ICONINFORMATION); */

    /* printf("--test--\n"); */
    /* TabbozAddKey("ciao","RTest RTest 2 1234567890 ABCD @@@"); */
    /* printf("%s\n",RRKey("ciao")); */
    /**/
    /* printf("%s\n",RRKey("TestKey")); */
    /* TabbozAddKey("TestKey","RTest RTest 2 1234567890 ABCD @@@"); */
    /* printf("%s\n",RRKey("TestKey")); */
    /* TabbozAddKey("TestKey","short key..."); */
    /* printf("%s\n",RRKey("TestKey")); */
    /* printf("%s\n",RRKey("NoKey")); */
    /* printf("%s\n",RRKey("Nome")); */
    /* printf("%s\n",RRKey("Cognome")); */
    /* printf("--end--\n"); */

    printf("Done\n");
    return 0;
}

#endif
