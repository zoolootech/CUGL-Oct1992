static char helptext[] = {
"TXT2MCR - this program converts an ascii file to a macro file\n"
"          the  .txt file can be created with any text editor.\n"
"          PARAMETERS: filename ( leave out the .txt extension )\n"
"                      filename.txt will be converted to filename.mcr\n"
	};

/*  to compile this program under TurboC:
 *		tcc -mt -lt -Z -d txt2mcr.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MOUSE 	128

main ( int argc, char **argv )
	{

	int    		n;
	unsigned char  	c;

	int nchar =0;

	char in_name[13], out_name[13];
	FILE *inf, *outf;

	if ( argc == 1  || *argv[1] == '?' || (strlen (argv[1]) > 8) )
		{
		puts (helptext);
		exit (1);
		}

	strcpy (in_name,  argv[1] );
	strcpy (out_name, in_name);
	strcat (in_name,  ".txt" );
	strcat (out_name, ".mcr" );

	if ( NULL == ( inf=fopen (in_name, "rb") ) )
		{
		printf ("FILE NOT FOUND %s\n\n%s\n",in_name,helptext);
		exit (1);
		}

	if ( NULL == ( outf=fopen (out_name, "wb") ) )
		{
		printf ("CANNOT OPEN  %s\n\n%s\n",out_name,helptext);
		exit (1);
		}


	while ( ! feof (inf) )
		{
		if ( 1 == fread ( &c, sizeof (c), 1, inf) )
			{
			++nchar;
			n = c;
			if ( n == MOUSE )
				{
				puts ("Input file contains MOUSE character\n"
				      "Illegal character 0x80 = 128\n"
				      "TERMINATING\n");
				exit (10);
				}

			if ( 1 != fwrite ( &n, sizeof(n), 1, outf ) )
				{
				puts ("ERROR DURING OUTPUT...TERMINATING");
				exit (10);
				}
			}

		}

	fclose (inf);
	fclose (outf);


	printf (
	"MACRO FILE %s successfully created, %i characters transferred\n",
	out_name, nchar);





	return 	(0);	/* main */
	}