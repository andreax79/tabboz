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

static int  spostamento;
static char descrizione[30];
int         figTemp;
char        nomeTemp[30];
HWND        tipahDlg; // Serve per BMPTipaWndProc (in Tabimg.c )

BOOL FAR PASCAL CercaTipa(HWND hDlg, WORD message, WORD wParam, LONG lParam);
BOOL FAR PASCAL DueDiPicche(HWND hDlg, WORD message, WORD wParam, LONG lParam);
void            DescrizioneTipa(int f);
void            DescrizioneTipo(int f);
long FAR PASCAL BMPTipaWndProc(HWND hWnd, WORD msg,
                               WORD wParam, LONG lParam);

// ------------------------------------------------------------------------------------------
// Tipa...
// ------------------------------------------------------------------------------------------

#pragma argsused
BOOL FAR PASCAL Tipa(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char    buf[128];
    char    tmp[128];
    FARPROC lpproc;
    int     lasciaoraddoppia;

    if (message == WM_INITDIALOG)
    {
        if (sesso == 'M')
            spostamento = 0;
        else
            spostamento = 100;
        AggiornaTipa(hDlg);
        tipahDlg = hDlg;
        return (TRUE);
    }

    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 110: // Cerca tipa
            lpproc = MakeProcInstance(CercaTipa, hInst);
            DialogBox(hInst,
                      MAKEINTRESOURCE(CERCATIPA + spostamento),
                      hDlg,
                      lpproc);
            FreeProcInstance(lpproc);

            AggiornaTipa(hDlg);
            return (TRUE);

        case 111: // Lascia tipa
            if (Rapporti <= 0)
            {
                if (sesso == 'M')
                    MessageBox(hDlg,
                               "Scusa, che ragazza avresti intenzione di lasciare ???",
                               "Lascia Tipa", MB_OK | MB_ICONINFORMATION);
                else
                    MessageBox(hDlg,
                               "Scusa, che tipo avresti intenzione di lasciare, visto che sei sola come un cane ???",
                               "Lascia Tipo", MB_OK | MB_ICONINFORMATION);
                return (TRUE);
            }

            sprintf(tmp, "Sei proprio sicuro di voler lasciare %s ?", Nometipa);

            if (sesso == 'M')
                lasciaoraddoppia = MessageBox(hDlg,
                                              tmp,
                                              "Lascia tipa", MB_YESNO | MB_ICONQUESTION);
            else
                lasciaoraddoppia = MessageBox(hDlg,
                                              tmp,
                                              "Molla tipo", MB_YESNO | MB_ICONQUESTION);

            if (lasciaoraddoppia == IDYES)
            {
                if (sound_active)
                    TabbozPlaySound(603);
                Rapporti = 0;

                if ((FigTipa >= 79) && (sesso == 'M'))
                {
                    MessageBox(hDlg,
                               "Appena vengono a sapere quello che hai fatto, i tuoi amici ti prendono a scarpate.\nQualcuno, piu' furbo di te, va a consolarla...",
                               "Idiota...", MB_OK | MB_ICONINFORMATION);
                    Reputazione -= 8;
                    if (Reputazione < 0)
                        Reputazione = 0;
                }

                if ((FigTipa <= 40) && (sesso == 'M'))
                {
                    Reputazione += 4;
                    if (Reputazione > 100)
                        Reputazione = 100;
                }

                Evento(hDlg);
            }
            AggiornaTipa(hDlg);
            return (TRUE);

        case 112:
            if (Rapporti <= 0)
            {
                if (sesso == 'M')
                    MessageBox(hDlg,
                               "Scusa, che ragazza vorresti chiamare ???",
                               "Non sei molto intelligente...", MB_OK | MB_ICONINFORMATION);
                else
                    MessageBox(hDlg,
                               "Scusa, che ragazzo vorresti chiamare, visto che sei sola ???",
                               "Non sei molto intelligente...", MB_OK | MB_ICONINFORMATION);
                return (TRUE);
            }

            if ((Soldi <= 5) && ((AbbonamentData.creditorest < 2) && (CellularData.stato < 0)))
            {
                MessageBox(hDlg,
                           ""
                           "Sei fai ancora una telefonata, ti spezzo le gambe"
                           ", disse tuo padre con un accetta in mano...",
                           "Non toccare quel telefono...", MB_OK | MB_ICONSTOP);
            }
            else
            {
                if (sound_active)
                    TabbozPlaySound(602);
                // 5 Maggio 1999 - Telefono di casa o telefonino ???

                if ((AbbonamentData.creditorest >= 2) && (CellularData.stato > -1))
                    AbbonamentData.creditorest -= 2;
                else
                    Soldi -= 5;

#ifdef TABBOZ_DEBUG
                sprintf(tmp, "tipa: Telefona alla tipa//o (%s)", MostraSoldi(5));
                writelog(tmp);
#endif
                if (Rapporti <= 60)
                    Rapporti++;
            }
            AggiornaTipa(hDlg);
            return (TRUE);

        case 113:
            if (Rapporti <= 0)
            {
                if (sesso == 'M')
                    MessageBox(hDlg,
                               "Scusa, con che tipa vorresti uscire ???",
                               "Non sei molto intelligente...", MB_OK | MB_ICONINFORMATION);
                else
                    MessageBox(hDlg,
                               "Scusa, ma con chi vorresti uscire, ???",
                               "Non sei molto intelligente...", MB_OK | MB_ICONINFORMATION);
                return (TRUE);
            }

            if ((ScooterData.stato <= 0) && (sesso == 'M'))
            {
                MessageBox(hDlg,
                           "Finche' non comprerai lo scooter, non usciremo piu' insieme...",
                           "Compra lo scooter", MB_OK | MB_ICONSTOP);
                return (TRUE);
            }

            if ((ScooterData.attivita != 1) && (sesso == 'M'))
            {
                sprintf(buf, "Finche' il tuo scooter restera' %s non potremo uscire insieme...", n_attivita[ScooterData.attivita]);
                MessageBox(hDlg, buf, "Risistema la scooter", MB_OK | MB_ICONINFORMATION);
                return (TRUE);
            }

            if ((ScooterData.stato <= 35) && (sesso == 'M'))
            {
                MessageBox(hDlg,
                           "Finche' non riparerai lo scooter, non usciremo piu' insieme...",
                           "Ripara lo scooter", MB_OK | MB_ICONSTOP);
                return (TRUE);
            }

            if (Soldi < 15)
            {
                if (sesso == 'M')
                    MessageBox(hDlg,
                               "Se mi vuoi portare fuori, cerca di avere almeno un po' di soldi...",
                               "Sei povero", MB_OK | MB_ICONSTOP);
                else
                    MessageBox(hDlg,
                               "Oh tipa... cioe' non posso pagare sempre tutto io, cioe' ohhhh...",
                               "Che palle", MB_OK | MB_ICONSTOP);
                return (TRUE);
            }

            Soldi -= 15;
#ifdef TABBOZ_DEBUG
            sprintf(tmp, "tipa: Esci con la tipa/o (%s)", MostraSoldi(15));
            writelog(tmp);
#endif

            Rapporti += 5;
            if (Rapporti > 100)
                Rapporti = 100;

            if (FigTipa > Fama)
                Fama++;
            if (Fama > 100)
                Fama = 100;

            benzina -= 3;
            if (benzina < 1)
                benzina = 0;
            CalcolaVelocita(hDlg);

            AggiornaTipa(hDlg);
            return (TRUE);

        case IDCANCEL:
        case IDOK:
            tipahDlg = 0; // Non si sa' mai...
            EndDialog(hDlg, TRUE);
            return (TRUE);

        default:
            return (TRUE);
        }
    }

    return (FALSE);
}

// ------------------------------------------------------------------------------------------
// Due Donne - 22 Aprile 1999
// ------------------------------------------------------------------------------------------

#pragma argsused
BOOL FAR PASCAL DueDonne(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char tmp[255];

    if (message == WM_INITDIALOG)
    {
        sprintf(tmp, "Resto con %s", Nometipa);
        SetDlgItemText(hDlg, 2, tmp);
        if (!strcmp(Nometipa, nomeTemp))
        { // Se le tipe si chiamano tutte e due con lo stesso nome
            if (sesso == 'M')
                sprintf(tmp, "Preferisco quella nuova");
            else
                sprintf(tmp, "Preferisco quello nuovo");
        }
        else
            sprintf(tmp, "Preferisco %s", nomeTemp);
        SetDlgItemText(hDlg, 102, tmp);
        return (TRUE);
    }
    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 101: // Ottima scelta...
            if (sesso == 'M')
                sprintf(tmp, "Mentre sei appartato con la %s, arriva la tua ragazza, %s, ti tira uno schiaffo e ti lascia.\
Capendo finalmente di che pasta sei fatto, anche la %s si allontana...",
                        nomeTemp, Nometipa, nomeTemp);
            else
                sprintf(tmp, "%s viene a sapere che di %s, gli spacca la faccia e ti molla...\
Dopo questa tragica esperienza anche %s sparisce...",
                        Nometipa, nomeTemp, nomeTemp);

            Rapporti = 0;
            Reputazione -= 8;
            if (Reputazione < 0)
                Reputazione = 0;
            Fama -= 4;
            if (Fama < 0)
                Fama = 0;

            MessageBox(hDlg,
                       tmp,
                       "La vita e' bella", MB_OK | MB_ICONSTOP);
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case 102: // Preferisci quella nuova...
            sprintf(Nometipa, "%s", nomeTemp);
            FigTipa = figTemp;
            Rapporti = 30 + random(15);
            Fama += FigTipa / 10;
            if (Fama > 100)
                Fama = 100;
            Reputazione += FigTipa / 13;
            if (Reputazione > 100)
                Reputazione = 100;
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case IDCANCEL: // Resti con la tua vecchia ragazza, bravo...
        default:
            EndDialog(hDlg, TRUE);
            return (TRUE);
        }
    }

    return (FALSE);
}

// ------------------------------------------------------------------------------------------
// Cerca Tipa
// ------------------------------------------------------------------------------------------

#pragma argsused
BOOL FAR PASCAL CercaTipa(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char       tmp[255];
    static int i;

    if (message == WM_INITDIALOG)
    {

        figTemp = random(71) + 30; // 30 -> 100

        if (sesso == 'M')
        {
            DescrizioneTipa(figTemp);
            i = 200 + random(20); // 200 -> 219 [nomi tipe]
        }
        else
        {
            DescrizioneTipo(figTemp);
            i = 1200 + random(20); // 1200 -> 1219 [nomi tipi]
        }

        SetDlgItemText(hDlg, 107, descrizione);
        LoadString(hInst, i, (LPSTR)nomeTemp, 30);

        sprintf(tmp, "%s", nomeTemp);
        SetDlgItemText(hDlg, 105, tmp);

        sprintf(tmp, "%d/100", figTemp);
        SetDlgItemText(hDlg, 106, tmp);

        return (TRUE);
    }

    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case 101:
            // Calcola se ce la fa o meno con la tipa... ------------------------------------

            if (((figTemp * 2) + random(50)) <= (Fama + Reputazione + random(30)))
            {
                // E' andata bene... ----------------------------------------------------

                if (sesso == 'M')
                    MessageBox(hDlg,
                               "Con il tuo fascino nascosto da tabbozzo, seduci la tipa e ti ci metti insieme.",
                               "E' andata bene !", MB_OK | MB_ICONINFORMATION);
                else
                    MessageBox(hDlg,
                               "Ora non ti puoi piu' lamentare di essere sola...",
                               "Qualcono ti caga...", MB_OK | MB_ICONINFORMATION);

                // ...ma comunque controlla che tu non abbia gia' una tipa -------------------------
                if (Rapporti > 0)
                { // hai gia' una tipa..
                    FARPROC lpproc = MakeProcInstance(DueDonne, hInst);
                    DialogBox(hInst,
                              MAKEINTRESOURCE(92 + spostamento),
                              hDlg,
                              lpproc);
                    FreeProcInstance(lpproc);
                }
                else
                { // bravo, no hai una tipa...
                    sprintf(Nometipa, "%s", nomeTemp);
                    FigTipa = figTemp;
                    Rapporti = 30 + random(15);
                    Fama += FigTipa / 10;
                    if (Fama > 100)
                        Fama = 100;
                    Reputazione += FigTipa / 13;
                    if (Reputazione > 100)
                        Reputazione = 100;
                }
            }
            else
            {
                FARPROC lpproc;

                // 2 di picche... -------------------------------------------------------
                if (sound_active)
                    TabbozPlaySound(601);

                lpproc = MakeProcInstance(DueDiPicche, hInst);
                DialogBox(hInst,
                          MAKEINTRESOURCE(95),
                          hDlg,
                          lpproc);
                FreeProcInstance(lpproc);
            }
            Evento(hDlg);
            EndDialog(hDlg, TRUE);
            return (TRUE);

        default:
            return (TRUE);
        }
    }

    return (FALSE);
}

// ------------------------------------------------------------------------------------------
// Abbina una descrizione(breve) alla figosita' di una tipa.
// ------------------------------------------------------------------------------------------

void DescrizioneTipa(int f)
{
    char buf[30];

    if (f > 97)
        sprintf(buf, "Ultramegafiga");
    else if (f > 90)
        sprintf(buf, "Fighissima");
    else if (f > 83)
        sprintf(buf, "Molto figa");
    else if (f > 72)
        sprintf(buf, "Figa");
    else if (f > 67)
        sprintf(buf, "Abbastanza Figa");
    else if (f > 55)
        sprintf(buf, "Interessante");
    else if (f > 45)
        sprintf(buf, "Passabile");
    else if (f > 35)
        sprintf(buf, "Puo' piacere..");
    else
        sprintf(buf, "E' un tipo...");

    strncpy(descrizione, buf, sizeof(buf));
}

// ------------------------------------------------------------------------------------------
// Abbina una descrizione(breve) alla figosita' di un tipo.
// ------------------------------------------------------------------------------------------

void DescrizioneTipo(int f)
{
    char buf[30];

    if (f > 97)
        sprintf(buf, "Ultramegafigo");
    else if (f > 90)
        sprintf(buf, "Bellissimo");
    else if (f > 83)
        sprintf(buf, "Molto figo");
    else if (f > 72)
        sprintf(buf, "Bello");
    else if (f > 67)
        sprintf(buf, "Abbastanza Figo");
    else if (f > 55)
        sprintf(buf, "Interessante");
    else if (f > 45)
        sprintf(buf, "Passabile");
    else if (f > 35)
        sprintf(buf, "Puo' piacere..");
    else
        sprintf(buf, "Inutile...");

    strncpy(descrizione, buf, sizeof(buf));
}

// ------------------------------------------------------------------------------------------
// 2 di picche (la vita e' bella...)
// ------------------------------------------------------------------------------------------

#pragma argsused
BOOL FAR PASCAL DueDiPicche(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char       tmp[1024];
    static int i;

    if (message == WM_INITDIALOG)
    {                     // un giorno fortunato...
        DDP++;            // log due di picche...
        Reputazione -= 2; // decremento reputazione
        if (Reputazione < 0)
            Reputazione = 0;

        Fama -= 2; // decremento figosita'
        if (Fama < 0)
            Fama = 0;

        // IN QUESTA PARTE C'ERA UN BUG CHE FACEVA CRASCIARE IL TABBOZ SIMULATOR...

        if (sesso == 'M')
            i = 300 + random(20); // 300 -> 319 [sfighe varie]
        else
            i = 1300 + random(20); // 300 -> 319 [sfighe varie]

        LoadString(hInst, i, (LPSTR)tmp, 1024);
        SetDlgItemText(hDlg, 105, tmp);

        i = 0;

        return (TRUE);
    }
    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 201:
            i++;
            if (i > 5)
            {
                sprintf(tmp, "Fino ad ora hai preso %lu due di picche !\nNon ti preoccupare, capita a tutti di prendere qualche due di picche nella vita ...", DDP);
                MessageBox(hDlg,
                           tmp, "La vita e' bella...", MB_OK | MB_ICONINFORMATION);
                i = 0;
            }
            return (TRUE);

        case IDCANCEL:
        case IDOK:
            EndDialog(hDlg, TRUE);
            return (TRUE);

        default:
            return (TRUE);
        }
    }

    return (FALSE);
}

// ------------------------------------------------------------------------------------------
// AggiornaTipa
// ------------------------------------------------------------------------------------------

void AggiornaTipa(HWND hDlg)
{
    char tmp[128];
    if (Rapporti == 0)
    {
        if (sesso == 'M')
            SetDlgItemText(hDlg, 101, "Cerca Tipa");
        SetDlgItemText(hDlg, 101, "Cerca Tipo");

        sprintf(tmp, "");
        SetDlgItemText(hDlg, 105, tmp);
        SetDlgItemText(hDlg, 106, tmp);
        SetDlgItemText(hDlg, 107, tmp);
    }
    else
    {
        if (sesso == 'M')
            SetDlgItemText(hDlg, 101, "Cerca Nuova Tipa");
        else
            SetDlgItemText(hDlg, 101, "Cerca Nuovo Tipo");
        sprintf(tmp, "%s", Nometipa);
        SetDlgItemText(hDlg, 105, tmp);
        sprintf(tmp, "%d", FigTipa);
        SetDlgItemText(hDlg, 106, tmp);
        sprintf(tmp, "%d/100", Rapporti);
        SetDlgItemText(hDlg, 107, tmp);
    }
    sprintf(tmp, "%d/100", Fama);
    SetDlgItemText(hDlg, 104, tmp);
}

// ------------------------------------------------------------------------------------------
// 4 gennaio 1999
// ------------------------------------------------------------------------------------------

#pragma argsused
BOOL FAR PASCAL MostraSalutieBaci(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    if (message == WM_INITDIALOG)
    {
        return (TRUE);
    }
    else if (message == WM_COMMAND)
    {

        switch (wParam)
        {

        case 205:
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
