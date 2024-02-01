/* Tabboz Simulator         */
/* (C) Copyright 1998-1999 by Andrea Bonomi */

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
#define TESTVERSION
// ------------------------------------------------------------------------------------------------

#include "os.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef TABBOZ_EM
#include <sys/stat.h>
#else
#include <sys\stat.h>
#endif

#include "zarrosim.h"

int AdV;

#if 0
void    SetSVideo() /* Deve avere un nome che non dia nell' occhio,                 */
            /* in modo che chi volesse attaccare il programma non la vada a spulciare   */
{

long     int        memorizzato;
long     int        xxx2=0;
//       int        crc;        // Deve essere un tipo a 16 bit...
         int        handle, bytes,i;
         char       *buf;
         char       writebuf[50];
         char       szBuf[256];

// 5 Maggio 1999 Visto che le funzioni MessageBox sono prive del papa'
// bisogna mettere 0 come hInstance... (primo parametro...)

#define READTHIS 16384

    if (( buf = malloc(READTHIS) ) == NULL ) {
        MessageBox( 0,
        "La memoria disponibile e' insufficente per lanciare il Tabboz Simulator, libera piu' RAM e riprova.",
        "Memoria insufficente", MB_OK | MB_ICONSTOP );
        exit(1);
    };

/* guardare la funzione sopen che permette di specificare gli accessi di condivisione di un file */

// OLD --- if ((handle = open(_argv[0], O_RDONLY | O_BINARY, S_IREAD)) == -1)   {

    GetModuleFileName(hInst,szBuf,sizeof(szBuf)); // 10 Mar 2000

    if ((handle = open(szBuf, O_RDONLY | O_BINARY, S_IREAD)) == -1) {
        MessageBox( 0,
        "Si e' verificato un errore durante l'apertura del file",
        "Errore di apertura del file", MB_OK | MB_ICONSTOP );
        exit(1);
    }

    /* Salta il primo kilobyte  */
    if ((bytes = read(handle, buf, 1024)) == -1) {
    MessageBox( 0,
        "Si e' verificato un errore durante la lettura del file",
        "Errore di lettura del file", MB_OK | MB_ICONSTOP );
        exit(1);
    }

// sprintf(writebuf,"t8è+#§ù0hiq"); /* 11 e non uno di piu' */

    for (i=0; i < 11;i++) {
        writebuf[i]=buf[96+i];
    }

    writebuf[12]=0;
    memorizzato=atol(writebuf);

//  for (i=0; i < 5;i++) {          /* Legge il CRC memorizzato nel file... */
//      writebuf[i]=buf[108+i];
//  }
//  crcr=atoi(writebuf);

// ------------------------------------------------------------------------------------------------

    // crc=0;
    // Computa il checksum...

    while ((bytes = read(handle, buf, READTHIS)) > 0) {

        for (i=0; i <= bytes;i++) {
             // crc=crctab[((crc >> 8) & 255)] ^ (crc << 8) ^ buf[i];
             xxx2+=buf[bytes-1] * bytes + 2;
             }

    }

// ------------------------------------------------------------------------------------------------
// #define ESAGERATO
#ifdef ESAGERATO
        sprintf(writebuf,"OK...");
        MessageBox( 0,
        writebuf,
        "Info...", MB_OK | MB_ICONSTOP );
#endif

    if (memorizzato == 0) {
#ifdef TESTVERSION
        MessageBeep(0x0050);
        AdV = 1;    /* VARIABILE DI CONTROLLO PROTEZIONE - DEVE ESSERE = A 1, ALTRIMENTI, BOOM !    */
#else
        exit(-1);
#endif
     } else AdV = memorizzato - xxx2 + 1;

    /* atol("12345") converte una stringa di numeri in un long int  */
    /* un long, scritto sotto forma di striga, puo' arrivare a 11 caratteri (segno compreso)    */

    close(handle);  /* Chiude il file               */
    free(buf);  /* Libera la memoria precedentemente allocata   */
    return;
}
#endif

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
// 10 Marzo 2000
// Correzzioni a tutte le procedure del checksum.

u_long      new_counter;
static char p_ruota1;
static char p_ruota2;
static int  ruota1[8] = {0xed0f, 0xff0d, 0x3392, 0xabcd, 0xc79c, 0x23df, 0x0706, 0xc39c};
static int  ruota2[5] = {0xb47f, 0xc37b, 0x1070, 0x1999, 0xfb1e};

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

// --------------------------------------------------------------------------
// Resetta il checksum e i valori che vengono usati per calcolarlo.

void new_reset_check()
{
    new_counter = 1979;
    p_ruota1 = 0;
    p_ruota2 = 2;
}

// --------------------------------------------------------------------------
// Chiama il calcolatore di checksum. Ritorna il valore passato come parametro.
// versione int.
int new_check_i(int i)
{
    clock_r1(i);
    return (i);
}

// --------------------------------------------------------------------------
// Chiama il calcolatore di checksum. Ritorna il valore passato come parametro.
// versione unsigned long.
u_long new_check_l(u_long i)
{
    clock_r1(i);
    return (i);
}

// --------------------------------------------------------------------------
