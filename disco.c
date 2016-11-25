// Tabboz Simulator
// (C) Copyright 1997-1999 by Andrea Bonomi
// 6 Aprile 1999 - Inizio implementazione lettore CDROM
// 30 Maggio 1999 - Conversione Tabbozzo -> Tabbozza

#include "os.h"

// Lettore di CD-ROM per Windows... (cagata !)
// #define CDROM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef TABBOZ_WIN
// #ifdef CDROM
// #include <mmsystem.h>
// #endif
#endif

#include "zarrosim.h"
static char sccsid[] = "@(#)" __FILE__ " " VERSION " (Andrea Bonomi) " __DATE__;
static int  numdisco;


// Per una questione di svogliatezza del programmatore, viene usata STSCOOTER
// anche x i vestiti e per le discoteche.

STSCOOTER DiscoMem[] =
	{   { 0,  0, 0, 0, 0, 0,   0,  0,"---"},
		 { 0, 30, 2,15, 1, 0,  36,  0,""   },
		 { 0,  0, 1, 7, 4, 0,  26,  0,""   },
		 { 0,  0, 1, 8, 1, 0,  30,  0,""   },
		 { 0, 35, 3,15, 1, 0,  36,  0,""   },
		 { 0,  0, 2, 6, 3, 0,  26,  0,""   },
		 { 0,  0, 2, 5, 2, 0,  22,  0,""   },
		 { 0,  0, 3, 8, 1, 0,  30,  0,""   },
		 { 1,  0, 2, 9, 7, 0,  36,  0,""   } };
/*	    |	|  |  |   \       \costo 					*/
/*	    |   |  |  |    \giorno di chiusura (1=lunedi',etc... - 0=nessuno) [24 marzo 1998] */
/*	    \	\  \  \incremento fama						*/
/*           \   \  \incremento reputazione					*/
/*            \   \figosita' minima x entrare (selezione all' ingresso) 	*/
/*	       1=disco fuori porta - ci puoi arrivare solo se hai lo scooter... */



//********************************************************************
// Routine per il pagamento della Discoteca.
//********************************************************************

static void
PagaDisco(HANDLE hInstance)
{
char	tmp[128];
long	Prezzo;

	if (numdisco != 0) {
					 if (DiscoMem[numdisco].speed == 1) {
		  if (ScooterData.stato == -1) {
				MessageBox( hInstance,
					"Senza lo scooter non puoi andare nelle discoteche fuori porta...",
						"Discoteca fuori porta", MB_OK | MB_ICONINFORMATION);
				Evento(hInstance);
				return;
		  }
		}

		if (DiscoMem[numdisco].mass == x_giornoset) {	/* [24 Marzo 1998] versione 0.6.2a */
			MessageBox( hInstance,
				"Un cartello recita che oggi e' il giorno di chiusura settimanale...",
					"Giorno di chiusura", MB_OK | MB_ICONINFORMATION);
			return;
	   }


		if (sesso == 'M')
			Prezzo=DiscoMem[numdisco].prezzo;
		else
			Prezzo=DiscoMem[numdisco].prezzo - 10;


		if (Prezzo > Soldi) {		/* check costo */
			nomoney(hInstance,DISCO);
		} else {
			if ((DiscoMem[numdisco].cc > Fama) && (sesso == 'M')) {			/* check selezione all'ingresso */
			  if (sound_active) TabbozPlaySound(302);
			  MessageBox( hInstance,
			  "Mi dispiace signore, conciato cosi', qui non puo' entrare...\nVenga vestito meglio la prossima volta, signore.",
				  "Selezione all' ingresso", MB_OK | MB_ICONINFORMATION);
			  if (Reputazione > 2) Reputazione-=1;
			  if (Fama > 2) Fama-=1;
			} else {
			  if (sound_active) TabbozPlaySound(303 + random(3));  // suoni: 0303 -> 0305
			  Soldi-= Prezzo;
			  #ifdef TABBOZ_DEBUG
			  sprintf(tmp,"discoteca: Paga %s",MostraSoldi(DiscoMem[numdisco].prezzo));
			  writelog(tmp);
			  #endif
			  Fama+=DiscoMem[numdisco].fama;
			  Reputazione+=DiscoMem[numdisco].xxx;
			  if (Fama > 100) 		 Fama=100;
			  if (Reputazione > 100) Reputazione=100;
			}
	  }
	  Evento(hInstance);
	}
}


//******************************************************************
// Disco...
//******************************************************************

# pragma argsused
BOOL FAR PASCAL Disco(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
		 char          buf[1024];
		 char				tmp[1024];
#ifdef TABBOZ_WIN
#ifdef CDROM
static char          mciTmp[1024];
static int				mciLen;
static DWORD			mciReturn;
#endif
#endif

	 if (message == WM_INITDIALOG) {
		numdisco=0;
		sprintf(buf, "O tip%c, in che disco andiamo ?",ao);
		SetDlgItemText(hDlg, 120, buf);

	/* [24 Marzo 1998] -  Perche' Discoteca era fino ad */
	/* oggi l'unica finestra che non mostrava i soldi   */
	/* che il tabbozzo ha ??? 			    */
		SetDlgItemText(hDlg, 110, MostraSoldi(Soldi));
#ifdef TABBOZ_WIN
#ifdef CDROM
//		mciReturn=mciSendString("open cdaudio", mciTmp, mciLen, 0);
		mciReturn=mciSendString("set cdaudio time format tmsf", mciTmp, mciLen, 0);

//		mciReturn=mciSendString("set cdaudio door open", mciTmp, mciLen, 0);
//		mciReturn=mciSendString("close cdaudio", mciTmp, mciLen, 0);
#endif
#endif
		return(TRUE);
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
			numdisco=wParam-100;
			LoadString(hInst, wParam, buf, 1024);

			if (sesso == 'M')	// Le donne pagano meno...
				sprintf(tmp,buf,MostraSoldi(DiscoMem[numdisco].prezzo));
			else
				sprintf(tmp,buf,MostraSoldi(DiscoMem[numdisco].prezzo - 10));

			SetDlgItemText(hDlg, 120, tmp);
			return(TRUE);
#ifdef TABBOZ_WIN
#ifdef CDROM
		 case 607: // Eject
			mciReturn=mciSendString("status cdaudio mode", mciTmp, mciLen, 0);
			sprintf(buf,"%s",mciTmp);
			SetDlgItemText(hDlg, 120, buf);

			if (! strcmp(mciTmp,"open") )
				mciReturn=mciSendString("set cdaudio door close", mciTmp, mciLen, 0);
			else
				mciReturn=mciSendString("set cdaudio door open", mciTmp, mciLen, 0);
			return(TRUE);
#endif
#endif

		 case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return(TRUE);

		 case IDOK:
			PagaDisco(hDlg);
			EndDialog(hDlg, TRUE);
			return(TRUE);

		 default:
			return(TRUE);

		}
	 }

	 return(FALSE);
}


