// Tabboz Simulator
// (C) Copyright 1997-2000 by Andrea Bonomi
// 31 Maggio 1999 - Conversione Tabbozzo -> Tabbozza

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

extern void     EventiPalestra(HANDLE hInstance);
BOOL FAR PASCAL CompraQualcosa(HWND hDlg, WORD message, WORD wParam, LONG lParam);
void            PagaQualcosa(HWND parent);

/* Per una questione di svogliatezza del programmatore, viene usata STSCOOTER anche x i vestiti. */

STSCOOTER VestitiMem[] =
    {
        {0, 0, 0, 0,  0, 0, 0,   0, "---"},
        {0, 0, 0, 8,  0, 0, 348, 0, ""   }, // -- Giubbotto "Fatiscenza"
        {0, 0, 0, 9,  0, 0, 378, 0, ""   }, //
        {0, 0, 0, 7,  0, 0, 298, 0, ""   }, //
        {0, 0, 0, 8,  0, 0, 248, 0, ""   }, //    Giacca di pelle (3 Nuovi Giubbotti)
        {0, 0, 0, 9,  0, 0, 378, 0, ""   }, //    Fatiscenza verde
        {0, 0, 0, 10, 0, 0, 418, 0, ""   }, //    Fatiscenza bianco
        {0, 0, 0, 3,  0, 0, 90,  0, ""   }, // -- Pantaloni gessati

        {0, 0, 0, 5,  0, 0, 170, 0, ""   }, //    Pantaloni tuta
        {0, 0, 0, 6,  0, 0, 248, 0, ""   }, //    Pantaloni in plastika
        {0, 0, 0, 5,  0, 0, 190, 0, ""   }, //    Pantaloni scacchiera 21 giugno 1999

        {0, 0, 0, 4,  0, 0, 122, 0, ""   }, // -- Scarpe da tabbozzi...
        {0, 0, 0, 6,  0, 0, 220, 0, ""   }, //    Buffalo
        {0, 0, 0, 2,  0, 0, 58,  0, ""   }, //    Scarpe da tabbozzi...
        {0, 0, 0, 4,  0, 0, 142, 0, ""   }, //    NUOVE Scarpe da tabbozzi...   23 Aprile 1998
        {0, 0, 0, 4,  0, 0, 142, 0, ""   }, //    ""		""
        {0, 0, 0, 5,  0, 0, 166, 0, ""   }, //    ""		""
        {0, 0, 0, 6,  0, 0, 230, 0, ""   }
}; //    Nuove Buffalo

/* Abbonamenti Palestra ---------------------------------------------------------------------------- */

STSCOOTER PalestraMem[] =
    {
        {0, 0, 0, 0, 0, 0, 50,  0, ""}, // Un mese	21 Apr 1998
        {0, 0, 0, 8, 0, 0, 270, 0, ""}, // Sei mesi
        {0, 0, 0, 9, 0, 0, 500, 0, ""}, // Un anno
        {0, 0, 0, 9, 0, 0, 14,  0, ""}
}; // Una lampada

/* Sigarette --------------------------------------------------------------------------------------- */

STSCOOTER SizeMem[] =
    {
        {5,  5,  0, 2, 0, 0, 6, 0, "Barclay"                  },
        {8,  7,  0, 1, 0, 0, 6, 0, "Camel"                    },
        {7,  6,  0, 2, 0, 0, 6, 0, "Davidoff Superior Lights" },
        {7,  6,  0, 2, 0, 0, 6, 0, "Davidoff Mildnes"         },
        {13, 9,  0, 2, 0, 0, 6, 0, "Davidoff Classic"         },
        {9,  7,  0, 1, 0, 0, 5, 0, "Diana Blu"                },
        {12, 9,  0, 1, 0, 0, 5, 0, "Diana Rosse"              },
        {8,  7,  0, 0, 0, 0, 6, 0, "Dunhill Lights"           },
        {7,  5,  0, 0, 0, 0, 6, 0, "Merit"                    },
        {14, 10, 0, 0, 0, 0, 6, 0, "Gauloises Blu"            },
        {7,  6,  0, 0, 0, 0, 6, 0, "Gauloises Rosse"          },
        {13, 10, 0, 1, 0, 0, 6, 0, "Unlucky Strike"           },
        {9,  7,  0, 1, 0, 0, 6, 0, "Unlucky Strike Lights"    },
        {8,  6,  0, 2, 0, 0, 6, 0, "Malborro Medium"          }, // dovrebbero essere come le lights 4 Marzo 1999
        {12, 9,  0, 2, 0, 0, 6, 0, "Malborro Rosse"           },
        {8,  6,  0, 2, 0, 0, 6, 0, "Malborro Lights"          },
        {11, 10, 0, 0, 0, 0, 5, 0, "NS Rosse"                 },
        {9,  8,  0, 0, 0, 0, 5, 0, "NS Mild"                  },
        {9,  7,  0, 1, 0, 0, 5, 0, "Poll Mon Blu"             },
        {12, 9,  0, 1, 0, 0, 5, 0, "Poll Mon Rosse"           },
        {12, 10, 0, 2, 0, 0, 6, 0, "Philip Morris"            },
        {4,  4,  0, 2, 0, 0, 6, 0, "Philip Morris Super Light"},
        {10, 9,  0, 1, 0, 0, 5, 0, "Armadis"                  },
        {11, 9,  0, 0, 0, 0, 5, 0, "Winston"                  }
};
/*									*/
/*	    |   |							*/
/*	    |   \nicotina * 10 ( 7 = nicotina 0.7, 10 = nicotina 1 )	*/
/*          \condensato							*/

/********************************************************************/
/* Vestiti...                                                       */
/********************************************************************/

#pragma argsused
BOOL FAR PASCAL Vestiti(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char    tmp[128];
    FARPROC lpproc;

    if (message == WM_INITDIALOG)
    {
        SetDlgItemText(hDlg, 120, MostraSoldi(Soldi));

        /* Vestito da Babbo Natale... 11 Marzo 1999 */
#define COSTO_VESTITO_NATALIZIO 58

        if ((x_mese == 12) && (Soldi >= COSTO_VESTITO_NATALIZIO))
            if ((x_giorno > 14) && (x_giorno < 25) && (current_gibbotto != 19) && (current_pantaloni != 19))
            {
                int scelta;
                sprintf(tmp, "Vuoi comperare, per %s, un meraviglioso vestito da Babbo Natale ?", MostraSoldi(COSTO_VESTITO_NATALIZIO));
                scelta = MessageBox(hDlg,
                                    tmp,
                                    "Offerte Natalizie...", MB_YESNO | MB_ICONQUESTION);
                if (scelta == IDYES)
                {
                    current_gibbotto = 19;
                    current_pantaloni = 19;
                    TabbozRedraw = 1; // E' necessario ridisegnare l' immagine del Tabbozzo...
                    Soldi -= COSTO_VESTITO_NATALIZIO;
                }
            }

        return (TRUE);
    }
    else if (message == WM_COMMAND)
    {

        switch (wParam)
        {

        case 101: // MAKEINTRESOURCE 80
        case 102: // MAKEINTRESOURCE 81
        case 103: // MAKEINTRESOURCE 82
        case 104: // MAKEINTRESOURCE 83
        case 105: // MAKEINTRESOURCE 84
            RunVestiti(hDlg, (wParam - 21));
            SetDlgItemText(hDlg, 120, MostraSoldi(Soldi));
            return (TRUE);

        case 110: // Tabaccaio
            RunTabacchi(hDlg);
            SetDlgItemText(hDlg, 120, MostraSoldi(Soldi));
            return (TRUE);

        case 111: // Palestra
            RunPalestra(hDlg);
            SetDlgItemText(hDlg, 120, MostraSoldi(Soldi));
            return (TRUE);

        case 112: // Cellulare
            lpproc = MakeProcInstance(Cellular, hInst);
            DialogBox(hInst,
                      MAKEINTRESOURCE(CELLULAR),
                      hDlg,
                      lpproc);
            FreeProcInstance(lpproc);

            SetDlgItemText(hDlg, 120, MostraSoldi(Soldi));
            return (TRUE);

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

//*******************************************************************
// Routine per il pagamento di qualunque cosa...
//*******************************************************************

void PagaQualcosa(HANDLE hInstance)
{
    char tmp[128];

    if (scelta != 0)
    {
        if (VestitiMem[scelta].prezzo > Soldi)
        {
            (void)nomoney(hInstance, VESTITI);
        }
        else
        {
            Soldi -= VestitiMem[scelta].prezzo;

            switch (scelta)
            { /* 25 Febbraio 1999 */
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                current_gibbotto = scelta;
                break;
            case 7:
            case 8:
            case 9:
            case 10:
                current_pantaloni = scelta - 6;
                break;
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
                current_scarpe = scelta - 10;
                break;
            }

            TabbozRedraw = 1; // E' necessario ridisegnare l' immagine del Tabbozzo...

#ifdef TABBOZ_DEBUG
            sprintf(tmp, "vestiti: Paga %s", MostraSoldi(VestitiMem[scelta].prezzo));
            writelog(tmp);
#endif
            Fama += VestitiMem[scelta].fama;
            if (Fama > 100)
                Fama = 100;
        }
        Evento(hInstance);
    }
}

//*******************************************************************
// Routine di acquisto generica
//*******************************************************************

#pragma argsused
BOOL FAR PASCAL CompraQualcosa(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char tmp[128];
    int  i;

    if (message == WM_INITDIALOG)
    {
        scelta = 0;
        SetDlgItemText(hDlg, 120, MostraSoldi(Soldi));
        sprintf(tmp, "%d/100", Fama);
        SetDlgItemText(hDlg, 121, tmp);

        for (i = 101; i < 120; i++)
        {
            SetDlgItemText(hDlg, i, MostraSoldi(VestitiMem[i - 100].prezzo));
        }
        return (TRUE);
    }

    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 101:
        case 102:
        case 103:
        case 104:
        case 105:
        case 106:
        case 107:
        case 108:
        case 109:
        case 110:
        case 111:
        case 112:
        case 113:
        case 114:
        case 115:
        case 116:
        case 117:
        case 118:
        case 119:
            scelta = wParam - 100;
            return (TRUE);

        case IDCANCEL:
            scelta = 0;
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case IDOK:
            PagaQualcosa(hDlg);
            EndDialog(hDlg, TRUE);
            return (TRUE);

        default:
            return (TRUE);
        }
    }

    return (FALSE);
}

//*******************************************************************
// Tabaccaio ! (che centra tra i vestiti ??? Come procedura e' simile...)
//*******************************************************************

#pragma argsused
BOOL FAR PASCAL Tabaccaio(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char  tmp_descrizione[1024];
    char  tmp[255];
    div_t nico;
    int   i;

    if (message == WM_INITDIALOG)
    {
        if (sound_active)
            TabbozPlaySound(203);
        scelta = -1; // Fino alla 0.8.51pr c'era un bug che non faceva comprare le Barclay...
        SetDlgItemText(hDlg, 104, MostraSoldi(Soldi));
        sprintf(tmp, "%d", sizze);
        SetDlgItemText(hDlg, 105, tmp);
        sprintf(tmp, "Che sigarette vuoi, ragazz%c ?", ao);
        SetDlgItemText(hDlg, 106, tmp);
        return (TRUE);
    }
    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 400:
        case 401:
        case 402:
        case 403:
        case 404:
        case 405:
        case 406:
        case 407:
        case 408:
        case 409:
        case 410:
        case 411:
        case 412:
        case 413:
        case 414:
        case 415:
        case 416:
        case 417:
        case 418:
        case 419:
        case 420:
        case 421:
        case 422:
        case 423:
            scelta = wParam - 400;
            LoadString(hInst, (wParam + 1000), tmp, 254);

            if (SizeMem[scelta].cc == 0)
            {
                /* Se i valori sono impostati a 0, non li scrive */
                sprintf(tmp_descrizione, "%s\n%s",
                        SizeMem[scelta].nome, tmp);
            }
            else
            {
                nico = div(SizeMem[scelta].cc, 10);
                sprintf(tmp_descrizione, "%s\n%sCondensato: %d Nicotina: %d.%d",
                        SizeMem[scelta].nome, tmp, SizeMem[scelta].speed, nico.quot, nico.rem);
            }

            SetDlgItemText(hDlg, 106, tmp_descrizione);
            return (TRUE);

        case IDCANCEL:
            scelta = -1;
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case IDOK:
            if (scelta != -1)
            {
                if (SizeMem[scelta].prezzo > Soldi)
                {
                    nomoney(hDlg, TABACCAIO);
                }
                else
                {
                    Soldi -= SizeMem[scelta].prezzo;
#ifdef TABBOZ_DEBUG
                    sprintf(tmp, "tabaccaio: Paga %s", MostraSoldi(SizeMem[scelta].prezzo));
                    writelog(tmp);
#endif
                    Fama += SizeMem[scelta].fama;
                    if (Fama > 100)
                        Fama = 100;
                    sizze += 20;
                }
                i = random(8) + 600;
                LoadString(hInst, i, (LPSTR)tmp, 254); // 600 -> 607
                MessageBox(hDlg,
                           (LPSTR)tmp,
                           "ART. 46 L. 29/12/1990 n. 428", MB_OK | MB_ICONINFORMATION);

                Evento(hDlg);
            }

            EndDialog(hDlg, TRUE);
            return (TRUE);

        default:
            return (TRUE);
        }
    }

    return (FALSE);
}

//*******************************************************************
// Palestra ! (che centra tra i vestiti ??? Come procedura e' simile...)
//*******************************************************************

static void AggiornaPalestra(HWND parent)
{
    char tmp[128];

    SetDlgItemText(parent, 104, MostraSoldi(Soldi));
    sprintf(tmp, "%d/100", Fama);
    SetDlgItemText(parent, 105, tmp);

    if (scad_pal_giorno < 1)
    {
        sprintf(tmp, "Nessun Abbonamento");
        SetDlgItemText(parent, 106, tmp);
    }
    else
    {
        sprintf(tmp, "Scadenza abbonamento: %d %s", scad_pal_giorno, InfoMese[scad_pal_mese - 1].nome);
        SetDlgItemText(parent, 106, tmp);
    }

    // Scrive il grado di abbronzatura... 4 Marzo 1999
    switch (current_testa)
    {
    case 1:
        sprintf(tmp, "Abbronzatura Lieve");
        break;
    case 2:
        sprintf(tmp, "Abbronzatura Media");
        break;
    case 3:
        sprintf(tmp, "Abbronzatura Pesante");
        break;
    case 4:
        sprintf(tmp, "Carbonizzat%c...", ao);
        break;
    default:
        sprintf(tmp, "Non abbronzat%c", ao);
    }
    SetDlgItemText(parent, 107, tmp);
}

//*******************************************************************

#pragma argsused
BOOL FAR PASCAL Palestra(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
// Costi... (migliaia di lire)
#define UN_MESE 50
#define SEI_MESI 270
#define UN_ANNO 500

    char tmp[128];
    int  i;

    if (message == WM_INITDIALOG)
    {
        AggiornaPalestra(hDlg);
        for (i = 0; i < 4; i++)
            SetDlgItemText(hDlg, 120 + i, MostraSoldi(PalestraMem[i].prezzo));

        return (TRUE);
    }

    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {

        case 110: // Vai in palestra
            if (scad_pal_giorno < 1)
            {
                MessageBox(hDlg,
                           "Prima di poter venire in palestra devi fare un abbonamento !",
                           "Palestra", MB_OK | MB_ICONINFORMATION);
            }
            else
            {
                if (sound_active)
                    TabbozPlaySound(201);
                if (Fama < 82)
                    Fama++;
                EventiPalestra(hDlg);
                AggiornaPalestra(hDlg);
                /* Evento(hDlg); */
            }
            return (TRUE);

        case 111: // Lampada
            if (PalestraMem[3].prezzo > Soldi)
            {
                nomoney(hDlg, PALESTRA);
            }
            else
            {
                if (current_testa < 3)
                {
                    current_testa++; // Grado di abbronzatura
                    if (Fama < 20)
                        Fama++; // Da 0 a 3 punti in piu' di fama
                    if (Fama < 45)
                        Fama++; // ( secondo quanta se ne ha gia')
                    if (Fama < 96)
                        Fama++;
                }
                else
                {
                    current_testa = 4; // Carbonizzato...
                    if (Fama > 8)
                        Fama -= 8;
                    if (Reputazione > 5)
                        Reputazione -= 5;
                    MessageBox(hDlg, "L' eccessiva esposizione del tuo corpo ai raggi ultravioletti,\
 provoca un avanzato grado di carbonizzazione e pure qualche piccola mutazione genetica...",
                               "Lampada", MB_OK | MB_ICONSTOP);
                }
                TabbozRedraw = 1; // E' necessario ridisegnare l' immagine del Tabbozzo...

                if (sound_active)
                    TabbozPlaySound(202);
                Soldi -= PalestraMem[3].prezzo;
#ifdef TABBOZ_DEBUG
                sprintf(tmp, "lampada: Paga %s", MostraSoldi(PalestraMem[3].prezzo));
                writelog(tmp);
#endif
            }
            i = random(5 + Fortuna);
            if (i == 0)
                Evento(hDlg);
            AggiornaPalestra(hDlg);
            return (TRUE);

        case 115: // Abbonamenti
        case 116:
        case 117:
            if (scad_pal_giorno > 0)
            {
                MessageBox(hDlg,
                           "Hai gia' un abbonamento, perche' te ne serve un altro ???",
                           "Palestra", MB_OK | MB_ICONINFORMATION);
                return (TRUE);
            }

            if (PalestraMem[wParam - 115].prezzo > Soldi)
            {
                nomoney(hDlg, PALESTRA);
                return (TRUE);
            }
            else
            {
                Soldi -= PalestraMem[wParam - 115].prezzo;
#ifdef TABBOZ_DEBUG
                sprintf(tmp, "palestra: Paga %s", MostraSoldi(PalestraMem[wParam - 115].prezzo));
                writelog(tmp);
#endif
            }

            switch (wParam)
            {
            case 115:
                scad_pal_mese = x_mese + 1; // UN MESE
                scad_pal_giorno = x_giorno;
                if (scad_pal_mese > 12)
                    scad_pal_mese -= 12;
                // Quello che segue evita che la palestra scada un giorno tipo il 31 Febbraio
                if (scad_pal_giorno > InfoMese[scad_pal_mese - 1].num_giorni)
                    scad_pal_giorno = InfoMese[scad_pal_mese - 1].num_giorni;
                break;
                ;

            case 116:
                scad_pal_mese = x_mese + 6; // SEI MESI
                scad_pal_giorno = x_giorno;
                if (scad_pal_mese > 12)
                    scad_pal_mese -= 12;
                // Quello che segue evita che la palestra scada un giorno tipo il 31 Febbraio
                if (scad_pal_giorno > InfoMese[scad_pal_mese - 1].num_giorni)
                    scad_pal_giorno = InfoMese[scad_pal_mese - 1].num_giorni;
                break;
                ;

            case 117:
                scad_pal_mese = x_mese; // UN ANNO ( meno un giorno...)
                scad_pal_giorno = x_giorno - 1;
                if (scad_pal_giorno < 1)
                {
                    scad_pal_mese--;
                    if (scad_pal_mese < 1)
                        scad_pal_mese += 12;
                    scad_pal_giorno = InfoMese[scad_pal_mese - 1].num_giorni;
                }
                break;
                ;
            }

            Evento(hDlg);
            AggiornaPalestra(hDlg);
            return (TRUE);

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

void RunTabacchi(HWND hDlg)
{
    FARPROC lpproc;

    if (x_vacanza != 2)
    { // 19 Mar 98 - Tabaccaio
        lpproc = MakeProcInstance(Tabaccaio, hInst);
        DialogBox(hInst,
                  MAKEINTRESOURCE(TABACCAIO),
                  hDlg,
                  lpproc);
        FreeProcInstance(lpproc);
    }
    else
    {
        MessageBox(hDlg,
                   "Rimani fisso a guardare la saracinesca del tabaccaio inrimediabilmente chiusa...",
                   "Bar Tabacchi", MB_OK | MB_ICONINFORMATION);
    }
}

void RunPalestra(HWND hDlg)
{
    FARPROC lpproc;

    if (x_vacanza != 2)
    { // 20 Mar 98 - Palestra
        lpproc = MakeProcInstance(Palestra, hInst);
        DialogBox(hInst,
                  MAKEINTRESOURCE(PALESTRA),
                  hDlg,
                  lpproc);
        FreeProcInstance(lpproc);
    }
    else
    {
        MessageBox(hDlg,
                   "Il tuo fisico da atleta dovra' aspettare... visto che oggi la palestra e' chiusa...",
                   "Palestra", MB_OK | MB_ICONINFORMATION);
    }
}

void RunVestiti(HWND hDlg, int numero)
{
    FARPROC lpproc;
    char    tmp[128];

    // Versione femminile di "Bau House" e "Blue Rider"
    if ((numero == 80) && (sesso == 'F'))
        numero = 85;
    if ((numero == 84) && (sesso == 'F'))
        numero = 86;

    if (x_vacanza != 2)
    { // 26 Feb 98... finalmente i negozi sono chiusi durante le vacanze...
        if (sound_active)
        {
            if (numero < 82)
                TabbozPlaySound(204);
            else
                TabbozPlaySound(205);
        }

        lpproc = MakeProcInstance(CompraQualcosa, hInst); // La funzione e' uguale x tutti...
        DialogBox(hInst,
                  MAKEINTRESOURCE(numero),
                  hDlg, lpproc);
        FreeProcInstance(lpproc);
    }
    else
    {
        sprintf(tmp, "Oh, tip%c... i negozi sono chiusi di festa...", ao);
        MessageBox(hDlg, tmp,
                   "Vestiti", MB_OK | MB_ICONINFORMATION);
    }
}

/********************************************************************/
/* EVENTI PALESTRA - 14 Luglio 1998                                 */
/********************************************************************/

void EventiPalestra(HANDLE hInstance)
{
    int  i;
    char messaggio[128];

    i = random(29 + (Fortuna / 2));

    if (i > 9)
        return; /* eventi: 0 - 10) */

    LoadString(hInst, (1100 + i), (LPSTR)messaggio, 255);

    MessageBox(hInstance,
               (LPSTR)messaggio,
               "Palestra...", MB_OK | MB_ICONSTOP);

    if (Reputazione > 10)
        Reputazione -= 4;

#ifdef TABBOZ_DEBUG
    writelog("eventi: Evento riguardante la palestra");
#endif
}
