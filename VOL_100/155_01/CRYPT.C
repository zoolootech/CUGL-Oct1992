/*	
HEADER:
TITLE:		crypt;
DATE:		04/18/85;
DESCRIPTION:	
   "This uses the simple encryption/decryption scheme of XORing each character 
with those of a comparison string. The security in this is that any string of
characters, known only by the user, can encode the text.  Furthermore to     
retrieve the original text, the user simply applies the same key string.";     

KEYWORDS:	encryption,decryption,XOR;
FILENAME:	crypt.c;
WARNINGS:
     "Do not encrypt a file with itself!  You will lose the   
original file ( it will be all blank spaces due to the method used )."       

AUTHORS:	Jim Pisano;
COMPILERS:	DeSmet;
REFERENCES:	AUTHORS:	Kernighan,Plauger;
		TITLE:		Software Tools;
		CITATION:	"pg 49";		
	ENDREF:
*/

/*   file name ... crypt.c
*    program ..... crypt(), xor()
*
*   Adapted from Software Tools by Kernighan & Plauger, p.49.  
*
*   C implementaion by 
*   Jim Pisano 
*   April 4, 1985
*
*    This uses the simple encryption/decryption scheme of XORing each character
* with those of a comparison string. The security in this is that any string of
* characters, known only by the user, can encode the text.  Furthermore to 
* retrieve the original text, the user simply applies the same key string.      
*
*   This implementation will accept the encoding key either directly from the
* keyboard or from a file.  If the key is not a valid file name then it is used
* as an enciphering key.  The encoded data are written to a temporary file.
* The original file is then deleted, the temporary file is renamed to the 
* original file eliminating the temporary file.
*
* **** CAUTION *****  Do not encrypt a file with itself!  You will lose the
* original file ( it will be all blank spaces due to the method used ).       
*/

#include  <stdio.h>

main( argc, argv )
int argc;
char *argv[];
{

	FILE *in_file, *in_crypt, *out_file, *fopen();
	int i, key_len, in_char, out_char, err;
	char *key;			/* encryption key */
/*
* Ensure that command line contains require inputs.
*/
	if( argc < 2 )
		pr_err_exit("No input file.");

	else if( argc < 3 )
		pr_err_exit("No encryption code declared."); 

	in_file = fopen( argv[1], "r" );    /* open input file to encrypt */
	if( in_file == ERR )
		pr_err_exit("Can't open input file.");

	out_file = fopen( "temp.dat", "w" );	/* open temporary data file */
	if( out_file == ERR )
		pr_err_exit("Can't open temporary file.");

/* deal with encoding key */
	i = 0;				/* init pointer to beginning of key */
	in_crypt = fopen( argv[2], "r" );
/* 
* check if encoding key is a valid filename if so open it & get the key else
* use the 3-rd command line arguement as a key
*/
	if( (in_crypt != NULL) && (in_crypt != ERR) )
	{
		while( ( in_char = fgetc( in_crypt ) ) != EOF )
			*(key + i++) = in_char;
		fclose( in_crypt );
	}
/*
* otherwise use the third command line arguement as a encryption key
*/
	else 	
		key = argv[2];

	key_len = strlen( key );
	i = 0;
/*
* encode input file using non-buffered I/O
*/
	while( (in_char = fgetc( in_file ) ) != EOF )
	{
/*
* wrap key phrase around if it's shorter than data
*/
		if( i >= key_len )	
			i = 0;		

		out_char = xor( in_char, *(key + i++)); 
		err = fputc( out_char, out_file );

		if( err == ERR )
			pr_err_exit("*** Error in writing to temporary file, check disk space.");
	}
	fclose( in_file );
	fclose( out_file );
/*
* rename temporary file to original one & delete extraneous files.
*/
	err = unlink( argv[1] );
	if( err == ERR )
		pr_err_exit("Error in delete input file.");

	err = rename( "temp.dat", argv[1] );
	if( err == ERR )
		pr_err_exit("Error in renaming file.");
}
/*
*  Do bitwise exclusive or of bytes a and b
*/
xor( a, b )
char a, b;
{
	return( (a & ~b) | (~a & b) );
}
/* 
* print error message and exit
*/
pr_err_exit( mess )
char *mess;
{
	printf( "\n%s\n", mess );
	printf( "Aborting crypt...\n");
	exit();
}