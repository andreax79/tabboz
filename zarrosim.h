/* Tabboz Simulator				*/
/* (C) Copyright 1997-2000 by Andrea Bonomi	*/

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

// #define VERSION	      "Version 0.9pr"
// #define VERSION	      "Version 0.92pr"
#define VERSION "Version 0.92q"

/* Per attivare il Debug... 12 giugno 1998	*/
#define TABBOZ_DEBUG

/* Per disattivare la possibilitta' di giocare con la tabbozza 21 giugno 1999 */
// #define NOTABBOZZA

/* Per disabilitare la rete... 26 luglio 1998	*/
#define NONETWORK

/* Per attivare il Prompt ... 16 marzo 1999 */
// #define PROMPT_ACTIVE

/* Per la versione a 32 bit (uscira' mai ??? 8 aprile 1999 */
#ifdef TABBOZ_WIN32
#define TABBOZ32
#endif

#ifndef lint
static char copyright[] =
    "@(#) Copyright (c) 1997-2001 Andrea Bonomi, Emanuele Caccialanza, Daniele Gazzarri.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifdef NOT_WINDOWS
#define IDOK 1
#define IDCANCEL 2
#define IDABORT 3
#define IDYES 6
#define IDNO 7
#endif

#define QX_NOME 102
#define QX_BMP 103
#define QX_SOLDI 105
#define QX_LOAD 106
#define QX_SAVE 107
#define QX_CLOSE 108

#define QX_ADDW 119
#define QX_NOREDRAW 120
#define QX_REDRAW 121

#define QX_ABOUT 120
#define QX_LOGO 121

#define QX_SCOOTER 130
#define QX_VESTITI 131
#define QX_DISCO 132
#define QX_TIPA 133
#define QX_COMPAGNIA 134
#define QX_FAMIGLIA 135
#define QX_SCUOLA 136
#define QX_LAVORO 137
#define QX_INFO 139
#define QX_CONFIG 140
#define QX_TABACCHI 141
#define QX_PALESTRA 142
#define QX_VESTITI1 143
#define QX_VESTITI2 144
#define QX_VESTITI3 145
#define QX_VESTITI4 146
#define QX_VESTITI5 147
#define QX_PROMPT 150
#define QX_NETWORK 151
#define QX_CELLULAR 155

/* DIALOG (finestre)		*/

#define MAIN 1
#define ABOUT 2
#define WARNING 3
#define DISCO 4
#define FAMIGLIA 5
#define COMPAGNIA 6
#define SCOOTER 7
#define VESTITI 8
#define TIPA 9
#define SCUOLA 10
#define PERSONALINFO 11
#define LOGO 12
#define LAVORO 13
#define CONFIGURATION 14
#define SPEGNIMI 16
#define NETWORK 17
#define PROMPT 20

#define ACQUISTASCOOTER 70
#define VENDISCOOTER 71
#define RIPARASCOOTER 72
#define TAROCCASCOOTER 73

#define BAUHOUSE 80
#define ZOCCOLARO 81
#define FOOTSMOCKER 82
#define ALTRIVESTITI4 83
#define ALTRIVESTITI5 84
#define ALTRIVESTITI6 85

#define TABACCAIO 88
#define PALESTRA 89

#define CERCATIPA 91
#define LASCIATIPA 92
#define ESCICONLATIPA 93
#define DUEDIPICCHE 95

#define CELLULAR 120
#define COMPRACELLULAR 121
#define VENDICELLULAR 122
#define CELLULRABBONAM 123

#define ATTESAMAX 5

typedef unsigned long u_long; // 28 Novembre 1998

// INFORMAZIONI SUGLI SCOOTER  (ora usato solo per cose generiche...)
typedef struct tagSTSCOOTER
{
    int  speed;    // velocita' massima
    int  cc;       // cilindrata
    int  xxx;      // [future espansioni]
    int  fama;     // figosita' scooter
    int  mass;     // massa sooter
    int  maneuver; // manovrabilita'
    int  prezzo;   // costo dello scooter (modifiche incluse)
    int  stato;    // quanto e' intero (in percuntuale); -1 nessuno scooter
    char nome[30]; // nome dello scooter
} STSCOOTER;

// NUOVE INFORMAZIONI SUGLI SCOOTER - 28 Aprile 1998

typedef struct tagNEWSTSCOOTER
{
    int  speed;       // 01  Velocita'
    int  marmitta;    // 02  Marmitta 		( +0, +7, +12, +15)
    int  carburatore; // 03  Carburatore  	( 0 - 4 )
    int  cc;          // 04  Cilindrata		( 0 - 4 )
    int  filtro;      // 05  Filtro dell' aria	( +0, +5, +10, +15)
    int  prezzo;      // 06  Costo dello scooter  (modifiche incluse)
    int  attivita;    // 07  Attivita' scooter
    int  stato;       // 08  Quanto e' intero (in percuntuale); -1 nessuno scooter
    char nome[30];    // 09  Nome dello scooter
    int  fama;        // 10  Figosita' scooter
} NEWSTSCOOTER;

// INFORMAZIONI SUI TELEFONINI  31 Marzo 1999
typedef struct tagCEL
{
    int  dual;  // Dual Band ?
    int  fama;  // figosita'
    int  stato; // quanto e' intero (in percuntuale)
    int  prezzo;
    char nome[30]; // nome del telefono
} STCEL;

/* INFORMAZIONI SULLE COMPAGNIE DEI TELEFONINI */
typedef struct tagABB
{
    int  abbonamento; // 0 = Ricarica, 1 = Abbonamento
    int  dualonly;    // Dual Band Only ?
    int  creditorest; // Credito Restante...
    int  fama;        // figosita'
    int  prezzo;
    char nome[30]; // nome del telefono
} STABB;

extern NEWSTSCOOTER ScooterData;
extern NEWSTSCOOTER ScooterMem[];

extern STSCOOTER MaterieMem[];

extern STCEL CellularData;
extern STCEL CellularMem[];

extern STABB AbbonamentData;
extern STABB AbbonamentMem[];

typedef struct tagMESI
{
    char nome[20];   // nome del mese
    int  num_giorni; // giorni del mese
} STMESI;

extern STMESI InfoMese[];
extern STMESI InfoSettimana[];

extern int x_giorno;
extern int x_mese;
extern int x_giornoset;
extern int x_vacanza;
extern int x_anno_bisesto;
extern int scad_pal_giorno; // Giorno e mese in cui scadra' l' abbonamento alla palestra.
extern int scad_pal_mese;

extern int comp_giorno; // giorno & mese del compleanno
extern int comp_mese;

#ifdef PROMPT_ACTIVE
extern int prompt_mode;
#endif

// PRIMA LE VARIABILI GENERIKE...

extern int  cheat;
extern int  scelta;
extern char Andrea[14];
extern char Caccia[21];
extern char Daniele[17];
extern int  ImgSelector;
extern int  TabbozRedraw; // E' necessario ridisegnare il Tabbozzo ???
extern int  ScuolaRedraw; // E' necessario ridisegnare la scuola ???
extern char showscooter;

// DOPO LE CARATTERISTIKE...

extern int    Attesa; // Tempo prima che ti diano altri soldi...
extern int    Fama;
extern int    Reputazione;
extern int    Studio;   // Quanto vai bene a scuola (1 - 100)
extern u_long Soldi;    // Long...per forza! lo zarro ha tanti soldi...
extern u_long Paghetta; // Paghetta Settimanale...
extern char   Nome[30];
extern char   Cognome[30];
extern char   Nometipa[30];
extern int    FigTipa;
extern int    Rapporti;
extern int    Stato;
extern u_long DDP;
extern int    AttPaghetta;
extern int    Fortuna;
extern int    numeroditta;
extern int    impegno;
extern int    giorni_di_lavoro; // Serve x calcolare lo stipendio SOLO per il primo mese...
extern int    stipendio;
extern int    benzina;
extern int    antifurto;
extern int    sizze;
extern int    Tempo_trascorso_dal_pestaggio;
extern int    current_testa;    // Grado di abbronzatura del tabbozzo
extern int    current_gibbotto; // Vestiti attuali del tabbozzo...
extern int    current_pantaloni;
extern int    current_scarpe;
extern int    current_tipa;
extern int    sound_active;
extern char   sesso; // M/F 29 Maggio 1999 --- Inizio...
extern char   ao;
extern char   un_una[];
extern char  *n_attivita[]; //  7 Maggio 1998

#ifdef TABBOZ_DEBUG // Sistema di Debug... [12 Giugno 1998]
#ifdef TABBOZ_EM
#define LOGFILE "/dev/stdout"
#else
#define LOGFILE "\\ZARROSIM.LOG"
#endif
extern void writelog(char *s); // 22 Giugno 1998
#endif

#ifndef NONETWORK
extern int  net_enable; // Rete Attiva/Disattiva [24 Giugno 1998]
extern char lastneterror[255];
extern char lastconnect[255];
extern int  addrLen;
extern HWND NEThDlg;                     // Punta alla procedura pricipale...
extern void TabbozStartNet(HANDLE hDlg); // 24 Giugno 1998
#endif

// POI LE STRONZATE PER LE FINESTRELLE...

extern HANDLE hInst;
extern HWND   hWndMain;
extern HANDLE hdlgr;

// ED I PROTOTIPI FUNZIONI...

#ifdef TABBOZ_WIN
extern int PASCAL      WinMain(HANDLE hInstance, HANDLE hPrevInstance,
                               LPSTR lpszCmdLine, int cmdShow);
extern BOOL FAR PASCAL MainDlgBoxProc(HWND hDlg, WORD message,
                                      WORD wParam, LONG lParam);
#endif

extern void Evento(HWND hWnd);
extern void RunPalestra(HWND hDlg);            /* 23 Aprile 1998 */
extern void RunTabacchi(HWND hDlg);            /* 23 Aprile 1998 */
extern void RunVestiti(HWND hDlg, int numero); /* 23 Aprile 1998 */

extern BOOL FAR PASCAL About(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL Warning(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL Disco(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL Famiglia(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL Compagnia(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL Tipa(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL Lavoro(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL Scuola(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL Scooter(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL Vestiti(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL Configuration(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL PersonalInfo(HWND hDlg, WORD message, WORD wParam, LONG lParam);
extern BOOL FAR PASCAL Logo(HWND hDlg, WORD message, WORD wParam, LONG lParam);              /* 13 Marzo 1998 */
extern BOOL FAR PASCAL Tabaccaio(HWND hDlg, WORD message, WORD wParam, LONG lParam);         /* 19 Marzo 1998 */
extern BOOL FAR PASCAL Palestra(HWND hDlg, WORD message, WORD wParam, LONG lParam);          /* iniziata il 20 Marzo 1998  */
extern BOOL FAR PASCAL Setup(HWND hDlg, WORD message, WORD wParam, LONG lParam);             /* 9 Giugno 1998 */
extern BOOL FAR PASCAL Spegnimi(HWND hDlg, WORD message, WORD wParam, LONG lParam);          /* 11 Giugno 1998 */
extern BOOL FAR PASCAL Network(HWND hDlg, WORD message, WORD wParam, LONG lParam);           /* 25 Giugno 1998 */
extern BOOL FAR PASCAL MostraSalutieBaci(HWND hDlg, WORD message, WORD wParam, LONG lParam); /* 4 Gennaio 1999 */
extern BOOL FAR PASCAL Cellular(HWND hDlg, WORD message, WORD wParam, LONG lParam);          /* 31 Marzo 1999 */

extern void  TabbozAddKey(char *key, char *v);
extern char *TabbozReadKey(char *key, char *buf);
extern void  TabbozPlaySound(int number);

#ifdef PROMPT_ACTIVE
extern BOOL FAR PASCAL Prompt(HWND hDlg, WORD message, WORD wParam, LONG lParam); /* iniziato il 15 Maggio 1998 */
#endif

extern void  FineProgramma(char *caller);
extern void  CalcolaStudio(void);
extern char *MostraSoldi(u_long i);

extern char  *RRKey(char *xKey);     // 29 Novembre 1998
extern int    new_check_i(int i);    // 14 Marzo 1999
extern u_long new_check_l(u_long i); // 14 Marzo 1999
extern void   new_reset_check(void); // 14 Marzo 1999

extern void openlog(void);
extern void closelog(void);
extern void nomoney(HWND parent, int tipo);
extern void AggiornaPrincipale();
extern void Giorno(HANDLE hInstance);
extern void CalcolaStudio(void);
extern void CalcolaVelocita(HANDLE hDlg);
extern void SalvaTutto(void);
extern void AggiornaTipa(HWND hDlg);
