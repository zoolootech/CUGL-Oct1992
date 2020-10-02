
/***************************************************************************/
/* WIN	    - Routines which provide windowing functionality		   */
/*									   */
/*									   */
/*									   */
/***************************************************************************/
/*			     Modification Log				   */
/***************************************************************************/
/* Version   Date   Programmer	 -----------  Description  --------------- */
/*									   */
/* V01.00   112787  Bob Withers  Program intially complete.		   */
/*									   */
/*									   */
/***************************************************************************/

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "win.h"

#define MAX_WINDOWS		   20


struct sWinData
{
    BYTE		cRow;
    BYTE		cCol;
    BYTE		cWidth;
    BYTE		cHeight;
    BYTE		cWinWidth;
    BYTE		cWinHeight;
    BYTE		cWinClr;
    BYTE		cBdrType;
    BYTE		cBdrClr;
    BYTE		cCurRow;
    BYTE		cCurCol;
    char	       *pHidden;
    char		cSaveData[1];
};
typedef struct sWinData WINDATA;
typedef struct sWinData *PWINDATA;

static PWINDATA 	WinHandle[MAX_WINDOWS + 1];


/***************************************************************************/
/*  WinCvtHandle    - Convert a window handle into a pointer to the	   */
/*		      window information data structure.		   */
/*  Parms:								   */
/*    hWnd	    - handle to the window				   */
/*									   */
/*  Return Value:     pointer to WINDATA or NULL if invalid handle	   */
/***************************************************************************/

static PWINDATA near pascal WinCvtHandle(hWnd)
HWND	   hWnd;
{
    if (hWnd < 0 || hWnd > MAX_WINDOWS)
	return(NULL);
    return(WinHandle[hWnd]);
}


/***************************************************************************/
/*  WinGetHandle    - Return an unused window handle.			   */
/*									   */
/*  Parms:	      None						   */
/*									   */
/*  Return Value:     Window handle or NULL if none available		   */
/***************************************************************************/

static HWND near pascal WinGetHandle()
{
    register int	 i;

    for (i = 1; i <= MAX_WINDOWS; ++i)
    {
	if (NULL == WinHandle[i])
	    return(i);
    }
    return(NULL);
}


/***************************************************************************/
/*  WinExplodeWindow - Draws an exploded window on the screen.		   */
/*									   */
/*  Parms:								   */
/*    nRow	    - Top row of requested window (1 relative)		   */
/*    nCol	    - Left column of requested window (1 relative)	   */
/*    nWidth	    - Width (in columns) of requested window		   */
/*    nHeight	    - Height (in rows) of requested window		   */
/*    nWinClr	    - Color of the window background			   */
/*    nBdrType	    - Type of border for this window (defined in WIN.H)    */
/*			  NO_BOX					   */
/*			  DBL_LINE_TOP_BOTTOM				   */
/*			  DBL_LINE_SIDES				   */
/*			  DBL_LINE_ALL_SIDES				   */
/*			  SNGL_LINE_ALL_SIDES				   */
/*			  GRAPHIC_BOX					   */
/*			  NO_WIND_BORDER				   */
/*    nBdrClr	    - Color or the window border			   */
/*									   */
/*  Return Value:     None						   */
/***************************************************************************/

void near pascal WinExplodeWindow(nRow, nCol, nWidth, nHeight,
				  nWinClr, nBdrType, nBdrClr)
short	   nRow, nCol, nWidth, nHeight;
short	   nWinClr, nBdrType, nBdrClr;
{
    register short     nLRR, nLRC, nX1, nY1, nX2, nY2;

    nLRR  = nRow + nHeight - 1;
    nLRC  = nCol + nWidth - 1;
    nX1   = (nRow + (nHeight >> 1)) - 1;
    nY1   = (nCol + (nWidth >> 1)) - 3;
    nX2   = nX1 + 2;
    nY2   = nY1 + 5;
    while (TRUE)
    {
	ScrClearRect(nX1, nY1, nY2 - nY1 + 1, nX2 - nX1 + 1, nWinClr);
	ScrDrawRect(nX1, nY1, nY2 - nY1 + 1, nX2 - nX1 + 1,
		    nBdrClr, nBdrType);
	if (nX1 == nRow && nY1 == nCol && nX2 == nLRR && nY2 == nLRC)
	    break;
	nX1 = (nX1 - 1 < nRow) ? nRow : nX1 - 1;
	nY1 = (nY1 - 3 < nCol) ? nCol : nY1 - 3;
	nX2 = (nX2 + 1 > nLRR) ? nLRR : nX2 + 1;
	nY2 = (nY2 + 3 > nLRC) ? nLRC : nY2 + 3;
    }
    return;
}



/***************************************************************************/
/*  WinDrawWindow    - Draws a window on the screen without creating the   */
/*		       WINDATA structure or saving the previous screen	   */
/*		       contents.					   */
/*									   */
/*  Parms:								   */
/*    nRow	    - Top row of requested window (1 relative)		   */
/*    nCol	    - Left column of requested window (1 relative)	   */
/*    nWidth	    - Width (in columns) of requested window		   */
/*    nHeight	    - Height (in rows) of requested window		   */
/*    nWinClr	    - Color of the window background			   */
/*    nBdrType	    - Type of border for this window (defined in WIN.H)    */
/*			  NO_BOX					   */
/*			  DBL_LINE_TOP_BOTTOM				   */
/*			  DBL_LINE_SIDES				   */
/*			  DBL_LINE_ALL_SIDES				   */
/*			  SNGL_LINE_ALL_SIDES				   */
/*			  GRAPHIC_BOX					   */
/*			  NO_WIND_BORDER				   */
/*    nBdrClr	    - Color or the window border			   */
/*    bExplodeWin   - Boolean value requesting either a pop-up or	   */
/*		      exploding window					   */
/*			  TRUE	 ==> Exploding window			   */
/*			  FALSE  ==> Pop-up window			   */
/*									   */
/*  Return Value:     None						   */
/***************************************************************************/

void pascal WinDrawWindow(nRow, nCol, nWidth, nHeight,
			  nWinClr, nBdrType, nBdrClr, bExplodeWin)
short	   nRow, nCol, nWidth, nHeight;
short	   nWinClr, nBdrType, nBdrClr;
short	   bExplodeWin;
{
    if (bExplodeWin)
	WinExplodeWindow(nRow, nCol, nWidth, nHeight,
			 nWinClr, nBdrType, nBdrClr);
    else
    {
	ScrClearRect(nRow, nCol, nWidth, nHeight, nWinClr);
	ScrDrawRect(nRow, nCol, nWidth, nHeight, nBdrClr, nBdrType);
    }
    return;
}


/***************************************************************************/
/*  WinCreateWindow - Create a window with the requested attributes and    */
/*		      return a handle which may be used to identify this   */
/*		      particular window in future calls.		   */
/*  Parms:								   */
/*    nRow	    - Top row of requested window (1 relative)		   */
/*    nCol	    - Left column of requested window (1 relative)	   */
/*    nWidth	    - Width (in columns) of requested window		   */
/*    nHeight	    - Height (in rows) of requested window		   */
/*    nWinClr	    - Color of the window background			   */
/*    nBdrType	    - Type of border for this window (defined in WIN.H)    */
/*			  NO_BOX					   */
/*			  DBL_LINE_TOP_BOTTOM				   */
/*			  DBL_LINE_SIDES				   */
/*			  DBL_LINE_ALL_SIDES				   */
/*			  SNGL_LINE_ALL_SIDES				   */
/*			  GRAPHIC_BOX					   */
/*			  NO_WIND_BORDER				   */
/*    nBdrClr	    - Color or the window border			   */
/*    bExplodeWin   - Boolean value requesting either a pop-up or	   */
/*		      exploding window					   */
/*			  TRUE	 ==> Exploding window			   */
/*			  FALSE  ==> Pop-up window			   */
/*									   */
/*  Return Value:							   */
/*    hWnd	    - Handle of the created window or NULL if an error	   */
/*		      prevented the creation				   */
/***************************************************************************/

HWND pascal WinCreateWindow(nRow, nCol, nWidth, nHeight,
			    nWinClr, nBdrType, nBdrClr, bExplodeWin)
short	   nRow, nCol, nWidth, nHeight;
short	   nWinClr, nBdrType, nBdrClr;
short	   bExplodeWin;
{
    register PWINDATA	     pWinData;
    auto     HWND	     hWnd;

    hWnd = WinGetHandle();
    if (NULL == hWnd)
	return(hWnd);
    pWinData = (PWINDATA) malloc(sizeof(WINDATA)
			       + ScrGetRectSize(nWidth, nHeight));
    if ((PWINDATA) NULL != pWinData)
    {
	WinHandle[hWnd]    = pWinData;
	pWinData->cRow	   = (BYTE) nRow;
	pWinData->cCol	   = (BYTE) nCol;
	pWinData->cWidth   = pWinData->cWinWidth  = (BYTE) nWidth;
	pWinData->cHeight  = pWinData->cWinHeight = (BYTE) nHeight;
	pWinData->cWinClr  = (BYTE) nWinClr;
	pWinData->cBdrType = (BYTE) nBdrType;
	pWinData->cBdrClr  = (BYTE) nBdrClr;
	pWinData->cCurRow  = pWinData->cCurCol = (BYTE) 1;
	pWinData->pHidden  = NULL;
	if (NO_WIND_BORDER != nBdrType)
	{
	    pWinData->cWinWidth  -= 2;
	    pWinData->cWinHeight -= 2;
	}
	ScrSaveRect(nRow, nCol, nWidth, nHeight, pWinData->cSaveData);
	if (bExplodeWin)
	    WinExplodeWindow(nRow, nCol, nWidth, nHeight,
			     nWinClr, nBdrType, nBdrClr);
	else
	{
	    ScrClearRect(nRow, nCol, nWidth, nHeight, nWinClr);
	    ScrDrawRect(nRow, nCol, nWidth, nHeight, nBdrClr, nBdrType);
	}
	WinSetCursorPos((HWND) pWinData, 1, 1);
    }
    return(hWnd);
}


/***************************************************************************/
/*  WinDestoryWindow - Destroy the window represented by hWnd and replace  */
/*		       the previous screen contents saved when the window  */
/*		       was created.					   */
/*  Parms:								   */
/*    hWnd	    - Handle to the window to be destroyed		   */
/*									   */
/*  Return Value:     TRUE => window destroyed, FALSE => invalid handle    */
/***************************************************************************/

BOOL pascal WinDestroyWindow(hWnd)
HWND	   hWnd;
{
    register PWINDATA	    pWinData;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return(FALSE);
    ScrRestoreRect(pWinData->cRow, pWinData->cCol, pWinData->cWidth,
		   pWinData->cHeight, pWinData->cSaveData);
    if (NULL != pWinData->pHidden)
	free(pWinData->pHidden);
    free((char *) pWinData);
    WinHandle[hWnd] = NULL;
    return(TRUE);
}


/***************************************************************************/
/*  WinScrollWindowUp  - Scrolls the requested window up one line.	   */
/*									   */
/*  Parms:								   */
/*    hWnd	    - Handle to the window to be scrolled		   */
/*									   */
/*  Return Value:     None						   */
/***************************************************************************/

void pascal WinScrollWindowUp(hWnd)
HWND	     hWnd;
{
    register PWINDATA	    pWinData;
    auto     short	    nRow, nCol;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return;
    if (NULL == pWinData->pHidden)
    {
	nRow = pWinData->cRow;
	nCol = pWinData->cCol;
	if (NO_WIND_BORDER != pWinData->cBdrType)
	{
	    nRow++;
	    nCol++;
	}
	ScrScrollRectUp(nRow, nCol, pWinData->cWinWidth,
			pWinData->cWinHeight, 1, pWinData->cWinClr);
    }
    return;
}


/***************************************************************************/
/*  WinScrollWindowDown - Scrolls the requested window down one line.	   */
/*									   */
/*  Parms:								   */
/*    hWnd	    - Handle to the window to be scrolled		   */
/*									   */
/*  Return Value:     None						   */
/***************************************************************************/

void pascal WinScrollWindowDown(hWnd)
HWND	     hWnd;
{
    register PWINDATA	    pWinData;
    auto     short	    nRow, nCol;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return;
    if (NULL == pWinData->pHidden)
    {
	nRow = pWinData->cRow;
	nCol = pWinData->cCol;
	if (NO_WIND_BORDER != pWinData->cBdrType)
	{
	    nRow++;
	    nCol++;
	}
	ScrScrollRectDown(nRow, nCol, pWinData->cWinWidth,
			  pWinData->cWinHeight, 1, pWinData->cWinClr);
    }
    return;
}


/***************************************************************************/
/*  WinSetCursorPos - Position the cursor relative to the selected window. */
/*		      The upper left hand corner of the window is (1,1)    */
/*									   */
/*  Parms:								   */
/*    hWnd	    - Handle to the window to position the cusor in	   */
/*    nRow	    - Row to position cursor to within window (1 relative) */
/*    nCol	    - Col to position cursor to within window (1 relative) */
/*									   */
/*  Return Value:     None						   */
/***************************************************************************/

void pascal WinSetCursorPos(hWnd, nRow, nCol)
HWND		hWnd;
short		nRow, nCol;
{
    register PWINDATA	    pWinData;
    auto     short	    nMaxRow, nMaxCol;

    if (NULL == hWnd)
    {
	ScrSetCursorPos(nRow, nCol);
	return;
    }
    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return;
    if (nRow > pWinData->cWinHeight && nCol > pWinData->cWinWidth)
	return;
    pWinData->cCurRow = (BYTE) nRow;
    pWinData->cCurCol = (BYTE) nCol;
    nRow = nRow + pWinData->cRow - 1;
    nCol = nCol + pWinData->cCol - 1;
    if (NO_WIND_BORDER != pWinData->cBdrType)
    {
	++nRow;
	++nCol;
    }
    ScrSetCursorPos(nRow, nCol);
    return;
}


/***************************************************************************/
/*  WinClearScreen - Clear a window to the desired color.		   */
/*									   */
/*  Parms:								   */
/*    hWnd	   - Handle to the window to be cleared 		   */
/*		     (A handle of NULL clears the entire screen)	   */
/*    nColor	   - Color to be used in clearing the window		   */
/*									   */
/*  Return Value:    None						   */
/***************************************************************************/

void pascal WinClearScreen(hWnd, nColor)
HWND	    hWnd;
short	    nColor;
{
    register PWINDATA	    pWinData;
    auto     short	    nRow, nCol;

    if (NULL == hWnd)
	ScrClearRect(1, 1, 80, 25, nColor);
    else
    {
	pWinData = WinCvtHandle(hWnd);
	if (NULL == pWinData)
	    return;
	nRow	 = pWinData->cRow;
	nCol	 = pWinData->cCol;
	if (NO_WIND_BORDER != pWinData->cBdrType)
	{
	    ++nRow;
	    ++nCol;
	}
	pWinData->cWinClr = (BYTE) nColor;
	ScrClearRect(nRow, nCol, pWinData->cWinWidth, pWinData->cWinHeight,
		     pWinData->cWinClr);
    }
    return;
}


/***************************************************************************/
/*  WinTextOut	   - Display a string to the requested window at the	   */
/*		     current cursor location (for that window) using the   */
/*		     passed color attribute.				   */
/*		     If the string extends beyond the boundries of the	   */
/*		     window it will be truncated.			   */
/*  Parms:								   */
/*    hWnd	   - Handle of the window				   */
/*    pStr	   - Pointer to the NULL terminated string to display	   */
/*    nAttr	   - Color attribute to be used in displaying the string   */
/*									   */
/*  Return Value:    None						   */
/***************************************************************************/

void pascal WinTextOut(hWnd, pStr, nAttr)
HWND	    hWnd;
char	   *pStr;
short	    nAttr;
{
    register PWINDATA	pWinData;
    auto     short	nCount;
    auto     short	nRow, nCol;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return;
    ScrGetCursorPos(&nRow, &nCol);
    WinSetCursorPos(hWnd, pWinData->cCurRow, pWinData->cCurCol);
    nCount = pWinData->cWinWidth - pWinData->cCurCol + 1;
    ScrTextOut(pStr, nAttr, nCount);
    ScrSetCursorPos(nRow, nCol);
    return;
}


/***************************************************************************/
/*  WinCenterText  - Centers a text string in a window. 		   */
/*									   */
/*  Parms:								   */
/*    hWnd	   - Handle of the window				   */
/*    nRow	   - Window row to place the string on			   */
/*    pStr	   - Pointer to the string to be displayed		   */
/*    nColor	   - Color attribute used to display the string 	   */
/*									   */
/*  Return Value:    None						   */
/***************************************************************************/
void pascal WinCenterText(hWnd, nRow, pStr, nColor)
HWND	    hWnd;
short	    nRow;
char	   *pStr;
short	    nColor;
{
    if (NULL == WinCvtHandle(hWnd))
	return;
    WinSetCursorPos(hWnd, nRow, (WinGetWindowWidth(hWnd) - strlen(pStr)) / 2);
    WinTextOut(hWnd, pStr, nColor);
    return;
}


/***************************************************************************/
/*  WinMoveWindow  - Move an existing window to a new screen location.	   */
/*		     In this version the window to be moved MUST be fully  */
/*		     visible on the screen for WinMoveWindow to perform    */
/*		     properly.	If the window being moved is completely or */
/*		     partially under another window the screen will not    */
/*		     be left in the correct state (i.e. garbage on screen).*/
/*		     It is the callers responsibility to insure that the   */
/*		     window is not being moved off the screen.	Even with  */
/*		     these restriction this can be a handy routine and is  */
/*		     included for that reason.	A future release of the    */
/*		     package may fix these shortcomings.		   */
/*									   */
/*  Parms:								   */
/*    hWnd	   - Handle to the window to be moved			   */
/*    nRow	   - Move the window to this row			   */
/*    nCol	   - Move the window to this column			   */
/*									   */
/*  Return Value:    None						   */
/***************************************************************************/

void pascal WinMoveWindow(hWnd, nRow, nCol)
HWND	    hWnd;
short	    nRow, nCol;
{
    register PWINDATA	    pWinData;
    register char	   *pBuf;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return;
    if (NULL != pWinData->pHidden)
    {
	pWinData->cRow = (BYTE) nRow;
	pWinData->cCol = (BYTE) nCol;
	return;
    }
    pBuf     = malloc(ScrGetRectSize(pWinData->cWidth, pWinData->cHeight));
    if (NULL != pBuf)
    {
	ScrSaveRect(pWinData->cRow, pWinData->cCol,
		    pWinData->cWidth, pWinData->cHeight, pBuf);
	ScrRestoreRect(pWinData->cRow, pWinData->cCol,
		       pWinData->cWidth, pWinData->cHeight,
		       pWinData->cSaveData);
	ScrSaveRect(nRow, nCol, pWinData->cWidth, pWinData->cHeight,
		    pWinData->cSaveData);
	ScrRestoreRect(nRow, nCol, pWinData->cWidth, pWinData->cHeight, pBuf);
	pWinData->cRow = (BYTE) nRow;
	pWinData->cCol = (BYTE) nCol;
	free(pBuf);
    }
    return;
}


/***************************************************************************/
/*  WinGetWindowRow - Returns the row value currently associated with the  */
/*		      passed window handle.				   */
/*  Parms:								   */
/*    hWnd	   - Handle to the window				   */
/*									   */
/*  Return Value:    Row the window currently resides at		   */
/***************************************************************************/

short pascal WinGetWindowRow(hWnd)
HWND	    hWnd;
{
    register PWINDATA	   pWinData;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return(0);
    return(pWinData->cRow);
}


/***************************************************************************/
/*  WinGetWindowCol - Returns the col value currently associated with the  */
/*		      passed window handle.				   */
/*  Parms:								   */
/*    hWnd	   - Handle to the window				   */
/*									   */
/*  Return Value:    Column the window currently resides at		   */
/***************************************************************************/

short pascal WinGetWindowCol(hWnd)
HWND	    hWnd;
{
    register PWINDATA	    pWinData;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return(0);
    return(pWinData->cCol);
}


/***************************************************************************/
/*  WinGetWindowWidth - Returns the column width of the passed window.	   */
/*									   */
/*  Parms:								   */
/*    hWnd	   - Handle to the window				   */
/*									   */
/*  Return Value:    Number of columns in the window			   */
/***************************************************************************/

short pascal WinGetWindowWidth(hWnd)
HWND	    hWnd;
{
    register PWINDATA	   pWinData;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return(0);
    return(pWinData->cWinWidth);
}


/***************************************************************************/
/*  WinGetWindowHeight - Returns the number of rows in the passed window.  */
/*									   */
/*  Parms:								   */
/*    hWnd	   - Handle to the window				   */
/*									   */
/*  Return Value:    Number of rows in the window			   */
/***************************************************************************/

short pascal WinGetWindowHeight(hWnd)
HWND	    hWnd;
{
    register PWINDATA	    pWinData;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return(0);
    return(pWinData->cWinHeight);
}


/***************************************************************************/
/*  WinGetWindowClr - Get the window background color			   */
/*									   */
/*  Parms:								   */
/*    hWnd	   - Handle to the window				   */
/*									   */
/*  Return Value:    Returns the attribute for the window color 	   */
/***************************************************************************/

short pascal WinGetWindowClr(hWnd)
HWND	    hWnd;
{
    register PWINDATA	    pWinData;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return(0);
    return(pWinData->cWinClr);
}


/***************************************************************************/
/*  WinGetWindowBdrClr - Get the window border color			   */
/*									   */
/*  Parms:								   */
/*    hWnd	   - Handle to the window				   */
/*									   */
/*  Return Value:    Returns the attribute for the window border color	   */
/***************************************************************************/

short pascal WinGetWindowBdrClr(hWnd)
HWND	    hWnd;
{
    register PWINDATA	    pWinData;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return(0);
    return(pWinData->cBdrClr);
}


/***************************************************************************/
/*  WinGetBorderType - Gets the border type of the passed window	   */
/*									   */
/*  Parms:								   */
/*    hWnd	   - Handle to the window				   */
/*									   */
/*  Return Value:    Returns the window border type			   */
/***************************************************************************/

short pascal WinGetBorderType(hWnd)
HWND	    hWnd;
{
    register PWINDATA	    pWinData;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return(0);
    return(pWinData->cBdrType);
}


/***************************************************************************/
/*  WinHideWindow  - Removes a window from the screen, saving it's         */
/*		     contents.	The window can later be placed back on	   */
/*		     the screen via WinShowWindow().  Note that in this    */
/*		     release the window MUST be fully visible for this	   */
/*		     operating to work correctly.			   */
/*  Parms:								   */
/*    hWnd	   - Handle to the window				   */
/*									   */
/*  Return Value:    TRUE => window hidden, FALSE => buf alloc failed	   */
/***************************************************************************/

BOOL pascal WinHideWindow(hWnd)
HWND	    hWnd;
{
    register PWINDATA	   pWinData;
    auto     char	  *pBuf;
    auto     short	   nBufSize;
    auto     short	   nRow, nCol, nWidth, nHeight;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return(FALSE);
    nRow     = pWinData->cRow;
    nCol     = pWinData->cCol;
    nWidth   = pWinData->cWidth;
    nHeight  = pWinData->cHeight;
    nBufSize = ScrGetRectSize(nWidth, nHeight);
    if (NULL != pWinData->pHidden)
	free(pWinData->pHidden);
    pBuf     = malloc(nBufSize);
    if (NULL == pBuf)
	return(FALSE);
    ScrSaveRect(nRow, nCol, nWidth, nHeight, pBuf);
    ScrRestoreRect(nRow, nCol, nWidth, nHeight, pWinData->cSaveData);
    pWinData->pHidden = pBuf;
    return(TRUE);
}


/***************************************************************************/
/*  WinShowWindow  - Places a hidden window back on the screen and frees   */
/*		     the buffer used to hold the window image.		   */
/*  Parms:								   */
/*    hWnd	   - Handle to the window				   */
/*									   */
/*  Return Value:    TRUE => window shown, FALSE => window wasn't hidden   */
/***************************************************************************/

BOOL pascal WinShowWindow(hWnd)
HWND	     hWnd;
{
    register PWINDATA	    pWinData;

    pWinData = WinCvtHandle(hWnd);
    if (NULL == pWinData)
	return(FALSE);
    if (NULL == pWinData->pHidden)
	return(FALSE);
    ScrRestoreRect(pWinData->cRow, pWinData->cCol, pWinData->cWidth,
		   pWinData->cHeight, pWinData->pHidden);
    free(pWinData->pHidden);
    pWinData->pHidden = NULL;
    return(TRUE);
}


/***************************************************************************/
/*  WinInitialize  - Init the windowing system. 			   */
/*									   */
/*  Parms:	     None						   */
/*									   */
/*  Return Value:    None						   */
/***************************************************************************/

void pascal WinInitialize()
{
    ScrInitialize();
    memset((char *) WinHandle, NULL, sizeof(WinHandle));
    return;
}


/***************************************************************************/
/*  WinTerminate   - Clean up the windowing package			   */
/*									   */
/*  Parms:	     None						   */
/*									   */
/*  Return Value:    None						   */
/***************************************************************************/

void pascal WinTerminate()
{
    register short     i;

    for (i = 1; i <= MAX_WINDOWS; ++i)
    {
	if (WinHandle[i] != NULL)
	    WinDestroyWindow(i);
    }
    return;
}
