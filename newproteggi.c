// Tabboz Simulator
// (C) Copyright 1998-1999 by Andrea Bonomi

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

// ------------------------------------------------------------------------------------------------
// #define TESTVERSION
// ------------------------------------------------------------------------------------------------

#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "crc.h"
#include "zarrosim.h"

/* 14 Marzo 1999 ------------------------------------------------------------ */

// 19 Giugno 1999 - credo che questa funzione faccia crashiare il Tabboz al' uscita.
/*
void clock_r1(u_long i1)
{
     if (i1 > 255 )
         new_counter=crctab[( (i1 % 255) >> 8) & 255] ^ (i1 << 8) ^ new_counter + 1999;
     new_counter=crctab[(i1 >> 8) & 255] ^ (i1 << 8) ^ new_counter + 79;
}
*/

// Nuova funzione ------------------------------------------------------------------

u_long      new_counter;
static char p_ruota1;
static char p_ruota2;
static char ruota1[8] = {0xed0f, 0xff00, 0x3392, 0xabcd, 0xc79c, 0x23df, 0x0006, 0xc39c};
static char ruota2[5] = {0xb47f, 0xc37b, 0x0070, 0x1999, 0xfb1e};

void clock_r1(u_long i1)
{
    u_long a1, a2;

    a1 = (i1 | ruota1[p_ruota1]) * (p_ruota2 + 1);
    p_ruota1++;
    if (p_ruota1 > 7)
        p_ruota1 = 0;

    a2 = (a1 & ruota2[p_ruota2]) * (p_ruota1 + 1);
    p_ruota2++;
    if (p_ruota2 > 4)
        p_ruota2 = 0;

    if (a2 >= a1)
        a2 -= a1;
    else
        a2 += a1;

    new_counter += a2;
}

/* -------------------------------------------------------------------------- */

void new_reset_check()
{
    new_counter = 0;
    p_ruota1 = 0;
    p_ruota1 = 0;
}

/* -------------------------------------------------------------------------- */

int new_check_i(int i)
{
    clock_r1(i);
    return (i);
}

/* -------------------------------------------------------------------------- */

u_long new_check_l(u_long i)
{
    clock_r1(i);
    return (i);
}

/* -------------------------------------------------------------------------- */
