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

#include <string.h>

#include "zarrosim.h"
#ifdef TABBOZ_EM

static MESAGES_QUEUE *queue = NULL;

static int next_idx(int idx)
{
    return (idx + 1) % queue->capacity;
}

//*******************************************************************
// Find next message index
//*******************************************************************

int FindMessage(HWND hwnd)
{
    // Check if empty
    if (queue->count == 0)
    {
        return -1;
    }

    // If hwnd is NULL, return the next message
    if (hwnd == NULL)
    {
        return queue->next;
    }

    for (int i = 0, idx = queue->next; i < queue->count; i++, idx = next_idx(idx))
    {
        // Get the message for the given hwnd
        if (queue->messages[idx].hwnd == hwnd)
        {
            return idx;
        }
    }
    return -1; // not found
}

//*******************************************************************
// Remove a message from the queue (pos must be a valid position).
//*******************************************************************

void DelMessage(int idx)
{
    if (idx >= queue->next)
    {
        int count = idx - queue->next;
        do
        {
            memmove(&queue->messages[next_idx(queue->next)],
                    &queue->messages[queue->next],
                    count * sizeof(MSG));
            queue->next = next_idx(queue->next);
            count--;
        } while (count >= 0);
    }
    else
    {
        int count = queue->free - idx;
        do
        {
            memmove(&queue->messages[idx],
                    &queue->messages[next_idx(idx)],
                    count * sizeof(MSG));
            queue->free = idx;
            idx = next_idx(idx);
            count--;
        } while (count >= 0);
    }
    queue->count--;
}

//*******************************************************************
// Retrieve the message if any exist
//*******************************************************************

BOOL PeekMessage(LPMSG msg, HWND hwnd, WORD wMsgFilterMin, WORD wMsgFilterMax, BOOL wRemoveMsg)
{
    int idx = FindMessage(hwnd);
    if (idx == -1)
    {
        return FALSE;
    }
    else
    {
        memcpy(msg, &queue->messages[idx], sizeof(MSG));
        // If the PM_REMOVE flag is set, remove the message from the queue
        if (wRemoveMsg & PM_REMOVE)
        {
            DelMessage(idx);
        }
        return TRUE;
    }
}

//*******************************************************************
// Retrieve a message, wait until a message is available for retrieval
//*******************************************************************

BOOL GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    int32_t message, wParam, lParam;

    // Retrieve the message if any exist
    if (PeekMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, PM_REMOVE))
    {
        // Return FALSE is the message is WM_QUIT
        return (lpMsg->message != WM_QUIT);
    }

    // Activate the window
    JS_CALL("setActiveWindow", ((HANDLE_ENTRY *)hWnd)->id);

    // Wait for a message
    JS_ASYNC_CALL_INT("waitListener", ((HANDLE_ENTRY *)hWnd)->id, &message, &wParam, &lParam);
    lpMsg->hwnd = hWnd;
    lpMsg->message = message;
    lpMsg->wParam = wParam;
    lpMsg->lParam = lParam;
    return TRUE;
}

//*******************************************************************
// Post a message in the message queue
//*******************************************************************

BOOL PostMessage(HWND hWnd, WORD Msg, WORD wParam, LONG lParam)
{

    if ((queue->next == queue->free) && (queue->count > 0))
    {
        return FALSE; // the queue is full
    }
    else
    {
        MSG msg = {
            .hwnd = hWnd,
            .message = Msg,
            .wParam = wParam,
            .lParam = lParam,
            .pt.x = 0,
            .pt.y = 0,
        };
        int idx = queue->free;
        queue->messages[idx] = msg;
        queue->free = next_idx(idx);
        queue->count++;
        return TRUE;
    }
}

//*******************************************************************
// Dispatch a message to a window procedure
//*******************************************************************

LRESULT DispatchMessage(const MSG *lpMsg)
{
    LRESULT retval = 0;
    DLGPROC lpDialogFunc = ((HANDLE_ENTRY *)lpMsg->hwnd)->lpDialogFunc;
    if (lpMsg->message == WM_SETICON)
    {
        // Set window icon
        HANDLE_ENTRY *handle = (HANDLE_ENTRY *)lpMsg->hwnd;
        if (handle != NULL)
        {
            JS_CALL("setIcon", handle->id, (int)lpMsg->lParam);
        }
    }
    else if (lpMsg->wParam == SC_CLOSE)
    {
        // Close window
        if (!lpDialogFunc(lpMsg->hwnd, WM_SYSCOMMAND, lpMsg->wParam, 0))
        {
            EndDialog(lpMsg->hwnd, TRUE);
            // Dispatch Destroy
            lpDialogFunc(lpMsg->hwnd, WM_DESTROY, lpMsg->wParam, 0);
        }
    }
    else if (lpMsg->wParam == SC_MINIMIZE)
    {
        // Minimize the app (hide all the windows)
        JS_CALL_INT("showApp", 0);
    }
    else
    {
        // Dispatch Commmand
        retval = lpDialogFunc(lpMsg->hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
        // Dispatch Repaint
        lpDialogFunc(lpMsg->hwnd, WM_PAINT, lpMsg->wParam, 0);
    }
    return retval;
}

//*******************************************************************
// Call the window procedure and wait until the procedure has finished
//*******************************************************************

LRESULT SendMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    MSG msg = {
        .hwnd = hWnd,
        .message = Msg,
        .wParam = wParam,
        .lParam = lParam,
        .pt.x = 0,
        .pt.y = 0,
    };
    return DispatchMessage(&msg);
}

//*******************************************************************
// Allocate the message queue
//*******************************************************************

BOOL SetMessageQueue(int size)
{
    if (queue)
    {
        return TRUE;
    }
    else if (!(queue = (MESAGES_QUEUE *)malloc(sizeof(MESAGES_QUEUE) + size * sizeof(MSG))))
    {
        return FALSE;
    }
    else
    {
        queue->count = 0;
        queue->next = 0;
        queue->free = 0;
        queue->capacity = size;
        return TRUE;
    }
}

#endif
