// Tabboz Simulator
// (C) Copyright 1997,1998 by Andrea Bonomi
// 30 Maggio 1999 - Conversione Tabbozzo -> Tabbozza

/*
     This file is part of Tabboz Simulator.

     Tabboz Simulator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Nome-Programma is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
     along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "os.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "zarrosim.h"
static char sccsid[] = "@(#)" __FILE__ " " VERSION " (Andrea Bonomi) " __DATE__;

BOOL FAR PASCAL RiparaScooter(HWND hDlg, WORD message, WORD wParam, LONG lParam);
BOOL FAR PASCAL VendiScooter(HWND hDlg, WORD message, WORD wParam, LONG lParam);
BOOL FAR PASCAL Concessionario(HWND hDlg, WORD message, WORD wParam, LONG lParam);
BOOL FAR PASCAL AcquistaScooter(HWND hDlg, WORD message, WORD wParam, LONG lParam);
BOOL FAR PASCAL TruccaScooter(HWND hDlg, WORD message, WORD wParam, LONG lParam);
BOOL FAR PASCAL CompraUnPezzo(HWND hDlg, WORD message, WORD wParam, LONG lParam);

/* Il 28 Aprile 1998 la procedura scooter e' cambiata radicalmente... */
extern void  AggiornaScooter(HWND hDlg);
extern void  CalcolaVeelocita(HWND hDlg);
extern char *MostraSpeed(void);

static char *n_carburatore[] = {"12/10", "16/16", "19/19", "20/20", "24/24", "custom"};
static char *n_cc[] = {"50cc", "70cc", "90cc", "120cc", "150cc", "3969cc"};
static char *n_marmitta[] = {"standard", "silenziosa", "rumorosa", "rumorosissima"};
static char *n_filtro[] = {"standard", "P1", "P2", "P2+", "Extreme"};

char *n_attivita[] = {"mancante", "funzionante", "ingrippato", "invasato", "parcheggiato", "sequestrato", "a secco"};

static int tabella[] =
    {65, 70, -100, -100, -100, -100,
     70, 80, 95, -100, -100, -100,
     -1000, 90, 100, 115, -100, -100,
     -1000, -1000, 110, 125, 135, -100,
     -1000, -1000, -1000, 130, 150, -100,
     -1000, -1000, -1000, -1000, -1000, 250};

char showscooter;

NEWSTSCOOTER ScooterMem[] =
    {
        {0,   0, 0, 0, 0, 0,    0, -1,  "Nessuno scooter",           0 },
        {65,  0, 0, 0, 0, 2498, 1, 100, "Magutty Firecow",           5 },
        {75,  0, 1, 1, 1, 4348, 1, 100, "Honda F98",                 10},
        {105, 1, 1, 2, 1, 6498, 1, 100, "Mizzubisci R200 Millenium", 15},

        {75,  0, 0, 1, 1, 4298, 1, 100, "Magutty Firecow+",          7 },
        {100, 0, 1, 2, 1, 5998, 1, 100, "Magutty Firecow II",        10},
        {100, 0, 1, 2, 1, 6348, 1, 100, "Honda F98s",                13},

        {250, 0, 5, 5, 0, 1450, 1, 100, "Lexux LS400 ",              60}
};

int benzina;
int antifurto;

int PezziMem[] =
    {
        400, 500, 600,       /* marmitte    */
        300, 470, 650, 800,  /* carburatori */
        200, 400, 800, 1000, /* cc          */
        50, 120, 270, 400    /* filtro      */
};

// ******************************************************************
// Calcola la velocita' massima dello scooter, sencodo il tipo di marmitta,
// carburatore, etc...

void CalcolaVelocita(HWND hDlg)
{
    /* 28 Novembre 1998 0.81pr Bug ! Se lo scooter era ingrippato, cambiando il filtro
    dell' aria o la marmitta la velocita' diventava un numero negativo... */

    char buf[128];

    ScooterData.speed = (ScooterData.marmitta * 5) + (ScooterData.filtro * 5) + tabella[ScooterData.cc + (ScooterData.carburatore * 6)];

    ScooterData.attivita = 1; /* Sano...    */

    /* 26 Novembre 1998 0.81pr  Arggg!!! Un bug !!! */
    /* Se lo scooter sta' per essere comprato, non fa' questi noiosi check... */

    if (!showscooter)
    {
        if (ScooterData.speed <= -500)
            ScooterData.attivita = 3; /* Invasato   */
        else if (ScooterData.speed <= -1)
            ScooterData.attivita = 2; /* Ingrippato */

        if (benzina < 1)
            ScooterData.attivita = 6; /* A secco    */

        if (ScooterData.attivita != 1)
        {
            sprintf(buf, "Il tuo scooter e' %s.", n_attivita[ScooterData.attivita]);
            MessageBox(hDlg, buf, "Attenzione", MB_OK | MB_ICONINFORMATION);
            return;
        }
    }
}

/********************************************************************/
/* Scooter...                                                       */
/********************************************************************/

#pragma argsused
BOOL FAR PASCAL Scooter(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char    buf[128];
    char    tmp[128];
    FARPROC lpproc;

    if (message == WM_INITDIALOG)
    {
        SetDlgItemText(hDlg, 104, MostraSoldi(Soldi));

        sprintf(tmp, "Parcheggia scooter");
        SetDlgItemText(hDlg, 105, tmp); /* 7 Maggio 1998 */

        if (ScooterData.stato != -1)
        {
            AggiornaScooter(hDlg);
            if (ScooterData.attivita == 4)
            {
                sprintf(tmp, "Usa scooter");
                SetDlgItemText(hDlg, 105, tmp);
            }
        }

        return (TRUE);
    }

    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 101:
            if (x_vacanza == 2)
            {
                sprintf(tmp, "Oh, tip%c... oggi il concessionario e' chiuso...", ao);
                MessageBox(hDlg,
                           tmp,
                           "Concessionario", MB_OK | MB_ICONINFORMATION);
                return (TRUE);
            }

            lpproc = MakeProcInstance(Concessionario, hInst);
            DialogBox(hInst,
                      MAKEINTRESOURCE(ACQUISTASCOOTER),
                      hDlg,
                      lpproc);
            FreeProcInstance(lpproc);
            Evento(hDlg);
            AggiornaScooter(hDlg);
            return (TRUE);

        case 102: /* Trucca */
            if (ScooterData.stato != -1)
            {
                if (x_vacanza != 2)
                {
                    /* 28 Aprile 1998 La procedura per truccare gli scooter cambia completamente... */
                    lpproc = MakeProcInstance(TruccaScooter, hInst);
                    DialogBox(hInst,
                              MAKEINTRESOURCE(73),
                              hDlg,
                              lpproc);

                    FreeProcInstance(lpproc);
                    Evento(hDlg);
                    AggiornaScooter(hDlg);

                    return (TRUE);
                }
                else
                {
                    sprintf(tmp, "Oh, tip%c... oggi il meccanico e' chiuso...", ao);
                    MessageBox(hDlg, tmp,
                               "Trucca lo scooter", MB_OK | MB_ICONINFORMATION);
                }
            }
            else
                MessageBox(hDlg,
                           "Scusa, ma quale scooter avresti intenzione di truccare visto che non ne hai neanche uno ???",
                           "Trucca lo scooter", MB_OK | MB_ICONQUESTION);

            sprintf(tmp, "Parcheggia scooter"); /* 7 Maggio 1998 */
            SetDlgItemText(hDlg, 105, tmp);

            if (ScooterData.attivita == 4)
            {
                sprintf(tmp, "Usa scooter");
                SetDlgItemText(hDlg, 105, tmp);
            }

            Evento(hDlg);
            return (TRUE);

        case 103: /* Ripara */
            if (ScooterData.stato != -1)
            {
                if (ScooterData.stato == 100)
                    MessageBox(hDlg,
                               "Che motivi hai per voleer riparare il tuo scooter\nvisto che e' al 100% di efficienza ???",
                               "Ripara lo scooter", MB_OK | MB_ICONQUESTION);
                else
                {
                    if (x_vacanza != 2)
                    {
                        lpproc = MakeProcInstance(RiparaScooter, hInst);
                        DialogBox(hInst,
                                  MAKEINTRESOURCE(RIPARASCOOTER),
                                  hDlg, lpproc);
                        FreeProcInstance(lpproc);
                        AggiornaScooter(hDlg);
                    }
                    else
                    {
                        sprintf(tmp, "Oh, tip%c... oggi il meccanico e' chiuso...", ao);
                        MessageBox(hDlg, tmp,
                                   "Ripara lo scooter", MB_OK | MB_ICONINFORMATION);
                    }
                }
                return (TRUE);
            }
            else
                MessageBox(hDlg,
                           "Mi spieghi come fai a farti riparare lo scooter se non lo hai ???",
                           "Ripara lo scooter", MB_OK | MB_ICONQUESTION);
            Evento(hDlg);
            return (TRUE);

        case 105: /* Parcheggia / Usa Scooter	7 Maggio 1998 */
            if (ScooterData.stato < 0)
            {
                MessageBox(hDlg,
                           "Mi spieghi come fai a parcheggiare lo scooter se non lo hai ???",
                           "Parcheggia lo scooter", MB_OK | MB_ICONQUESTION);
                return (TRUE);
            }

            switch (ScooterData.attivita)
            {
            case 1:
                ScooterData.attivita = 4;
                sprintf(tmp, "Usa scooter");
                SetDlgItemText(hDlg, 105, tmp);
                break;
            case 4:
                ScooterData.attivita = 1;
                sprintf(tmp, "Parcheggia scooter");
                SetDlgItemText(hDlg, 105, tmp);
                break;
            default:
                sprintf(buf, "Mi spieghi come fai a parcheggiare lo scooter visto che e' %s ???", n_attivita[ScooterData.attivita]);
                MessageBox(hDlg,
                           buf,
                           "Parcheggia lo scooter", MB_OK | MB_ICONQUESTION);
            };

            AggiornaScooter(hDlg);
            return (TRUE);

        case 106: /* Fai Benzina	8 Maggio 1998 */
            if (ScooterData.stato < 0)
            {
                MessageBox(hDlg,
                           "Mi spieghi come fai a far benzina allo scooter se non lo hai ???",
                           "Fai benza", MB_OK | MB_ICONQUESTION);
                return (TRUE);
            }

            switch (ScooterData.attivita)
            {
            case 1:
            case 2:
            case 3:
            case 6:
                if (Soldi < 10)
                {
                    sprintf(buf, "Al distributore automatico puoi fare un minimo di %s di benzina...", MostraSoldi(10));
                    MessageBox(hDlg,
                               buf,
                               "Fai benza", MB_OK | MB_ICONQUESTION);
                    break;
                }

                Soldi -= 10;
#ifdef TABBOZ_DEBUG
                sprintf(tmp, "scooter: Paga benzina (%s)", MostraSoldi(10));
                writelog(tmp);

#endif

                benzina = 50; /* 5 litri, il massimo che puo' contenere... */

                if (ScooterData.cc == 5)
                    benzina = 850; /* 85 litri, x la macchinina un po' figa... */
                showscooter = 0;
                CalcolaVelocita(hDlg);

                sprintf(buf, "Fai %s di benzina e riempi lo scooter...", MostraSoldi(10));
                MessageBox(hDlg,
                           buf,
                           "Fai benza", MB_OK | MB_ICONINFORMATION);

                break;

            default:
                sprintf(buf, "Mi spieghi come fai a far benzina allo scooter visto che e' %s ???", n_attivita[ScooterData.attivita]);
                MessageBox(hDlg,
                           buf,
                           "Fai benza", MB_OK | MB_ICONQUESTION);
            };

            AggiornaScooter(hDlg);
            Evento(hDlg);
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

/********************************************************************/
/* Acquista Scooter                                                 */
/********************************************************************/

#pragma argsused
BOOL FAR PASCAL AcquistaScooter(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    int                 num_moto;
    static NEWSTSCOOTER ScooterTemp;

    if (message == WM_INITDIALOG)
    {
        scelta = -1;
        ScooterTemp = ScooterData;
        SetDlgItemText(hDlg, 104, MostraSoldi(Soldi));
        return (TRUE);
    }
    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 121:
        case 122:
        case 123:
        case 124:
        case 125:
        case 126:
            num_moto = wParam - 120;
            scelta = num_moto;
            ScooterData = ScooterMem[num_moto];
            showscooter = 1;
            CalcolaVelocita(hDlg);
            showscooter = 0;

            AggiornaScooter(hDlg);
            ScooterData = ScooterMem[0];
            return (TRUE);

        case IDCANCEL:
            scelta = -1;
            ScooterData = ScooterTemp;
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case IDOK:
            ScooterData = ScooterTemp;
            EndDialog(hDlg, TRUE);
            return (TRUE);

        default:
            return (TRUE);
        }
    }

    return (FALSE);
}

/********************************************************************/
/* Vendi Scooter                                                    */
/********************************************************************/

#pragma argsused
BOOL FAR PASCAL VendiScooter(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char        tmp[128]; /* Arghhhh ! fino alla 0.8.0 qui c'era un 30 che faceva crashiare tutto !!!! */
    static long offerta;  /* importante lo static !!! */
    ldiv_t      lx;

    if (message == WM_INITDIALOG)
    {
        lx = ldiv(ScooterData.prezzo, 100L);
        if ((ScooterData.attivita == 1) || (ScooterData.attivita == 4)) /* 0.8.1pr 28 Novembre 1998 - Se lo scooter e' sputtanato, vale meno... */
            offerta = (lx.quot * (ScooterData.stato - 10 - random(10)));
        else
            offerta = (lx.quot * (ScooterData.stato - 50 - random(10)));

        if (offerta < 50)
            offerta = 50; /* se vale meno di 50.000 nessuno lo vuole... */
                          /* 0.8.1pr 28 Novembre 1998 - se vale meno di 50.000, viene pagato 50.000      */

        AggiornaScooter(hDlg);

        SetDlgItemText(hDlg, 118, MostraSoldi(offerta));
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
            ScooterData = ScooterMem[0]; /* nessuno scooter			*/
            benzina = 0;                 /* serbatoio vuoto	7 Maggio 1998	*/
            ScooterData.stato = -1;

            Soldi = Soldi + offerta;

#ifdef TABBOZ_DEBUG
            sprintf(tmp, "scooter: Vendi lo scooter per %s", MostraSoldi(offerta));
            writelog(tmp);
#endif

            EndDialog(hDlg, TRUE);
            return (TRUE);

        default:
            return (TRUE);
        }
    }

    return (FALSE);
}

/********************************************************************/
/* Ripara Scooter                                                   */
/********************************************************************/

#pragma argsused
BOOL FAR PASCAL RiparaScooter(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char        tmp[128];
    static long costo; // Importante lo static !!!

    if (message == WM_INITDIALOG)
    {
        // Calcola il costo della riparazione dello scooter...
        costo = (ScooterData.prezzo / 100 * (100 - ScooterData.stato)) + 10;

        SetDlgItemText(hDlg, 104, MostraSoldi(Soldi));
        SetDlgItemText(hDlg, 105, MostraSoldi(costo));

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

            if (costo > Soldi)
                nomoney(hDlg, SCOOTER);
            else
            {

#ifdef TABBOZ_DEBUG
                sprintf(tmp, "scooter: Paga riparazione (%s)", MostraSoldi(costo));
                writelog(tmp);
#endif

                // Per questa cagata, crascia il tabboz all' uscita...
                //					if (sound_active) TabbozPlaySound(102);

                ScooterData.stato = 100;
                Soldi -= costo;
                CalcolaVelocita(hDlg);
            }
            EndDialog(hDlg, TRUE);
            return (TRUE);

        default:
            return (TRUE);
        }
    }

    return (FALSE);
}

/********************************************************************/
/* Trucca Scooter                                                   */
/* 28 Aprile 1998 La procedura per truccare gli scooter cambia completamente... */
/********************************************************************/

#pragma argsused
BOOL FAR PASCAL TruccaScooter(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    FARPROC lpproc;

    if (message == WM_INITDIALOG)
    {
        if (sound_active)
            TabbozPlaySound(101);
        AggiornaScooter(hDlg);
        return (TRUE);
    }
    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 121:
        case 122:
        case 123:
        case 124:
            lpproc = MakeProcInstance(CompraUnPezzo, hInst);
            DialogBox(hInst,
                      MAKEINTRESOURCE(wParam - 121 + 74),
                      hDlg, lpproc);
            FreeProcInstance(lpproc);
            /* CalcolaVelocita(hDlg); */
            AggiornaScooter(hDlg);
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

void AggiornaScooter(HWND hDlg)
{
    char  tmp[128];
    div_t d;
    SetDlgItemText(hDlg, 104, MostraSoldi(Soldi));

    if (ScooterData.stato != -1)
    {
        sprintf(tmp, "%s", ScooterData.nome);
        SetDlgItemText(hDlg, 116, tmp);
        d = div(benzina, 10);
        sprintf(tmp, "%d.%dl", d.quot, d.rem);
        SetDlgItemText(hDlg, 107, tmp);

        SetDlgItemText(hDlg, 110, MostraSpeed());
        SetDlgItemText(hDlg, 111, n_marmitta[ScooterData.marmitta]);
        SetDlgItemText(hDlg, 112, n_carburatore[ScooterData.carburatore]);
        SetDlgItemText(hDlg, 113, n_cc[ScooterData.cc]);
        SetDlgItemText(hDlg, 114, n_filtro[ScooterData.filtro]);
        sprintf(tmp, "%d%", ScooterData.stato);
        SetDlgItemText(hDlg, 115, tmp);

        SetDlgItemText(hDlg, 117, MostraSoldi(ScooterData.prezzo));
    }
    else
    {
        SetDlgItemText(hDlg, 107, "");
        SetDlgItemText(hDlg, 110, "");
        SetDlgItemText(hDlg, 111, "");
        SetDlgItemText(hDlg, 112, "");
        SetDlgItemText(hDlg, 113, "");
        SetDlgItemText(hDlg, 114, "");
        SetDlgItemText(hDlg, 115, "");
        SetDlgItemText(hDlg, 116, "");
        SetDlgItemText(hDlg, 117, "");
    }
}

// -----------------------------------------------------------------------
// Routine di acquisto generika di un pezzo di motorino
// -----------------------------------------------------------------------

#pragma argsused
BOOL FAR PASCAL CompraUnPezzo(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char tmp[128];
    int  i;

    if (message == WM_INITDIALOG)
    {
        sprintf(tmp, "%s", ScooterData.nome);
        SetDlgItemText(hDlg, 109, tmp);
        SetDlgItemText(hDlg, 105, n_carburatore[ScooterData.carburatore]);
        SetDlgItemText(hDlg, 106, n_marmitta[ScooterData.marmitta]);
        SetDlgItemText(hDlg, 107, n_cc[ScooterData.cc]);
        SetDlgItemText(hDlg, 108, n_filtro[ScooterData.filtro]);

        SetDlgItemText(hDlg, 104, MostraSoldi(Soldi));

        for (i = 110; i < 125; i++)
        {
            SetDlgItemText(hDlg, i, MostraSoldi(PezziMem[i - 110]));
        }

        return (TRUE);
    }

    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 130: /* marmitte ----------------------------------------------------------- */
        case 131:
        case 132:
            if (Soldi < PezziMem[wParam - 130])
            {
                nomoney(hDlg, SCOOTER);
                return (TRUE);
            }
            Soldi -= PezziMem[wParam - 130];
#ifdef TABBOZ_DEBUG
            sprintf(tmp, "scooter: Paga marmitta (%s)", MostraSoldi(PezziMem[wParam - 130]));
            writelog(tmp);
#endif

            ScooterData.marmitta = (wParam - 129); /* (1 - 3 ) */
            showscooter = 0;
            CalcolaVelocita(hDlg);
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case 133: /* carburatore -------------------------------------------------------- */
        case 134:
        case 135:
        case 136:
            if (Soldi < PezziMem[wParam - 130])
            {
                nomoney(hDlg, SCOOTER);
                return (TRUE);
            }
            Soldi -= PezziMem[wParam - 130];
#ifdef TABBOZ_DEBUG
            sprintf(tmp, "scooter: Paga carburatore (%s)", MostraSoldi(PezziMem[wParam - 130]));
            writelog(tmp);
#endif

            ScooterData.carburatore = (wParam - 132); /* ( 1 - 4 ) */
            showscooter = 0;
            CalcolaVelocita(hDlg);
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case 137: /* cc ----------------------------------------------------------------- */
        case 138:
        case 139:
        case 140:
            if (Soldi < PezziMem[wParam - 130])
            {
                nomoney(hDlg, SCOOTER);
                return (TRUE);
            }
            Soldi -= PezziMem[wParam - 130];
#ifdef TABBOZ_DEBUG
            sprintf(tmp, "scooter: Paga cilindro e pistone (%s)", MostraSoldi(PezziMem[wParam - 130]));
            writelog(tmp);
#endif

            /* Piccolo bug della versione 0.6.91, qui c'era scritto ScooterData.marmitta */
            /* al posto di ScooterData.cc :-) */
            ScooterData.cc = (wParam - 136); /* ( 1 - 4 ) */
            showscooter = 0;
            CalcolaVelocita(hDlg);
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case 141: /* filtro dell' aria -------------------------------------------------- */
        case 142:
        case 143:
        case 144:
            if (Soldi < PezziMem[wParam - 130])
            {
                nomoney(hDlg, SCOOTER);
                return (TRUE);
            }
            Soldi -= PezziMem[wParam - 130];
#ifdef TABBOZ_DEBUG
            sprintf(tmp, "scooter: Paga filtro dell' aria (%s)", MostraSoldi(PezziMem[wParam - 130]));
            writelog(tmp);
#endif

            ScooterData.filtro = (wParam - 140); /* (1 - 4) */
            showscooter = 0;
            CalcolaVelocita(hDlg);
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return (TRUE);

        default:
            return (TRUE);
        }
    }

    return (FALSE);
}

// -----------------------------------------------------------------------

char *MostraSpeed(void)
{
    static char tmp[128];

    switch (ScooterData.attivita)
    {
    case 1:
        sprintf(tmp, "%dKm/h", ScooterData.speed);
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        sprintf(tmp, "(%s)", n_attivita[ScooterData.attivita]);
        break;
    default:
        sprintf(tmp, "");
    };

    return tmp;
}

// -----------------------------------------------------------------------
// Concessionario...  7 Maggio 1998
// -----------------------------------------------------------------------

#pragma argsused
BOOL FAR PASCAL Concessionario(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char    tmp[128];
    FARPROC lpproc;

    if (message == WM_INITDIALOG)
    {
        SetDlgItemText(hDlg, 104, MostraSoldi(Soldi));
        return (TRUE);
    }

    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 101: /* Compra Scooter Malagutty	  */
        case 102: /* Compra Scooter di altre marche */
            scelta = -1;
            lpproc = MakeProcInstance(AcquistaScooter, hInst);
            DialogBox(hInst,
                      MAKEINTRESOURCE(wParam - 101 + 78),
                      hDlg,
                      lpproc);
            FreeProcInstance(lpproc);
            if (scelta != -1)
            {
                if (ScooterData.stato != -1)
                {
                    sprintf(tmp, "Per il tuo vecchio scooter da rottamare ti diamo %s di supervalutazione...", MostraSoldi(1000));
                    MessageBox(hDlg,
                               tmp,
                               "Incentivi", MB_OK | MB_ICONINFORMATION);
                    Soldi += 1000;
#ifdef TABBOZ_DEBUG
                    sprintf(tmp, "scooter: Imcentivo rottamazione %s", MostraSoldi(1000));
                    writelog(tmp);
#endif

                    ScooterData = ScooterMem[0]; /* nessuno scooter			*/
                    benzina = 0;                 /* serbatoio vuoto	7 Maggio 1998	*/
                    ScooterData.stato = -1;
                }
                if (ScooterMem[scelta].prezzo > Soldi)
                {
                    MessageBox(hDlg,
                               "Ti piacerebbe comprare lo scooter, vero ?\nPurtroppo, non hai abbastanza soldi...",
                               "Non hai abbastanza soldi", MB_OK | MB_ICONSTOP);
                    if (Reputazione > 3)
                        Reputazione -= 1;
                }
                else
                {
                    Soldi -= ScooterMem[scelta].prezzo;
#ifdef TABBOZ_DEBUG
                    sprintf(tmp, "scooter: Acquista uno scooter per %s", MostraSoldi(ScooterMem[scelta].prezzo));
                    writelog(tmp);
#endif

                    ScooterData = ScooterMem[scelta];
                    benzina = 20;
                    MessageBox(hDlg,
                               "Fai un giro del quartiere per farti vedere con lo scooter nuovo...",
                               "Lo scooter nuovo", MB_OK | MB_ICONINFORMATION);
                    Reputazione += 4;
                    if (Reputazione > 100)
                        Reputazione = 100;
                }
                Evento(hDlg);
            }
            SetDlgItemText(hDlg, 104, MostraSoldi(Soldi));
            return (TRUE);

        case 103:
            if (ScooterData.stato != -1)
            {
                lpproc = MakeProcInstance(VendiScooter, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(VENDISCOOTER),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);
            }
            else
                MessageBox(hDlg,
                           "Scusa, ma quale scooter avresti intenzione di vendere visto che non ne hai neanche uno ???",
                           "Vendi lo scooter", MB_OK | MB_ICONQUESTION);

            SetDlgItemText(hDlg, 104, MostraSoldi(Soldi));
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
