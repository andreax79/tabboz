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

#include "os.h"
#include <stdio.h>
#include <string.h>

#include "zarrosim.h"
static char sccsid[] = "@(#)" __FILE__ " " VERSION " (Andrea Bonomi) " __DATE__;


char nome_del_file_su_cui_salvare[256];


// Test delle funzioni di lettura/scrittura
// #define RTEST /* 12 Marzo 1999 */

#ifdef RTEST
#include <stdio.h>
#endif


// --------------------------------------------------------------------------
// Windows 16 ---------------------------------------------------------------
// Per Windows 3.1x

#ifdef TABBOZ_WIN16


// Memorizza una stringa nel profilo.

void   TabbozAddKey(char *KeyName,char *KeyValue);
{
	WriteProfileString("Tabboz",KeyName,KeyValue);
};

// Legge una stringa dal profilo.

extern  char  *TabbozReadKey(char *key,char *buf);
{
	GetProfileString("Tabboz",key,NULL,buf,32);
	if (*buf == NULL) return NULL;
	return buf;
};

// Inizia la riproduzione di un suono
void  TabbozPlaySound(int number)
{
	char filename[20];
	sprintf(filename,"Tabs%04.Wav",number);
	sndPlaySound(filename, SND_ASYNC | SND_NODEFAULT | SND_FILENAME);
};

// Legge una stringa dal profilo memorizzandola in un buffer locale.
char *RRKey(char *xKey)
{
static char  a[255];
	if (TabbozReadKey(xKey,a) == 0)
	  *a='\0';
	return(a);
}

#endif




// --------------------------------------------------------------------------
// Windows 32 ---------------------------------------------------------------
// Per Windows 9X, NT, 2000, ...

#ifdef TABBOZ32

#include <mmsystem.h> // Per sndPlaySound

#define REG_MAXSIZE 	30	// Numero massimo di caratteri
#define TABBOZ_KEY_NAME "Software\\Obscured Truckware\\Tabboz Simulator 32"

void SpegniISuoni()
{
	PlaySound(NULL,NULL,0x0040);
//	PlaySound(NULL,NULL,SND_PURGE);
}

// Aggiunge una stringa al registro di configurazione
void   TabbozAddKey(char *KeyName,char *KeyValue)
{
	LONG  Status;
	HKEY  xKey;
	DWORD KeyLen;
	LONG	 lpdwDisposition;

	if (nome_del_file_su_cui_salvare[0] == 0) {

		 Status=RegCreateKeyEx( HKEY_CURRENT_USER,TABBOZ_KEY_NAME,
					0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&xKey,&lpdwDisposition);

		 if (Status == 0) {
				KeyLen=strlen(KeyValue);
				RegSetValue(xKey,KeyName,1,KeyValue,KeyLen);
				RegCloseKey(xKey);
		 }
	} else // 21 lug 2000 - Scrittura su di un file specifico.
		WritePrivateProfileString("Tabboz",KeyName,KeyValue,nome_del_file_su_cui_salvare);
};


// Legge la stringa KeyName e la restitisce come OutPut usando KeyValue come buffer...

char  *TabbozReadKey(char *KeyName,char *KeyValue)
{
	LONG  Status;
	HKEY  xKey;
	LONG	KeyLen;

	if (nome_del_file_su_cui_salvare[0] == 0) {

		 Status=RegOpenKeyEx( HKEY_CURRENT_USER,TABBOZ_KEY_NAME,
					0,KEY_ALL_ACCESS,&xKey);

		 if (Status == 0) {
				KeyLen=REG_MAXSIZE;
				Status=RegQueryValue(xKey,KeyName,KeyValue,&KeyLen);
				RegCloseKey(xKey);

				if (Status == 0) return(KeyValue);

		 }

		return(NULL);
  } else { // 21 lug 2000 - Lettura da un file specifico.
		GetPrivateProfileString("Tabboz",KeyName,NULL,KeyValue,32,nome_del_file_su_cui_salvare);
		if (*KeyValue == NULL) return NULL;
		return KeyValue;
  }
}


// Inizia la riproduzione di un suono
void  TabbozPlaySound(int number)
{
	char filename[20];
	sprintf(filename,"Tabs%04d.Wav",number);
	sndPlaySound(filename, SND_ASYNC | SND_NODEFAULT);
};



// Legge una stringa dal profilo memorizzandola in un buffer locale.
char *RRKey(char *xKey)
{
static char  a[255];
	if (TabbozReadKey(xKey,a) == 0)
	  *a='\0';
	return(a);
}

#endif









// -----------------------------------------------------------------
// Test delle funzioni di lettura/scrittura

#ifdef RTEST
main()
{
	printf("--test--\n");
	printf("%s\n",RRKey("TestKey"));
	TabbozAddKey("TestKey","RTest RTest 2 1234567890 ABCD @@@");
	printf("%s\n",RRKey("TestKey"));
	TabbozAddKey("TestKey","short key...");
	printf("%s\n",RRKey("TestKey"));
	printf("%s\n",RRKey("NoKey"));
	printf("%s\n",RRKey("Nome"));
	printf("%s\n",RRKey("Cognome"));
	printf("--end--\n");
}
#endif

