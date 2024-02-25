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

#define SetCheck(hDlg, nIDDlgItem, wParam) SendMessage(GetDlgItem(hDlg, nIDDlgItem), BM_SETCHECK, wParam, 0L)
#define ComboBoxAddString(hDlg, nIDDlgItem, lParam) SendMessage(GetDlgItem(hDlg, nIDDlgItem), CB_ADDSTRING, (WPARAM)0, (LPARAM)lParam)
#define ComboBoxSelect(hDlg, nIDDlgItem, wParam) SendMessage(GetDlgItem(hDlg, nIDDlgItem), CB_SETCURSEL, (WPARAM)wParam, (LPARAM)0)

#ifndef OFN_LONGNAMES
#define OFN_LONGNAMES 0x00200000L
#endif
#endif

// Windows 32bit -----------------------------------------------------

#ifdef TABBOZ_WIN32
#define TABBOZ_WIN
#include <windows.h>

#define SetCheck(hDlg, nIDDlgItem, wParam) SendMessage(GetDlgItem(hDlg, nIDDlgItem), BM_SETCHECK, wParam, 0L)
#define ComboBoxAddString(hDlg, nIDDlgItem, lParam) SendMessage(GetDlgItem(hDlg, nIDDlgItem), CB_ADDSTRING, (WPARAM)0, (LPARAM)lParam)
#define ComboBoxSelect(hDlg, nIDDlgItem, wParam) SendMessage(GetDlgItem(hDlg, nIDDlgItem), CB_SETCURSEL, (WPARAM)wParam, (LPARAM)0)

#ifndef OFN_LONGNAMES
#define OFN_LONGNAMES 0x00200000L
#endif
#endif

// Emscripten --------------------------------------------------------

#ifdef TABBOZ_EM
#include <emscripten.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include "windefs.h"

#define _JS_ARG_0 ""
#define _JS_ARG_1 "$0"
#define _JS_ARG_2 "$0,$1"
#define _JS_ARG_3 "$0,$1,$2"
#define _JS_ARG_4 "$0,$1,$2,$3"
#define _JS_ARG_5 "$0,$1,$2,$3,$4"
#define _JS_ARG_6 "$0,$1,$2,$3,$4,$5"
#define _JS_ARG_7 "$0,$1,$2,$3,$4,$5,$6"
#define _JS_ARG_8 "$0,$1,$2,$3,$4,$5,$6,$7"
#define _JS_ARG_9 "$0,$1,$2,$3,$4,$5,$6,$7,$8"
#define _JS_ARG_10 "$0,$1,$2,$3,$4,$5,$6,$7,$8,$9"
#define _JS_ARG(...) _EM_ASM_CONCATENATE(_JS_ARG_, _EM_ASM_COUNT_ARGS(__VA_ARGS__))

#define JS_CALL_INT(code, ...) emscripten_asm_const_int(CODE_EXPR("return " code "(" _JS_ARG(__VA_ARGS__) ")") _EM_ASM_PREP_ARGS(__VA_ARGS__))
#define JS_ASYNC_CALL_INT(code, ...) emscripten_asm_const_int(CODE_EXPR("{return Asyncify.handleAsync(function(){ return " code "(" _JS_ARG(__VA_ARGS__) ")})}") _EM_ASM_PREP_ARGS(__VA_ARGS__))
#define JS_CALL(code, ...) (void)JS_CALL_INT(code, __VA_ARGS__)
#define JS_ASYNC_CALL(code, ...) (void)JS_ASYNC_CALL_INT(code, __VA_ARGS__)
#endif
