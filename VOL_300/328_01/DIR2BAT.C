char *help_string =
 "  DIR2BAT  \n\n"
 " takes DOS DIR output as its input (STDIN - amy be redirection)      \n"
 " and creates a .BAT file as output (STDOUT)                        \n\n"
 " dir  file listings have this form: (note collumn count)             \n"
 "   01234567890123456789                                              \n"
 "   FILENAME EXT                                                      \n"
 "   DIRNAME      <DIR>                                              \n\n"
 " the output will be                                                  \n"
 "   user_stringFILENAME.EXTuser_string                              \n\n"
 " The user_strings are set on the command line                        \n"
 "  with the @ symbol to indicate where to put the file name         \n\n"
 "  example:                                                           \n"
 "       dir | dir2bat copy @ a: > newbat.bat                        \n\n"
 "  the file newbat will have each line read:                          \n"
 "	copy filename.ext a:                                         \n\n"
 "  a useful way to call this program is:  DIR2BAT %1 %2 @ >newbat.bat \n"
 "      then use NEWBAT CALL otherbat                                  \n"
 "      and DOS will execute otherbat once for each file             \n\n"
 "  to place the symbols    |    <    or  >  in the ouptut listing,    \n"
 "  use ^ followed by       \\    ,        .   (lowercase, same key)   \n"
 "  example:   dir | dir2bat type @ ^. contents.txt > newbat.bat       \n"
 "             The lines in the output file (newbat.bat) will then read:\n"
 "             type filename.ext > contents.txt                      \n\n"
 " @copyright D BLUM 1989                                              \n";


/* to compile this program under TurboC (produces dir2bat.com): 
 *		tcc  -mt -lt -N-  dir2bat.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

main (int argc, char **argv) {

int n = 0;
char inline[256];
char outline[81];
char *out_start;
char *outptr;
char second_string[256];
char *ptr;

outline[0] = 0x00;
out_start = outline;

/* get parameters and cocnatenate them to make a feeder */
while ( ++n < argc)
	{
	strcat (outline, argv[n] );
	strcat (outline, " ");
	}

/* replace "^." with "> ",  "^," with "< " and  "^\\" with "| "
 */
outptr = outline;
while ( (outptr = strchr(outptr, '^')) != NULL)
	{
	switch( *(outptr+1) )
	{
	case (','):
		*outptr 	= '<';
		*(outptr+1)     = ' ';
		break;
	case ('.'):
		*outptr 	= '>';
		*(outptr+1)     = ' ';
		break;
	case ('\\'):
		*outptr 	= '|';
		*(outptr+1)     = ' ';
		break;
	default:
		/* user actually wants ^ without replacement in output
		 * so must skip past it, otherwise would get infinite loop
		 */
		++outptr;
	}    /*end switch */
	}    /* end while */



/*look for the @ */
out_start = strchr(outline, '@');
if (out_start == NULL)
	{
	perror("ERROR - program DIR2BAT requires a @ on the command line\n\n\n");
	perror(help_string);
	exit(99);
	}

/* move the text that follws the @ to a save area
*/
strcpy(second_string, out_start+1);


/*read the directory listing
*/
while (gets(inline) != NULL)
	{

	memset (out_start,' ', 13);

	if ( strchr (inline, '-')  && !strstr (inline, "<DIR>"  ))
		{
		/* if a dash is in the line (tru for files & dirs )
                 *  and if it's not a DIR
		 *  then it is a file, not a directory
		 */


		/* move the filename and the blank that follows */
		memcpy(out_start, inline, 9);

		/* setup the period to separate filename from extension */
		outptr = strchr ( out_start , ' ');
		if (outptr == NULL)
			{
			outptr = out_start + 8;
			}
		*outptr  = '.';
		++outptr;

		/* copy the extension */
		memcpy ( outptr, inline+9, 3);
		/*move the second string in after the filename */
                ptr = strchr (outptr, ' ');
                if (ptr == NULL)
                   {
                   ptr = outptr + 3;
                   }
		strcpy ( ptr, second_string);
		puts (outline);
		}  /*end of file processing */

	else if ( strstr (inline, "<DIR>") )
		{
		/* this line lists a directory
		 */
		memcpy ( out_start, inline, 8 );
		outptr = strchr (out_start, ' ');
		if (outptr == NULL)
			{
			outptr = out_start + 8;
			}
		/* mark this as a directory */
		*outptr = '\\';
		strcpy (outptr+1, second_string);
		puts (outline);
		} /*end of directory processing */



	} /*end of while !feof()*/


return (0);
} /*end of main */