
/***************************************************************************/
/* SCRIO    - Routines which directly access the video screen		   */
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
#include <dos.h>
#include <string.h>
#include "win.h"

#define MAXDIM(array)		   (sizeof(array) / sizeof(array[0]))

#define SCR_BYTES_PER_ROW	   160
#define CGA_MODE_SEL		   0x3d8
#define CGA_ENABLE		   0x29
#define CGA_DISABLE		   0x21

#define BIOS_VID_INT		   0x10
#define BIOS_VID_SET_CRTMODE	   0
#define BIOS_VID_SET_CURSORTYPE    1
#define BIOS_VID_SET_CURSORPOS	   2
#define BIOS_VID_GET_CURSORPOS	   3
#define BIOS_VID_SCROLL_UP	   6
#define BIOS_VID_SCROLL_DOWN	   7
#define BIOS_VID_WRITE_CHATTR	   9
#define BIOS_VID_GET_CRTMODE	   15


struct sBoxType
{
    BYTE		cUpperLeft;
    BYTE		cLowerLeft;
    BYTE		cUpperRight;
    BYTE		cLowerRight;
    BYTE		cLeft;
    BYTE		cRight;
    BYTE		cTop;
    BYTE		cBottom;
};
typedef struct sBoxType BOXTYPE;


unsigned		uScrSeg 	= 0xb800;
unsigned		uCsrType	= 0x0107;
short			nCurrActivePage = 0;
short			nVideoCard	= VIDEO_CGA;
short			nScrCols	= 80;


/***************************************************************************/
/*  ScrCvtAttr	  - Test for a monochrome video card and convert the	   */
/*		    requested attribute to black & white as best we can    */
/*		    and still honor the callers request.		   */
/*  Parms:								   */
/*    nAttr	  - The passed color attribute. 			   */
/*									   */
/*  Return Value:   A converted black & white attribute if the current	   */
/*		    video mode is monochrome.				   */
/***************************************************************************/

static short near pascal ScrCvtAttr(nAttr)
register short	  nAttr;
{
    short     nRev, nClr, nBlink, nIntensity;

    if (VIDEO_MONO != nVideoCard)
	return(nAttr);
    nIntensity = nAttr & 0x40;
    nBlink     = nAttr & 0x80;
    nRev       = nAttr & 0x70;
    nClr       = nAttr & 0x07;
    if (REV_BLACK == nRev)
	nClr = WHITE;
    else
    {
	nRev = REV_WHITE;
	nClr = BLACK;
    }
    return(nRev | nClr | nBlink | nIntensity);
}


/***************************************************************************/
/*  ScrEnableVideoCGA - Test the current video equipment for a snowy CGA   */
/*			card.  If running on a CGA enable/disable the	   */
/*			video signal based on the passed parameter which   */
/*			MUST be one of the predefined constants CGA_ENABLE */
/*			or CGA_DISABLE.  If the current video equipment is */
/*			not a CGA, the routine returns without taking any  */
/*			action. 					   */
/*  Parms:								   */
/*    nStatus	      - Enable or disable the CGA video signal		   */
/*			    CGA_ENABLE or CGA_DISABLE			   */
/*									   */
/*  Return Value:	None						   */
/***************************************************************************/

static void pascal ScrEnableVideoCGA(nStatus)
short	    nStatus;
{
    if (VIDEO_CGA == nVideoCard)
	outp(CGA_MODE_SEL, nStatus);
    return;
}


/***************************************************************************/
/*  ScrGetRectSize - This routine will calculate and return the number of  */
/*		     bytes required to store a screen image which is nWidth*/
/*		     columns by nHeight rows.				   */
/*  Parms:								   */
/*    nWidth	   - Column width of the screen rectangle		   */
/*    nHeight	   - Number of rows in the screen rectangle		   */
/*									   */
/*  Return Value:    Size in bytes required to store the screen rectangle  */
/***************************************************************************/

short pascal ScrGetRectSize(nWidth, nHeight)
short	    nWidth, nHeight;
{
    return(nWidth * nHeight * 2);
}


/***************************************************************************/
/*  ScrClearRect - This routine will clear a screen rectangle to the	   */
/*		   color attribute passed.				   */
/*  Parms:								   */
/*    nRow	 - Row of the screen rectangle				   */
/*    nCol	 - Column of the screen rectangle			   */
/*    nWidth	 - Width in columns of the screen rectangle		   */
/*    nHeight	 - Number of rows in the screen rectangle		   */
/*    nAttr	 - Color attribute used to clear screen rectangle	   */
/*									   */
/*  Return Value:  None 						   */
/***************************************************************************/

void pascal ScrClearRect(nRow, nCol, nWidth, nHeight, nAttr)
short			 nRow, nCol, nWidth, nHeight, nAttr;
{
    auto     union REGS     r;

    nAttr  = ScrCvtAttr(nAttr);
    r.h.ah = (BYTE) BIOS_VID_SCROLL_UP;
    r.h.al = 0;
    r.h.bh = (BYTE) nAttr;
    r.h.ch = (BYTE) (nRow - 1);
    r.h.cl = (BYTE) (nCol - 1);
    r.h.dh = (BYTE) (nRow + nHeight - 2);
    r.h.dl = (BYTE) (nCol + nWidth - 2);
    int86(BIOS_VID_INT, &r, &r);
    return;
}


/***************************************************************************/
/*  ScrSaveRect - This routine will save a screen rectangle in a caller    */
/*		  supplied buffer area.  nRow, nCol define the row and	   */
/*		  column of the upper left corner of the rectangle.	   */
/*  Parms:								   */
/*    nRow	- Row of the screen rectangle				   */
/*    nCol	- Column of the screen rectangle			   */
/*    nWidth	- Width in columns of the screen rectangle		   */
/*    nHeight	- Number of rows in the screen rectangle		   */
/*    pBuf	- Buffer used to store the saved screen rectangle	   */
/*									   */
/*  Return Value:  None 						   */
/***************************************************************************/

void pascal ScrSaveRect(nRow, nCol, nWidth, nHeight, pBuf)
short	   nRow, nCol, nWidth, nHeight;
char	  *pBuf;
{
    register unsigned	    uNumRows;
    register unsigned	    uColLen;
    auto     unsigned	    uScrOfs;
    auto     unsigned	    uBufSeg, uBufOfs;
    auto     char far	   *fpBuf;

    uColLen = nWidth * 2;
    uScrOfs = ((nRow - 1) * SCR_BYTES_PER_ROW) + (nCol - 1) * 2;
    fpBuf   = (char far *) pBuf;
    uBufSeg = FP_SEG(fpBuf);
    uBufOfs = FP_OFF(fpBuf);
    ScrEnableVideoCGA(CGA_DISABLE);
    for (uNumRows = nHeight; uNumRows > 0; --uNumRows)
    {
	movedata(uScrSeg, uScrOfs, uBufSeg, uBufOfs, uColLen);
	uScrOfs += SCR_BYTES_PER_ROW;
	uBufOfs += uColLen;
    }
    ScrEnableVideoCGA(CGA_ENABLE);
    return;
}


/***************************************************************************/
/*  ScrRestoreRect - This routine will restore a screen rectangle from	   */
/*		     a previously saved caller buffer.	nRow and nCol	   */
/*		     define the upper left corner of the rectangle on	   */
/*		     the screen and are not required to be the same	   */
/*		     coordinates used in the save call.  nWidth and	   */
/*		     nHeight should remain unchanged from the save call    */
/*		     but are not required to do so.			   */
/*  Parms:								   */
/*    nRow	- Row of the screen rectangle				   */
/*    nCol	- Column of the screen rectangle			   */
/*    nWidth	- Width in columns of the screen rectangle		   */
/*    nHeight	- Number of rows in the screen rectangle		   */
/*    pBuf	- Buffer used to restore the saved screen rectangle	   */
/*									   */
/*  Return Value:  None 						   */
/***************************************************************************/

void pascal ScrRestoreRect(nRow, nCol, nWidth, nHeight, pBuf)
short	   nRow, nCol, nWidth, nHeight;
char	  *pBuf;
{
    register unsigned	    uNumRows;
    register unsigned	    uColLen;
    auto     unsigned	    uScrOfs;
    auto     unsigned	    uBufSeg, uBufOfs;
    auto     char far	   *fpBuf;

    uColLen = nWidth * 2;
    uScrOfs = ((nRow - 1) * SCR_BYTES_PER_ROW) + (nCol - 1) * 2;
    fpBuf   = (char far *) pBuf;
    uBufSeg = FP_SEG(fpBuf);
    uBufOfs = FP_OFF(fpBuf);
    ScrEnableVideoCGA(CGA_DISABLE);
    for (uNumRows = nHeight; uNumRows > 0; --uNumRows)
    {
	movedata(uBufSeg, uBufOfs, uScrSeg, uScrOfs, uColLen);
	uScrOfs += SCR_BYTES_PER_ROW;
	uBufOfs += uColLen;
    }
    ScrEnableVideoCGA(CGA_ENABLE);
    return;
}


/***************************************************************************/
/*  ScrScrollRectUp   - Scrolls a screen rectangle up the requested number */
/*			of lines.					   */
/*  Parms:								   */
/*    nRow	- Row of the screen rectangle				   */
/*    nCol	- Column of the screen rectangle			   */
/*    nWidth	- Width in columns of the screen rectangle		   */
/*    nHeight	- Number of rows in the screen rectangle		   */
/*    nNoRows	- Number of rows to scroll				   */
/*    nAttr	- Color attribute to fill blank line on bottom		   */
/*									   */
/*  Return Value:  None 						   */
/***************************************************************************/

void pascal ScrScrollRectUp(nRow, nCol, nWidth, nHeight, nNoRows, nAttr)
short	 nRow, nCol, nWidth, nHeight, nNoRows, nAttr;
{
    auto     union REGS      r;

    nAttr  = ScrCvtAttr(nAttr);
    r.h.ah = BIOS_VID_SCROLL_UP;
    r.h.al = (BYTE) nNoRows;
    r.h.bh = (BYTE) nAttr;
    r.h.ch = (BYTE) (nRow - 1);
    r.h.cl = (BYTE) (nCol - 1);
    r.h.dh = (BYTE) (nRow + nHeight - 2);
    r.h.dl = (BYTE) (nCol + nWidth - 2);
    int86(BIOS_VID_INT, &r, &r);
    return;
}


/***************************************************************************/
/*  ScrScrollRectDown - Scrolls a screen rectangle up the requested number */
/*			of lines.					   */
/*  Parms:								   */
/*    nRow	- Row of the screen rectangle				   */
/*    nCol	- Column of the screen rectangle			   */
/*    nWidth	- Width in columns of the screen rectangle		   */
/*    nHeight	- Number of rows in the screen rectangle		   */
/*    nNoRows	- Number of rows to scroll				   */
/*    nAttr	- Color attribute to fill blank lines on top		   */
/*									   */
/*  Return Value:  None 						   */
/***************************************************************************/

void pascal ScrScrollRectDown(nRow, nCol, nWidth, nHeight, nNoRows, nAttr)
short	 nRow, nCol, nWidth, nHeight, nNoRows, nAttr;
{
    auto     union REGS      r;

    nAttr  = ScrCvtAttr(nAttr);
    r.h.ah = BIOS_VID_SCROLL_DOWN;
    r.h.al = (BYTE) nNoRows;
    r.h.bh = (BYTE) nAttr;
    r.h.ch = (BYTE) (nRow - 1);
    r.h.cl = (BYTE) (nCol - 1);
    r.h.dh = (BYTE) (nRow + nHeight - 2);
    r.h.dl = (BYTE) (nCol + nWidth - 2);
    int86(BIOS_VID_INT, &r, &r);
    return;
}


/***************************************************************************/
/*  ScrSetCursorPos - This routine will position the cursor to an absolute */
/*		      screen coordinate using the BIOS video services.	   */
/*  Parms:								   */
/*    nRow	    - Absolute screen row				   */
/*    nCol	    - Absolute screen column				   */
/*									   */
/*  Return Value      None						   */
/***************************************************************************/

void pascal ScrSetCursorPos(nRow, nCol)
short	    nRow, nCol;
{
    auto     union REGS     r;

    r.h.ah = BIOS_VID_SET_CURSORPOS;
    r.h.dh = (BYTE) (nRow - 1);
    r.h.dl = (BYTE) (nCol - 1);
    r.h.bh = (BYTE) nCurrActivePage;
    int86(BIOS_VID_INT, &r, &r);
    return;
}


/***************************************************************************/
/*  ScrGetCursorPos - This routine will return the current absolute	   */
/*		      cursor position.					   */
/*  Parms:								   */
/*    nRow	    - Pointer to location to save current row		   */
/*    nCol	    - Pointer to location to save current column	   */
/*									   */
/*  Return Value:     None						   */
/***************************************************************************/

void pascal ScrGetCursorPos(nRow, nCol)
short	    *nRow, *nCol;
{
    auto     union REGS     r;

    r.h.ah = BIOS_VID_GET_CURSORPOS;
    r.h.bh = (BYTE) nCurrActivePage;
    int86(BIOS_VID_INT, &r, &r);
    *nRow  = r.h.dh + 1;
    *nCol  = r.h.dl + 1;
    return;
}


/***************************************************************************/
/*  ScrCusrosOn     - Enables the screen cursor.			   */
/*									   */
/*  Parms:	      None						   */
/*									   */
/*  Return Value:     None						   */
/***************************************************************************/

void pascal ScrCursorOn()
{
    auto     union REGS     r;

    r.h.ah = BIOS_VID_SET_CURSORTYPE;
    r.x.cx = uCsrType;
    int86(BIOS_VID_INT, &r, &r);
    return;
}


/***************************************************************************/
/*  ScrCusrosOff    - Disables the screen cursor.			   */
/*									   */
/*  Parms:	      None						   */
/*									   */
/*  Return Value:     None						   */
/***************************************************************************/

void pascal ScrCursorOff()
{
    auto     union REGS     r;

    r.h.ah = BIOS_VID_SET_CURSORTYPE;
    r.x.cx = 0x0f00;
    int86(BIOS_VID_INT, &r, &r);
    return;
}


/***************************************************************************/
/*  ScrTextOut - This function uses the BIOS write character and attribute */
/*		 service routine to display a string within a window.  The */
/*		 passed nCount is used to limit a string from overflowing  */
/*		 a window boundry.					   */
/*  Parms:								   */
/*    pStr     - Pointer to the string to be displayed			   */
/*    nAttr    - Color attribute used to display string 		   */
/*    nCount   - Maximum number of characters to dispalay		   */
/*									   */
/*  Return Value: None							   */
/***************************************************************************/

void pascal ScrTextOut(pStr, nAttr, nCount)
register char	*pStr;
short		 nAttr, nCount;
{
    register short	    i;
    auto     short	    nRow, nCol;
    auto     union REGS     r, r1;

    ScrGetCursorPos(&nRow, &nCol);
    nAttr  = ScrCvtAttr(nAttr);
    r.h.ah = BIOS_VID_WRITE_CHATTR;
    r.h.bh = (BYTE) nCurrActivePage;
    r.h.bl = (BYTE) nAttr;
    r.x.cx = 1;
    while (*pStr && nCount-- > 0)
    {
	ScrSetCursorPos(nRow, nCol++);
	r.h.al = *pStr++;
	int86(BIOS_VID_INT, &r, &r1);
    }
    return;
}


/***************************************************************************/
/*  ScrDrawRect - This routine is used to draw borders around a screen	   */
/*		  window.  The passed parameters define the rectangle	   */
/*		  being used by the window as well as the border color	   */
/*		  and type.						   */
/*  Parms:								   */
/*    nRow	- Top row of screen border				   */
/*    nCol	- Left column of screen border				   */
/*    nWidth	- Column width of the window				   */
/*    nHeight	- Number of rows in the window				   */
/*    nColor	- Color attribute for the window border 		   */
/*    nType	- Type of border to be displayed			   */
/*									   */
/*  Return Value: None							   */
/***************************************************************************/

void pascal ScrDrawRect(nRow, nCol, nWidth, nHeight, nColor, nType)
short	    nRow, nCol, nWidth, nHeight, nColor, nType;
{
    register short	   i;
    auto     union REGS    r, r1;
    static   BOXTYPE	   BoxTypes[] =
    {
	{  32,	32,  32,  32,  32,  32,  32,  32 }, /* NO_BOX		   */
	{ 213, 212, 184, 190, 179, 179, 205, 205 }, /* DBL_LINE_TOP_BOTTOM */
	{ 214, 211, 183, 189, 186, 186, 196, 196 }, /* DBL_LINE_SIDES	   */
	{ 201, 200, 187, 188, 186, 186, 205, 205 }, /* DBL_LINE_ALL_SIDES  */
	{ 218, 192, 191, 217, 179, 179, 196, 196 }, /* SNGL_LINE_ALL_SIDES */
	{ 219, 219, 219, 219, 219, 219, 223, 220 }  /* GRAPHIC BOX     */
    };

    if (nType < 0 || nType >= MAXDIM(BoxTypes))
	return;
    if (nWidth < 2 || nHeight < 2)
	return;
    nColor = ScrCvtAttr(nColor);

    /* Draw upper left corner */
    ScrSetCursorPos(nRow, nCol);
    r.h.ah = (BYTE) BIOS_VID_WRITE_CHATTR;
    r.h.al = (BYTE) BoxTypes[nType].cUpperLeft;
    r.h.bh = (BYTE) nCurrActivePage;
    r.h.bl = (BYTE) nColor;
    r.x.cx = 1;
    int86(BIOS_VID_INT, &r, &r1);

    /* Draw upper right corner */
    ScrSetCursorPos(nRow, nCol + nWidth - 1);
    r.h.al = (BYTE) BoxTypes[nType].cUpperRight;
    int86(BIOS_VID_INT, &r, &r1);

    /* Draw lower left corner */
    ScrSetCursorPos(nRow + nHeight - 1, nCol);
    r.h.al = (BYTE) BoxTypes[nType].cLowerLeft;
    int86(BIOS_VID_INT, &r, &r1);

    /* Draw lower right corner */
    ScrSetCursorPos(nRow + nHeight - 1, nCol + nWidth - 1);
    r.h.al = (BYTE) BoxTypes[nType].cLowerRight;
    int86(BIOS_VID_INT, &r, &r1);

    if (nHeight > 2)
    {
	/* Draw left side line */
	r.h.al = (BYTE) BoxTypes[nType].cLeft;
	for (i = 1; i <= nHeight - 2; ++i)
	{
	    ScrSetCursorPos(nRow + i, nCol);
	    int86(BIOS_VID_INT, &r, &r1);
	}

	/* Draw right side line */
	r.h.al = (BYTE) BoxTypes[nType].cRight;
	for (i = 1; i <= nHeight - 2; ++i)
	{
	    ScrSetCursorPos(nRow + i, nCol + nWidth - 1);
	    int86(BIOS_VID_INT, &r, &r1);
	}
    }

    if (nWidth > 2)
    {
	/* Draw top line */
	ScrSetCursorPos(nRow, nCol + 1);
	r.h.al = (BYTE) BoxTypes[nType].cTop;
	r.x.cx = nWidth - 2;
	int86(BIOS_VID_INT, &r, &r1);

	/* Draw bottom line */
	ScrSetCursorPos(nRow + nHeight - 1, nCol + 1);
	r.h.al = BoxTypes[nType].cBottom;
	int86(BIOS_VID_INT, &r, &r1);
    }

    return;
}


/***************************************************************************/
/*  ScrInitialize - Determine type of video card and init global data.	   */
/*									   */
/*  Parms:	    None						   */
/*									   */
/*  Return Value:   None						   */
/***************************************************************************/

void pascal ScrInitialize()
{
    auto     union REGS     r;

    r.h.ah = BIOS_VID_GET_CURSORPOS;
    r.h.bh = (BYTE) nCurrActivePage;
    int86(BIOS_VID_INT, &r, &r);
    uCsrType = r.x.cx;

    r.h.ah = BIOS_VID_GET_CRTMODE;
    int86(BIOS_VID_INT, &r, &r);
    nScrCols	    = r.h.ah;
    nCurrActivePage = r.h.bh;
	if (7 == r.h.al)
    {
	uScrSeg    = 0xb000;
	nVideoCard = VIDEO_MONO;
	return;
    }
    r.h.ah = BIOS_VID_SET_CRTMODE;
    r.h.al = 3;
	int86(BIOS_VID_INT, &r, &r);
    uScrSeg    = 0xb800;
    nVideoCard = VIDEO_CGA;
    return;
}
