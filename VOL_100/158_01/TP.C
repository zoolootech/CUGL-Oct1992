/*	/usr/src/filters/tp.c					*/
/*	=====================					*/



#include	<stdio.h>
#include	<setjmp.h>
#define		MAXLINE 	640	/* #of chars in input line	      */
#define		LONGMAXLINE	640	/* #of chars in constructed column    */
#define		MAXROW		64	/* #of non-trivial rows		      */
#define		SHORTLINE	64	/* #of characters in a word	      */
#define		SHELLBAD	1	/* UNIX shell convention for `bad'    */
#define		SHELLGOOD	0	/* UNIX shell convention for `good'   */
#define		FALSE		0
#define		TRUE		-1


jmp_buf	env;				/* setjmp environment		*/


main(argc, argv)
int	argc;
char	*argv[];
{

	FILE	*fp, *tty_fp, *fopen();
	int	status;			/* main's status		*/

	tty_fp = fopen( "/dev/tty", "w" );

	if (argc == 1)  {		/* input from pipe		*/
		fprintf( tty_fp, "tp: can't work with pipe input.\n" );
		return( SHELLBAD );
	}
	else if (argc == 2)  {		/* file on command line		*/
		if ( (fp = fopen(*++argv, "r") )  ==  NULL ) {
			fprintf( tty_fp, "tp: can't open %s\n", *argv );
			return( SHELLBAD );
		}
		else
			;		/* successful file open		*/
	}
	else  {
		fprintf( tty_fp, "tp: too many arguments\n" );
		return( SHELLBAD );
	}


	/* Error return at the level of `do_transpose' and lower
	   are not implemented, i.e. "errors" are handled gracefully
	   and no aborts nor longjumps are used.
	*/

	if ( status = setjmp(env) )
		;
	else
		do_transpose( fp );
	fclose( fp );
	if ( status )  {
		fprintf( tty_fp, "tp: transpose operation encounters error.\n" );
		return( SHELLBAD );
	}
	else
		return( SHELLGOOD );

}


do_transpose( fp )
FILE	*fp;				/* file-pointer: general-case	*/
{

	char	line[MAXLINE];		/* primary buffer		*/
	char	*word_pointer[MAXROW];
	int	row_space, col_space;
	char	mini_buffer[SHORTLINE];
	char	col_buffer[LONGMAXLINE];
	int	i, j;

	row_space = 0;
	while ( getline(line, MAXLINE, fp) )
		if ( test_nontrivial_line_length( line ) )  {
			if ( row_space == 0 )
				col_space = find_column_space( line );
			word_pointer[row_space++] = line;
		}
	for ( i = 0 ; i < col_space ; i++ )  {
		rewind( fp );
		*col_buffer = '\0';
		for ( j = 0 ; j < row_space ; )  {
			getline(line, MAXLINE, fp);
			if ( test_nontrivial_line_length( line ) )  {
				read_next_col( word_pointer+j++, mini_buffer );
				append_with_gap( mini_buffer, col_buffer );
			}
		}
		printf( "%s\n", col_buffer );
	}

}


getline(s, lim, fp)	/* get line into s[0 ---> (lim-1)], return length */
char	s[]; 		/* generalized to use a file-pointer for the get  */
int	lim;		/* During normal operation, a line is returned	  */
FILE	*fp;		/* and the trailing \n is also returned.	  */
			/* The whole line is a string, of course, and is  */
			/* terminated with a \0.			  */
			/* s[] is guaranteed to end with a \0 even if the */
			/* source character stream from the file has no   */
			/* \n and is incredibly long.  In that case,	  */
			/* s[0] thru s[lim-2] would have source-file	  */
			/* characters and s[lim-1] would hold the \0.	  */
			/* This puts the caller's mind at ease because	  */
			/* he now knows that the returned "line" (which   */
			/* may not actually end with \n if it is longer   */
			/* that the buffer length) is always a legitimate */
			/* string, i.e. it always end with \0.		  */
{
	int	c, i;

	i = 0;
	while ( --lim > 0  &&  (c=getc(fp)) != EOF  &&  c != '\n' )
		s[i++] = c;
	if ( c == '\n' )
		s[i++] = c;
	s[i] = '\0';
	return(i);
}


test_nontrivial_line_length( line )	/* If line has reasonable characters, */
char	*line;				/* 1 is returned, else 0 returned.    */
{
	for ( ; *line != '\0' ; line++ )
		if ( *line != '\40' && *line != '\t' && *line != '\n' )
			return( 1 );
	return( 0 );
}


find_column_space( line )		/* Will find out how many words	*/
char	*line;				/* are on line.			*/
{
	int	col_space;		/* measurement quantity		*/
	int	scanning_word;		/* useful flag			*/

	for ( scanning_word = FALSE, col_space = 0 ; *line != '\0' ; line++ )
		if ( *line != '\40' && *line != '\t' && *line != '\n' )  {
			if ( !scanning_word )  {
				scanning_word = TRUE;
				col_space++;
			}
		}
		else if ( scanning_word )
			scanning_word = FALSE;

	return( col_space );
}


read_next_col( wpp, mini_buffer )	/* Reads the column pointed by *wpp
					   into mini_buffer and try to advance
					   pointer to next column.  If next
					   column does not exist, leave pointer
					   at current position so that next call
					   will read the same (last) column.  */
char	**wpp;				/* wpp :: word pointer pointer	      */
char	mini_buffer[];
{
	int	scanning_word;		/* useful flag			      */
	char	*old_position;		/* last word tag in case next word
					   does not exists		      */
	char	*p;			/* scratch pointer		      */

	old_position = *wpp;
	while ( (**wpp=='\40' || **wpp=='\t' || **wpp=='\n') && **wpp!='\0' )
		(*wpp)++;
	if (**wpp == '\0')
		*wpp = old_position;
	old_position = *wpp;
	while ( **wpp!='\40' && **wpp!='\t' && **wpp!='\n' && **wpp!='\0' )
		*mini_buffer++ = *((*wpp)++);
	*mini_buffer = '\0';
	while ( (**wpp=='\40' || **wpp=='\t' || **wpp=='\n') && **wpp!='\0' )
		(*wpp)++;
	if (**wpp == '\0')
		*wpp = old_position;
}


append_with_gap( mini_buffer, col_buffer )   /* Append content of mini-buffer to
					        content of col-buffer.	      */
char	mini_buffer[], col_buffer[];
{
	while (*col_buffer++ != '\0')
		;
	*--col_buffer= '\40';
	col_buffer++;
	while (*mini_buffer != '\0')
		*col_buffer++ = *mini_buffer++;
	*col_buffer = '\0';
}


/******************************************************************************/
/*

	This program is designed to take a file with a "matrix" of words
	and output a file with the matrix transposed.
	The rows are delimited by \n and the columns are delimited by
	spaces.  Multiple spaces functioning as column separators
	are compressed as are multiple newlines functioning as row separators.
	Thus, the matrix need not look like a matrix.
	In fact, the number of columns is defined by the first row.  Following
	rows with excess columns have their excess discarded.  Those with
	deficient columns are padded with the value of their last columns.
	No parameters are required: the program will discover the number of
	rows in its input by scanning the input file.
	This program is intended to handle matrices of up to 64 by 64 and
	where field lengths are reasonable, say 9 characters long.
	Usage is defined for input file on command line.  Pipe usage
	fails because of the need in the program to rewind the input stream.

*/
/******************************************************************************/
                                               