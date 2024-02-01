// Tabboz Simulator
// (C) Copyright 1998,1999 by Andrea Bonomi
// 15 Maggio 1998

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

// ------------------ Non piu' in uso... -----------------------

#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "zarrosim.h"
static char sccsid[] = "@(#)" __FILE__ " " VERSION " (Andrea Bonomi) " __DATE__;

/* 10 Gennaio 1999 */
#ifdef PROMPT_ACTIVE

#ifndef isblank
#define isblank(ch) (((ch) == ' ') || ((ch) == '\t'))
#endif

int  prompt_mode;
HWND hQryDlgBox1;

char *n_tipo[] = {"<XXX/XXX>", "<EXE/EXT>", "<EXE/INT>"};

typedef struct
{
    char *name;
    char *usage;
    void (*func)();
    int minargs;
    int maxargs;
} CMDTAB;

static CMDTAB cmdtab[] = {
    "compagnia", "1", 0,
    1, 1,
    "config", "1", 0,
    1, 1,
    "date", "1", 0,
    2, 1,
    "disco", "1", 0,
    1, 1,
    "exit", "1", 0,
    2, 1,
    "famiglia", "1", 0,
    1, 1,
    "help", "1", 0,
    2, 1,
    "info", "1", 0,
    1, 1,
#ifndef TABBOZ_EM
    "logo", "1", 0,
    1, 1,
#endif
    "scoooter", "1", 0,
    1, 1,
    "status", "1", 0,
    2, 1,
    "scuola", "1", 0,
    1, 1,
    "ver", "1", 0,
    2, 1,
    NULL, 0, 0,
    0, 0};

typedef struct
{
    char *name;
    char *usage;
    void (*func)();
    int minargs;
    int maxargs;
} VARTAB;

#ifdef XXX

sprintf(tmp, "%ld", Soldi);
WriteProfileString("Tabbozzo", "Soldi", tmp);

sprintf(tmp, "%d", Paghetta);
WriteProfileString("Tabbozzo", "Paghetta", tmp);

sprintf(tmp, "%d", Reputazione);
WriteProfileString("Tabbozzo", "Reputazione", tmp);

sprintf(tmp, "%d", Rapporti);
WriteProfileString("Tabbozzo", "Rapporti", tmp);

sprintf(tmp, "%d", DDP);
WriteProfileString("Tabbozzo", "DdP", tmp);

sprintf(tmp, "%d", Studio);
WriteProfileString("Tabbozzo", "Studio", tmp);

sprintf(tmp, "%d", Fama);
WriteProfileString("Tabbozzo", "Fama", tmp);

sprintf(tmp, "%d", FigTipa);
WriteProfileString("Tabbozzo", "FigTipa", tmp);

WriteProfileString("Tabbozzo", "Nome", Nome);
WriteProfileString("Tabbozzo", "Cognome", Cognome);
WriteProfileString("Tabbozzo", "Nometipa", Nometipa);
WriteProfileString("Tabbozzo", "City", City);
WriteProfileString("Tabbozzo", "Street", Street);

sprintf(tmp, "123456789"); // 9 materie
for (i = 1; i < 10; i++)
{
    tmp[i - 1] = 65 + MaterieMem[i].xxx;
}

WriteProfileString("Tabbozzo", "Materie", tmp);

sprintf(tmp, "%d", Fortuna);
WriteProfileString("Tabbozzo", "Fortuna", tmp);

sprintf(tmp, "%d", x_mese);
WriteProfileString("Tabbozzo", "Mese", tmp);
sprintf(tmp, "%d", x_giorno);
WriteProfileString("Tabbozzo", "Giorno", tmp);
sprintf(tmp, "%d", x_giornoset);
WriteProfileString("Tabbozzo", "GiornoSet", tmp);

sprintf(tmp, "%d", comp_mese);
WriteProfileString("Tabbozzo", "CompMese", tmp);
sprintf(tmp, "%d", comp_giorno);
WriteProfileString("Tabbozzo", "CompGiorno", tmp);

sprintf(tmp, "%d", numeroditta);
WriteProfileString("Tabbozzo", "NumeroDitta", tmp);
sprintf(tmp, "%d", scad_pal_giorno);
WriteProfileString("Tabbozzo", "ScadPalGiorno", tmp);
sprintf(tmp, "%d", scad_pal_mese);
WriteProfileString("Tabbozzo", "ScadPalMese", tmp);

sprintf(tmp, "%d", impegno);
WriteProfileString("Tabbozzo", "Impegno", tmp);
sprintf(tmp, "%d", stipendio);
WriteProfileString("Tabbozzo", "Stipendio", tmp);

sprintf(tmp, "%d", sizze);
WriteProfileString("Tabbozzo", "Sigarette", tmp);

sprintf(tmp, "%d", euro);
WriteProfileString("Tabbozzo", "Euro", tmp);

sprintf(tmp, "%d", STARTcmdShow);
WriteProfileString("Tabbozzo", "STARTcmdShow", tmp);

sprintf(tmp, "%d", ScooterData.speed);
WriteProfileString("Scooter", "Speed", tmp);
sprintf(tmp, "%d", ScooterData.marmitta);
WriteProfileString("Scooter", "Marmitta", tmp);
sprintf(tmp, "%d", ScooterData.carburatore);
WriteProfileString("Scooter", "Carburatore", tmp);
sprintf(tmp, "%d", ScooterData.cc);
WriteProfileString("Scooter", "CC", tmp);
sprintf(tmp, "%d", ScooterData.filtro);
WriteProfileString("Scooter", "Filtro", tmp);
sprintf(tmp, "%d", ScooterData.prezzo);
WriteProfileString("Scooter", "Prezzo", tmp);
sprintf(tmp, "%d", ScooterData.attivita);
WriteProfileString("Scooter", "Attivita", tmp);
sprintf(tmp, "%d", ScooterData.stato);
WriteProfileString("Scooter", "Stato", tmp);
sprintf(tmp, "%d", benzina);
WriteProfileString("Scooter", "Benzina", tmp);
sprintf(tmp, "%d", antifurto);
WriteProfileString("Scooter", "Antifurto", tmp);
WriteProfileString("Scooter", "Nome", ScooterData.nome);

#endif

/*
 * Take a command string, and break it up into an argc, argv list.
 * The returned argument list and strings are in static memory, and so
 * are overwritten on each call.  The argument array is ended with an
 * extra NULL pointer for convenience.  Returns TRUE if successful,
 * or FALSE on an error with a message already output.
 */
BOOL    makeargs(cmd, argcptr, argvptr)
char   *cmd;
int    *argcptr;
char ***argvptr;
{
    char        *cp;
    int          argc;
    static char  strings[129];
    static char *argtable[1001];

    /*
     * Copy the command string and then break it apart
     * into separate arguments.
     */
    strcpy(strings, cmd);
    argc = 0;
    cp = strings;

    while (*cp)
    {
        if (argc >= 1000)
        {
            fprintf(stderr, "Too many arguments\n");
            return FALSE;
        }

        argtable[argc++] = cp;

        while (*cp && !isblank(*cp))
            cp++;

        while (isblank(*cp))
            *cp++ = '\0';
    }

    argtable[argc] = NULL;

    *argcptr = argc;
    *argvptr = argtable;

    return TRUE;
}

void
    writeME(tmp) char *tmp;
{
    SendDlgItemMessage(hQryDlgBox1, 101, LB_ADDSTRING, 0, (LONG)((LPSTR)tmp));
}

void
    do_help(argc, argv) char **argv;
{
    char    tmp[128];
    CMDTAB *cmdptr;

    for (cmdptr = cmdtab; cmdptr->name; cmdptr++)
    {
        sprintf(tmp, "%-15s %s", cmdptr->name, n_tipo[cmdptr->minargs]);
        writeME(tmp);
    }
}

#pragma argsused
BOOL FAR PASCAL Prompt(HWND hDlg, WORD message,
                       WORD wParam, LONG lParam)
{
    char        buf[128];
    char        tmp[1024];
    FARPROC     lpproc;
    static char Comando[128];
    int        *argc;
    char     ***argv;
    static HWND hlistwnd;

    if (message == WM_INITDIALOG)
    {
        prompt_mode = 1;
        hQryDlgBox1 = hDlg;
        //			hlistwnd = GetDlgItem(hDlg, 101);

        //	SendMessage(GetDlgItem(hDlg, 101), WM_SETFONT, hnewsfont, FALSE);

        //			sprintf(tmp,"Welcome to Tabboz Simulator");
        //			writeME(tmp);

        //			sprintf(tmp,"   %s",VERSION);
        //			writeME(tmp);

        //			InvalidateRect(hlistwnd, NULL, TRUE);
        //		UpdateWindow(hlistwnd);

        return (TRUE);
    }
    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 110:
            GetDlgItemText(hDlg, wParam, Comando, sizeof(Comando));

            break;
        case IDCANCEL:
            prompt_mode = 0;
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case IDOK:
            if ((Comando == '\0') || !makeargs(Comando, &argc, &argv))
                break;

            if (!strcmp(Comando, ""))
                break;

            SendMessage(hlistwnd, LB_RESETCONTENT, TRUE, 0L);
            InvalidateRect(hlistwnd, NULL, TRUE);
            UpdateWindow(hlistwnd);

            //		    sprintf(tmp, "%s %s",Comando,argv[0]);
            //		    SetDlgItemText(hDlg, 101, tmp);

            SetDlgItemText(hDlg, 110, "");

            //		    while (isblank(*Comando))
            //			    break;

            if (!strcmp(argv[0], "exit"))
            { /* EXIT ----------------------------------- */
                prompt_mode = 0;
                EndDialog(hDlg, TRUE);
                return (TRUE);
            }
            else if (!strcmp(argv[0], "ver"))
            { /* VER ------------------------------------ */
                sprintf(tmp, "Tabboz Simulator %s", VERSION);
                writeME(tmp);
                sprintf(tmp, "  %s", __DATE__);
                writeME(tmp);
                sprintf(tmp, "  Copyright(C) 1997,1998");
                writeME(tmp);
                sprintf(tmp, "  by Obscured Truckware");
                writeME(tmp);
                sprintf(tmp, "  Created by:");
                writeME(tmp);
                sprintf(tmp, "     %s", Andrea);
                writeME(tmp);
                sprintf(tmp, "     %s", Caccia);
                writeME(tmp);
                sprintf(tmp, "     %s", Daniele);
                writeME(tmp);

                break;
            }
            else if (!strcmp(argv[0], "status"))
            { /* STATUS --------------------------------- */
                sprintf(tmp, "Nome        = %s %s", Nome, Cognome);
                writeME(tmp);
                sprintf(tmp, "Soldi       = %s", MostraSoldi(Soldi));
                writeME(tmp);
                sprintf(tmp, "Figosita'   = %d/100", Fama);
                writeME(tmp);
                sprintf(tmp, "Reputazione = %d/100", Reputazione);
                writeME(tmp);
                sprintf(tmp, "Prof.Scol.  = %d/100", Studio);
                writeME(tmp);

                if (Rapporti != 0)
                {
                    sprintf(tmp, "Tipa        = %s", Nometipa);
                    writeME(tmp);
                    sprintf(tmp, "  Rapporti  = %d/100", Rapporti);
                    writeME(tmp);
                }
                if (ScooterData.stato != -1)
                {
                    sprintf(tmp, "Scooter     = %s", ScooterData.nome);
                    writeME(tmp);
                    sprintf(tmp, "  Stato     = %d/100", ScooterData.stato);
                    writeME(tmp);
                }
                sprintf(tmp, "%s %d %s", InfoSettimana[x_giornoset - 1].nome, x_giorno, InfoMese[x_mese - 1].nome);
                writeME(tmp);
                break;
            }
            else if (!strcmp(argv[0], "date"))
            { /* DATE ----------------------------------- */
                sprintf(tmp, "%s %d %s", InfoSettimana[x_giornoset - 1].nome, x_giorno, InfoMese[x_mese - 1].nome);
                writeME(tmp);
                break;
            }
            else if (!strcmp(argv[0], "help"))
            { /* HELP ----------------------------------- */
                do_help(argc, argv);
                break;
            }
            else if (!strcmp(argv[0], "about"))
            { /* ABOUT ---------------------------------- */
                /* Display about box. */
                lpproc = MakeProcInstance(About, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(ABOUT),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);
                break;
            }
#ifndef TABBOZ_EM
            else if (!strcmp(argv[0], "logo"))
            { /* LOGO ----------------------------------- */
                /* Display startup logo. */
                lpproc = MakeProcInstance(Logo, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(LOGO),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);
                break;
            }
#endif
            else if (!strcmp(argv[0], "config"))
            { /* CONFIG --------------------------------- */
                /* Display configuration box. */

                lpproc = MakeProcInstance(Configuration, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(CONFIGURATION),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);
                break;
            }
            else if (!strcmp(argv[0], "info"))
            {
                /* Display Personal Information box. */
                lpproc = MakeProcInstance(PersonalInfo, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(PERSONALINFO),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);
                break;
            }
            else if (!strcmp(argv[0], "famiglia"))
            {
                /* Display Famiglia box. */
                lpproc = MakeProcInstance(Famiglia, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(FAMIGLIA),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);
                break;
            }
            else if (!strcmp(argv[0], "disco"))
            {
                /* Display Disco box. */
                scelta = 0;
                lpproc = MakeProcInstance(Disco, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(DISCO),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);

                if (Fama > 100)
                    Fama = 100;

                break;
            }
            else if (!strcmp(argv[0], "compagnia"))
            {
                /* Display Compagnia box. */
                lpproc = MakeProcInstance(Compagnia, hInst);

                DialogBox(hInst,
                          MAKEINTRESOURCE(COMPAGNIA),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);

                break;
            }
            else if (!strcmp(argv[0], "scuola"))
            {
                if (x_vacanza == 0)
                {
                    /* Display Scuola box. */
                    lpproc = MakeProcInstance(Scuola, hInst);
                    DialogBox(hInst,
                              MAKEINTRESOURCE(SCUOLA),
                              hDlg,
                              lpproc);
                    FreeProcInstance(lpproc);
                }
                else
                {
                    MessageBox(hDlg,
                               "Non puoi andare a scuola in un giorno di vacanza !",
                               "Scuola", MB_OK | MB_ICONINFORMATION);
                }

                break;
            }
            else if (!strcmp(argv[0], "scooter"))
            {
                /* Display Scooter box. */
                lpproc = MakeProcInstance(Scooter, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(SCOOTER),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);

                break;
            }
#ifdef DISASTRO
            else if (!strcmp(argv[0], ""))
            {

            case QX_VESTITI:
                /* Display Vestiti box. */
                lpproc = MakeProcInstance(Vestiti, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(VESTITI),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);

                break;
            }
            else if (!strcmp(argv[0], ""))
            {

            case QX_TABACCHI:
                RunTabacchi(hDlg);
                break;
            }
            else if (!strcmp(argv[0], ""))
            {

            case QX_PALESTRA:
            }
            else if (!strcmp(argv[0], ""))
            {

                RunPalestra(hDlg);
                break;
            }
            else if (!strcmp(argv[0], ""))
            {

            case QX_VESTITI1:
            case QX_VESTITI2:
            case QX_VESTITI3:
            case QX_VESTITI4:
            case QX_VESTITI5:
                RunVestiti(hDlg, (wParam - QX_VESTITI1 + BAUHOUSE));
                break;
            }
            else if (!strcmp(argv[0], ""))
            {

            case QX_TIPA:
                // Display Tipa box.
                lpproc = MakeProcInstance(Tipa, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(TIPA),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);

                break;
            }
            else if (!strcmp(argv[0], ""))
            {

            case QX_LAVORO:
                // Display Lavoro box.
                lpproc = MakeProcInstance(Lavoro, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(LAVORO),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);

                break;

#endif
                else
                {
                    sprintf(tmp, "tabboz: %s: command not found", argv[0]);
                    SendDlgItemMessage(hQryDlgBox1, 101, LB_ADDSTRING, 0, (LONG)((LPSTR)tmp));
                }

                InvalidateRect(hlistwnd, NULL, TRUE);
                UpdateWindow(hlistwnd);

                break;

                //		    prompt_mode=0;
                //		    EndDialog(hDlg, TRUE);
                //		    return(TRUE);
            }
        }
        return (FALSE);
    }

#endif
