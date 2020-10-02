/* wfprintf.c
 *
 *	contains procedure wfprintform()
 *
 *
 *	This procedure writes the contents of a WFORM table to a file
 *
 *	The information in the WFORM may be gathered from the user
 *		using the function wscanform()
 *
 *	The resulting file may then be read back in using wfscanform()
 *	 	(with the same WFORM table )
 *
 *	This procedure cna create header records or lists of options
 *
 *	The 4 procedures wscanform, wprintform, wfscanform, and wfprintform
 *      are a matched set for
 *		gathering options from the user,
 *		writing them to a file, (which is used by other programs)
 *		reading back the options at another time
 *		displaying current sets of options on the screen
 *	Each of these can be done using the same WFORM.
 *
 *	This system is flexible in that, if you initialize the userdata
 *		to defaults before calling wfscanform
 *		then not all options have to be in the file.
 *
 *
 *	In addition, this system is 'upwardly mobile'
 *		in that you can add a new element to the WFORM table
 *		and set the user data = a default,
 *		older files won't be affected,
 *		newer files will contain the new line
 *		and your program will be able to read any version.
 *
 *
 * 	PARAMETERS:
 *		FILE *outfile  = file to write to.
 *			If the file is openned in 'binary' mode,
 *			... each line will end with a \n\r
 *			If the file is openned in 'text' mode,
 *			...a blank line will be written.
 *			This won't affect wfscanform() though,
 *			   (wfscanform doesn't care about blanks)
 *
 *		WFORM form[] = table giving labels and data pointers, etc...
 *	RETURNS:
 *		void
 */
#include  "wsys.h"


#define wftype wfspecial






void wfprintform ( FILE *outfile, WFORM *form )
	{
	WFORM 	*item, *firstentry;

	char *newline;

	/* decide if file is opened as BINARY, if so, needs a \r\n
	 * 									   if TEXT, needs just a \n
	 */
	newline =  ( (outfile->flags) & _F_BIN )  ? "\r\n"  : "\n";


	_NORMALIZE (form);



	firstentry = form;


	/* scan form, decide what format data is in, write the data
	 */


	for ( 	item = firstentry;
		item->wflabel != NULL &&   ! ferror (outfile);
		++item
	    )
		{
		fprintf (outfile, item->wflabel );



		if ( item->wfuser  )
			{


			/* check data type against supported types,
			 * move single char type into form table
			 */
			item->wftype = *(item->wformat +
				strcspn ( item->wformat, "csidfegEGouxX") );




			switch ( item ->wftype )
				{
			case ( 'c' ):
				fputc (*((char *) (item->wfuser)), outfile);
				break;

			case ( 's' ):
				fprintf (outfile, item->wformat,
					 ((char *)item->wfuser));

				break;

			case ( 'f' ):
			case ( 'e' ):
			case ( 'g' ):
			case ( 'E' ):
			case ( 'G' ):
				fprintf (outfile, item->wformat,
					*((float*)(item->wfuser)) );
				break;
			case ( 'i' ):
			case ( 'd' ):
				fprintf (outfile, item->wformat,
					 *((int*)(item->wfuser)) );
				break;
			case ( 'u' ):
			case ( 'x' ):
			case ( 'X' ):
			case ( 'o' ):
				fprintf (outfile, item->wformat,
					*((unsigned int*)(item->wfuser)) );
				break;
			
				} 	/* end switch */



			} /* end if ...user data to be printed */
		fprintf ( outfile, newline );
		} /* end for... scanning table ... */

	return;	/* wfprintform */
	}


/*-------------------- end of wfprintfm.c ----------------------*/
