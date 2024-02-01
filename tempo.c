// Tabboz Simulator
// (C) Copyright 1997-2000 by Andrea Bonomi
// 5 Giugno 1999 - Conversione Tabbozzo -> Tabbozza

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
static char sccsid[] = "@(#)" __FILE__ " " VERSION " (Andrea Bonomi) " __DATE__;
static char natale2;

BOOL FAR PASCAL MostraPagella(HWND hDlg, WORD message, WORD wParam, LONG lParam);

typedef struct tagVACANZE
{
    char *nome;        /* nome del giorno di vacanza	 */
    int   giorno;      /* mese				 */
    int   mese;        /* giorno		         */
    char *descrizione; /* descrizione giorno di vacanza */
} STVACANZE;

STMESI InfoMese[] =
    {
        {"Gennaio",   31},
        {"Febbraio",  28},
        {"Marzo",     31},
        {"Aprile",    30},
        {"Maggio",    31},
        {"Giugno",    30},
        {"Luglio",    31},
        {"Agosto",    31},
        {"Settembre", 30},
        {"Ottobre",   31},
        {"Novembre",  30},
        {"Dicembre",  31}
};

STMESI InfoSettimana[] =
    {
        {"Lunedi'",    0},
        {"Martedi'",   0},
        {"Mercoledi'", 0},
        {"Giovedi'",   0},
        {"Venerdi'",   0},
        {"Sabato",     0},
        {"Domenica",   1}
};

STVACANZE InfoVacanze[] =
    {
        {"Capodanno",                1,  1,  "Oggi e' capodanno !"                                                       },
        {"Epifania",                 6,  1,  "Epifania..."                                                               },
        {"Anniversario Liberazione", 25, 4,  "Oggi mi sento liberato"                                                    },
        {"Festa dei lavoratori",     1,  5,  "Nonostante nella tua vita, tu non faccia nulla, oggi fai festa anche tu..."},
        {"Ferragosto",               15, 8,  "Oggi e' ferragosto..."                                                     },
        {"Tutti i Santi",            1,  11, "Figata, oggi e' vacanza..."                                                },
        {"Sant' Ambrogio",           7,  12, "Visto che siamo a Milano, oggi facciamo festa."                            },
        {"Immacolata Concezione",    8,  12, "Oggi e' festa..."                                                          },
        {"Natale",                   25, 12, "Buon Natale !!!"                                                           },
        {"Santo Stefano",            26, 12, "Buon Santo Stefano..."                                                     },
        {NULL,                       0,  0,  NULL                                                                        }
};

int x_giorno;
int x_mese;
int x_anno_bisesto; // Anno Bisestile - 12 Giugno 1999
int x_giornoset;
int x_vacanza;       /* Se e' un giorno di vacanza, e' uguale ad 1 o 2 altrimenti a 0	*/
int scad_pal_giorno; /* Giorno e mese in cui scadra' l' abbonamento alla palestra.		*/
int scad_pal_mese;

// ------------------------------------------------------------------------------------------
// Giorno...
// ------------------------------------------------------------------------------------------

void Giorno(HANDLE hInstance)
{
    int     i;
    FARPROC lpproc;
    char    tmp[255];

    x_giorno++;
    if (x_giorno > InfoMese[x_mese - 1].num_giorni)
    {
        if ((x_mese == 2) && (x_anno_bisesto == 1) && (x_giorno == 29))
        {
            MessageBox(hInstance, "Anno bisesto, anno funesto...",
                       "Anno Bisestile", MB_OK | MB_ICONSTOP);
        }
        else
        {
            x_giorno = 1;
            x_mese += 1;
        }
    }

    if (x_mese > 12)
    {
        x_mese = 1;
        x_anno_bisesto = +1;
        if (x_anno_bisesto > 3)
            x_anno_bisesto = 0;
        /* Capodanno();	*/
    }

    x_giornoset++;
    if (x_giornoset > 7)
    {
        x_giornoset = 1;
        if (current_testa > 0)
        {
            current_testa--;  // Ogni 7 giorni diminuisce l' abbronzatura - 26 Feb 1999
            TabbozRedraw = 1; // e si deve aggiornare il disegno... (BUG ! Mancava fino alla versione 0.83pr )
        }
    }

    /* ---------------> S T I P E N D I O <--------------- */

    if (impegno > 0)
    {
        giorni_di_lavoro += 1; // C' era scritto =+1 al posto di +=1

        if ((x_giorno == 27) && (giorni_di_lavoro > 3))
        {
            long stipendietto; /* Stipendio calcolato secondo i giorni effettivi di lavoro */

            if (giorni_di_lavoro > 29)
                stipendietto = (long)stipendio;
            else
                stipendietto = (long)stipendio * (long)giorni_di_lavoro / 30;

            giorni_di_lavoro = 1;

            sprintf(tmp, "Visto che sei stat%c %s brav%c dipendente sottomess%c, ora ti arriva il tuo misero stipendio di %s",
                    ao, un_una, ao, ao, MostraSoldi(stipendietto));

            MessageBox(hInstance, tmp,
                       "Stipendio !", MB_OK | MB_ICONINFORMATION);

            Soldi += stipendietto;

#ifdef TABBOZ_DEBUG
            sprintf(tmp, "giorno: Stipendio (%s)", MostraSoldi(stipendietto));
            writelog(tmp);
#endif
        }
    }

    /* ---------------> P A L E S T R A <---------------    21 Apr 1998	*/

    if (scad_pal_mese == x_mese)
        if (scad_pal_giorno == x_giorno)
        {
            MessageBox(hInstance,
                       "E' appena scaduto il tuo abbonamento della palestra...",
                       "Palestra", MB_OK | MB_ICONINFORMATION);
            scad_pal_giorno = 0;
            scad_pal_mese = 0;
#ifdef TABBOZ_DEBUG
            writelog("giorno: E' scaduto l' abbonamento alla palestra");
#endif
        }

    /* Calcola i giorni di vacanza durante l' anno ( da finire...)	*/
    x_vacanza = 0;
    current_tipa = 0;

    /* Hai gia' ricevuto gli auguri di natale ??? 04/01/1999*/
    natale2 = 0;

    switch (x_mese)
    {
    case 1: /* Gennaio --------------------------------------------------------- */
        if (x_giorno < 7)
        {
            x_vacanza = 1;
            if (Rapporti > 0)
                current_tipa = 1; /* 6 Maggio 1999 - Tipa vestita da Babbo Natale...*/
        }
        break; /* Vacanze di Natale */

    case 6: /* Giugno ---------------------------------------------------------- */
        if (x_giorno == 15)
            MessageBox(hInstance,
                       "Da domani iniziano le vacanza estive !",
                       "Ultimo giorno di scuola", MB_OK | MB_ICONINFORMATION);

        if (x_giorno == 22)
        { /* Pagella */
            lpproc = MakeProcInstance(MostraPagella, hInst);
            DialogBox(hInst,
                      MAKEINTRESOURCE(110),
                      hInstance,
                      lpproc);
            FreeProcInstance(lpproc);
        }
        if (x_giorno > 15)
            x_vacanza = 1;

        break;
    case 7: /* Luglio e */
    case 8: /* Agosto   */
        x_vacanza = 1;
        if ((Rapporti > 93) && (FigTipa > 95))
            current_tipa = 2; /* 6 Maggio 1999 - Tipa al mare...*/
        break;

    case 9: /* Settembre ------------------------------------------------------- */
        if (x_giorno < 15)
            x_vacanza = 1;
        if (x_giorno == 15)
        {
            MessageBox(hInstance,
                       "Questa mattina devi tornare a scuola...",
                       "Primo giorno di scuola", MB_OK | MB_ICONINFORMATION);
            Studio = 0; /* Azzera le materie...	*/
            for (i = 1; i < 10; i++)
                MaterieMem[i].xxx = 0;
        }
        break;

    case 12: /* Dicembre -------------------------------------------------------- */
        if (x_giorno > 22)
        {
            x_vacanza = 1; /* Vacanze di Natale */
            if (Rapporti > 0)
                current_tipa = 1; /* 6 Maggio 1999 - Tipa vestita da Babbo Natale...*/
        }

        if (x_giorno == 25)
        {
            if ((current_pantaloni == 19) && (current_gibbotto == 19))
            {
                MessageBox(hInstance,
                           "Con il tuo vestito da Babbo Natale riesci a stupire tutti...",
                           "Natale...", MB_OK | MB_ICONINFORMATION);
                Fama += 20;
                if (Fama > 100)
                    Fama = 100;
            }

#ifdef VERAMENTE_INUTILE
            if (Rapporti > 0)
            { /* Buon Natale dalla tipa (04/01/1999) */
                natale2 = 1;
                lpproc = MakeProcInstance(MostraSalutieBaci, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(96),
                          hInstance,
                          lpproc);
                FreeProcInstance(lpproc);
            }
#endif
        }

        if ((x_giorno == 28) && ((current_pantaloni == 19) || (current_gibbotto == 19)))
        {
            MessageBox(hInstance,
                       "Natale e' gia' passato... Togliti quel dannato vestito...",
                       "Natale...", MB_OK | MB_ICONINFORMATION);
            Fama -= 5;
            if (Fama < 0)
                Fama = 0;
        }

        break;
    }

    /* Domeniche e festivita' varie				VACANZE DI TIPO 2 */

    if (x_giornoset == 7)
        x_vacanza = 2; /* Domenica */

    if (natale2 == 0)
    {
        int a = 0;
        while (InfoVacanze[a].giorno != 0)
        {
            if (InfoVacanze[a].mese == x_mese)
                if (InfoVacanze[a].giorno == x_giorno)
                {
                    MessageBox(hInstance,
                               InfoVacanze[a].descrizione,
                               InfoVacanze[a].nome, MB_OK | MB_ICONINFORMATION);

                    x_vacanza = 2; /* 2 = sono chiusi anche i negozi... */
                }
            a++;
        }
    }

#ifdef TABBOZ_DEBUG
    /* Mostra data e soldi */
    sprintf(tmp, "giorno: %s %d %s, %s", InfoSettimana[x_giornoset - 1].nome, x_giorno, InfoMese[x_mese - 1].nome, MostraSoldi(Soldi));
    writelog(tmp);
#endif
}

// ------------------------------------------------------------------------------------------
// Mostra la pagella...
// ------------------------------------------------------------------------------------------

#pragma argsused
BOOL FAR PASCAL MostraPagella(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char tmp[1024];
    int  i, k;

    if (message == WM_INITDIALOG)
    {
        k = 0;
        for (i = 1; i < 10; i++)
        {
            if (MaterieMem[i].xxx < 6)
                k++; /* k = materie insuff o grav. insuf. */
            if (MaterieMem[i].xxx < 4)
                k++; /* k = materie insuff o grav. insuf. */
            sprintf(tmp, "%d", MaterieMem[i].xxx);
            SetDlgItemText(hDlg, i + 119, tmp);
        }

        if (Fama > 75) // Condotta... + un e' figo, + sembra un bravo ragazzo...
            SetDlgItemText(hDlg, 129, "8");
        else
            SetDlgItemText(hDlg, 129, "9");

        if (k > 4)
        {
            if (sound_active)
                TabbozPlaySound(401);
            sprintf(tmp, "NON ammess%c", ao); /* bocciata/o */
#ifdef TABBOZ_DEBUG
            writelog("giorno: Pagella... Bocciato !!!");
#endif
        }
        else
        {
            sprintf(tmp, "ammess%c", ao); /* promossa/o */
            Soldi += 200;
#ifdef TABBOZ_DEBUG
            writelog("giorno: Pagella... Promosso...");
#endif
        }

        SetDlgItemText(hDlg, 119, tmp);
        return (TRUE);
    }

    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case IDOK:
            EndDialog(hDlg, TRUE);
            return (TRUE);

        default:
            return (TRUE);
        }
    }

    return (FALSE);
}
