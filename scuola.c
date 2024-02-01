// Tabboz Simulator
// (C) Copyright 1997-1999 by Andrea Bonomi

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

/* Per una questione di svogliatezza del programmatore, viene usata STSCOOTER anche x i vestiti, */
/* le materie scolastiche e qualche altra cosa che adesso non mi viene in mente...               */

STSCOOTER MaterieMem[] =
{
        {0, 0, 0, 0, 0, 0, 0, 0, "---"                  },
        {0, 0, 0, 0, 0, 0, 0, 0, "Agraria"              },
        {0, 0, 0, 0, 0, 0, 0, 0, "Fisica"               },
        {0, 0, 0, 0, 0, 0, 0, 0, "Attivita' culturali"  }, /* fine alla 0.6.3 era "culurali..." */
        {0, 0, 0, 0, 0, 0, 0, 0, "Attivita' matematiche"},
        {0, 0, 0, 0, 0, 0, 0, 0, "Scienze industriali"  },
        {0, 0, 0, 0, 0, 0, 0, 0, "Elettrochimica"       },
        {0, 0, 0, 0, 0, 0, 0, 0, "Petrolchimica"        },
        {0, 0, 0, 0, 0, 0, 0, 0, "Filosofia aziendale"  }, /* fino alla 0.5.3 "aziendale" aveva due zeta... */
        {0, 0, 0, 0, 0, 0, 0, 0, "Metallurgia"          }
};
/*     |            */
/*     \voto in una materia */

void ScriviVoti(HWND parent);
void Aggiorna(HWND parent);
BOOL CheckVacanza(HWND parent);

/********************************************************************/
/* Scuola...                                                        */
/********************************************************************/

#pragma argsused
BOOL FAR PASCAL Scuola(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char tmp[128];
    int  i, i2;

    if (message == WM_INITDIALOG)
    {
        scelta = 1;
        SetCheck(hDlg, 110, TRUE); /* Seleziona agraria */

        sprintf(tmp, "Corrompi il prof di %s", MaterieMem[1].nome);
        SetDlgItemText(hDlg, 101, tmp);
        if (sesso == 'M')
            sprintf(tmp, "Minaccia il prof di %s", MaterieMem[1].nome);
        else
            sprintf(tmp, "Seduci il prof di %s", MaterieMem[1].nome);

        SetDlgItemText(hDlg, 102, tmp);
        sprintf(tmp, "Studia %s", MaterieMem[1].nome);
        SetDlgItemText(hDlg, 103, tmp);

        CalcolaStudio();
        ScriviVoti(hDlg); /* Scrive i voti, soldi, reputazione e studio nelle apposite caselle */
        return (TRUE);
    }
    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 101: /* Corrompi i professori */
            if (!CheckVacanza(hDlg))
            {
                i = 30 + (random(30) * 2); /* 21 Apr 1998 - I valori dei soldi e' meglio che siano sempre pari, in modo da facilitare la divisione x gli euro... */
                sprintf(tmp, "Ma... forse per %s potrei dimenticare i tuoi ultimi compiti in classe...", MostraSoldi(i));
                i2 = MessageBox(hDlg, tmp,
                                "Corrompi i professori", MB_YESNO | MB_ICONQUESTION);

                if (i2 == IDYES)
                {
                    if (Soldi >= i)
                    {
                        Soldi -= i;
#ifdef TABBOZ_DEBUG
                        sprintf(tmp, "scuola: Corrompi un professore per %s", MostraSoldi(i));
                        writelog(tmp);
#endif
                        MaterieMem[scelta].xxx += 3;
                        if (MaterieMem[scelta].xxx > 10)
                            MaterieMem[scelta].xxx = 10;
                    }
                    else
                    {
                        if (MaterieMem[scelta].xxx < 2)
                            MaterieMem[scelta].xxx -= 2;
                        MessageBox(hDlg,
                                   "Cosa ??? Prima cerchi di corrompermi, poi si scopre che non hai abbastanza soldi !!!",
                                   "Errore critico", MB_OK | MB_ICONSTOP);
                    }
                }

                Evento(hDlg);
                Aggiorna(hDlg);
            }
            return (TRUE);

        case 102: /* Minaccia-Seduci i professori */
            if (!CheckVacanza(hDlg))
            {
                if (sesso == 'M')
                { // Maschietto - minaccia prof.
                    if ((Reputazione >= 30) || (random(10) < 1))
                    {
                        MaterieMem[scelta].xxx += 2;
                        if (MaterieMem[scelta].xxx > 10)
                            MaterieMem[scelta].xxx = 10;
                    }
                    else
                    {
                        if (sound_active)
                            TabbozPlaySound(402);
                        MessageBox(hDlg,
                                   "Cosa ??? Credi di farmi paura piccolo pezzettino di letame vestito da zarro... Deve ancora nasce chi puo' minacciarmi...",
                                   "Bella figura", MB_OK | MB_ICONINFORMATION);
                        if (Reputazione > 3)
                            Reputazione -= 2;

                        if (MaterieMem[scelta].xxx > 2)
                            MaterieMem[scelta].xxx -= 1;
                    }
                }
                else
                { // Femminuccia - seduci prof.
                    if ((Fama >= 50) || (random(10) < 2))
                    {
                        MaterieMem[scelta].xxx += 2;
                        if (MaterieMem[scelta].xxx > 10)
                            MaterieMem[scelta].xxx = 10;
                    }
                    else
                    {
                        if (sound_active)
                            TabbozPlaySound(402);
                        MessageBox(hDlg,
                                   "Infastidito dalla tua presenza, il prof ti manda via a calci.",
                                   "Bella figura", MB_OK | MB_ICONINFORMATION);
                        if (Reputazione > 3)
                            Reputazione -= 2;

                        if (MaterieMem[scelta].xxx > 2)
                            MaterieMem[scelta].xxx -= 1;
                    }
                }

                Aggiorna(hDlg);
                Evento(hDlg);
            }

            return (TRUE);

        case 103: /* Studia */
            if (!CheckVacanza(hDlg))
            {
                if (Reputazione > 10) /* Studiare costa fatica... */
                    Reputazione -= 5; /* (oltre che Reputazione e Fama...) */

                if (Fama > 5)
                    Fama -= 1;

                MaterieMem[scelta].xxx += 1;
                if (MaterieMem[scelta].xxx > 10)
                    MaterieMem[scelta].xxx = 10;

                Aggiorna(hDlg);
                Evento(hDlg);
            }

            return (TRUE);

        case 110:
        case 111:
        case 112:
        case 113:
        case 114:
        case 115:
        case 116:
        case 117:
        case 118:
            scelta = wParam - 109;
            sprintf(tmp, "Corrompi il prof di %s", MaterieMem[scelta].nome);
            SetDlgItemText(hDlg, 101, tmp);
            if (sesso == 'M')
                sprintf(tmp, "Minaccia il prof di %s", MaterieMem[scelta].nome);
            else
                sprintf(tmp, "Seduci il prof di %s", MaterieMem[scelta].nome);
            SetDlgItemText(hDlg, 102, tmp);
            sprintf(tmp, "Studia %s", MaterieMem[scelta].nome);
            SetDlgItemText(hDlg, 103, tmp);
            return (TRUE);

        case IDOK:
            EndDialog(hDlg, TRUE);
            return (TRUE);
        case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return (TRUE);
        default:
            return (TRUE);
        }
    }
    return (FALSE);
}

/* Scrive i voti nelle apposite caselle */
void ScriviVoti(HWND parent)
{
    int  i;
    char tmp[128];
    ScuolaRedraw = 0;

    SetDlgItemText(parent, 104, MostraSoldi(Soldi));
    sprintf(tmp, "%d/100", Reputazione);
    SetDlgItemText(parent, 105, tmp);
    sprintf(tmp, "%d/100", Studio);
    SetDlgItemText(parent, 106, tmp);

    for (i = 1; i < 10; i++)
    {
        sprintf(tmp, "%d", MaterieMem[i].xxx);
        SetDlgItemText(parent, i + 119, tmp);
    }
}

/* Calcola Studio */
void CalcolaStudio()
{
    int   i, i2;
    div_t x;

    i2 = 0;
    for (i = 1; i < 10; i++)
        i2 += MaterieMem[i].xxx;

    i2 = i2 * 10;

    x = div(i2, 9);
    Studio = x.quot;
}

/* Aggiorna */
void Aggiorna(HWND parent)
{
    char tmp[128];

    CalcolaStudio();

    if (ScuolaRedraw == 1)
        ScriviVoti(parent);

    SetDlgItemText(parent, 104, MostraSoldi(Soldi));
    sprintf(tmp, "%d/100", Reputazione);
    SetDlgItemText(parent, 105, tmp);
    sprintf(tmp, "%d/100", Studio);
    SetDlgItemText(parent, 106, tmp);

    sprintf(tmp, "%d", MaterieMem[scelta].xxx);
    SetDlgItemText(parent, scelta + 119, tmp);
}

BOOL CheckVacanza(HWND parent)
{
    if (x_vacanza != 0)
    {
        MessageBox(parent,
                   "Non puoi andare a scuola in un giorno di vacanza !",
                   "Scuola", MB_OK | MB_ICONINFORMATION);
        return (TRUE);
    }
    else
        return (FALSE);
}
