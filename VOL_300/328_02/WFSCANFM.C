/* wfscanfm.c
 *     	procedure: wfscanform(FILE *infile, WFORM *form, char *stopcode)
 *
 *	This procedure is a file-header parser.
 *	Text files are read and each line is checked to see if
 *	it matches an item in the WFORM table. If so, the data is
 *	formatted and placed in the calling program's storage area
 *	...as indicated by the table.
 *
 *	The input file may be in the format:
 *		KEYWORDdata\r\n
 *		KEYWORDdata\r\n
 *		STOPCODE\r\n
 *	The KEYWORD may have a punctuation mark or whitespace at the end,
 *		these are ignored for the purposes of comparison.
 *		So the file may be in any of the following forms:
 *		KEYWORD: filedata\r\n
 *		KEYWORD:filedata\r\n
 *		KEYWORD filedata\r\n
 *		KEYWORD-filedata\r\n
 *		KEYWORD$  filedata\r\n
 *
 *
 *		ONLY 1 punctuation mark may be used as a delimiter,
 *			but it doesn't have to be the same one each time.
 *
 *
 *		in each of these cases, the data that will be returned
 *              will be 'filedata' without the punctuation or leading space.
 *
 *	However, if no delimiter or space is indicated in the WFORM label,
 *		then any delimiter found will be included in the data.
 *
 *	Data may continue to the next line if the last line is a + - or \
 *		ex: col 0123456789....
 *              KEYWORD: This is dat+\r\n
 *              a for the program\r\n
 *		will be read as This is data for the program.
 *
 *
 *	Open the input file in either text or binary mode,
 *		This routine actively ignores \n\r
 *		and does not depend on fgets() to remove these codes.
 *
 *
 *	If a KEYWORD is not found in the file, no values are moved to the
 *		calling program's data area.
 *
 *	Input stops on FEOF or on an input line entirely contained in STOPCODE
 *
 *		OK if you specify STOPCODE as DATA: and the input line is DATA\r\n
 *		   	or if the input line is DATA:\r\n and stopcode is just DATA
 *			
 *			ie: ok for a single punct. mark at end of stopcode or input line
 *				for a header record, STOPCODE might be DATA: or DATA- or DATA
 *
 *	to display the KEYWORDdata information onscreen, use the same WFORM[]
 *		and call WPRINTFORM ()
 *	to create compatible file headers, use the same WFORM[]
 *		and call WFPRINTFORM[]
 *
 *
 *
 *
 */


#include  "wsys.h"


#define wftype wfspecial




/* static function:
 *	read input from infile,
 *	piece together continuation lines,
 *	check for stopcode.	RETURNS: zero if no more data to check.
 */
static int do_another(	FILE *infile, char *buffer,
			int buffsize, char *stopcode );





void wfscanform ( FILE *infile, WFORM *form, char *stopcode )
	{
	char 	*buffer;
	WFORM 	*item, *firstentry;
	int  	n, buffsize, lbl_len;
	char 	*data_ptr,
		*userend;



	_NORMALIZE (form);



	firstentry = form;

	item = firstentry;

	/* scan form, fill in data types, figure out longest data element
	 */


	for ( 	item = firstentry, buffsize = 0;
		item->wflabel != NULL;
		++item
	    )
		{
		_NORMALIZE (item->wfuser);
		_NORMALIZE (item->wflabel);
		_NORMALIZE (item->wformat);

		/* find the longest piece of data (plus size of it's label
		 */

		n= strlen (item->wflabel) + item->wflen;
		if ( buffsize < n )
			{
			buffsize = n;
			}


		/* check data type against supported types,
		 * move single char type into form table
		 */
		item->wftype = *(item->wformat +
				strcspn ( item->wformat, "csidfegEGouxX") );

		} /* end for... scanning table ... */




	buffsize += 256;	/* add enough to cover longest line */
	buffer = wmalloc ( buffsize, "WFSCANFORM" );



	while ( do_another ( infile, buffer, buffsize, stopcode ) )
		{
		for ( item = firstentry; item-> wflabel != NULL; ++item )
			{
			lbl_len = strlen (item->wflabel);

			/* ignore punctuation marks and whitespace
			 * at the end of the label
			 */
			while( --lbl_len &&
			      ( ispunct( item->wflabel[ lbl_len ] )
			      || isspace (item->wflabel[ lbl_len ] )
			      )
			     )
			     { /* empty */
			     }
			++lbl_len;




			if (memicmp ( item->wflabel, buffer, lbl_len ) ==0 )
				{
				/* line in file matches line in form 
				 */
				data_ptr = buffer + lbl_len;		/* pt past label */

				/* skip whitespace and punctuation between label and data.
				 */
				while ( ispunct( *data_ptr )  || isspace ( *data_ptr ) )
					{
					++data_ptr;
					}

				if ( *data_ptr == 0 )
					{
					/* skip NULL entry */
					}
				else
				if (item ->wfuser)			/* not a pure label, so get data */
					{
					/* update user data from buffer
					 */
					switch ( item ->wftype )
						{
					case ( 'c' ):
						*((char*)item->wfuser)
							= *data_ptr;
						break;
					case ( 's' ):

						memcpy(
						(char*)(item->wfuser),
						   data_ptr,
						   item->wflen  );

						/* make sure string has a
						 * terminal NULL
						 */
						userend =((char*)item->wfuser)
							  + (item->wflen)-1;

						*userend = 0;


						break;
					default:
					sscanf (data_ptr,
						    item->wformat,
							   item->wfuser);
						}/* end switch */

				/* end of updating */
					}	/* end non-NULL data */

				}	/* end if... data matches from line */

			}	/* end for... item */
		}	/* end while... infile */

	free (buffer);

	return; 	/* wfscanform */
	}


/* do_another()
 *	static routine that reads one line,
 *	strips any DOS end-line characters,
 *
 *	concateneates next line if last line ended with a continuation mark.
 *
 * 	RETURNS   0 if end of file or stopcode
 *		  1 if need to read more.
 */
static int do_another(	FILE *infile, char *buffer,
			int buffsize, char *stopcode )
	{

	int retcode;
	int inlen;
	int remaining;
	int keep_going;
	register int actual_inlen;
	


	inlen =0;
	retcode = 1;

	do	{

		remaining = buffsize - inlen;

		fgets ( buffer + inlen, remaining, infile );

		if ( feof (infile) )
			{
			keep_going = retcode = 0;	/* don't do any more */
			}
		else
			{

			/* strip any \n or \r or terminal spaces from end of string
			 *	(there may be two of these (\n and \r)
			 *	in either order
			 */
			inlen = strlen (buffer) -1;
			while ( inlen >0 && isspace(buffer[inlen]) )
				{
				--inlen;
				}

			/* is this a continuation line ?
			 */
			switch ( buffer[inlen] )
				{
				case ('+'):
				case ('-'):
				case ('\\'):
					keep_going = 1;
					/* NOTE: by not advancing inlen, 
					 * terminal NULL will be placed on the continuation mark.
					 */
				default:
					keep_going = 0;

					/* inlen now points to last letter,
					 * move forward to terminal NULL
					 */
					++inlen;
				}

			/*  ensure that terminal NULL is in correct place.
			 */
			buffer[inlen] = 0;

			}


		}
	while ( keep_going );		/* read continuation lines */
	
	/* now look for 'stopcode'.
	 * allow a punct. mark after stopcode, so check for actual length to compare
	 */
	actual_inlen = inlen-1;		
	if ( ispunct ( buffer[actual_inlen] ) )
		{
		inlen = actual_inlen;		
		}


	/* NOTE: must be memicmp, not stricmp, because of possibly shortened inlen
	 * 	(see check for ispunct() above)
	 */
	if ( memicmp( buffer, stopcode, inlen ) == 0 )
		{
		retcode =0;
		}




	return (retcode);	/* do_another */
	}


/*-------------------- end of wfscanform.c ----------------------*/
