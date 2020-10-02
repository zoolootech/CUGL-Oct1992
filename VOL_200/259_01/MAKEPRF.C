
/***************************************************************************/
/* MAKEPRF - Utility used to generate a .PRF symbol file for use in the    */
/*	     program profile utility package.				   */
/*									   */
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
#include <ctype.h>
#include "profile.h"

#ifndef TRUE
#define TRUE			1
#define FALSE			0
#endif

#define STATIC			static


STATIC void    near pascal ProcessMapFile(FILE *, FILE *);
STATIC void    near pascal WritePrfID(FILE *);
STATIC long    near pascal WritePrfHdr(FILE *);
STATIC short   near pascal WriteSegRec(FILE *, char *);
STATIC short   near pascal WritePubRec(FILE *, char *);
STATIC short   near pascal WriteLinRec(FILE *, char *);
STATIC short   near pascal WriteModRec(FILE *, char *);
STATIC ulong   near pascal GetHexVal(char *);
STATIC ushort  near pascal GetDecVal(char *);
STATIC void    near pascal BuildMapFileName(char *, char *);
STATIC void    near pascal BuildPrfFileName(char *, char *);
STATIC void    near pascal WriteError(FILE *);
STATIC void    near pascal ErrorMsg(char *);

STATIC short		   nCurModNum = -1;
STATIC char		   cRevision[] = "$Revision:   1.0  $";
STATIC char		   cMapFileName[MAX_PATHNAME];
STATIC char		   cPrfFileName[16];
STATIC FILEREC		   PrfRec;
STATIC char		   cWrk[512];
STATIC ushort		   uMaxSegs = 0;
STATIC ushort		   uSegs[MAX_SEGMENTS];


int main(argc, argv)
int	  argc;
char	**argv;
{
    auto     char	*pRev;
    auto     FILE	*MapFile, *PrfFile;

    for (pRev = cRevision; *pRev; ++pRev)
    {
	if ('$' == *pRev)
	    *pRev = SPACE;
    }
    sprintf(cWrk, "Profile Symbol File Generator %s", cRevision);
    ErrorMsg(cWrk);
    ErrorMsg("Public Domain Software by Bob Withers, 1988");
    if (argc < 2)
    {
	ErrorMsg("\nError - command usage is:");
	ErrorMsg("        MAKEPRF mapfile");
	ErrorMsg("           ^       ^");
	ErrorMsg("           |       |");
	ErrorMsg("           |       +---- Name of .MAP file from linker");
	ErrorMsg("           +------------ Utility program name");
	return(1);
    }
    BuildMapFileName(argv[1], cMapFileName);
    BuildPrfFileName(cMapFileName, cPrfFileName);
    MapFile = fopen(cMapFileName, "rt");
    if (NULL == MapFile)
    {
	sprintf(cWrk, "\nUnable to open input file %s", cMapFileName);
	ErrorMsg(cWrk);
	return(2);
    }
    PrfFile = fopen(cPrfFileName, "wb");
    if (NULL == PrfFile)
    {
	sprintf(cWrk, "\nUnable to open output file %s", cPrfFileName);
	ErrorMsg(cWrk);
	return(3);
    }
    ProcessMapFile(MapFile, PrfFile);
    fclose(PrfFile);
    fclose(MapFile);
    return(0);
}


STATIC void near pascal ProcessMapFile(MapFile, PrfFile)
FILE	    *MapFile, *PrfFile;
{
    register char    *pPtr;
    register char     cStatus  = SPACE;
    auto     short    nSegRecs = 0, nModRecs = 0, nPubRecs = 0, nLinRecs = 0;
    auto     long     lHdrPos;

    WritePrfID(PrfFile);
    lHdrPos = WritePrfHdr(PrfFile);
    cStatus = 'S';
    while (TRUE)
    {
	pPtr = fgets(cWrk, sizeof(cWrk), MapFile);
	if (NULL == pPtr)
	    break;
	if ('\n' == cWrk[0])
	    continue;
	if (strstr(cWrk, "Publics by Value") != NULL)
	    cStatus = 'P';
	if (strstr(cWrk, "Publics by Name") != NULL)
	    cStatus = SPACE;
	if (strstr(cWrk, "Origin ") != NULL)
	    cStatus = SPACE;
	if (strstr(cWrk, "Detailed map") != NULL)
	    cStatus = SPACE;
	if (strstr(cWrk, "Line numbers") != NULL)
	{
	    nModRecs += WriteModRec(PrfFile, cWrk);
	    cStatus = 'L';
	    continue;
	}
	switch (cStatus)
	{
	    case 'S':
		nSegRecs += WriteSegRec(PrfFile, cWrk);
		break;
	    case 'P':
		nPubRecs += WritePubRec(PrfFile, cWrk);
		break;
	    case 'L':
		nLinRecs += WriteLinRec(PrfFile, cWrk);
		break;
	}
    }

    REC_TYPE = PRF_EOF_REC;
    REC_LEN  = sizeof(FILEPFX) + 1;
    if (fwrite((char *) &PrfRec, 1, REC_LEN, PrfFile) != REC_LEN)
	WriteError(PrfFile);

    if (fseek(PrfFile, lHdrPos, SEEK_SET) != 0)
	WriteError(PrfFile);
    NO_SEGS = nSegRecs;
    NO_PUBS = nPubRecs;
    NO_LINS = nLinRecs;
    NO_MODS = nModRecs;
    WritePrfHdr(PrfFile);
    sprintf(cWrk,
      "\nProcessed:\n%5d Segment(s)\n%5d Public Symbol(s)",
      nSegRecs, nPubRecs);
    ErrorMsg(cWrk);
    sprintf(cWrk, "%5d Module(s)\n%5d Line Number(s)", nModRecs, nLinRecs);
    ErrorMsg(cWrk);
    return;
}


STATIC void near pascal WritePrfID(PrfFile)
FILE	    *PrfFile;
{
    memset((char *) &PrfRec, SPACE, sizeof(PrfRec));
    REC_TYPE = PRF_ID_REC;
    FILE_VER = PRF_FILE_VER;
    memcpy(FILE_ID, PRF_FILE_ID, sizeof(FILE_ID));
    REC_LEN  = sizeof(FILEID) + sizeof(FILEPFX);
    if (fwrite((char *) &PrfRec, 1, REC_LEN, PrfFile) != REC_LEN)
	WriteError(PrfFile);
    return;
}


STATIC long near pascal WritePrfHdr(PrfFile)
FILE	    *PrfFile;
{
    auto     long      lHdrPos;

    lHdrPos  = ftell(PrfFile);
    REC_TYPE = PRF_HDR_REC;
    REC_LEN  = sizeof(FILEHDR) + sizeof(FILEPFX);
    if (fwrite((char *) &PrfRec, 1, REC_LEN, PrfFile) != REC_LEN)
	WriteError(PrfFile);
    return(lHdrPos);
}


STATIC short near pascal WriteSegRec(PrfFile, pMapRec)
FILE		 *PrfFile;
register char	 *pMapRec;
{
    /*	is this really a segment record?  */
    while (SPACE == *pMapRec)
	++pMapRec;
    if (*pMapRec < '0' || *pMapRec > '9')
	return(0);

    /*	get the segment value  */
    SEG_VAL = (ushort) (GetHexVal(pMapRec) >> 4);

    /*	skip the Stop value and pick up the segment length  */
    while (SPACE != *pMapRec++)     /* skip 'Stop' value */
	;
    while (SPACE != *pMapRec++)
	;
    SEG_LEN = (ushort) GetHexVal(pMapRec);

    /*	pick up the segment name  */
    while (SPACE != *pMapRec++)
	;
    while (SPACE == *pMapRec)
	++pMapRec;
    SEG_NAME_LEN = 0;
    while (! (SPACE == *pMapRec || '\0' == *pMapRec || '\n' == *pMapRec))
	SEG_NAMES[SEG_NAME_LEN++] = *pMapRec++;

    /*	pick up the class name	*/
    while (SPACE == *pMapRec)
	++pMapRec;
    SEG_CLASS_LEN = SEG_NAME_LEN;
    while (! (SPACE == *pMapRec || '\0' == *pMapRec || '\n' == *pMapRec))
	SEG_NAMES[SEG_CLASS_LEN++] = *pMapRec++;
    SEG_CLASS_LEN -= SEG_NAME_LEN;

    /*	allow only segment with class 'CODE', remove this if for all segs  */
    if (memcmp(&SEG_NAMES[SEG_NAME_LEN], "CODE ", SEG_CLASS_LEN) != 0)
	return(0);

    if (uMaxSegs < MAX_SEGMENTS)
	uSegs[uMaxSegs++] = SEG_VAL;
    else
	return(0);

    /*	write the segment record to the .PRF file  */
    REC_TYPE = PRF_SEG_REC;
    REC_LEN  = sizeof(FILEPFX) + sizeof(FILESEG) - MAX_SEGNAME_LEN
			+ SEG_NAME_LEN + SEG_CLASS_LEN;
    if (fwrite((char *) &PrfRec, 1, REC_LEN, PrfFile) != REC_LEN)
	WriteError(PrfFile);

    return(1);
}


STATIC short near pascal WritePubRec(PrfFile, pMapRec)
FILE		*PrfFile;
register char	*pMapRec;
{
    register short     i;

    /*	is this really a pubdef record?  */
    while (SPACE == *pMapRec)
	++pMapRec;
    if (*pMapRec < '0' || *pMapRec > '9')
	return(0);

    /* pick up the public symbol's segment value  */
    PUB_SEG = (ushort) GetHexVal(pMapRec);
    for (i = 0; i < uMaxSegs; ++i)
    {
	if (uSegs[i] == PUB_SEG)
	    break;
    }
    if (i >= uMaxSegs)
	return(0);

    /*	pick up the public symbol's offset value  */
    while (':' != *pMapRec++)
	;
    PUB_OFS = (ushort) GetHexVal(pMapRec);

    /*	position pointer and check for absolute value  */
    while (SPACE != *pMapRec)
	++pMapRec;
    while (SPACE == *pMapRec)
	++pMapRec;
    if (memcmp(pMapRec, "Abs ", 4) == 0)
    {
	PUB_ABS = TRUE;
	while (SPACE != *pMapRec)
	    ++pMapRec;
	while (SPACE == *pMapRec)
	    ++pMapRec;
    }
    else
	PUB_ABS = FALSE;

    /*	pick up the public symbol's name  */
    PUB_NAME_LEN = 0;
    while (! (SPACE == *pMapRec || '\0' == *pMapRec || '\n' == *pMapRec))
	PUB_NAME[PUB_NAME_LEN++] = *pMapRec++;

    /*	remove following code to include absolute addresses  */
    if (PUB_ABS)
	return(0);

    /*	write the public record to the .PRF file  */
    REC_TYPE = PRF_PUB_REC;
    REC_LEN  = sizeof(FILEPFX) + sizeof(FILEPUB) - MAX_PUBNAME_LEN
			       + PUB_NAME_LEN;
    if (fwrite((char *) &PrfRec, 1, REC_LEN, PrfFile) != REC_LEN)
	WriteError(PrfFile);

    return(1);
}


STATIC short near pascal WriteLinRec(PrfFile, pMapRec)
FILE		*PrfFile;
register char	*pMapRec;
{
    register short	i;
    auto     short	nRecCnt = 0;

    while (SPACE == *pMapRec)
	++pMapRec;
    if (*pMapRec < '0' || *pMapRec > '9')
	return(0);

    REC_TYPE = PRF_LIN_REC;
    REC_LEN  = sizeof(FILEPFX) + sizeof(FILELIN);
    while (! ('\0' == *pMapRec || '\n' == *pMapRec))
    {
	LIN_MOD_NUM = nCurModNum;
	LIN_NO	    = GetDecVal(pMapRec);
	while (SPACE != *pMapRec)
	    ++pMapRec;
	while (SPACE == *pMapRec)
	    ++pMapRec;
	LIN_SEG = (ushort) GetHexVal(pMapRec);
	while (':' != *pMapRec++)
	    ;
	LIN_OFS = (ushort) GetHexVal(pMapRec);
	while (! (SPACE == *pMapRec || '\n' == *pMapRec || '\0' == pMapRec))
	    ++pMapRec;
	while (SPACE == *pMapRec)
	    ++pMapRec;
	for (i = 0; i < uMaxSegs; ++i)
	{
	    if (uSegs[i] == LIN_SEG)
		break;
	}
	if (i < uMaxSegs)
	{
	    if (fwrite((char *) &PrfRec, 1, REC_LEN, PrfFile) != REC_LEN)
		WriteError(PrfFile);
	    ++nRecCnt;
	}
    }
    return(nRecCnt);
}


STATIC short near pascal WriteModRec(PrfFile, pMapRec)
FILE		*PrfFile;
register char	*pMapRec;
{
    register short	i;

    pMapRec = strchr(pMapRec, '(');
    if (NULL != pMapRec)
    {
	MOD_NUM      = (uchar) ++nCurModNum;
	++pMapRec;
	for (i = 0; *pMapRec != ')' && i < MAX_MODNAME_LEN; ++i, ++pMapRec)
	    MOD_NAME[i] = toupper(*pMapRec);
	MOD_NAME_LEN = (uchar) i;
	REC_TYPE     = PRF_MOD_REC;
	REC_LEN      = sizeof(FILEPFX) + sizeof(FILEMOD) - MAX_MODNAME_LEN
				       + MOD_NAME_LEN;
	if (fwrite((char *) &PrfRec, 1, REC_LEN, PrfFile) != REC_LEN)
	    WriteError(PrfFile);
	return(1);
    }
    return(0);
}


STATIC ulong near pascal GetHexVal(pStr)
register char	 *pStr;
{
    auto     bool	bDone	= FALSE;
    auto     ulong	lHexVal = 0L;

    while (SPACE == *pStr)
	++pStr;
    while (! bDone)
    {
	switch (*pStr)
	{
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
		lHexVal = (lHexVal << 4) + (ulong) (*pStr - '0');
		break;
	    case 'A':
	    case 'B':
	    case 'C':
	    case 'D':
	    case 'E':
	    case 'F':
		lHexVal = (lHexVal << 4) + (ulong) (*pStr - 'A' + 10);
		break;
	    case 'a':
	    case 'b':
	    case 'c':
	    case 'd':
	    case 'e':
	    case 'f':
		lHexVal = (lHexVal << 4) + (ulong) (*pStr - 'a' + 10);
		break;
	    default:
		bDone = TRUE;
		break;
	}
	++pStr;
    }
    return(lHexVal);
}


STATIC ushort near pascal GetDecVal(pStr)
register char	 *pStr;
{
    auto     bool	bDone	= FALSE;
    auto     ushort	nDecVal = 0;

    while (SPACE == *pStr)
	++pStr;
    while (! bDone)
    {
	switch (*pStr)
	{
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
		nDecVal = (nDecVal * 10) + (ushort) (*pStr - '0');
		break;
	    default:
		bDone = TRUE;
		break;
	}
	++pStr;
    }
    return(nDecVal);
}


STATIC void near pascal BuildMapFileName(pInName, pOutName)
char	  *pInName, *pOutName;
{
    register char      *pPtr;

    pPtr = strchr(strupr(strcpy(pOutName, pInName)), '.');
    if (NULL == pPtr)
	strcat(pOutName, ".MAP");
    return;
}


STATIC void near pascal BuildPrfFileName(pInName, pOutName)
char	    *pInName, *pOutName;
{
    register char	*pPtr;
    auto     short	 nCnt = 0;

    pPtr = strchr(pInName, '.');
    if (NULL != pPtr)
    {
	--pPtr;
	while (pPtr >= pInName)
	{
	    if ('\\' == *pPtr || ':' == *pPtr)
		break;
	    ++nCnt;
	    --pPtr;
	}
	memcpy(pOutName, pPtr + 1, nCnt);
    }
    pOutName[nCnt] = '\0';
    strcat(pOutName, ".PRF");
    return;
}


STATIC void near pascal WriteError(OutFile)
FILE	 *OutFile;
{
    fclose(OutFile);
    ErrorMsg("\nError while writing output file - disk full?");
    exit(4);
}


STATIC void near pascal ErrorMsg(pStr)
char	    *pStr;
{
    fprintf(stderr, "%s\n", pStr);
    return;
}
