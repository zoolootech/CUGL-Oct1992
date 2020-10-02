
/***************************************************************************/
/* PRFRPT - Utility to generate a profile report showing the areas where   */
/*          an application program is spending its time.                   */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
/*                           Modification Log                              */
/***************************************************************************/
/* Version   Date   Programmer   -----------  Description  --------------- */
/*                                                                         */
/* V01.00   010788  Bob Withers  Program intially complete.                */
/*                                                                         */
/*                                                                         */
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
#define TRUE                    1
#define FALSE                   0
#endif

#ifndef min
#define min(a, b)               ((a) < (b) ? (a) : (b))
#endif

#ifdef __TURBOC__
#define _fmalloc(size)          farmalloc((ulong) size)
#define _ffree(ptr)             farfree(ptr)
#endif

#define STATIC                  static

#define NO_ERRORS               0
#define ERROR_READING_PRF       1
#define ERROR_READING_PR1       2
#define INVALID_PRF_FILE        3
#define OUT_OF_MEMORY           4

struct sSegInfo
{
    ushort          uNoPubs;
    ushort          uNoLins;
    LPFILEPUB       lpPubEntry;
    LPFILELIN       lpLinEntry;
    ulong           lSegHitCnt;
    FILESEG         SegRec;
};
typedef struct sSegInfo       SEGINFO, far *LPSEGINFO;

struct sPubRptLine
{
    char            cFill01[2];
    char            cPubName[15];
    char            cPubCnt[8];
    char            cFill02[2];
    char            cTotPer[4];
    char            cFill03[2];
    char            cHitPer[4];
    char            cFill04[2];
    char            cHisto[40];
    char            cEos;
};
typedef struct sPubRptLine      PUBRPTLINE;

struct sLinRptLine
{
    char            cFill01[7];
    char            cLineNo[5];
    char            cFill02[5];
    char            cLinCnt[8];
    char            cFill03[2];
    char            cTotPer[4];
    char            cFill04[2];
    char            cHitPer[4];
    char            cFill05[2];
    char            cHisto[40];
    char            cEos;
};
typedef struct sLinRptLine      LINRPTLINE;


STATIC short   near pascal BuildTables(FILE *);
STATIC short   near pascal WriteReport(FILE *);
STATIC void    near pascal PrintPubLine(PRPTREC, LPFILEPUB);
STATIC void    near pascal PrintLinLine(PRPTREC, LPFILELIN);
STATIC void    near pascal PrintLine(char *);
STATIC bool    near pascal ReadPrfRec(FILE *, PFILEREC);
STATIC void    near pascal ErrorMsg(char *);
STATIC void    near pascal CopyStr(char far *, char far *, ushort);

static ushort              uTotSegs = 0;
static ulong               lDosCnt  = 0L;
static ulong               lBiosCnt = 0L;
static ulong               lOthCnt  = 0L;
static ulong               lHitCnt  = 0L;
static ulong               lTotCnt  = 0L;
static LPSEGINFO           lpSegTab = NULL;
static LPFILEPUB           lpPubTab = NULL;
static LPFILELIN           lpLinTab = NULL;
static LPFILEMOD           lpModTab = NULL;
static char                cRevision[] = "$Revision:   1.1  $";
static char                cPrfFileName[MAX_PATHNAME];
static char                cWrk[80];
static char                cNameWrk[MAX_SEGNAME_LEN];
static char               *cErrorMsgTab[] =
                            {
                               "",
                               "Error encountered reading the .PRF file",
                               "Error encountered reading the .PR1 file",
                               "Invalid .PRF file encountered",
                               "Out of memory while building tables"
                            };


int main(argc, argv)
int       argc;
char    **argv;
{
    auto     char       *pRev;
    auto     short       nStatus;
    auto     FILE       *PrfFile, *Pr1File;

    for (pRev = cRevision; *pRev; ++pRev)
    {
        if ('$' == *pRev)
            *pRev = SPACE;
    }
    sprintf(cWrk, "Profile Reporting Module %s", cRevision);
    ErrorMsg(cWrk);
    ErrorMsg("Public Domain Software by Bob Withers, 1988");
    if (argc < 2)
    {
        ErrorMsg("\nError - command usage is:");
        ErrorMsg("        PRFRPT prffile");
        ErrorMsg("           ^      ^");
        ErrorMsg("           |      |");
        ErrorMsg("           |      +---- Name of .PRF file from MAKEPRF");
        ErrorMsg("           +------------ Utility program name");
        return(1);
    }
    if (strchr(strcpy(strupr(cPrfFileName), argv[1]), '.') == NULL)
        strcat(cPrfFileName, ".PRF");
    PrfFile = fopen(cPrfFileName, "rb");
    if (NULL == PrfFile)
    {
        sprintf(cWrk, "\nUnable to open file %s\n", cPrfFileName);
        ErrorMsg(cWrk);
        return(2);
    }

    strcpy(strchr(cPrfFileName, '.'), ".PR1");
    Pr1File = fopen(cPrfFileName, "rb");
    if (NULL == Pr1File)
    {
        fclose(PrfFile);
        sprintf(cWrk, "\nUnable to open file %s\n", cPrfFileName);
        ErrorMsg(cWrk);
        return(3);
    }

    nStatus = BuildTables(PrfFile);
    if (NO_ERRORS == nStatus)
        nStatus = WriteReport(Pr1File);
    if (NO_ERRORS != nStatus)
        ErrorMsg(cErrorMsgTab[nStatus]);
    fclose(PrfFile);
    fclose(Pr1File);
    return(0);
}


STATIC short near pascal BuildTables(PrfFile)
FILE        *PrfFile;
{
    register short          i;
    auto     ushort         uNoSegs = 0, uNoMods = 0;
    auto     ushort         uNoPubs, uNoLins;
    auto     FILEREC        PrfRec;

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
    uTotSegs = NO_SEGS;
    uNoPubs  = NO_PUBS;
    uNoLins  = NO_LINS;
    lpSegTab = (LPSEGINFO) _fmalloc(uTotSegs * sizeof(SEGINFO));
    if ((LPSEGINFO) NULL == lpSegTab)
        return(OUT_OF_MEMORY);
    lpLinTab = (LPFILELIN) _fmalloc(uNoLins * sizeof(FILELIN));
    if ((LPFILELIN) NULL == lpLinTab)
    {
        _ffree(lpSegTab);
        return(OUT_OF_MEMORY);
    }
    lpPubTab = (LPFILEPUB) _fmalloc(uNoPubs * sizeof(FILEPUB));
    if ((LPFILEPUB) NULL == lpPubTab)
    {
        _ffree(lpSegTab);
        _ffree(lpLinTab);
        return(OUT_OF_MEMORY);
    }
    lpModTab = (LPFILEMOD) _fmalloc(NO_MODS * sizeof(FILEMOD));
    if ((LPFILEMOD) NULL == lpModTab)
    {
        _ffree(lpSegTab);
        _ffree(lpLinTab);
        _ffree(lpPubTab);
        return(OUT_OF_MEMORY);
    }
    while (PRF_EOF_REC != REC_TYPE)
    {
        if (!ReadPrfRec(PrfFile, &PrfRec))
            return(ERROR_READING_PRF);
        switch (REC_TYPE)
        {
            case PRF_SEG_REC:
                lpSegTab[uNoSegs].uNoPubs    = 0;
                lpSegTab[uNoSegs].uNoLins    = 0;
                lpSegTab[uNoSegs].lpPubEntry = NULL;
                lpSegTab[uNoSegs].lpLinEntry = NULL;
                lpSegTab[uNoSegs].lSegHitCnt = 0L;
                lpSegTab[uNoSegs].SegRec     = PrfRec.Rec.Seg;
                ++uNoSegs;
                break;
            case PRF_PUB_REC:
                for (i = 0; i < uTotSegs; ++i)
                {
                    if (PUB_SEG == lpSegTab[i].SegRec.uSegVal)
                    {
                        lpPubTab[--uNoPubs]    = PrfRec.Rec.Pub;
                        lpSegTab[i].lpPubEntry = &lpPubTab[uNoPubs];
                        ++lpSegTab[i].uNoPubs;
                        break;
                    }
                }
                break;
            case PRF_LIN_REC:
                for (i = 0; i < uTotSegs; ++i)
                {
                    if (LIN_SEG == lpSegTab[i].SegRec.uSegVal)
                    {
                        lpLinTab[--uNoLins]    = PrfRec.Rec.Lin;
                        lpSegTab[i].lpLinEntry = &lpLinTab[uNoLins];
                        ++lpSegTab[i].uNoLins;
                        break;
                    }
                }
                break;
            case PRF_MOD_REC:
                lpModTab[uNoMods++] = PrfRec.Rec.Mod;
                break;
        }
    }
    return(NO_ERRORS);
}


STATIC short near pascal WriteReport(Pr1File)
FILE            *Pr1File;
{
    register short       i;
    register ushort      uCurSeg  = 0xffff;
    auto     LPSEGINFO   lpCurSeg = NULL;
    auto     RPTREC      RptRec;

    do
    {
        if (fread((char *) &RptRec, sizeof(RptRec), 1, Pr1File) != 1)
            return(ERROR_READING_PR1);
        switch (RptRec.cRecType)
        {
            case RPT_SEG_REC:
                uCurSeg = RptRec.uSegOfs;
                for (i = 0; i < uTotSegs; ++i)
                {
                    if (uCurSeg == lpSegTab[i].SegRec.uSegVal)
                    {
                        lpCurSeg = &lpSegTab[i];
                        CopyStr(cNameWrk, lpSegTab[i].SegRec.cSegNames,
                                lpSegTab[i].SegRec.cSegNameLen);
                        sprintf(cWrk,
                                "\nSegment: %s  Length: %u (%Xh)",
                                cNameWrk, lpSegTab[i].SegRec.uSegLen,
                                lpSegTab[i].SegRec.uSegLen);
                        PrintLine(cWrk);
                        PrintLine("\n   Public Symbols   Count  Tot%  Hit%\n");
                        break;
                    }
                }
                break;
            case RPT_PUB_REC:
                for (i = 0; i < lpCurSeg->uNoPubs; ++i)
                {
                    if (RptRec.uSegOfs == lpCurSeg->lpPubEntry[i].uPubOfs)
                    {
                        PrintPubLine(&RptRec, &lpCurSeg->lpPubEntry[i]);
                        break;
                    }
                }
                break;
            case RPT_LIN_REC:
                for (i = 0; i < lpCurSeg->uNoLins; ++i)
                {
                    if (RptRec.uSegOfs == lpCurSeg->lpLinEntry[i].uLinOfs)
                    {
                        PrintLinLine(&RptRec, &lpCurSeg->lpLinEntry[i]);
                        break;
                    }
                }
                break;
            case RPT_OTH_REC:
                lOthCnt  = RptRec.lCnt;
                lTotCnt += lOthCnt;
                break;
            case RPT_BIO_REC:
                lBiosCnt = RptRec.lCnt;
                lTotCnt += lBiosCnt;
                break;
            case RPT_DOS_REC:
                lDosCnt  = RptRec.lCnt;
                lTotCnt += lDosCnt;
                break;
            case RPT_HIT_REC:
                lHitCnt  = RptRec.lCnt;
                lTotCnt += lHitCnt;
                break;
        }
    } while (RPT_EOF_REC != RptRec.cRecType);
    _ffree(lpSegTab);
    _ffree(lpPubTab);
    _ffree(lpLinTab);
    _ffree(lpModTab);
    sprintf(cWrk, "\n    %8lu Hits in MS-DOS", lDosCnt);
    PrintLine(cWrk);
    sprintf(cWrk, "    %8lu Hits in ROM BIOS", lBiosCnt);
    PrintLine(cWrk);
    sprintf(cWrk, "    %8lu Hits in Application Pgm", lHitCnt);
    PrintLine(cWrk);
    sprintf(cWrk, "    %8lu Other hits", lOthCnt);
    PrintLine(cWrk);
    sprintf(cWrk, "    %8lu Total hits recorded", lTotCnt);
    PrintLine(cWrk);
    return(NO_ERRORS);
}


STATIC void near pascal PrintPubLine(pRptRec, lpPubRec)
PRPTREC       pRptRec;
LPFILEPUB     lpPubRec;
{
    auto     short          nNumStars;
    auto     double         dTotPer, dHitPer;
    auto     PUBRPTLINE     PubLine;

    memset((char *) &PubLine, SPACE, sizeof(PubLine));
    PubLine.cEos = '\0';
    CopyStr(cNameWrk, lpPubRec->cPubName, lpPubRec->cPubNameLen);
    memcpy(PubLine.cPubName, cNameWrk,
           min(lpPubRec->cPubNameLen, sizeof(PubLine.cPubName)));
    sprintf(cWrk, "%*lu", sizeof(PubLine.cPubCnt), pRptRec->lCnt);
    memcpy(PubLine.cPubCnt, cWrk, sizeof(PubLine.cPubCnt));
    dTotPer = (double) pRptRec->lCnt / (double) lTotCnt * 100.0;
    dHitPer = (double) pRptRec->lCnt / (double) lHitCnt * 100.0;
    sprintf(cWrk, "%*.1lf", sizeof(PubLine.cTotPer), dTotPer);
    memcpy(PubLine.cTotPer, cWrk, sizeof(PubLine.cTotPer));
    sprintf(cWrk, "%*.1lf", sizeof(PubLine.cHitPer), dHitPer);
    memcpy(PubLine.cHitPer, cWrk, sizeof(PubLine.cHitPer));
    nNumStars = (short) (sizeof(PubLine.cHisto) * (dHitPer / 100.0));
    if (nNumStars > 0)
        memset(PubLine.cHisto, '*', nNumStars);
    PrintLine((char *) &PubLine);
    return;
}


STATIC void near pascal PrintLinLine(pRptRec, lpLinRec)
PRPTREC       pRptRec;
LPFILELIN     lpLinRec;
{
    auto     short          nNumStars;
    auto     double         dTotPer, dHitPer;
    auto     LINRPTLINE     LinLine;
    static   ushort         uCurMod = 0xffff;

    if (uCurMod != lpLinRec->uLinModNum)
    {
        uCurMod = lpLinRec->uLinModNum;
        CopyStr(cNameWrk, lpModTab[uCurMod].cModName,
                          lpModTab[uCurMod].cModNameLen);
        sprintf(cWrk, "\n  Line number statistics for module %s\n", cNameWrk);
        PrintLine(cWrk);
        PrintLine("     Line Number    Count  Tot%  Hit%\n");
    }
    memset((char *) &LinLine, SPACE, sizeof(LinLine));
    LinLine.cEos = '\0';
    sprintf(cWrk, "%*u", sizeof(LinLine.cLineNo), lpLinRec->uLinNo);
    memcpy(LinLine.cLineNo, cWrk, sizeof(LinLine.cLineNo));
    sprintf(cWrk, "%*lu", sizeof(LinLine.cLinCnt), pRptRec->lCnt);
    memcpy(LinLine.cLinCnt, cWrk, sizeof(LinLine.cLinCnt));
    dTotPer = (double) pRptRec->lCnt / (double) lTotCnt * 100.0;
    dHitPer = (double) pRptRec->lCnt / (double) lHitCnt * 100.0;
    sprintf(cWrk, "%*.1lf", sizeof(LinLine.cTotPer), dTotPer);
    memcpy(LinLine.cTotPer, cWrk, sizeof(LinLine.cTotPer));
    sprintf(cWrk, "%*.1lf", sizeof(LinLine.cHitPer), dHitPer);
    memcpy(LinLine.cHitPer, cWrk, sizeof(LinLine.cHitPer));
    nNumStars = (short) (sizeof(LinLine.cHisto) * (dHitPer / 100.0));
    if (nNumStars > 0)
        memset(LinLine.cHisto, '*', nNumStars);
    PrintLine((char *) &LinLine);
    return;
}


STATIC void near pascal PrintLine(pLine)
register char       *pLine;
{
    register char      *pPtr;

    for (pPtr = &pLine[strlen(pLine) - 1]; pPtr > pLine; --pPtr)
    {
        if (SPACE == *pPtr)
            *pPtr = '\0';
        else
            break;
    }
    fprintf(stdout, "%s\n", pLine);
    return;
}


STATIC bool near pascal ReadPrfRec(PrfFile, PrfRec)
FILE            *PrfFile;
PFILEREC         PrfRec;
{
    if (fread((char *) PrfRec, sizeof(FILEPFX), 1, PrfFile) != 1)
        return(FALSE);
    if (fread((char *) &PrfRec->Rec,
              PrfRec->Pfx.uRecLen - sizeof(FILEPFX), 1, PrfFile) != 1)
        return(FALSE);
    return(TRUE);
}


STATIC void near pascal ErrorMsg(pStr)
char        *pStr;
{
    fprintf(stderr, "%s\n", pStr);
    return;
}


STATIC void near pascal CopyStr(lpDest, lpSors, uCnt)
char             far *lpDest, far *lpSors;
register ushort       uCnt;
{
    while (uCnt--)
        *lpDest++ = *lpSors++;
    *lpDest = '\0';
    return;
}
