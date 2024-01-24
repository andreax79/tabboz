// Tabboz Simulator
// (C) Copyright 1998 by Andrea Bonomi
// Iniziato il 24 Giugno 1998
// 5 Gigno 1999 - Conversione Tabbozzo -> Tabbozza

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

#ifdef TABBOZ_WIN

#include "winsock.h"
#include <conio.h>
#include <math.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "net.h"
#include "zarrosim.h"

static char sccsid[] = "@(#)" __FILE__ " " VERSION " (Andrea Bonomi) " __DATE__;

#ifndef NONETWORK /* ---------------------------------------------------------------------------*/

/* global variable */
int swivel_angle, pitch_angle, yaw_angle;

/* function prototype */
void SendReceiveToClient(void *cs);
void transferData(char[]);

WSADATA     Data;
SOCKADDR_IN serverSockAddr;
SOCKADDR_IN clientSockAddr;
SOCKET      serverSocket;
SOCKET      clientSocket;
int         addrLen = sizeof(SOCKADDR_IN);
int         status;

int  net_enable;
char lastconnect[255];
char lastneterror[255];
HWND NEThDlg;
int  PortNumber;

/**************************************************************/

void TabbozStartNet(HANDLE hDlg)
{
    char tmp[255];
    int  iSndSize;

    net_enable = 1;

    sprintf(lastconnect, "none");

    /*Initialize the winsock.dll*/
    status = WSAStartup(0x101, &Data);

    if (status != 0)
    {
        MessageBox(hDlg,
                   "La funzione WSAStartup non e' andata a buon fine...\nLe funzioni di rete del Tabboz Simulator verranno disabilitate...", "Network Startup", MB_OK | MB_ICONHAND);
#ifdef TABBOZ_DEBUG
        neterrorlog("ERROR: WSAStartup Unsuccessful");
#endif
        net_enable = 0;
        return;
    }

#ifdef TABBOZ_DEBUG
    writelog("net: WSAStart() is OK");
    sprintf(tmp, "net: Windows Sockets  = %-40s", Data.szDescription);
    writelog(tmp);
    sprintf(tmp, "net: System status    = %-40s", Data.szSystemStatus);
    writelog(tmp);
    sprintf(tmp, "net: Vendor info      = %-40s", Data.lpVendorInfo);
    writelog(tmp);
    sprintf(tmp, "net: Winsock version  = %04x", Data.wVersion);
    writelog(tmp);
    sprintf(tmp, "net: Max Sockets      = %d", Data.iMaxSockets);
    writelog(tmp);
    sprintf(tmp, "net: Max Datagram     = %d", Data.iMaxUdpDg);
    writelog(tmp);
#endif

#define INUTILE
#ifdef INUTILE
    {
        char      szLocalHostName[MAXHOSTNAMELEN];
        LPHOSTENT lpheHostEnt;

        /* Get the local host name and save it */
        if (gethostname((LPSTR)szLocalHostName, sizeof(szLocalHostName)) == SOCKET_ERROR)
        {
            lstrcpy((LPSTR)szLocalHostName, (LPSTR)STRING_UNKNOWN);
        }
        else
        {
            lpheHostEnt = gethostbyname((LPSTR)szLocalHostName);
            if (lpheHostEnt != NULL)
            {
                lstrcpy((LPSTR)szLocalHostName, lpheHostEnt->h_name);
            }
        }

        sprintf(tmp, "net: Hostname         = %s", szLocalHostName);
        writelog(tmp);
    }
#endif

    /* Zero the sockaddr in structure		*/
    memset(&serverSockAddr, 0, sizeof(serverSockAddr));

    /* Specify the port portion of the address  */
    serverSockAddr.sin_port = htons(PortNumber);

    /* SPecify family				*/
    serverSockAddr.sin_family = AF_INET;
    serverSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

/* Create the socket			*/
#ifdef TABBOZ_DEBUG
    writelog("net: Starting the Server.....");
#endif

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET)
    {
#ifdef TABBOZ_DEBUG
        neterrorlog("ERROR: Socket Unsuccessful");
#endif

        status = WSACleanup();
        if (status == SOCKET_ERROR)
        {
#ifdef TABBOZ_DEBUG
            neterrorlog("ERROR: WSA CLeanup Unsuccessful");
#endif
        }
        net_enable = 0;
        return;

    } /* end if serverSocket */

    /* Associate the Socket with the address */
    status = bind(serverSocket, (LPSOCKADDR)&serverSockAddr, sizeof(serverSockAddr));
    if (status == SOCKET_ERROR)
    {
#ifdef TABBOZ_DEBUG
        neterrorlog("ERROR: Bind Unsuccessful");
        net_enable = 0;
        return;
#endif
    }

    /* Start to listen to connections */
    status = listen(serverSocket, 1);
    if (status == SOCKET_ERROR)
    {
#ifdef TABBOZ_DEBUG
        neterrorlog("ERROR: Listen Unsuccessful");
        net_enable = 0;
        return;
#endif
    }

#ifdef TABBOZ_DEBUG
    sprintf(tmp, "net: Tabboz Simulator Listening on Port %d", PortNumber);
    writelog(tmp);
#endif

    if (WSAAsyncSelect(serverSocket, hDlg, SOCKET_MESSAGE, FD_ACCEPT) == SOCKET_ERROR)
    {
#ifdef TABBOZ_DEBUG
        neterrorlog("ERROR: select() Unsuccessful");
#endif
        closesocket(serverSocket);
        net_enable = 0;
        return;
    }

    iSndSize = 1024;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char FAR *)&iSndSize, sizeof(iSndSize)) == SOCKET_ERROR)
    {
#ifdef TABBOZ_DEBUG
        neterrorlog("Error setting socket SO_SNDBUF");
#endif
    }

    sprintf(lastneterror, "Network Server is Running");

} /* End TabbozStartNet */

/**************************************************************/

/**************************************************************/

void SendReceiveToClient(void *cs)
{
    char buffer[MAXBUFLEN];
    int  status;
    int  numsnt;
    int  numrcv;
    int  ctr = 0;
    char sent[1024];
    char tmp[255];
    char finished[] = "The task has been finished!\n";
    char unfinished[] = "Not finished yet!!!\n";

    SOCKET clientSocket = (SOCKET)cs;

    sprintf(sent, "Tabboz Simulator %s %s\n\r\n\r"
                  "Nome: [%s %s]  Soldi: [%s]  Sesso: [%c]\n\r"
                  "Fig: [%03d]  Rep: [%03d]  Prof.Scol.: [%03d]\n\r",
            VERSION, __DATE__, Nome, sesso, Cognome, MostraSoldi(Soldi), Fama, Reputazione, Studio);

    if (Rapporti != 0)
    {
        if (sesso == 'M')
            sprintf(tmp, "Tipa: [%s]  Rap: [%03d]\n\r", Nometipa, Rapporti);
        else
            sprintf(tmp, "Tipo: [%s]  Rap: [%03d]\n\r", Nometipa, Rapporti);

        strcat(sent, tmp);
    }
    if (ScooterData.stato != -1)
    {
        sprintf(tmp, "Scooter: [%s]  Stato: [%03d]\n\r", ScooterData.nome, ScooterData.stato);
        strcat(sent, tmp);
    }

    numsnt = send(clientSocket, sent, strlen(sent) + 1, NO_FLAGS_SET);

    //	sprintf(sent, "\n\r%s %d %s",InfoSettimana[x_giornoset-1].nome,x_giorno,InfoMese[x_mese-1].nome); strcat(sent,tmp);

    //	numsnt = send(clientSocket,sent,strlen(sent)+1,NO_FLAGS_SET);

    status = shutdown(clientSocket, 2);
    if (status == SOCKET_ERROR)
    {
#ifdef TABBOZ_DEBUG
        writelog("net: ERROR: Shutdown Unsuccessful");
#endif
    }

    /* Close Socket	*/
    status = closesocket(clientSocket);
    if (status == SOCKET_ERROR)
    {
#ifdef TABBOZ_DEBUG
        writelog("net: ERROR: Close Socket Unsuccessful");
#endif
    }

    return;
    /*
        while(1)
        {
             numrcv = recv(clientSocket,buffer, MAXBUFLEN, NO_FLAGS_SET);
             if ((numrcv == 0) || (numrcv == SOCKET_ERROR))
             {
            printf("\n Connection Terminated \n");
            break;
             }
             ctr=1;
             if (ctr == 1)
             {

            strcpy(sent,finished);
            printf("\n Done \n");

            printf("\n Sending back to Client the message => %s",sent);
            numsnt = send(clientSocket,sent,strlen(sent)+1,NO_FLAGS_SET);
            status = shutdown(clientSocket, 2);
            if (status == SOCKET_ERROR)
            {
                 printf("\n ERROR: Shutdown Unsucessful\n");
            }
            // close Socket
            status = closesocket(clientSocket);
            if (status == SOCKET_ERROR)
            {
                 printf("\n ERROR: Close Socket Unsuccessful \n");
            }
             }
             ctr++;
        }
        printf("\n The Java Client's message is => %s", &buffer);
        transferData(buffer);
    */
}

/**************************************************************/

void transferData(char newbuf[])

{
    char str1, str2, str3;
    int  angle1, angle2, angle3;
    int  dirstr1, dirstr2, dirstr3;

    sscanf(newbuf, "%c %d %d %c %d %d %c %d %d", &str1, &dirstr1, &angle1,
           &str2, &dirstr2, &angle2, &str3, &dirstr3, &angle3);
    printf("\n str1 : %c", str1);
}

/**************************************************************/
/* Network Config / Stat		    [25 Giugno 1998]  */
/**************************************************************/

#pragma argsused
BOOL FAR PASCAL Network(HWND hDlg, WORD message, WORD wParam, LONG lParam)
{
    char       tmp[1024];
    static int net_temp_status;
    static int temp_PortNumber;

    if (message == WM_INITDIALOG)
    {

        net_temp_status = net_enable;
        temp_PortNumber = PortNumber;

        if (Data.wVersion != 0)
        {
            sprintf(tmp, "%s", Data.szDescription);
            SetDlgItemText(hDlg, 101, tmp);
            sprintf(tmp, "%s", Data.szSystemStatus);
            SetDlgItemText(hDlg, 102, tmp);
            sprintf(tmp, "%04x", Data.wVersion);
            SetDlgItemText(hDlg, 103, tmp);
        }
        else
        {
            sprintf(tmp, "");
            SetDlgItemText(hDlg, 101, tmp);
            SetDlgItemText(hDlg, 102, tmp);
            SetDlgItemText(hDlg, 103, tmp);
        }
        sprintf(tmp, "%s", lastconnect);
        SetDlgItemText(hDlg, 104, tmp);

        sprintf(tmp, "%d", PortNumber);
        SetDlgItemText(hDlg, 105, tmp);
        SetFocus(GetDlgItem(hDlg, 105));

        if (net_enable)
            SendMessage(GetDlgItem(hDlg, 106), BM_SETCHECK, TRUE, 0L);

        sprintf(tmp, "%s", lastneterror);
        SetDlgItemText(hDlg, 107, tmp);

        return (TRUE);
    }
    else if (message == WM_COMMAND)
    {
        switch (wParam)
        {
        case 105:
            GetDlgItemText(hDlg, wParam, tmp, sizeof(tmp));
            temp_PortNumber = atoi(tmp);
            if (temp_PortNumber == 0)
                temp_PortNumber = 79;
            return (TRUE);

        case 106:
            net_temp_status = !net_temp_status;
            return (TRUE);

        case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return (TRUE);

        case IDOK:
            if (net_temp_status != net_enable)
            {
                net_enable = net_temp_status;
                if (net_enable)
                {
                    /* Attiva il server... */
                    TabbozStartNet(NEThDlg);
                }
                else
                {
                    /* Disattiva il server... */
                    closesocket(serverSocket);
                    WSACleanup();
#ifdef TABBOZ_DEBUG
                    writelog("net: Tabboz Simulator Server is going down...");
                    sprintf(lastneterror, "Network Server is Down");
#endif
                }
            }

            if (PortNumber != temp_PortNumber)
            {
                PortNumber = temp_PortNumber;

                if (net_enable)
                {
                    /* Disattiva il server... */
                    closesocket(serverSocket);
                    WSACleanup();
#ifdef TABBOZ_DEBUG
                    writelog("net: Tabboz Simulator Server is going down...");
#endif

                    /* Riattiva il server... */
                    TabbozStartNet(NEThDlg);
                }
            }

            EndDialog(hDlg, TRUE);
            return (TRUE);

        default:
            return (TRUE);
        }
    }

    return (FALSE);
}

/**************************************************************/

void neterrorlog(char *s)
{
    char tmp[255];

    sprintf(lastneterror, "%s", s);
#ifdef TABBOZ_DEBUG
    sprintf(tmp, "net: %s", s);
    writelog(tmp);
#endif
}
/**************************************************************/

#endif /* NONETWORK */
#endif
