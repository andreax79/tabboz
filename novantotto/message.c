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

#include <string.h>
#include <stdio.h>
#include "novantotto.h"
#include "handler.h"
#include "debug.h"

static MESAGES_QUEUE *queue = NULL;

static int next_idx(int idx)
{
    return (idx + 1) % queue->capacity;
}

static int prev_idx(int idx)
{
    return idx > 0 ? idx - 1 : queue->capacity - 1;
}

static inline void RequireQueue()
{
    if (queue == NULL)
    {
        SetMessageQueue(MESSAGE_LIMIT);
    }
}

//*******************************************************************
// Find next message index
//*******************************************************************

int FindMessage(HWND hwnd)
{
    RequireQueue();

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
    RequireQueue();
    DEBUG_PRINTF("DelMessage %d next=%d\n", idx, queue->next);

    if (idx >= queue->next)
    {
        int count = idx - queue->next;
        if (count)
        {
            memmove(&queue->messages[queue->next + 1],
                    &queue->messages[queue->next],
                    count * sizeof(MSG));
        }
        queue->next = next_idx(queue->next);
    }
    else
    {
        int count = queue->free - idx;
        if (count)
        {
            memmove(&queue->messages[idx],
                    &queue->messages[idx + 1],
                    count * sizeof(MSG));
        }
        queue->free = prev_idx(queue->free);
    }
    queue->count--;
    DEBUG_PRINTF("DelMessage done - count=%d free=%d\n", queue->count, queue->free);
}

//*******************************************************************
// Retrieve the message if any exist
//*******************************************************************

BOOL PeekMessage(LPMSG msg, HWND hwnd, WORD wMsgFilterMin, WORD wMsgFilterMax, BOOL wRemoveMsg)
{
    RequireQueue();
    DEBUG_PRINTF("PeekMessage hwnd=%ld\n", (long)hwnd);

    int idx = FindMessage(hwnd);
    if (idx == -1)
    {
        DEBUG_PRINTF("PeekMessage not found\n");
        return FALSE;
    }
    else
    {
        DEBUG_PRINTF("PeekMessage found - idx=%d\n", idx);
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
    RequireQueue();
    DEBUG_PRINTF("GetMessage\n");

    // Activate the window
    DEBUG_PRINTF("setActiveWindow %d\n", (int)hWnd);
    JS_CALL("setActiveWindow", hWnd);

    while (TRUE)
    {
        // Retrieve the message if any exist
        if (PeekMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, PM_REMOVE))
        {
            // Return FALSE is the message is WM_QUIT
            DEBUG_PRINTF("GetMessage done (peek) - retval=%d\n", (lpMsg->message != WM_QUIT));
            return (lpMsg->message != WM_QUIT);
        }

        // Wait for an event
        DEBUG_PRINTF("waitEvent\n");
        JS_ASYNC_CALL("waitEvent");
        DEBUG_PRINTF("waitEvent - done\n");
    }
    return TRUE;
}

//*******************************************************************
// Post a message in the message queue
//*******************************************************************

BOOL PostMessage(HWND hWnd, WORD Msg, WORD wParam, LONG lParam)
{
    RequireQueue();
    DEBUG_PRINTF("PostMessage - hWnd: %ld Msg: %d wParam: %d lParam: %ld\n", (long)hWnd, Msg, wParam, lParam);
    if ((queue->next == queue->free) && (queue->count > 0))
    {
        DEBUG_PRINTF("PostMessage failed - the queue is full\n");
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
        memcpy(&queue->messages[idx], &msg, sizeof(MSG));
        queue->free = next_idx(idx);
        queue->count++;
        DEBUG_PRINTF("PostMessage - count=%d free=%d\n", queue->count, queue->free);
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
    if (lpDialogFunc == NULL)
    {
        DEBUG_PRINTF("DispatchMessage - lpDialogFunc is NULL\n");
        return 0;
    }
    // Dispatch to children
    if (lpMsg->message == WM_DESTROY || lpMsg->message == WM_CREATE)
    {
        DispatchToChildren(lpMsg->hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
    }
    if (lpMsg->message == WM_INITDIALOG)
    {
        DispatchToChildren(lpMsg->hwnd, WM_CREATE, lpMsg->wParam, lpMsg->lParam);
    }
    if (lpMsg->message == WM_SETICON)
    {
        // Set window icon
        DEBUG_PRINTF("DispatchMessage - WM_SETICON\n");
        HANDLE_ENTRY *handle = (HANDLE_ENTRY *)lpMsg->hwnd;
        if (handle != NULL)
        {
            JS_CALL("setIcon", handle, (int)lpMsg->lParam);
        }
    }
    else if (lpMsg->wParam == SC_CLOSE)
    {
        // Close window
        DEBUG_PRINTF("DispatchMessage - SC_CLOSE\n");
        if (!lpDialogFunc(lpMsg->hwnd, WM_SYSCOMMAND, lpMsg->wParam, 0))
        {
            EndDialog(lpMsg->hwnd, TRUE);
            // Dispatch destroy to children
            DispatchToChildren(lpMsg->hwnd, WM_DESTROY, lpMsg->wParam, 0);
            // Dispatch Destroy
            lpDialogFunc(lpMsg->hwnd, WM_DESTROY, lpMsg->wParam, 0);
        }
    }
    else if (lpMsg->wParam == SC_MINIMIZE)
    {
        // Minimize the app (hide all the windows)
        DEBUG_PRINTF("DispatchMessage - SC_MINIMIZE\n");
        JS_CALL_INT("showApp", 0);
    }
    else
    {
        // Dispatch Commmand
        DEBUG_PRINTF("DispatchMessage - Dispatch command hwnd=%ld message=%d wParam=%d lParam=%ld\n", (long)lpMsg->hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
        retval = lpDialogFunc(lpMsg->hwnd, lpMsg->message, lpMsg->wParam, lpMsg->lParam);
        // Dispatch Repaint
        DEBUG_PRINTF("DispatchMessage - WM_PAINT\n");
        /* lpDialogFunc(lpMsg->hwnd, WM_PAINT, lpMsg->wParam, 0); */
        RedrawWindow(lpMsg->hwnd, NULL, NULL, 0);
    }
    DEBUG_PRINTF("DispatchMessage - done retval=%ld\n", retval);
    return retval;
}

//*******************************************************************
// Call the window procedure and wait until the procedure has finished
//*******************************************************************

LRESULT SendMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DEBUG_PRINTF("SendMessage hwnd=%ld message=%d wParam=%d lParam=%ld\n", (long)hWnd, uMsg, wParam, lParam);
    MSG msg = {
        .hwnd = hWnd,
        .message = uMsg,
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
    DEBUG_PRINTF("SetMessageQueue %d malloc=%ld\n", size, sizeof(MESAGES_QUEUE) + size * sizeof(MSG));
    if (queue)
    {
        DEBUG_PRINTF("SetMessageQueue failed - already exists\n");
        return TRUE;
    }
    else if (!(queue = (MESAGES_QUEUE *)malloc(sizeof(MESAGES_QUEUE) + size * sizeof(MSG))))
    {
        DEBUG_PRINTF("SetMessageQueue failed - malloc\n");
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

//*******************************************************************
// Send WM_QUIT message
//*******************************************************************

void PostQuitMessage(int nExitCode)
{
    PostMessage(0, WM_QUIT, nExitCode, 0);
}

//*******************************************************************
// Print message queue
//*******************************************************************

void PrintMessages()
{
    RequireQueue();
    DEBUG_PRINTF("PrintMessages\n");
    printf("count: %d next: %d free: %d capacity: %d\n",
           queue->count, queue->next, queue->free, queue->capacity);
    for (int i = 0, idx = queue->next; i < queue->count; i++, idx = next_idx(idx))
    {
        LPMSG msg = &queue->messages[idx];
        printf("> %d idx: %d hwnd: %ld message: %d\n", i, idx, (long)msg->hwnd, msg->message);
    }
}
