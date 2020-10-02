
/***************************************************************************/
/* CPROFILE - Utility routines which are static linked to an application   */
/*	      program to allow access to the features provided by the	   */
/*	      program profile package.					   */
/*									   */
/***************************************************************************/
/*			     Modification Log				   */
/***************************************************************************/
/* Version   Date   Programmer	 -----------  Description  --------------- */
/*									   */
/* V01.00   010788  Bob Withers  Program intially complete.		   */
/*									   */
/*									   */
/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __TURBOC__
#include <alloc.h>
#else
#include <malloc.h>
#endif
#include "profile.h"

#ifndef TRUE
#define TRUE			1
#define FALSE			0
#endif

#ifdef __TURBOC__
#define _fmalloc(size)		farmalloc((ulong) size)
#define _ffree(ptr)		farfree(ptr)
#endif

#define NO_ERRORS		0
#define CANT_OPEN_PRF		1
#define ERROR_READING_PRF	2
#define INVALID_PRF_FILE	3
#define OUT_OF_MEMORY		4


struct sOfsEntry
{
    ushort	uOfs;
    ulong	lCnt;
};
typedef struct sOfsEntry       OFSENTRY, *POFSENTRY, far *LPOFSENTRY;


struct sSegEntry
{
    ushort	uSeg;
    ushort	uNoLin;
    ushort	uNoPub;
    LPOFSENTRY	lpLin;
    LPOFSENTRY	lpPub;
};
typedef struct sSegEntry       SEGENTRY, *PSEGENTRY, far *LPSEGENTRY;

extern short	   StartProfile(char *);
extern void	   EndProfile(void);
extern void	   PrfInit(ushort, LPSEGENTRY);
extern ulong	   PrfTerm(void);
extern ulong	   PrfBiosCnt(void);
extern ulong	   PrfDosCnt(void);
extern ulong	   PrfHitCnt(void);

static void    near pascal WriteRptFile(FILE *, ulong);
static short   near pascal LoadPrfTables(FILE *);
static bool    near pascal ReadPrfRec(FILE *, PFILEREC);


#ifdef __TURBOC__
extern unsigned 	       _psp;   /* MSC has defined in stdlib.h */
#endif

static ushort		       uTotSegs = 0;
static LPSEGENTRY	       lpSegTab = (LPSEGENTRY) NULL;
static LPOFSENTRY	       lpLinTab = (LPOFSENTRY) NULL;
static LPOFSENTRY	       lpPubTab = (LPOFSENTRY) NULL;
static ushort		       uBaseSeg;
static SEGENTRY 	       SegEntry;
static OFSENTRY 	       OfsEntry;
static char		       InFileName[64];



short StartProfile(pFileName)
char	    *pFileName;
{
    auto     FILE	*PrfFile;
    auto     char	*pStr;
    auto     short	 nStatus;

    pStr = strchr(strcpy(InFileName, pFileName), '.');
    if (NULL != pStr)
	*pStr = '\0';
    strcat(InFileName, ".PRF");
    PrfFile = fopen(InFileName, "rb");
    if (NULL == PrfFile)
	return(CANT_OPEN_PRF);
    uBaseSeg = _psp + 0x10;
    nStatus = LoadPrfTables(PrfFile);
    fclose(PrfFile);
    if (uTotSegs > 0)
	PrfInit(uTotSegs, lpSegTab);
    return(nStatus);
}


void EndProfile()
{
    auto     FILE     *Pr1File;
    auto     ulong     lOtherCnt;

    if ((LPSEGENTRY) NULL == lpSegTab)
	return;
    lOtherCnt = PrfTerm();
    strcpy(strchr(InFileName, '.'), ".PR1");
    Pr1File = fopen(InFileName, "wb");
    if (NULL != Pr1File)
    {
	WriteRptFile(Pr1File, lOtherCnt);
	fclose(Pr1File);
    }
    _ffree(lpSegTab);
    _ffree(lpLinTab);
    _ffree(lpPubTab);
    return;
}


static void near pascal WriteRptFile(Pr1File, lOtherCnt)
FILE	    *Pr1File;
ulong	     lOtherCnt;
{
    register short	   i, j;
    auto     LPOFSENTRY    lpOfsEntry;
    auto     RPTREC	   RptRec;
    auto     ulong	   lTotalTicks = 0L;

    RptRec.cRecType = RPT_OTH_REC;
    RptRec.uSegOfs  = 0;
    RptRec.lCnt     = lOtherCnt;
    if (fwrite((char *) &RptRec, sizeof(RptRec), 1, Pr1File) != 1)
	return;
    RptRec.cRecType = RPT_BIO_REC;
    RptRec.uSegOfs  = 0;
    RptRec.lCnt     = PrfBiosCnt();
    if (fwrite((char *) &RptRec, sizeof(RptRec), 1, Pr1File) != 1)
	return;
    RptRec.cRecType = RPT_DOS_REC;
    RptRec.uSegOfs  = 0;
    RptRec.lCnt     = PrfDosCnt();
    if (fwrite((char *) &RptRec, sizeof(RptRec), 1, Pr1File) != 1)
	return;
    RptRec.cRecType = RPT_HIT_REC;
    RptRec.uSegOfs  = 0;
    RptRec.lCnt     = PrfHitCnt();
    if (fwrite((char *) &RptRec, sizeof(RptRec), 1, Pr1File) != 1)
	return;
    for (i = uTotSegs - 1; i >= 0; --i)
    {
	if (0 == lpSegTab[i].uNoPub && 0 == lpSegTab[i].uNoLin)
	    continue;
	RptRec.cRecType = RPT_SEG_REC;
	RptRec.uSegOfs	= lpSegTab[i].uSeg - uBaseSeg;
	RptRec.lCnt	= 0L;
	if (fwrite((char *) &RptRec, sizeof(RptRec), 1, Pr1File) != 1)
	    return;
	RptRec.cRecType = RPT_PUB_REC;
	lpOfsEntry	= lpSegTab[i].lpPub + (lpSegTab[i].uNoPub - 1);
	for (j = lpSegTab[i].uNoPub; j > 0; --j)
	{
	    RptRec.uSegOfs = lpOfsEntry->uOfs;
	    RptRec.lCnt    = lpOfsEntry->lCnt;
	    if (RptRec.lCnt > 0L)
	    {
		if (fwrite((char *) &RptRec, sizeof(RptRec),
				    1, Pr1File) != 1)
		    return;
	    }
	    --lpOfsEntry;
	}
	RptRec.cRecType = RPT_LIN_REC;
	lpOfsEntry	= lpSegTab[i].lpLin + (lpSegTab[i].uNoLin - 1);
	for (j = lpSegTab[i].uNoLin; j > 0; --j)
	{
	    RptRec.uSegOfs = lpOfsEntry->uOfs;
	    RptRec.lCnt    = lpOfsEntry->lCnt;
	    if (RptRec.lCnt > 0L)
	    {
		if (fwrite((char *) &RptRec, sizeof(RptRec),
				    1, Pr1File) != 1)
		    return;
	    }
	    --lpOfsEntry;
	    lTotalTicks += RptRec.lCnt;
	}
    }
    memset((char *) &RptRec, 0, sizeof(RptRec));
    RptRec.cRecType = RPT_EOF_REC;
    RptRec.lCnt     = lTotalTicks;
    fwrite((char *) &RptRec, sizeof(RptRec), 1, Pr1File);
    return;
}


static short near pascal LoadPrfTables(PrfFile)
FILE	    *PrfFile;
{
    register short	    i;
    auto     ushort	    uNoSegs, uNoLins, uNoPubs;
    auto     FILEREC	    PrfRec;

    if (!ReadPrfRec(PrfFile, &PrfRec))
	return(ERROR_READING_PRF);
    if (PRF_ID_REC == REC_TYPE && PRF_FILE_VER == FILE_VER)
    {
	if (memcmp(FILE_ID, PRF_FILE_ID, sizeof(FILE_ID)) != 0)
	    return(INVALID_PRF_FILE);
    }
    else
	return(INVALID_PRF_FILE);

    if (!ReadPrfRec(PrfFile, &PrfRec))
	return(ERROR_READING_PRF);
    if (PRF_HDR_REC != REC_TYPE)
	return(INVALID_PRF_FILE);
    uNoSegs = uTotSegs = NO_SEGS;
    uNoLins = NO_LINS;
    uNoPubs = NO_PUBS;
    lpSegTab = (LPSEGENTRY) _fmalloc(uNoSegs * sizeof(SEGENTRY));
    if ((LPSEGENTRY) NULL == lpSegTab)
	return(OUT_OF_MEMORY);
    lpLinTab = (LPOFSENTRY) _fmalloc(uNoLins * sizeof(OFSENTRY));
    if ((LPOFSENTRY) NULL == lpLinTab)
    {
	_ffree(lpSegTab);
	lpSegTab = (LPSEGENTRY) NULL;
	return(OUT_OF_MEMORY);
    }
    lpPubTab = (LPOFSENTRY) _fmalloc(uNoPubs * sizeof(OFSENTRY));
    if ((LPOFSENTRY) NULL == lpPubTab)
    {
	_ffree(lpSegTab);
	_ffree(lpLinTab);
	lpSegTab = (LPSEGENTRY) NULL;
	return(OUT_OF_MEMORY);
    }
    while (PRF_EOF_REC != REC_TYPE)
    {
	if (!ReadPrfRec(PrfFile, &PrfRec))
	    return(ERROR_READING_PRF);
	switch (REC_TYPE)
	{
	    case PRF_SEG_REC:
		memset((char *) &SegEntry, 0, sizeof(SegEntry));
		SegEntry.uSeg	    = SEG_VAL + uBaseSeg;
		lpSegTab[--uNoSegs] = SegEntry;
		break;
	    case PRF_PUB_REC:
		PUB_SEG += uBaseSeg;
		for (i = 0; TRUE; ++i)
		{
		    if (PUB_SEG == lpSegTab[i].uSeg)
			break;
		}
		OfsEntry.uOfs = PUB_OFS;
		OfsEntry.lCnt = 0L;
		lpPubTab[--uNoPubs] = OfsEntry;
		lpSegTab[i].uNoPub++;
		lpSegTab[i].lpPub = &lpPubTab[uNoPubs];
		break;
	    case PRF_LIN_REC:
		LIN_SEG += uBaseSeg;
		for (i = 0; TRUE; ++i)
		{
		    if (LIN_SEG == lpSegTab[i].uSeg)
			break;
		}
		OfsEntry.uOfs = LIN_OFS;
		OfsEntry.lCnt = 0L;
		lpLinTab[--uNoLins] = OfsEntry;
		lpSegTab[i].uNoLin++;
		lpSegTab[i].lpLin = &lpLinTab[uNoLins];
		break;
	}
    }
    return(NO_ERRORS);
}


static bool near pascal ReadPrfRec(PrfFile, PrfRec)
FILE		*PrfFile;
PFILEREC	 PrfRec;
{
    if (fread((char *) PrfRec, sizeof(FILEPFX), 1, PrfFile) != 1)
	return(FALSE);
    if (fread((char *) &PrfRec->Rec,
	      PrfRec->Pfx.uRecLen - sizeof(FILEPFX), 1, PrfFile) != 1)
	return(FALSE);
    return(TRUE);
}
