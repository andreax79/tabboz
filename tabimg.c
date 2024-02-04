//      Tabboz Simulator
//      Copyright (c) 1999 Andrea Bonomi

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

/* Iniziato il 26 Febbraio 1999         */
/* In questo file ci sono le funzioni x */
/* la visualizzazione delle immagini.   */
/* 1 Giugno 1999 -                      */
/* Conversione Tabbozzo -> Tabbozza     */

#include <stdio.h>
#include "os.h"
#include "zarrosim.h"

#define IMG_SIZEX 143
#define IMG_SIZEY 275
#define IMG_X_INC 10
#define IMG_Y_INC 0

static char sccsid[] = "@(#)" __FILE__ " " VERSION " (Andrea Bonomi) " __DATE__;

/*********************************************************************/
/* Parte per (cercare) di disegnare le immagini trasparenti...       */
/*********************************************************************/

// Disegna una BMP trasparente
// Necessita della BMP stessa e di una maschera.

#pragma argsused
static void DrawTransparentBitmap(HDC hdc, HBITMAP hbmpSrc,
                                  HBITMAP hbmpMsk, int x, int y, int cx, int cy, int sx, int sy)
{
    HDC      hdcSrc = CreateCompatibleDC(hdc);
    HDC      hdcMsk = CreateCompatibleDC(hdc);
    COLORREF bc;
    hbmpSrc = SelectObject(hdcSrc, hbmpSrc);
    hbmpMsk = SelectObject(hdcMsk, hbmpMsk);

    bc = SetBkColor(hdc, RGB(255, 255, 255)); // paint the mask

    BitBlt(hdc, x, y, cx, cy, hdcMsk, sx, sy, SRCAND);   // paint the source
    BitBlt(hdc, x, y, cx, cy, hdcSrc, sx, sy, SRCPAINT); // clean up

    hbmpSrc = SelectObject(hdcSrc, hbmpSrc);
    hbmpMsk = SelectObject(hdcMsk, hbmpMsk);

    // restore text color and background color
    SetBkColor(hdc, bc);
    DeleteDC(hdcSrc);
    DeleteDC(hdcMsk);
}

/*********************************************************************/
// Genera una maschera per una determinata immagine ed adatta
// l' immagine stessa per essere visualizzata in trasparenza...

#pragma argsused
static HBITMAP CreateTransparentMask(HBITMAP hbmpSrc, COLORREF rgbTransparent)
{
    BITMAP  bm;
    HBITMAP hbmpMsk;
    HDC     hdc = GetDC(NULL); // get the screen DC
    HDC     hdcSrc = CreateCompatibleDC(hdc);
    HDC     hdcMsk = CreateCompatibleDC(hdc);

    ReleaseDC(NULL, hdc);
    GetObject(hbmpSrc, sizeof(bm), (LPSTR)&bm);

    hbmpSrc = SelectObject(hdcSrc, hbmpSrc); // create mono bitmap the same size
    hbmpMsk = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
    hbmpMsk = SelectObject(hdcMsk, hbmpMsk);

    // set transparent bits to black in the mask
    // color->mono conversion does this for us, if the bk color is
    //  set to the color bitmap's transparent color
    SetBkColor(hdcSrc, rgbTransparent); // transparent color

    BitBlt(hdcMsk, 0, 0, bm.bmWidth, bm.bmHeight, hdcSrc, 0, 0, SRCCOPY);
    // now, we set the transparent bits in the source bitmaps to
    //  black so we can draw using the black source method.

    SetBkColor(hdcSrc, RGB(0, 0, 0));
    SetTextColor(hdcSrc, RGB(255, 255, 255));
    BitBlt(hdcSrc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMsk, 0, 0, SRCAND); // clean up

    hbmpMsk = SelectObject(hdcMsk, hbmpMsk);
    hbmpSrc = SelectObject(hdcSrc, hbmpSrc);

    DeleteDC(hdcMsk);
    DeleteDC(hdcSrc);
    return hbmpMsk;
}

/*********************************************************************/
/* Parte per (cercare) di disegnare immagini nelle dialogs			   */
/*********************************************************************/
// WM_CREATE handler for BMPView
// load bitmap that has the same name as this window

#pragma argsused
int static NEAR PASCAL WMCreate(HWND hwnd, LPCREATESTRUCT lpCS)
{
    HBITMAP hbmp_testa;
    HBITMAP hbmp_giubbotto;
    HBITMAP hbmp_pantaloni;
    HBITMAP hbmp_scarpe;
    HBITMAP hbmp_sfondo;

    TabbozRedraw = 0; // Visto che sta venendo caricato ora, non necessita di essere caricato ancora...

    switch (ImgSelector)
    {
    case 0:
        if (sesso == 'M')
        {
            hbmp_pantaloni = LoadBitmap(hInst, MAKEINTRESOURCE(current_pantaloni + 1520));
            hbmp_testa = LoadBitmap(hInst, MAKEINTRESOURCE(current_testa + 1390));
            hbmp_sfondo = LoadBitmap(hInst, MAKEINTRESOURCE(1560));
            hbmp_giubbotto = LoadBitmap(hInst, MAKEINTRESOURCE(current_gibbotto + 1500));
            hbmp_scarpe = LoadBitmap(hInst, MAKEINTRESOURCE(current_scarpe + 1540));
        }
        else
        {
            hbmp_pantaloni = LoadBitmap(hInst, MAKEINTRESOURCE(current_pantaloni + 1525));
            hbmp_testa = LoadBitmap(hInst, MAKEINTRESOURCE(current_testa + 1370));
            hbmp_sfondo = LoadBitmap(hInst, MAKEINTRESOURCE(1561));
            hbmp_giubbotto = LoadBitmap(hInst, MAKEINTRESOURCE(current_gibbotto + 1507));
            hbmp_scarpe = LoadBitmap(hInst, MAKEINTRESOURCE(current_scarpe + 1540));
        }

        break;
    }

    if (hbmp_testa)
    {
        HBITMAP hmask_testa;
        HBITMAP hmask_giubbotto;
        HBITMAP hmask_pantaloni;
        HBITMAP hmask_scarpe;
        HBITMAP hmask_sfondo;

        hmask_giubbotto = CreateTransparentMask(hbmp_giubbotto, RGB(255, 255, 0));
        hmask_testa = CreateTransparentMask(hbmp_testa, RGB(255, 255, 0));
        hmask_pantaloni = CreateTransparentMask(hbmp_pantaloni, RGB(255, 255, 0));
        hmask_scarpe = CreateTransparentMask(hbmp_scarpe, RGB(255, 255, 0));
        hmask_sfondo = CreateTransparentMask(hbmp_sfondo, RGB(255, 255, 0));

        SetProp(hwnd, "hmask_testa", hmask_testa);
        SetProp(hwnd, "hmask_giubbotto", hmask_giubbotto);
        SetProp(hwnd, "hmask_pantaloni", hmask_pantaloni);
        SetProp(hwnd, "hmask_scarpe", hmask_scarpe);
        SetProp(hwnd, "hmask_sfondo", hmask_sfondo);

        SetProp(hwnd, "hbmp_testa", hbmp_testa);
        SetProp(hwnd, "hbmp_giubbotto", hbmp_giubbotto);
        SetProp(hwnd, "hbmp_pantaloni", hbmp_pantaloni);
        SetProp(hwnd, "hbmp_scarpe", hbmp_scarpe);
        SetProp(hwnd, "hbmp_sfondo", hbmp_sfondo);

        SetWindowPos(hwnd, NULL, 0, 0, IMG_SIZEX, IMG_SIZEY, SWP_NOMOVE | SWP_NOZORDER);
        return 0;
    }

    return 0; // report failure
}

/*********************************************************************/
// WM_DESTORY handler for BMPView

#pragma argsused
int static NEAR PASCAL WMDestroy(HWND hwnd)
{
    HBITMAP hbmp_testa = GetProp(hwnd, "hbmp_testa");
    HBITMAP hbmp_giubbotto = GetProp(hwnd, "hbmp_giubbotto");
    HBITMAP hbmp_pantaloni = GetProp(hwnd, "hbmp_pantaloni");
    HBITMAP hbmp_scarpe = GetProp(hwnd, "hbmp_scarpe");
    HBITMAP hbmp_sfondo = GetProp(hwnd, "hbmp_sfondo");
    HBITMAP hmask_testa = GetProp(hwnd, "hmask_testa");
    HBITMAP hmask_giubbotto = GetProp(hwnd, "hmask_giubbotto");
    HBITMAP hmask_pantaloni = GetProp(hwnd, "hmask_pantaloni");
    HBITMAP hmask_scarpe = GetProp(hwnd, "hmask_scarpe");
    HBITMAP hmask_sfondo = GetProp(hwnd, "hmask_sfondo");

    if (hbmp_testa)
    {
        DeleteObject(hbmp_testa);
        RemoveProp(hwnd, "hbmp_testa");
    }
    if (hbmp_giubbotto)
    {
        DeleteObject(hbmp_giubbotto);
        RemoveProp(hwnd, "hbmp_giubbotto");
    }
    if (hbmp_pantaloni)
    {
        DeleteObject(hbmp_pantaloni);
        RemoveProp(hwnd, "hbmp_pantaloni");
    }
    if (hbmp_scarpe)
    {
        DeleteObject(hbmp_scarpe);
        RemoveProp(hwnd, "hbmp_scarpe");
    }
    if (hbmp_sfondo)
    {
        DeleteObject(hbmp_sfondo);
        RemoveProp(hwnd, "hbmp_sfondo");
    }

    if (hmask_testa)
    {
        DeleteObject(hmask_testa);
        RemoveProp(hwnd, "hmask_testa");
    }
    if (hmask_giubbotto)
    {
        DeleteObject(hmask_giubbotto);
        RemoveProp(hwnd, "hmask_giubbotto");
    }
    if (hmask_pantaloni)
    {
        DeleteObject(hmask_pantaloni);
        RemoveProp(hwnd, "hmask_pantaloni");
    }
    if (hmask_scarpe)
    {
        DeleteObject(hmask_scarpe);
        RemoveProp(hwnd, "hmask_scarpe");
    }
    if (hmask_sfondo)
    {
        DeleteObject(hmask_sfondo);
        RemoveProp(hwnd, "hmask_sfondo");
    }

    return 0;
}

/*********************************************************************/
// WM_PAINT handler for BMPView

#pragma argsused
int static NEAR PASCAL WMPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HBITMAP     hbmp_testa;
    HBITMAP     hbmp_giubbotto;
    HBITMAP     hbmp_pantaloni;
    HBITMAP     hbmp_scarpe;
    HBITMAP     hbmp_sfondo;
    HBITMAP     hmask_testa;
    HBITMAP     hmask_giubbotto;
    HBITMAP     hmask_pantaloni;
    HBITMAP     hmask_scarpe;
    HBITMAP     hmask_sfondo;

    HDC hdc = BeginPaint(hwnd, &ps);

    if (TabbozRedraw)
    { // Ricarica le immagini...
        WMDestroy(hwnd);
        WMCreate(hwnd, NULL);
    }

    hbmp_testa = GetProp(hwnd, "hbmp_testa");
    hbmp_giubbotto = GetProp(hwnd, "hbmp_giubbotto");
    hbmp_pantaloni = GetProp(hwnd, "hbmp_pantaloni");
    hbmp_scarpe = GetProp(hwnd, "hbmp_scarpe");
    hbmp_sfondo = GetProp(hwnd, "hbmp_sfondo");

    hmask_testa = GetProp(hwnd, "hmask_testa");
    hmask_giubbotto = GetProp(hwnd, "hmask_giubbotto");
    hmask_pantaloni = GetProp(hwnd, "hmask_pantaloni");
    hmask_scarpe = GetProp(hwnd, "hmask_scarpe");
    hmask_sfondo = GetProp(hwnd, "hmask_sfondo");

    // Mantenere l' ordine !!!
    if (hbmp_sfondo)
        DrawTransparentBitmap(hdc, hbmp_sfondo, hmask_sfondo, 0, 0, IMG_SIZEX, IMG_SIZEY, 0, 0);

    if (sesso == 'M')
    {
        if (hbmp_pantaloni)
            DrawTransparentBitmap(hdc, hbmp_pantaloni, hmask_pantaloni, 18 + IMG_X_INC, 93 + IMG_Y_INC, IMG_SIZEX, IMG_SIZEY, 0, 0);
    }
    else
    {
        if (hbmp_pantaloni)
            DrawTransparentBitmap(hdc, hbmp_pantaloni, hmask_pantaloni, 23 + IMG_X_INC, 120 + IMG_Y_INC, IMG_SIZEX, IMG_SIZEY, 0, 0);
    }

    if (hbmp_scarpe)
        DrawTransparentBitmap(hdc, hbmp_scarpe, hmask_scarpe, -16 + IMG_X_INC, 205 + IMG_Y_INC, IMG_SIZEX, IMG_SIZEY, 0, 0);

    if (sesso == 'M')
    {
        if (hbmp_giubbotto)
            DrawTransparentBitmap(hdc, hbmp_giubbotto, hmask_giubbotto, -30 + IMG_X_INC, 18 + IMG_Y_INC, IMG_SIZEX, IMG_SIZEY, 0, 0);
        if (hbmp_testa)
            DrawTransparentBitmap(hdc, hbmp_testa, hmask_testa, 32 + IMG_X_INC, 3 + IMG_Y_INC, IMG_SIZEX, IMG_SIZEY, 0, 0);
    }
    else
    {
        if (hbmp_giubbotto)
            DrawTransparentBitmap(hdc, hbmp_giubbotto, hmask_giubbotto, -27 + IMG_X_INC, 39 + IMG_Y_INC, IMG_SIZEX, IMG_SIZEY, 0, 0);
        if (hbmp_testa)
            DrawTransparentBitmap(hdc, hbmp_testa, hmask_testa, 30 + IMG_X_INC, 1 + IMG_Y_INC, IMG_SIZEX, IMG_SIZEY, 0, 0);
    }

    EndPaint(hwnd, &ps);

    return 0;
}

/*********************************************************************/
// Classe BMPView

#pragma argsused

long FAR PASCAL BMPViewWndProc(HWND hWnd, WORD msg,
                               WORD wParam, LONG lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        return WMCreate(hWnd, (LPCREATESTRUCT)lParam);
    case WM_DESTROY:
        return WMDestroy(hWnd);
    case WM_PAINT:
        return WMPaint(hWnd);
    case WM_LBUTTONDOWN:
        /* Display Personal Information box. */
        DialogBox(hInst, MAKEINTRESOURCE(PERSONALINFO), hWnd, PersonalInfo);
        AggiornaPrincipale(hWndMain);
        return (0);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*********************************************************************/
// Registra la Classe BMPView

#pragma argsused
ATOM RegisterBMPViewClass(HANDLE hInst)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));

    wc.lpfnWndProc = BMPViewWndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
#ifdef TABBOZ_WIN16
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW) + 1;
#else
    wc.hbrBackground = (COLOR_WINDOW);
#endif
    wc.lpszClassName = "BMPView";
    return RegisterClass(&wc);
}

/*********************************************************************/
/*********************************************************************/

/*********************************************************************/
// Questa parte si okkupa del disegno della tipa... 6 Maggio 1999

/*********************************************************************/
// WMTipaCreate

#pragma argsused
int static NEAR PASCAL WMTipaCreate(HWND hwnd, LPCREATESTRUCT lpCS)
{
    HBITMAP hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(current_tipa + 1204));
    if (hbmp)
    {
        BITMAP bm;
        SetProp(hwnd, "hTipa", hbmp);
        GetObject(hbmp, sizeof(bm), (LPSTR)&bm);
        SetWindowPos(hwnd, NULL, 0, 0, bm.bmWidth, bm.bmHeight,
                     SWP_NOMOVE | SWP_NOZORDER);
        return 0;
    }
    return -1; // report failure
}

/*********************************************************************/
// WMTipaDestroy

#pragma argsused
int static NEAR PASCAL WMTipaDestroy(HWND hwnd)
{
    HBITMAP hbmp = GetProp(hwnd, "hTipa");
    if (hbmp)
    {
        DeleteObject(hbmp);
        RemoveProp(hwnd, "hTipa");
    }
    return 0;
}

/*********************************************************************/
// WMTipaPaint

#pragma argsused
int static NEAR PASCAL WMTipaPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC         hdc = BeginPaint(hwnd, &ps);
    HDC         hdcMem = CreateCompatibleDC(hdc);
    HBITMAP     hbmp = GetProp(hwnd, "hTipa");
    if (hbmp)
    {
        BITMAP bm;
        GetObject(hbmp, sizeof(bm), (LPSTR)&bm);
        hbmp = SelectObject(hdcMem, hbmp);
        BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
        hbmp = SelectObject(hdcMem, hbmp);
    }
    DeleteDC(hdcMem);
    EndPaint(hwnd, &ps);
    return 0;
}

/*********************************************************************/
// Classe BMPTipa

extern HWND tipahDlg;

#pragma argsused
long FAR PASCAL BMPTipaWndProc(HWND hWnd, WORD msg,
                               WORD wParam, LONG lParam)
{
    char tmp[255];

    switch (msg)
    {
    case WM_CREATE:
        return WMTipaCreate(hWnd, (LPCREATESTRUCT)lParam);
    case WM_DESTROY:
        return WMTipaDestroy(hWnd);
    case WM_PAINT:
        return WMTipaPaint(hWnd);
    case WM_LBUTTONDOWN:
        //								sprintf(tmp,"%d %d",LOWORD(lParam),HIWORD(lParam));
        //								MessageBox( 0, tmp, "BMPTipa", MB_OK );
        if ((((LOWORD(lParam) >= 138) && (LOWORD(lParam) <= 170)) && // Coordinate Tette
             ((HIWORD(lParam) >= 50) && (HIWORD(lParam) <= 65))) ||
            (((LOWORD(lParam) >= 104) && (LOWORD(lParam) <= 136)) && // Coordinate Culo
             ((HIWORD(lParam) >= 78) && (HIWORD(lParam) <= 166))))
        {
            // Palpatina...
            if (Rapporti < 0)
            {
                //			MessageBox( hWnd, "Brutto porco, che cazzo tocchi ?", "Palpatina...", MB_OK | MB_ICONSTOP);
                //			if (sound_active) TabbozPlaySound(604);
                //			AggiornaTipa(tipahDlg);
            }
            else if (Rapporti < (20 + (FigTipa / 2)))
            { // + e' figa, - te la da' (perla di saggezza)
                if (sound_active)
                    TabbozPlaySound(604);
                MessageBox(hWnd, "Brutto porco, che cazzo tocchi ?", "Palpatina...", MB_OK);
                if (Rapporti > 5)
                    Rapporti -= 3;
                AggiornaTipa(tipahDlg);
            }
            else if (Rapporti < (30 + (FigTipa / 2)))
            {
                MessageBox(hWnd, "Dai, smettila... Voi uomini pensato solo a quello...", "Palpatina...", MB_OK | MB_ICONQUESTION);
            }
            else
            {
                MessageBox(hWnd, "Mmhhhhhhhh.........", "Palpatina...", MB_OK | MB_ICONINFORMATION);
                Rapporti + 3;
                if (Rapporti < 100)
                    Rapporti = 100;
                Giorno(hWnd);
                AggiornaTipa(tipahDlg);
            }
        }
        return (0);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*********************************************************************/
// Registra la Classe BMPTipa

#pragma argsused
ATOM RegisterBMPTipaClass(HANDLE hInst)
{
    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = BMPTipaWndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(hInst, MAKEINTRESOURCE(10));
//	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
#ifdef TABBOZ_WIN16
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
#else
    wc.hbrBackground = (COLOR_WINDOW);
#endif
    wc.lpszClassName = "BMPTipa";
    return RegisterClass(&wc);
}
