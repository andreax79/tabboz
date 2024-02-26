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

#ifndef OFN_LONGNAMES
#define OFN_LONGNAMES 0x00200000L
#endif
#endif

// Windows 32bit -----------------------------------------------------

#ifdef TABBOZ_WIN32
#define TABBOZ_WIN
#include <windows.h>

#ifndef OFN_LONGNAMES
#define OFN_LONGNAMES 0x00200000L
#endif
#endif

// Emscripten --------------------------------------------------------

#ifdef TABBOZ_EM
#include "novantotto/novantotto.h"
#endif
