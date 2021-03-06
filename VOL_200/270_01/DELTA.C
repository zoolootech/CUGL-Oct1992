/**

	DELTA.C
	
	Delta file creation program.
	
	The algorithm used here is the "recursive longest matching 
	sequence" algorithm discussed in the 10/87 issue of DDJ and 
	modified to find the differences in reverse order. Some code
	is borrowed from the accompanying article.
	
	This program's output is in a form like the UNIX editor 'ed'
	would understand.  There are three actions which can be taken
	on a file.  These are as follows:
	
	CHANGE: Change a range of lines in a file.
	
		This command lists a range of lines to delete in the
		original file, and text for the new lines to insert in
		their place.  The list of lines is terminated by a line
		consisting of a single period character ('.').
	
		Example:	
		--------------------------------------
		40,42c
		Insert these four lines
		where the lines 40, 41, and 42
		were in the
		orignal file
		.
		--------------------------------------
		
	DELETE:	Delete a range of lines in a file.
	
		This command lists a range of lines to delete in the
		original file.  No new lines are added to the new file.
		
		Example:
		--------------------------------------
		40,42d
		--------------------------------------
		this command would delete lines 40, 41, and 42 from the

		original file.
		
	ADD:	Add new lines to the file.
	
		This command lists a location in the file, and a range of
		lines to add at this location.  As in the change command,
		the list of lines to add is terminated with a line with a
		single period character.
		
		Example:
		--------------------------------------
		40a
		These lines are inserted after
		line 40 in the original file
		.
		--------------------------------------

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diff.h"

static char szBuffer[MAXLINE] ;

/**

	Hash()
	
	Calculate a hash value for a string.  This value can be used
	for a first level equality check for strings, thus eliminating
	a time consuming strcmp() for each comparison.
	
	The hash value is a variation on the standard checksum hash, which
	includes the length of the string in the high word of the int.

**/

static unsigned Hash( szPtr ) 
char *szPtr ;
{
	unsigned chksum ;
	char *s ;

	/*--- Compute checksum of string ---*/
	for ( chksum = 0, s = szPtr ; *s ; chksum ^= *s++ )
		;

	/*--- return combined 7-bit checksum and length ---*/
	return ( (chksum & 0xFF) | ((s - szPtr) << 8) ) ;
}

/**

	ReadFile()
	
	Read a file into an array of LINEs.

**/

static LINE *ReadFile( pFile, nLines )
FILE *pFile ;
int *nLines ;
{
	LINE *pLine ;
	int nSize ;

	/*--- allocate one page of lines ---*/
	pLine = (LINE *)calloc( PAGESIZE, sizeof(LINE *) ) ;

	*nLines = 0 ;

	while (pLine != NULL) 
	{
		pLine[*nLines].addr = ftell( pFile ) ;
		fgets( szBuffer, MAXLINE, pFile ) ;

		if ( feof(pFile) )
			break ;

		pLine[*nLines].hash = Hash(szBuffer) ;
		pLine[*nLines].linenum = *nLines + 1;
		++(*nLines) ;	

		/*--- check to see if page filled ---*/
		if ( *nLines % PAGESIZE == 0 )
		{
			nSize = (*nLines + PAGESIZE) * sizeof(LINE *) ;
			pLine = (LINE *)realloc(pLine, nSize) ;
		}
	}

	return pLine ;
}

/**

	CheckHashes()
	
	Check to see if the next N hash codes match.  This function
	assumes that the hash codes for the first compare match.
	
	Returns:
		TRUE  if N or greater hash codes match
		FALSE if less than N hash codes match

**/

static BOOL CheckHashes( pLine1, pLine2, nCount )
LINE *pLine1,		/* Starting line in file 1 */
*pLine2 ;		/* starting line in file 2 */
int  nCount ;		/* minimum number of codes to match */
{
	int i ;

	for ( i = 0 ; i < nCount && (++pLine1)->hash == (++pLine2)->hash ; i++ )
		;

	return ( i < nCount ) ? FALSE : TRUE ;
}

/**

	CheckStrings()

	This routine actually compares the strings associated with the
	specified file lines.
	
	Returns:
		A count of how many consecutive strings match
		
**/

static int CheckStrings( pFile1, pLine1, pFile2, pLine2, nMost )
FILE *pFile1 ;		/* file 1 stream */
LINE *pLine1 ;		/* line in file 1 */
FILE *pFile2 ;		/* file 2 stream */
LINE *pLine2 ;		/* line in file 2 */
int  nMost ;		/* most possible matches */
{
	static char szBuf1[MAXLINE], szBuf2[MAXLINE] ;
	int i ;

	/*--- seek to starting locations for strings ---*/
	fseek( pFile1, pLine1->addr, SEEK_SET ) ;
	fseek( pFile2, pLine2->addr, SEEK_SET ) ;

	for ( i = 0 ; i < nMost ; i ++ )
	{
		fgets( szBuf1, MAXLINE, pFile1 ) ;
		fgets( szBuf2, MAXLINE, pFile2 ) ;

		if ( strcmp(szBuf1, szBuf2) )
			break ;
	}
	return i ;
}

/**

	PrintLines()
	
	Print a range of lines from a file.  Output format is like that
	of the UNIX 'ed' editor.

**/

static void PrintLines( pFile1, pLine1, nLines1, pFile2, pLine2, nLines2 ) 
FILE *pFile1 ;
LINE *pLine1 ;
int nLines1 ;
FILE *pFile2 ;
LINE *pLine2 ;
int nLines2 ;
{
	int nType ;
	
	if ( nLines1 > 0 && nLines2 > 0 )
		nType = CHANGE ;
	else if ( nLines1 > 0 )
		nType = DELETE ;
	else if ( nLines2 > 0 )
		nType = ADD ;
	
	if ( nLines1 > 0 )
		fseek( pFile1, pLine1->addr, SEEK_SET ) ;
		
	if ( nLines2 > 0 )
		fseek( pFile2, pLine2->addr, SEEK_SET ) ;
		
	switch( nType )
	{
	case CHANGE:		/* change a range of lines */
		if ( nLines1 > 1 )
			printf( "%d,%dc\n", pLine1->linenum, 
					pLine1->linenum+nLines1-1 ) ;
		else
			printf( "%dc\n", pLine1->linenum ) ;
		break ;
	case DELETE:
		if ( nLines1 > 1 )
			printf( "%d,%dd\n", pLine1->linenum, 
					pLine1->linenum+nLines1-1 ) ;	
		else
			printf( "%dd\n", pLine1->linenum ) ;
		break ;
	case ADD:
		printf( "%da\n", pLine1->linenum-1 ) ;
		break ;
	}

	/*--- print the line range(s) ---*/
	if ( nLines2 > 0 )
	{
		while ( nLines2-- > 0 )
		{
			fgets( szBuffer, MAXLINE, pFile2 ) ;
			printf( "%s", szBuffer ) ;
		}
		printf(".\n") ;
	}
}

/**

	DoDiff()
	
	Difference two files using "recursive longest matching sequence"
	algorithm.
	
	Note that this routine is coded to return the LAST matching
	sequence in the file first.  This is different than most file
	differencing programs.  The reason for doing this is so that
	the delta file can be applied to a text file without the line
	numbers in the delta changing with each insert/delete.
	
	The 'Last-First' output requires only a simple reversal of
	the order of recursion in the algorithm.  The new algorithm 
	works as such:
	
		Find the longest sequence (A)
		Recursively find the longest sequence after sequence A 
		Recursively find the longest sequence before sequence A
	
**/

static void DoDiff( pFile1, pLine1, nMost1, pFile2, pLine2, nMost2, nLong )
FILE *pFile1 ;		/* file 1 stream */
LINE *pLine1 ;		/* start line in file 1 */
int  nMost1 ;		/* Max lines in file 1 */
FILE *pFile2 ;		/* file 2 stream */
LINE *pLine2 ;		/* start line in file 1 */
int  nMost2 ;		/* Max lines in file 1 */
int  nLong ;		/* 'long enough' match value */
{
	int nMostSoFar = 0,	/* longest sequence so far */
	nLongest = 0,		/* longest possible sequence */
	nLines = 0,		/* Lines in current match */
	nLongStart1 = 0,	/* start of longest sequence in file 1 */
	nLongStart2 = 0,	/* start of longest sequence in file 2 */
	nLong1 = 0,		/* length of longest possible in file 1 */
	nLong2 = 0,		/* length of longest possible in file 2 */
	nStart1 = 0,		/* starting line for recursion in file 1 */
	nStart2 = 0 ;		/* starting line for recursion in file 2 */
	int i, j ;
	

	for ( i = 0 ; i < nMost1 ; i ++ )
	{
		for ( j = 0 ; j < nMost2 ; j ++ )
		{
			if ( pLine1[i].hash == pLine2[j].hash )
				if ( CheckHashes( pLine1+i, pLine2+j, 
							nMostSoFar) )
				{
					/* count this sequence */
					nLongest = min( nMost1 -i, nMost2 -j );
					nLines = CheckStrings( pFile1, pLine1+i,
								pFile2, pLine2+j,
								nLongest ) ;
					if ( nLines > nMostSoFar )
					{
						nLongStart1 = i ;
						nLongStart2 = j ;
						nMostSoFar = nLines ;
					}

					if ( nMostSoFar > nLong )
					{
						/*
						   recurse using all lines
						   above the matching set
						   of lines.
						*/
						nStart1=nLongStart1+nMostSoFar;
						nStart2=nLongStart2+nMostSoFar;
						nLong1=nMost1-nStart1;
						nLong2=nMost2-nStart2;
						if ( nLong1 > 0 && nLong2 > 0 )
							DoDiff( pFile1,
								pLine1+nStart1,
								nLong1,
								pFile2,
								pLine2+nStart2,
								nLong2,
								nLong );
						else if ( nLong1>0 || nLong2>0)
							PrintLines( pFile1,
								    pLine1+nStart1,
								    nLong1,
								    pFile2,
								    pLine2+nStart2,
								    nLong2 ) ;

						/*
						   recurse using all lines
						   below the matching set
						   of lines.
						*/
						nLong1 = nLongStart1 ;
						nLong2 = nLongStart2 ;
						if ( nLong1 > 0 && nLong2 > 0 )
							DoDiff( pFile1,
								pLine1,
								nLong1,
								pFile2,
								pLine2,
								nLong2,
								nLong );
						else if ( nLong1>0 || nLong2>0)
							PrintLines( pFile1,
								    pLine1,
								    nLong1,
								    pFile2,
								    pLine2,
								    nLong2 ) ;

						return ;
					}
				}
		}
	}
	/* 
	   At this point, we have searched the entire range and
	   have the longest range of matches specified as starting
	   at line nLongestStartX and nMostSoFar lines long.
	*/

	if ( nMostSoFar > 0 )		/* any matches? */
	{
		/*
		   recurse using all lines
		   above the matching set
		   of lines.
		*/
		nStart1=nLongStart1+nMostSoFar;
		nStart2=nLongStart2+nMostSoFar;
		nLong1=nMost1-nStart1;
		nLong2=nMost2-nStart2;
		if ( nLong1 > 0 && nLong2 > 0 )
			DoDiff( pFile1,	pLine1+nStart1, nLong1,	
				pFile2,	pLine2+nStart2, nLong2, nLong );
		else if ( nLong1 > 0 || nLong2 > 0)
			PrintLines( pFile1, pLine1+nStart1, nLong1,
				    pFile2, pLine2+nStart2, nLong2 ) ;

		/*
		   recurse using all lines
		   below the matching set
		   of lines.
		*/
		nLong1 = nLongStart1 ;
		nLong2 = nLongStart2 ;
		if ( nLong1 > 0 && nLong2 > 0 )
			DoDiff( pFile1,	pLine1,	nLong1,	
				pFile2,	pLine2,	nLong2, nLong );
		else if ( nLong1 > 0 || nLong2 > 0)
			PrintLines( pFile1, pLine1, nLong1,
				    pFile2, pLine2, nLong2 ) ;
	}
	else		/* no match, this is a difference */
		PrintLines( pFile1, pLine1, nMost1, pFile2, pLine2, nMost2 ) ;
}

/**

	Banner()
	
	Print a startup banner
	
**/

static void banner()
{
	fprintf( stderr, "DELTA - Delta File Generator\n" ) ;
	fprintf( stderr, "By Ralph E. Brendler\n\n" ) ;
}
	

/**

	MAIN ROUTINE
	
**/

void main( argc, argv )
int argc ;
char *argv[] ;
{
	FILE *pFile1, *pFile2 ;
	LINE *pLine1, *pLine2 ;
	int  nLine1, nLine2 ;

	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: DELTA file1 file2\n" ) ;
		exit(1) ;
	}
	
	banner() ;		/* print startup banner */

	if ( (pFile1 = fopen(argv[1],"rt")) == NULL )
	{
		fprintf( stderr, "Unable to open %s\n", argv[1] ) ;
		exit(1) ;
	}

	if ( (pFile2 = fopen(argv[2],"rt")) == NULL )
	{
		fprintf( stderr, "Unable to open %s\n", argv[2] ) ;
		exit(1) ;
	}

	if ( (pLine1 = ReadFile(pFile1, &nLine1)) == NULL )
	{
		fprintf( stderr, "Out of memory\n" ) ;
		exit(1) ;
	}

	if ( (pLine2 = ReadFile(pFile2, &nLine2)) == NULL )
	{
		fprintf( stderr, "Out of memory\n" ) ;
		exit(1) ;
	}

	DoDiff( pFile1, pLine1, nLine1, pFile2, pLine2, nLine2, 25 ) ;
}

