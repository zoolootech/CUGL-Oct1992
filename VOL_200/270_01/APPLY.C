/**

	APPLY.C
	
	Apply a delta script to a text file.
	
**/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include "apply.h"

/* Static data */
static char szBuffer[MAXLINE] ;

/**

	AddNode()
	
	Add a linked list node after specified node.
	This routine actually does the memory allocation to create
	the node in memory.
	
	Returns:
		Pointer to the new node if successful
		NULL if out of memory
		
**/

static ELINE *AddNode( pNode, szText, nLine )
ELINE *pNode ;
char *szText ;
int nLine ;
{
	ELINE *pTmp ;
	
	if ( (pTmp = (ELINE *)calloc(1,sizeof(ELINE))) != NULL )
	{
		if ( (pTmp->text = malloc(strlen(szText)+1) ) == NULL )
			return FALSE ;
			
		pTmp->linenum = nLine ;
		strcpy( pTmp->text, szText ) ;
			
		/* fix all the list links */

		if ( pNode != NULL )
			pTmp->next = pNode->next ;
		else
			pTmp->next = NULL ;

		pTmp->prev = pNode ;
		if ( pTmp->next != NULL )
			pTmp->next->prev = pTmp ;
		if ( pTmp->prev != NULL )
			pTmp->prev->next = pTmp ;
	}
	return pTmp ;
}

/**

	DeleteNode()
	
	Remove a node from the list
	
	Returns:
		Pointer to the next node in the list.
	
**/

static ELINE *DeleteNode( pNode )
ELINE *pNode ;
{
	ELINE *pRetval ;
	
	pRetval = pNode->next ;
	
	if ( pNode->next != NULL )
		pNode->next->prev = pNode->prev ;
	if ( pNode->prev != NULL )
		pNode->prev->next = pNode->next ;
	
	free( pNode->text ) ;
	free( pNode ) ;
	
	return pRetval ;
}

/**

	ReadFile()
	
	Read a file into a linked list of ELINEs
	
	Returns:
		TRUE if file read properly
		FALSE if out of memory
	
**/

BOOL ReadFile( pFile, pHead, pTail )
FILE *pFile ;
ELINE **pHead, **pTail;
{
	int nLine = 0 ;
	
	*pHead = *pTail = NULL ;
	
	while ( ! feof(pFile) )
	{
		if ( fgets( szBuffer, MAXLINE, pFile ) )
		{
			++nLine ;
		
			if ( (*pTail = AddNode(*pTail, szBuffer, nLine)) == NULL )
				return FALSE ;
			
			if ( *pHead == NULL )
				*pHead = *pTail ;
		}
	}
	return TRUE ;
}

/**

	DoDelete()
	
	Do a deletion of a range of lines

**/

void DoDelete( nStart, nEnd, pHead, pTail ) 
int nStart, nEnd ;
ELINE **pHead, **pTail ;
{
	/* back up to start of range to delete */
	while ( (*pTail)->linenum != nStart && (*pTail)->linenum > 0 )
		*pTail = (*pTail)->prev ;
		
	if ( (*pTail)->linenum > 0 )
	{
		while ( *pTail != NULL && (*pTail)->linenum <= nEnd )
		{
			if ( *pTail == *pHead ) 
				*pHead = (*pHead)->next ;

			*pTail = DeleteNode( *pTail ) ;
		}
	}
}

/**

	DoAdd()
	
	Do an add of a range of lines

**/

void DoAdd( nAfter, pHead, pTail, pNode )
int nAfter ;
ELINE **pHead, **pTail, **pNode ;
{
	ELINE *pTmp, *pNext ;
	
	if ( nAfter == 0 )		/* insert at head */
		*pTail = *pHead ;
	else				/* back up to insertion point */
		while ( (*pTail)->linenum != nAfter && (*pTail)->linenum > 0 )
			*pTail = (*pTail)->prev ;
		
	if ( (*pTail)->linenum > 0 )
	{
		pTmp = (*pTail)->next ;	/* save end link */
		
		(*pTail)->next = *pNode ;
		(*pNode)->prev = *pTail ;
		
		while ( *pNode != NULL && strcmp((*pNode)->text, ".\n") )
			*pNode = (*pNode)->next ;
			
		if ( *pNode != NULL )
		{
			pNext = (*pNode)->next ;
			(*pNode)->prev->next = pTmp ;
			if ( pTmp != NULL )
				pTmp->prev = (*pNode)->prev ;
			*pNode = pNext ;
		}
	}
}			

/**

	DoChange()
	
	Do a change on a range of lines

**/

void DoChange( nStart, nEnd, pHead, pTail, pNode )
int nStart, nEnd ;
ELINE **pHead, **pTail, **pNode ;
{
	DoDelete( nStart, nEnd, pHead, pTail ) ;
	DoAdd( nStart-1, pHead, pTail, pNode ) ;
}

/**

	ParseDelta()
	
	Process the commands in the delta file

**/

void ParseDelta( pDHead, pFHead, pFTail ) 
ELINE **pDHead ;		/* head of delta list */
ELINE **pFHead, **pFTail ;	/* head, tail of file list */
{
	char *szStart, *szEnd, *szType ;
	int nStart, nEnd ;
	
	while ( *pDHead != NULL )
	{
		szStart = (*pDHead)->text ;
		szEnd = szStart ;

		/* get the range of numbers from string */
		nStart = atoi( szStart ) ;
		while ( isdigit(*szEnd) )
			++szEnd ;
		if ( *szEnd == ',' )
		{
			szType = ++szEnd ;
			while ( isdigit(*szType) )
				++szType ;
			nEnd = atoi( szEnd ) ;
		}
		else
		{
			nEnd = nStart ;
			szType = szEnd ;
		}
		
		*pDHead = (*pDHead)->next ;
		
		switch( *szType )
		{
		case 'c':		/* change */
			DoChange( nStart, nEnd, pFHead, pFTail, pDHead ) ;
			break ;
		case 'd':		/* delete */
			DoDelete( nStart, nEnd, pFHead, pFTail ) ;
			break ;
		case 'a':		/* add */
			DoAdd( nStart, pFHead, pFTail, pDHead ) ;
			break ;
		}
	}
}

/**

	Banner()
	
	Print a startup banner
	
**/

static void banner()
{
	fprintf( stderr, "APPLY - Delta File Applicator\n" ) ;
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
	ELINE *pDHead, *pDTail, *pFHead, *pFTail;
	
	pDHead = pDTail = pFHead = pFTail = NULL ;

	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: APPLY file delta\n" ) ;
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

	if ( ReadFile(pFile1, &pFHead, &pFTail) == FALSE )
	{
		fprintf( stderr, "Out of memory\n" ) ;
		exit(1) ;
	}

	if ( ReadFile(pFile2, &pDHead, &pDTail) == FALSE )
	{
		fprintf( stderr, "Out of memory\n" ) ;
		exit(1) ;
	}

	ParseDelta( &pDHead, &pFHead, &pFTail ) ;
	
	fclose( pFile1 ) ;
	fclose( pFile2 ) ;
	
	while ( pFHead != NULL )
	{
		printf( "%s", pFHead->text ) ;
		pFHead = pFHead->next ;
	}
}


