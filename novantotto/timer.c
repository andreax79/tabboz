/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <emscripten/html5.h>
#include "novantotto.h"
#include "debug.h"

//*******************************************************************
// Timer callback
//*******************************************************************

static void TimerCb(TIMER *timer)
{
    DEBUG_PRINTF("Timer callback %ld\n", timer->timeoutId);
    if (timer->lpTimerFunc != NULL)
    {
        // Call timer function
        timer->lpTimerFunc(timer->hwnd, WM_TIMER, timer->nIDEvent, 0);
    }
    else
    {
        // Dispatch WM_TIMER
        PostMessage(timer->hwnd, WM_TIMER, timer->nIDEvent, 0);
        // Stop waiting for an event
        JS_CALL("stopWaiting");
    }
}

//*******************************************************************
// Creates a timer
//*******************************************************************

UINT_PTR SetTimer(HWND hWnd, UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc)
{
    DEBUG_PRINTF("SetTimer %d\n", nIDEvent);
    KillTimer(hWnd, nIDEvent);
    char timerPropertyName[20];
    snprintf(timerPropertyName, sizeof(timerPropertyName), "timer%d", nIDEvent);
    TIMER *timer = malloc(sizeof(TIMER));
    timer->hwnd = hWnd;
    timer->nIDEvent = nIDEvent;
    timer->lpTimerFunc = lpTimerFunc;
    timer->timeoutId = emscripten_set_timeout((void (*)(void *))TimerCb, uElapse, timer);
    return TRUE;
}

//*******************************************************************
// Destroy the specified timer
//*******************************************************************

BOOL KillTimer(HWND hWnd, UINT_PTR uIDEvent)
{
    char timerPropertyName[20];
    snprintf(timerPropertyName, sizeof(timerPropertyName), "timer%d", uIDEvent);
    TIMER *timer = (TIMER *)GetProp(hWnd, timerPropertyName);
    if (timer != NULL)
    {
        DEBUG_PRINTF("KillTimer %d\n", uIDEvent);
        emscripten_clear_timeout(timer->timeoutId);
        free(timer);
        RemoveProp(hWnd, timerPropertyName);
    }
    return TRUE;
}
