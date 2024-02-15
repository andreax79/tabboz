/* Tabboz Simulator                     */
/* (C) Copyright 1998 by Andrea Bonomi  */
/* Definizione per il modulo di rete    */

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

/**************************************************************/
/* Define Socket Constants				      */
/**************************************************************/

#define NO_FLAGS_SET 0
#define PORT (u_short)79
#define MAXBUFLEN 256

/**************************************************************/
/* Roba generica riguardante windows e i sockets	      */
/**************************************************************/

#define WM_LISTEN (WM_USER + 1)     // listen for connections
#define WM_CONNECTION (WM_USER + 2) // connection request awaits
#define WM_GETREQUEST (WM_USER + 3) // request is waiting
#define WM_REPLY (WM_USER + 4)      // start reply

#define NET_BASE (WM_USER + 5) // network messages start here...
#define SOCKET_MESSAGE NET_BASE
#define NET_NAME (NET_BASE + 1)

/**************************************************************/
/* Altre definizioni...					      */
/**************************************************************/

#define MAXHOSTNAMELEN 64

#define STRING_UNKNOWN "localhost" // can't determine local host name

#define MAXLINE (80) /* max length of a string */

#define WSADESCRIPTION_LEN 256
#define WSASYS_STATUS_LEN 128

/**************************************************************/
/* Strutture						      */
/**************************************************************/

enum tagClientState
{
    STATE_WAIT_FOR_USER,
    STATE_SENDING,
    STATE_SEND_TIME,
    STATE_FINISHED,
    STATE_CLOSING,
    STATE_WSERROR
};

typedef enum tagClientState ClientState;

typedef struct tagCLIENT
{
    SOCKET sSocket; /* The clients socket */
    HANDLE hClient; /* The memory handle  */

    SOCKADDR_IN saPeer; /* The Internet address of peer */
    char        szPeer[MAXLINE];

    ClientState iState;

    int iExtErr;

    //	HFILE		hfFile;		     /* The MS-DOS file handle */

    /* We need buffers to talk with - unique ones at that */

    HANDLE hOutputBuffer;  // Handle to output FIFO buffer
    LPSTR  lpOutputBuffer; // FIFO buffer for output
    int    iOutputSize;
    HANDLE hInputBuffer;  // Handle to input FIFO buffer
    LPSTR  lpInputBuffer; // FIFO buffer for input
    int    iInputSize;    // Size of FIFO buffer for input

    /* Keep the buffer state information here... */

    int  fifoOutputStart;
    int  fifoOutputStop;
    BOOL fifoOutputFull;
    int  fifoInputStart;
    int  fifoInputStop;
    BOOL fifoInputFull;

    struct tagCLIENT FAR *lpPrevClient; // Link to previous client struct
    struct tagCLIENT FAR *lpNextClient; // Link to next client struct

} CLIENT;

typedef CLIENT FAR *LPCLIENT;

/**************************************************************/
/* Definizioni delle Funzioni				      */
/**************************************************************/

extern void     neterrorlog(char *s);
extern LPCLIENT fingerAddClient(SOCKET sSocket);
