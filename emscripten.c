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

EM_JS(BOOL, ShowWindowEm, (int dialogId, BOOL show), {
    const win = document.querySelector('#win' + dialogId);
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

EM_ASYNC_JS(int, MessageBoxEm, (int dialogId, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType), {
    const element = document.getElementById('messagebox');
    const win = element.cloneNode(true);
    win.id = 'win' + dialogId;
    win.style.zIndex = '' + dialogId;
    win.style.position = 'absolute';
    win.querySelector('.title-bar-text').innerText = UTF8ToString(lpCaption);
    win.querySelector('.content').innerText = UTF8ToString(lpText);
    const destination = document.getElementById('screen');
    destination.appendChild(win);
    await waitListener(win.querySelector("button"), "click");
    destination.removeChild(win);
    return 0; // TODO
});

int MessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    int                  result;
    struct handle_entry *handle = alloc_handle();
    result = MessageBoxEm(handle->id, lpText, lpCaption, uType);
    release_handle(handle);
    return result;
}

//*******************************************************************
// Create a modal dialog box from a dialog box template resource
//*******************************************************************

EM_ASYNC_JS(void, DialogBoxEm, (int dialogId, int dialog), {
    const response = await fetch("resources/dialogs/includes/" + dialog + ".inc.html");
    const html = await response.text();
    const c = fromHTML(html);
    c.id = 'win' + dialogId;
    const destination = document.getElementById('screen');
    destination.appendChild(c);
    // Add menu
    addMainMenu(c);
    // Make the window draggrable
    makeDraggable(c);
});

EM_ASYNC_JS(int, DialogBoxWaitEvent, (int dialogId), {
    return await waitListenerS(`#win${dialogId} button, #win${dialogId} input, #win${dialogId} .menu`, "click");
});

EM_JS(void, RemoveDialogBoxEm, (int dialogId), {
    const destination = document.getElementById('screen');
    destination.removeChild(document.getElementById('win' + dialogId));
});

INT_PTR DialogBox(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc)
{
    INT_PTR              retval;
    int                  controlId;
    int                  dialog = (int)lpTemplateName;
    struct handle_entry *handle = alloc_handle();
    DialogBoxEm(handle->id, dialog);
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

EM_JS(BOOL, SetDlgItemTextEm, (int dialogId, int nIDDlgItem, LPCSTR lpString), {
    const control = document.querySelector('#win' + dialogId + ' .control' + nIDDlgItem);
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

LRESULT SendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    // SendMessage(GetDlgItem(hDlg, 110), BM_SETCHECK, TRUE, 0L); /* Seleziona agraria */
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
