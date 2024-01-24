// Tabboz Simulator
// (C) Copyright 1997-1999 by Andrea Bonomi

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

char                   messaggio[256];
BOOL FAR PASCAL        MostraMetallone(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL DueDonne(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern int             figTemp;
extern char            nomeTemp[];
// extern	int	AdV;

/********************************************************************/
/* EVENTI CASUALI...                                                */
/********************************************************************/

void Evento(HANDLE hInstance)
{
    int     caso, i;
    char    tmp[128];
    FARPROC lpproc;

    if (Fortuna < 0)
        Fortuna = 0; /* Prima che qualcuno bari... */
    if (Fortuna > 100)
        Fortuna = 100;

    // if ( (AdV - 1) != 0 ) return;

    Giorno(hInstance);

    if (Tempo_trascorso_dal_pestaggio > 0)
        Tempo_trascorso_dal_pestaggio--;

    /* Sigarette -------------------------------------------------------- */
    if (sizze > 0)
    {
        sizze--;
        if (sizze == 0)
        {
            MessageBox(hInstance,
                       "Apri il tuo pacchetto di sigarette e lo trovi disperatamente vuoto...",
                       "Sei senza sigarette !", MB_OK | MB_ICONSTOP);
            if (Reputazione > 10)
                Reputazione -= 3;
        }
        else if (sizze < 3)
            MessageBox(hInstance,
                       "Ti accorgi che stai per finire le tue sizze.",
                       "Sigarette...", MB_OK | MB_ICONINFORMATION);
    }

    /* Cellulare ----------------------------------------16 Apr 1999----- */

    if ((AbbonamentData.creditorest > 0) && (CellularData.stato > -1))
    {
        AbbonamentData.creditorest -= 1;
        if (Fama < 55)
            Fama++;
        if (AbbonamentData.creditorest == 0)
        {
            MessageBox(hInstance,
                       "Cerchi di telefonare e ti accorgi di aver finito i soldi a tua disposizione...",
                       "Telefonino", MB_OK | MB_ICONSTOP);
        }
        else if (AbbonamentData.creditorest < 3)
            MessageBox(hInstance,
                       "Ti accorgi che stai per finire la ricarica del tuo telefonino.",
                       "Telefonino", MB_OK | MB_ICONINFORMATION);
    }

    if (CellularData.stato == 1)
    { // Cellulate 'morente'...
        CellularData.stato = -1;
        MessageBox(hInstance,
                   "Dopo una vita di duro lavoro, a furia di prendere botte, il tuo cellulare si spacca...",
                   "Telefonino", MB_OK | MB_ICONSTOP);
    }

    /* Rapporti Tipa ---------------------------------------------------- */

    if (Rapporti > 3)
    {
        i = random(5) - 3;
        if (i > 0)
            Rapporti--;
    }

    if (Rapporti > 0)
        if (Rapporti < 98)
        {
            i = random(((Rapporti + Fortuna + Fama) * 3) + 1) + 1;
            if (i < 11)
            { /* da 1 a 10, la donna ti molla... */
                if (sound_active)
                    TabbozPlaySound(603);
                Rapporti = 0;
                FigTipa = 0;

                if (sesso == 'M')
                {
                    LoadString(hInst, (1040 + i), (LPSTR)messaggio, 255); /* 1041 -> 1050 */
                    MessageBox(hInstance,
                               (LPSTR)messaggio,
                               "La tipa ti molla...", MB_OK | MB_ICONSTOP);
                }
                else
                {
                    LoadString(hInst, (1340 + i), (LPSTR)messaggio, 255); /* 1041 -> 1050 */
                    MessageBox(hInstance,
                               (LPSTR)messaggio,
                               "Vieni mollata...", MB_OK | MB_ICONSTOP);
                }

                Reputazione -= (11 - i); // quelle con numero piu' basso, sono peggiori...
                if (Reputazione < 0)
                    Reputazione = 0;
            }
        }

    /* Lavoro ----------------------------------------------------------- */

    if (impegno > 3)
    {
        i = random(7) - 3;
        if (i > 0)
            impegno--;
    }

    if (numeroditta > 0)
    {
        i = random(impegno * 2 + Fortuna * 3);
        if (i < 2)
        { /* perdi il lavoro */
            impegno = 0;
            giorni_di_lavoro = 0;
            stipendio = 0;
            numeroditta = 0;

            // LoadString(hInst, (1040 + i), (LPSTR)messaggio, 255);  /* 1041 -> 1050 */
            // MessageBox( hInstance,
            //	(LPSTR)messaggio,
            //	"Perdi il lavoro...", MB_OK | MB_ICONSTOP);

            if (sound_active)
                TabbozPlaySound(504);

            sprintf(tmp, "Un bel giorno ti svegli e scopri di essere stat%c licenziat%c.", ao, ao);
            MessageBox(hInstance, tmp,
                       "Perdi il lavoro...", MB_OK | MB_ICONSTOP);
        }
    }

    /* Paghetta --------------------------------------------------------- */

    if (x_giornoset == 6)
    { /* Il Sabato c'e' la paghetta... */
        if (Studio >= 45)
        {
            Soldi += Paghetta;
#ifdef TABBOZ_DEBUG
            sprintf(tmp, "eventi: Paghetta (%s)", MostraSoldi(Paghetta));
            writelog(tmp);
#endif

            if (Studio >= 80)
            {
                if (sound_active)
                    TabbozPlaySound(1100);
                Soldi += Paghetta;
#ifdef TABBOZ_DEBUG
                writelog("eventi: Paghetta doppia !!!");
#endif

                MessageBox(hInstance,
                           "Visto che vai bene a scuola, ti diamo il doppio della paghetta...",
                           "Paghetta settimanale", MB_OK | MB_ICONINFORMATION);
            }
        }
        else
        {
            if (sound_active)
                TabbozPlaySound(1200);
            Soldi += (Paghetta / 2);
#ifdef TABBOZ_DEBUG
            sprintf(tmp, "eventi: Meta' paghetta (%s)...", MostraSoldi(Paghetta));
            writelog(tmp);
#endif

            MessageBox(hInstance,
                       "Finche' non andrai bene a scuola, ti daremo solo meta' della paghetta...",
                       "Paghetta settimanale", MB_OK | MB_ICONINFORMATION);
        }
    }

    /* Eventi casuali --------------------------------------------------- */
    caso = random(100 + (Fortuna * 2));

    //	caso = 21;	/* TEST - TEST - TEST - TEST - TEST - TEST - TEST */

#ifdef TABBOZ_DEBUG
    sprintf(tmp, "eventi: Evento casule n. %d", caso);
    writelog(tmp);
#endif

    if (caso < 51)
    {
        switch (caso)
        {

            // -------------- Metalloni e Manovali ---------------------------------------------------------------------

        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:

            if (sesso == 'F')
                break; // Se sei una tipa non vieni pestata...

            Reputazione -= caso;
            if (Reputazione < 0)
                Reputazione = 0;

            lpproc = MakeProcInstance(MostraMetallone, hInst);

            /* 12 GIUGNO 1998 - LE FINESTRE 100,101 E 102 FANNO CRASCHIARE TUTTO ! */
            /* TEMPORANEA SOLUZIONE: */
            /* i=103 + random(3);	// 103 - 105 <-------------------------------- */

            i = 100 + random(6); /* 100 - 105 */

#ifdef TABBOZ_DEBUG
            sprintf(tmp, "eventi: Metallaro n. %d", i);
            writelog(tmp);
#endif

            DialogBox(hInst,
                      MAKEINTRESOURCE(i),
                      hInstance,
                      lpproc);
            FreeProcInstance(lpproc);

            Tempo_trascorso_dal_pestaggio = 5;
            break;
            ;

            // -------------- Scooter -----------------------------------------------------------------------

        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
            if ((ScooterData.stato != -1) & (ScooterData.attivita == 1))
            {

                if (CellularData.stato > -1)
                { // A furia di prendere botte, il cellulare si spacca...
                    CellularData.stato -= random(8);
                    // 0 = 'morente', -1 = 'morto'
                    if (CellularData.stato < 0)
                        CellularData.stato = 0;
                }

                if (caso < 17)
                {
                    ScooterData.stato -= 35;
                    lpproc = MakeProcInstance(MostraMetallone, hInst); // Camionista ---------------------

                    DialogBox(hInst,
                              MAKEINTRESOURCE(106),
                              hInstance,
                              lpproc);
                    FreeProcInstance(lpproc);
#ifdef TABBOZ_DEBUG
                    writelog("eventi: Scooter - Camionista...");
#endif
                }
                else
                {
                    ScooterData.stato -= 20;
                    lpproc = MakeProcInstance(MostraMetallone, hInst); // Muro ! --------------------------

                    DialogBox(hInst,
                              MAKEINTRESOURCE(107),
                              hInstance,
                              lpproc);
                    FreeProcInstance(lpproc);
#ifdef TABBOZ_DEBUG
                    writelog("eventi: Scooter - Muro...");
#endif
                }

                Reputazione -= 2;
                if (Reputazione < 0)
                    Reputazione = 0;

                if (ScooterData.stato <= 0)
                {
                    MessageBox(hInstance,
                               "Quando ti rialzi ti accorgi che il tuo scooter e' ormai ridotto ad un ammasso di rottami.",
                               "Scooter Distrutto", MB_OK | MB_ICONSTOP);
                    ScooterData.stato = -1;
                    ScooterData.attivita = 0;
                    ScooterData = ScooterMem[0];
#ifdef TABBOZ_DEBUG
                    writelog("eventi: Lo scooter si e' completamente distrutto...");
#endif
                }
            }
            break;
            ;

            // -------------- Figosita' --------------------------------------------------------------------

        case 21: // + gravi
        case 22: //  |
        case 23:
            Fama -= 5; //  |
        case 24:       //  |
        case 25:
            Fama -= 1; //  |
        case 26:       //  |
        case 27:
            Fama -= 1; //  |
        case 28:       // \|/
        case 29:       // - gravi
        case 30:
            LoadString(hInst, (1000 + caso), (LPSTR)messaggio, 255);
            sprintf(tmp, "Sei fortunat%c...", ao);
            MessageBox(hInstance,
                       (LPSTR)messaggio, tmp, MB_OK | MB_ICONSTOP);
            Fama -= 2;
            if (Fama < 0)
                Fama = 0;

#ifdef TABBOZ_DEBUG
            writelog("eventi: Evento riguardante la figosita'...");
#endif

            break;
            ;

            // -------------- Skuola --------------------------------------------------------------------------

        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40: // Durante i giorni di vacanza non ci sono eventi riguardanti la scuola
            if (x_vacanza == 0)
            {
                i = random(9) + 1; // Fino alla versione 0.5 c'era scritto 10 ed era un bug...
                LoadString(hInst, (1000 + caso), (LPSTR)messaggio, 255);
                strcat(messaggio, MaterieMem[i].nome);
                MessageBox(hInstance,
                           (LPSTR)messaggio,
                           "Scuola...", MB_OK | MB_ICONSTOP);

                if (MaterieMem[i].xxx >= 2)
                    MaterieMem[i].xxx -= 2;
                CalcolaStudio();
#ifdef TABBOZ_DEBUG
                writelog("eventi: Evento riguardante la scuola");
#endif
                ScuolaRedraw = 1; /* E' necessario ridisegnare la finestra della scuola... */
            }

            break;
            ;

            // -------------- Tipa/o ---------------------------------------------------------------------------

        case 41:
        case 42: // Una tipa/o ci prova... 7 Maggio 1999

            if (Fama < 35)
                break; // Figosita' < 35 = nessuna speranza...

            figTemp = random(Fama - 30) + 30; // Figosita' minima tipa = 30...

            if (sesso == 'M')
            {
                LoadString(hInst, (200 + random(20)), (LPSTR)nomeTemp, 30); // 200 -> 219 [nomi tipe]
                sprintf(tmp, "Una tipa, di nome %s (Figosita' %d/100), ci prova con te'...\nCi stai ???", nomeTemp, figTemp);
            }
            else
            {
                LoadString(hInst, (1200 + random(20)), (LPSTR)nomeTemp, 30); // 200 -> 219 [nomi tipi]
                sprintf(tmp, "Una tipo, di nome %s (Figosita' %d/100), ci prova con te'...\nCi stai ???", nomeTemp, figTemp);
            }

            if (MessageBox(hInstance, tmp, "Qualcuno ti caga...", MB_YESNO) == IDNO)
            {
                if ((figTemp >= 79) && (Rapporti < 1) && (sesso == 'M'))
                { // Se non hai gia' una tipa e rifiuti una figona...
                    MessageBox(hInstance,
                               "Appena vengono a sapere che non ti vuoi mettere insieme ad una figona come quella, i tuoi amici ti prendono a scarpate.",
                               "Idiota...", MB_OK | MB_ICONSTOP);
                    Reputazione -= 4;
                    if (Reputazione < 0)
                        Reputazione = 0;
                }
                break;
            }

            // Controlla che tu non abbia gia' una tipa -------------------------
            if (Rapporti > 0)
            { // hai gia' una tipa..<<<<<<<<<<<<<<<<<<<<<<<<<<<
                lpproc = MakeProcInstance(DueDonne, hInst);
                if (sesso == 'M')
                    DialogBox(hInst,
                              MAKEINTRESOURCE(92),
                              hInstance,
                              lpproc);
                else
                    DialogBox(hInst,
                              MAKEINTRESOURCE(192),
                              hInstance,
                              lpproc);
                FreeProcInstance(lpproc);
            }
            else
            { // bravo, no hai una tipa...<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                sprintf(Nometipa, "%s", nomeTemp);
                FigTipa = figTemp;
                Rapporti = 45 + random(15);
                Fama += FigTipa / 10;
                if (Fama > 100)
                    Fama = 100;
                Reputazione += FigTipa / 13;
                if (Reputazione > 100)
                    Reputazione = 100;
            }

#ifdef TABBOZ_DEBUG
            writelog("eventi: Una tipa//o ci prova...");
#endif
            break;
            ;

        case 43: // Domande inutili... 11 Giugno 1999
        case 44:

            if ((Rapporti > 0) && (sesso == 'M'))
                if (caso == 43)
                {
                    i = MessageBox(hInstance,
                                   "Mi ami ???",
                                   "Domande inutili della Tipa...", MB_YESNO | MB_ICONQUESTION);
                    if (i != IDYES)
                    {
                        MessageBox(hInstance,
                                   "Sei sempre il solito stronzo.. non capisco perche' resto ancora con uno come cosi'...",
                                   "Risposta sbagliata...", MB_OK | MB_ICONSTOP);
                        Rapporti -= 45;
                        if (Rapporti < 5)
                            Rapporti = 5;
                    }
                }
                else
                {
                    i = MessageBox(hInstance,
                                   "Ma sono ingrassata ???",
                                   "Domande inutili della Tipa...", MB_YESNO | MB_ICONQUESTION);
                    if (i != IDNO)
                    {
                        MessageBox(hInstance,
                                   "Sei un bastardo, non capisci mai i miei problemi...",
                                   "Risposta sbagliata...", MB_OK | MB_ICONSTOP);
                        Rapporti -= 20;
                        if (Rapporti < 5)
                            Rapporti = 5;
                    }
                }

#ifdef TABBOZ_DEBUG
            writelog("eventi: Domande inutili della tipa...");
#endif
            break;
            ;

        case 45:
        case 46:
        case 47:
        case 48:
#ifdef TABBOZ_DEBUG
            writelog("eventi: Evento riguardante la tipa//o (da fare...)");
#endif
            break;
            ;

            // -------------- Vari ed eventuali ----------------------------------------------------------------

        case 49:
        case 50:
            if (CellularData.stato > -1)
            {
                CellularData.stato -= random(8);
                // 0 = 'morente', -1 = 'morto'
                if (CellularData.stato < 0)
                    CellularData.stato = 0;
                MessageBox(hInstance,
                           "Il telefonino di cade di tasca e vola per terra...",
                           "Telefonino", MB_OK | MB_ICONSTOP);
#ifdef TABBOZ_DEBUG
                writelog("eventi: Telefonino - Cade...");
#endif
            }
            break;
            ;

        default:;
            ;
        }
    }
}

//
//	MOSTRA METALLONE, non e' solo per i metalloni, e' molto utile anche per mostrare
//	tutte quelle finestre in cui non c'e' altro di particolare se non il pulsante di
//	[OK] ed un nome casuale di [VIA]...
//

#pragma argsused
BOOL FAR PASCAL MostraMetallone(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char tmp[1024];

    if (message == WM_INITDIALOG)
    {
        LoadString(hInst, (450 + random(50)), tmp, (sizeof(tmp) - 1));

        /* 15 giugno 1998 - La prima lettera viene scritta minuscola (appare "via..." al posto di "Via..." ) */

        tmp[0] = tolower(tmp[0]);
        SetDlgItemText(hDlg, 111, tmp);

        if (sound_active)
            TabbozPlaySound(1400);
        return (TRUE);
    }
    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {

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
