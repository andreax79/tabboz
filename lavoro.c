// Tabboz Simulator
// (C) Copyright 1998-1999 by Andrea Bonomi

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

// 31 Maggio 1999 - Conversione Tabbozzo -> Tabbozza

#include "os.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "zarrosim.h"
static char sccsid[] = "@(#)" __FILE__ " " VERSION " (Andrea Bonomi) " __DATE__;

int    numeroditta;
int    impegno;
int    stipendio;
int    giorni_di_lavoro;
int    punti_scheda;

BOOL FAR PASCAL ElencoDitte(HWND hDlg, WORD message, WORD wParam, LONG lParam);
BOOL FAR PASCAL CercaLavoro(HWND hDlg, WORD message, WORD wParam, LONG lParam);
void AggiornaLavoro(HWND hDlg);


char Risposte1[3];
char Risposte2[3];
char Risposte3[3];

// Numero di ditte
#define NUM_DITTE 8
STSCOOTER LavoroMem[] =
	{   {0,  0, 0, 0, 0, 0,   0,  0,"---"},
		 {0,  0, 0, 0, 0, 0,   0,  0,"Magneti Budelli"},
		 {0,  0, 0, 0, 0, 0,   0,  0,"Diamine"},
		 {1,  0, 0, 0, 0, 0,   0,  0,"Testmec"},
		 {0,  0, 0, 0, 0, 0,   0,  0,"Ti Impalo Bene Bene"},
		 {1,  0, 0, 0, 0, 0,   0,  0,"October"},
		 {1,  0, 0, 0, 0, 0,   0,  0,"Arlond's"},
		 {1,  0, 0, 0, 0, 0,   0,  0,"286 - Computer d' annata"}, // 14/01/2000
		 {1,  0, 0, 0, 0, 0,   0,  0,"Ricopio" }}; 			       // 14/01/2000
//     |
//     \lavoro fuori porta (solo con lo scooter puoi arrivarci...)



int	scheda;	/* numero scheda del quiz ( 0 - 9 ) */
int	accetto;

/********************************************************************/
/* Lavoro...                                                        */
/********************************************************************/

int	GiornoDiLavoro(HWND hDlg, char *s)
{
	char	tmp[255];

	if (numeroditta < 1)  {
		sprintf(tmp,"Forse non ti ricordi che sei disokkupat%c...",ao);
		MessageBox( hDlg, tmp, s, MB_OK | MB_ICONINFORMATION);
		return(TRUE);
		}

	if ( x_vacanza == 2 ) {
		sprintf(tmp,"Arrivat%c davanti ai cancelli della ditta li trovi inrimediabilmente chiusi...",ao);
		MessageBox( hDlg, tmp, s, MB_OK | MB_ICONINFORMATION);
		return(TRUE);
		}

	return FALSE;
}


# pragma argsused
BOOL FAR PASCAL        Lavoro(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
	 char 	  tmp[255];
	 FARPROC	  lpproc;
	 int		  n_ditta;
	 int		  i;
static    int	  Rcheck;
static    int	  Lcheck;

	 if (message == WM_INITDIALOG) {
		if (sesso == 'M')
			SetDlgItemText(hDlg, 113, "Fai il leccaculo");
		else
			SetDlgItemText(hDlg, 113, "Fai la leccaculo");

		AggiornaLavoro(hDlg);
		return(TRUE);
		}

	 else if (message == WM_COMMAND)
	 {
		switch (wParam)
		{

		 case 110:		// Cerca Lavoro ----------------------------------------------------------------------------------
		if ( x_vacanza == 2 ) {
			sprintf(tmp,"Arrivat%c davanti ai cancelli della ditta li trovi inrimediabilmente chiusi...",ao);
			MessageBox( hDlg, tmp, "Cerca Lavoro", MB_OK | MB_ICONINFORMATION);
			return(TRUE);
			 }

		if (numeroditta > 0)  {
			MessageBox( hDlg,
				 "Forse non ti ricordi che hai gia' un lavoro...",
				"Cerca Lavoro", MB_OK | MB_ICONINFORMATION);
			return(TRUE);
			}

		Rcheck = 0;	/* Check per il numero di risposte */
		Lcheck = 0;	/* Check delle risposte */

		n_ditta = random(NUM_DITTE) + 1;

		lpproc = MakeProcInstance(CercaLavoro, hInst);

		DialogBox(hInst,
			 MAKEINTRESOURCE(389 + n_ditta),
			 hDlg,
			 lpproc);
		FreeProcInstance(lpproc);

		if (accetto == IDNO ) { // Viva la finezza...
			MessageBox( hDlg,
			  "Allora sparisci...",
						"Cerca Lavoro", MB_OK | MB_ICONINFORMATION);
			return(TRUE);
		}

		lpproc = MakeProcInstance(CercaLavoro, hInst);
		scheda = random(9);
		punti_scheda = 0;

		DialogBox(hInst,
			 MAKEINTRESOURCE(200+scheda),
			 hDlg,
			 lpproc);
		FreeProcInstance(lpproc);

		/* Facciamo finta che la scheda venga effettivamente tenuta in considerazione...            */
		/* forse in un futuro verranno controllate le risposte, ma per ora non servono a nulla.     */


		/* 24 Maggio 1998 - v0.6.94 */
		/* Le risposte cominciano a venire controllate.... */
		for(i=0;i<3;i++)
			if (Risposte1[i]) Rcheck+=1;
		for(i=0;i<3;i++)
			if (Risposte2[i]) Rcheck+=10;
		for(i=0;i<3;i++)
			if (Risposte3[i]) Rcheck+=100;


		if ( Rcheck != 111 ) {
			if (sesso == 'M')
				MessageBox( hDlg, "Mi spieghi perche' dovremmo assumere qualcuno che non e' neanche in grado di mettere delle crocette su un foglio ???", "Sei un po' stupido...", MB_OK | MB_ICONQUESTION);
			else
				MessageBox( hDlg, "Signorina,mi spieghi perche' dovremmo assumere qualcuno che non e' neanche in grado di mettere delle crocette su un foglio ???", "Sei un po' stupida...", MB_OK | MB_ICONQUESTION);
			AggiornaLavoro(hDlg);
			return(TRUE);
			}

		if (( Reputazione + Fortuna + random(80) ) > random(200))   {
			impegno=10 + random(20);
         giorni_di_lavoro=1;
			stipendio=1000 + (random(10) * 100);
			numeroditta=n_ditta;

			if (sesso == 'M')
				sprintf(tmp, "SEI STATO ASSUNTO ! Ora sei un felice dipendente della %s !", LavoroMem[n_ditta].nome);
			else
				sprintf(tmp, "SEI STATO ASSUNTA ! Ora sei una felice dipendente della %s !", LavoroMem[n_ditta].nome);

			MessageBox( hDlg, tmp, "Hai trovato lavoro !", MB_OK | MB_ICONINFORMATION);
		} else {
			if (sesso == 'M')
				MessageBox( hDlg,
				  "Mi dispiace ragazzo, ma non sei riuscito a superare il test... Ora puoi anche portare la tua brutta faccia fuori dal mio ufficio, prima che ti faccia buttare fuori a calci... Grazie e arrivederci...",
				  "Cerca Lavoro", MB_OK | MB_ICONINFORMATION);
			else
				MessageBox( hDlg,
				  "Mi dispiace signorina, ma non e' riuscito a superare il test... Se ne vada immediatamente, grazie...",
				  "Cerca Lavoro", MB_OK | MB_ICONINFORMATION);

			if (Reputazione > 10) Reputazione-=2;
		}

		Evento(hDlg);
		AggiornaLavoro(hDlg);
		return(TRUE);

	    case 111:		/* Licenziati ------------------------------------------------------------------------------------ */
		if (GiornoDiLavoro(hDlg,"Licenziati")) return(TRUE);

		sprintf(tmp,"Sei proprio sicur%c di voler dare le dimissioni dalla %s ?",ao,LavoroMem[numeroditta].nome);
		accetto=MessageBox( hDlg,
		  tmp,
		  "Licenziati", MB_YESNO | MB_ICONQUESTION);

		if (accetto == IDYES) {
			numeroditta=0;
			impegno=0;
         giorni_di_lavoro=0;
			stipendio=0;
			Evento(hDlg);
			}
		AggiornaLavoro(hDlg);
		return(TRUE);

	    case 112:		/* Chiedi aumento salario ------------------------------------------------------------------------ */
		if (GiornoDiLavoro(hDlg,"Chiedi aumento salario")) return(TRUE);

		if (impegno > 90) {
			if ((30 + Fortuna) > (30 + random(50))) {
				sprintf(tmp,"Forse per questa volta potremmo darti qualcosina in piu'...");
				MessageBox( hDlg, tmp,
				  "Chiedi aumento salario", MB_OK | MB_ICONINFORMATION);
				stipendio+=((random(1)+1) * 100);
				impegno-=30;
				Evento(hDlg);
			} else {
				if (sesso == 'M' )
					MessageBox( hDlg,
					"Vedi di scordartelo,bastardo...",
					"Chiedi aumento salario", MB_OK | MB_ICONHAND);
				else
					MessageBox( hDlg,
					"Vedi di scordartelo,stronzetta...",
					"Chiedi aumento salario", MB_OK | MB_ICONHAND);

			impegno-=20;
			Evento(hDlg);
			}
		} else {
			MessageBox( hDlg,
					"Che cosa vorresti ??? SCORDATELO !!!!",
					"Chiedi aumento salario", MB_OK | MB_ICONHAND);
		}

		AggiornaLavoro(hDlg);
		return(TRUE);

		 case 113:		/* Fai il leccaculo ------------------------------------------------------------------------------ */
			if (sesso == 'M') {
				if (GiornoDiLavoro(hDlg,"Fai il leccaculo")) return(TRUE);
			} else {
				if (GiornoDiLavoro(hDlg,"Fai la leccaculo")) return(TRUE);
			}

			if (sound_active) TabbozPlaySound(503);

			if (Reputazione > 20 )	/* Facendo il leccaculo perdi reputazione e fama... */
				Reputazione-=1;
			if (impegno < 99) impegno++;

			i=random(Fortuna+3);
			if ( i == 0 )
			Evento(hDlg);

			AggiornaLavoro(hDlg);
			return(TRUE);

		 case 114:		/* Elenco ditte ---------------------------------------------------------------------------------- */
			if (numeroditta == 0) {
				lpproc = MakeProcInstance(ElencoDitte, hInst);
				DialogBox(hInst,
				 MAKEINTRESOURCE(210),
				 hDlg,
				 lpproc);
			FreeProcInstance(lpproc);
		} else {
			lpproc = MakeProcInstance(CercaLavoro, hInst);
			DialogBox(hInst,
				 MAKEINTRESOURCE(numeroditta + 289),
				 hDlg,
				 lpproc);
			FreeProcInstance(lpproc);
		}
		return(TRUE);

		 case 115:		/* Sciopera  ----------------------------------------------------------------------------------  */
		if (GiornoDiLavoro(hDlg,"Sciopera")) return(TRUE);

		if (sound_active) TabbozPlaySound(502);
		if (Reputazione < 85 )
			Reputazione+=10;

		if (impegno > 19 )
			impegno-=15;

		i=random(Fortuna+3);
		if ( i == 0 )
			Evento(hDlg);

		Evento(hDlg);
		AggiornaLavoro(hDlg);
		return(TRUE);

		 case 116:		/* Lavora  ----------------------------------------------------------------------------------  */
		if (GiornoDiLavoro(hDlg,"Lavora")) return(TRUE);
		if (impegno < 85) impegno++;

		if (sound_active) TabbozPlaySound(501);
		Evento(hDlg);
		AggiornaLavoro(hDlg);
		return(TRUE);

		 case IDCANCEL:
		EndDialog(hDlg, TRUE);
		return(TRUE);

		 case IDOK:
		EndDialog(hDlg, TRUE);
		return(TRUE);

	    default:
		return(TRUE);
	}
    }

    return(FALSE);
}


/* Cerca Lavoro ------------------------------------------------------------------------------- */

# pragma argsused
BOOL FAR PASCAL CercaLavoro(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
int 		i;

    if (message == WM_INITDIALOG) {
	for(i=0;i<3;i++)		/* Azzera le risposte... */
		Risposte1[i]=0;
	for(i=0;i<3;i++)
		Risposte2[i]=0;
	for(i=0;i<3;i++)
		Risposte3[i]=0;
	return(TRUE);
	}

    else if (message == WM_COMMAND)
    {
	switch (wParam)
	{
	    case 101:
	    case 102:
	    case 103:
		Risposte1[wParam-101]=!(Risposte1[wParam-101]);
		return(TRUE);
	    case 104:
	    case 105:
	    case 106:
		Risposte2[wParam-104]=!(Risposte2[wParam-104]);
		return(TRUE);
	    case 107:
	    case 108:
	    case 109:
		Risposte3[wParam-107]=!(Risposte3[wParam-107]);
		return(TRUE);
	    case IDCANCEL:
		EndDialog(hDlg, TRUE);
		accetto = IDNO;
		return(TRUE);
	    case IDOK:
		EndDialog(hDlg, TRUE);
		accetto = IDYES;
		return(TRUE);

	}
    }

    return(FALSE);
}



void AggiornaLavoro(HWND hDlg)
{
char 	  tmp[128];
   if (numeroditta < 1) {	// Nessun lavoro
    sprintf(tmp, "");
    SetDlgItemText(hDlg, 105, tmp);	// Ditta
    SetDlgItemText(hDlg, 106, tmp);	// Stipendio
    SetDlgItemText(hDlg, 107, tmp);	// Impegno
  } else {
    sprintf(tmp, "%s", LavoroMem[numeroditta].nome);
    SetDlgItemText(hDlg, 105, tmp);	// Ditta
    SetDlgItemText(hDlg, 106, MostraSoldi(stipendio)); // Stipendio
    sprintf(tmp, "%d/100", impegno);
	 SetDlgItemText(hDlg, 107, tmp);	// Impegno
  }
  SetDlgItemText(hDlg, 104, MostraSoldi(Soldi));
}


# pragma argsused
BOOL FAR PASCAL ElencoDitte(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
	 FARPROC	  lpproc;

	if (message == WM_INITDIALOG) {
		return(TRUE);
	}
	else if (message == WM_COMMAND)
	{
	 switch (wParam)
	  {			// Informazioni su ogni ditta
		 case 100:
		 case 101:
		 case 102:
		 case 103:
		 case 104:
		 case 105:
		 case 106:
		 case 107:
		 case 108:
	    case 109:
		lpproc = MakeProcInstance(CercaLavoro, hInst);
		DialogBox(hInst,
			 MAKEINTRESOURCE(wParam+190),
			 hDlg,
			 lpproc);
		FreeProcInstance(lpproc);
		return(TRUE);

		 case IDCANCEL:
		EndDialog(hDlg, TRUE);
		return(TRUE);
	    case IDOK:
		EndDialog(hDlg, TRUE);
		return(TRUE);

	}
    }

    return(FALSE);
}

