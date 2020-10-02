
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <conio.h>
#include "win.h"


static void Delay(nSeconds)
short	   nSeconds;
{
    auto     time_t    lQuitTime;

    time(&lQuitTime);
    lQuitTime += (time_t) nSeconds;
    while (time(NULL) < lQuitTime)
	;
    return;
}



static HWND IntroWindow()
{
    register HWND	 hWnd;
    auto     short	 nTxtClr   = WHITE | REV_BLUE | HI_INTENSITY;
    auto     short	 nBriteClr = YELLOW | REV_BLUE | HI_INTENSITY;

    hWnd = WinCreateWindow(7, 10, 60, 10, nTxtClr,
			   SNGL_LINE_ALL_SIDES, nTxtClr, TRUE);
    WinCenterText(hWnd, 1, "Welcome to C-WIN", nTxtClr);
    WinCenterText(hWnd, 3, "a public domain contribution by", nTxtClr);
    WinCenterText(hWnd, 5, "Bob Withers", nBriteClr);
    WinCenterText(hWnd, 6, "649 Meadowbrook St", nBriteClr);
    WinCenterText(hWnd, 7, "Allen, Texas 75002", nBriteClr);
    Delay(5);
    WinMoveWindow(hWnd, 1, 10);
    return(hWnd);
}


static void DocWindow()
{
    register HWND     hWnd;
    auto     short    nTxtClr = REV_WHITE | BLUE;

    hWnd = WinCreateWindow(9, 10, 60, 16, nTxtClr,
			   DBL_LINE_ALL_SIDES, nTxtClr, FALSE);
    WinCenterText(hWnd, 1, "C-WIN Version 1.00", nTxtClr);
    WinSetCursorPos(hWnd, 3, 2);
    WinTextOut(hWnd,
	       "C-WIN is a collection of simple windowing routines for",
	       nTxtClr);
    WinSetCursorPos(hWnd, 4, 2);
    WinTextOut(hWnd,
	       "the IBM/PC and true clones. The package was written with",
	       nTxtClr);
    WinSetCursorPos(hWnd, 5, 2);
    WinTextOut(hWnd,
	       "the Microsoft C Compiler V5.00 and has been tested under",
	       nTxtClr);
    WinSetCursorPos(hWnd, 6, 2);
    WinTextOut(hWnd,
	       "the Turbo-C as well as Quick-C compilers. In the past I",
	       nTxtClr);
    WinSetCursorPos(hWnd, 7, 2);
    WinTextOut(hWnd,
	       "have developed several windowing packages for the PC but",
	       nTxtClr);
    WinSetCursorPos(hWnd, 8, 2);
    WinTextOut(hWnd,
	       "always in assembly language. I wanted a package written",
	       nTxtClr);
    WinSetCursorPos(hWnd, 9, 2);
    WinTextOut(hWnd,
	       "in C to allow it to easily be ported to other compilers.",
	       nTxtClr);
    WinSetCursorPos(hWnd, 10, 2);
    WinTextOut(hWnd,
	       "C-WIN is the result of my first efforts at this goal.",
	       nTxtClr);
    WinSetCursorPos(hWnd, 11, 2);
    WinTextOut(hWnd,
	       "I'm releasing it to the public domain in the hopes that",
	       nTxtClr);
    WinSetCursorPos(hWnd, 12, 2);
    WinTextOut(hWnd,
	       "others may find it a suitable base for their own work.",
	       nTxtClr);
    WinCenterText(hWnd, 14, "* Press any key to continue *",
	       REV_RED | WHITE | HI_INTENSITY);
    getch();
    WinDestroyWindow(hWnd);
    return;
}


static void ShowBorders()
{
    register short	i;
    auto     short	nTxtClr = REV_WHITE | BLUE;
    auto     HWND	hWnd[6];
    static   char      *pText[] = {  "Windows can",
				     "be drawn with",
				     "various borders,",
				     "or with",
				     "no border",
				     "at all!"
				  };

    for (i = 0; i < 6; ++i)
    {
	hWnd[i] = WinCreateWindow(2 + i * 2, 2 + i * 2, 30, 6,
				  nTxtClr, i,
				  REV_WHITE | i | HI_INTENSITY, TRUE);
	WinCenterText(hWnd[i], 1, pText[i], nTxtClr);
	Delay(1);
    }
    Delay(5);
    for (i = 5; i >= 0; --i)
    {
	WinMoveWindow(hWnd[i], 2 + (5 - i) * 2, 50 - (5 - i) * 2);
	Delay(1);
    }
    Delay(5);
    for (i = 0; i < 6; ++i)
	WinMoveWindow(hWnd[i], 2 + i * 2, 1);
    Delay(5);
    for (i = 5; i >= 0; --i)
	WinDestroyWindow(hWnd[i]);
    return;
}


void HideShowWindow(hHideWnd)
HWND	    hHideWnd;
{
    auto     HWND    hWnd;
    auto     short   nTxtClr = REV_MAGENTA | BLUE | HI_INTENSITY;

    hWnd = WinCreateWindow(1, 1, 30, 6, nTxtClr, NO_BOX, nTxtClr, FALSE);
    WinCenterText(hWnd, 1, "Windows can be hidden", nTxtClr);
    Delay(5);
    WinHideWindow(hHideWnd);
    WinCenterText(hWnd, 2, "Moved while hidden", nTxtClr);
    Delay(2);
    WinMoveWindow(hHideWnd, 7, 10);
    WinCenterText(hWnd, 3, "and", nTxtClr);
    WinCenterText(hWnd, 4, "re-shown at any time", nTxtClr);
    Delay(5);
    WinShowWindow(hHideWnd);
    WinDestroyWindow(hWnd);
    return;
}


void ScrollWindow(hWnd)
register HWND	 hWnd;
{
    auto     short     nRow, nCol;
    auto     short     nWidth, nHeight;
    auto     short     nWinClr;
    auto     short     nNoRows;
    auto     char     *pBuf;

    nRow    = WinGetWindowRow(hWnd);
    nCol    = WinGetWindowCol(hWnd);
    nWidth  = WinGetWindowWidth(hWnd);
    nHeight = nNoRows = WinGetWindowHeight(hWnd);
    nWinClr = WinGetWindowClr(hWnd);
    if (WinGetBorderType(hWnd) != NO_WIND_BORDER)
    {
	nWidth	+= 2;
	nHeight += 2;
    }
    pBuf = malloc(ScrGetRectSize(nWidth, nHeight));
    if (NULL == pBuf)
	return;
    ScrSaveRect(nRow, nCol, nWidth, nHeight, pBuf);
    WinScrollWindowUp(hWnd);
    WinScrollWindowUp(hWnd);
    WinCenterText(hWnd, nNoRows, "Windows can be scrolled up", nWinClr);
    Delay(2);
    WinScrollWindowUp(hWnd);
    Delay(2);
    WinScrollWindowUp(hWnd);
    Delay(2);
    WinScrollWindowDown(hWnd);
    WinScrollWindowDown(hWnd);
    WinCenterText(hWnd, 1, "Windows can be scrolled down", nWinClr);
    Delay(2);
    WinScrollWindowDown(hWnd);
    Delay(2);
    WinScrollWindowDown(hWnd);
    Delay(2);
    ScrRestoreRect(nRow, nCol, nWidth, nHeight, pBuf);
    free(pBuf);
    WinCenterText(hWnd, nNoRows, " Press any key to end demo ",
		  REV_RED | WHITE | HI_INTENSITY);
    return;
}


main()
{
    auto     HWND      hIntroWnd;

    WinInitialize();
    ScrCursorOff();
    WinClearScreen(NULL, BLACK);
    hIntroWnd = IntroWindow();
    DocWindow();
    WinMoveWindow(hIntroWnd, 16, 10);
    ShowBorders();
    HideShowWindow(hIntroWnd);
    ScrollWindow(hIntroWnd);
    getch();
    WinDestroyWindow(hIntroWnd);
    WinTerminate();
    ScrCursorOn();
    return(0);
}
