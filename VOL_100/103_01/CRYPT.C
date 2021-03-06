/* last update 7 May 81

CRYPT program by kathy bacon and neal somos

	CRYPT can be used to "encrypt" a file for secrecy, using a 
special keyword which the user specifies; later, the original file
can be retrieved by re-encrypting the file with the same keyword.
If you forget your keyword...well, sorry.


CRYPT accepts command line arguments of three forms:

	 INFILE
	 INFILE>OUTFILE
	 <KEYWORD

If no output file is specified, the encrypted form of INFILE will be
written out as INFILE.CRP ( if the input file has an extension of 
".CRP", however, it will be written out as TEMP.$$$ ).
	In the second case, the ">" means that the output file name
will follow.
	The key word itself can also be specified on the command line,
which is useful if you want to use a submit file. Only one keyword per
command line, however.
	CRYPT will ask for the keyword if none is given; it will not
be echoed to the console.

	:::::::::::::::::::::::::::::::::::::::::::

NOTE!!!!!  There should be NO SPACES between a ">" and the filenames;
	   nor between a "<" and its keyword.

	:::::::::::::::::::::::::::::::::::::::::::

a sample call might therefore be:

A>crypt  tomb.c  help.crp>help.c   <yippeezoop


************************************************************/

#include "bdscio.h"

int debug;

#define	KEYCHAR	'<'
#define OUTCHAR '>'
#define READ	0
#define	ERROR_EXIT	{ close(fd_in);	exit();	}
#define NEWLINE	'\n'
#define DEBUG 	(debug != 0)
#define	BLANK	' '
#define	BACKSPACE	'\b'
#define CR	'\r'
#define NUL	'\0'


main(argc, argv)
int argc;
char **argv;
{
char filename[30], filecrypt[30], after[10];
char key[100], t[100], argument[100];
char *comarg;
char cryptbuf [8*SECSIZ];
int fd_in, fd_crypt;
int i, j, k, l, len;
int seed[3];
int keylen, fine, nread;

debug = FALSE;
if (argc == 1)	/* we'll change this later, so no snide remarks, neal */
      { printf("\nthis is a cryptic program!! heeheeheehee");
	exit();
      }
setmem (key, 100, NUL);
for (i=1; i<argc; i++)		/* check for key */
      { comarg = argv[i];
	if (*comarg == KEYCHAR)	/* key found */
	      { strcpy (key, &comarg[1]);
		break;
	      }
	if (OK == strcmp (comarg, "-D"))	/* debug */
	      debug = TRUE;
      }

if (i == argc)			/* no key found on command line */
      {
	while (1)	/* get 'key' for encrypting file */
	      { puts ("\nkey ?     (no echo)\n");
		if (0 != (keylen =no_echo (key)))
		      { if DEBUG printf("\nthanks that was <%s>",key);
			puts("\nplease retype for verification:\n");
			if (0 != (l = no_echo (t)))
			      { if (OK != strcmp (key,t))
				      { puts("\nthey aren't the same,");
					puts(" so we'd better start over.\n");
					continue;	/* while (1) */
				      }
				else puts("\nkey verified.");
				break;	/* from while (1) */
			      }
		      }
	
		puts ("\nOK, be that way!");
		
		exit();
	
	      }	/* end while (1) */
      }
argv++;
while (--argc)
      { strcpy (argument,*argv++);

	if DEBUG printf("\nnext argument is    |%s|", argument);
	if (OK == strcmp (argument, "-D"))	/* debug */
		continue;		/* while (--argc) */
	if (argument[0] == KEYCHAR)		/* key */
		continue;

	len = strlen (argument);
	if (argument[0]==OUTCHAR || argument[len-1]== OUTCHAR)	/* ERROR!!*/
	      { puts("\n\nPROFOUND error:");
	        puts ("\nNO spaces allowed around output specifier > ");
		printf("\n%s is illegal.", argument);
		continue;
	      }

	setmem (filename, 30, NUL); setmem(filecrypt, 30, NUL);
	if (ERROR != index (argument, ">"))
	      sscanf (argument, "%s>%s", filename, filecrypt);
	else strcpy (filename, argument);

	printf("\nNow processing file <%s>", filename);
	if (ERROR == (fd_in = open(filename, READ)))
	      { printf("\ncan not open <%s>", filename);
		continue;		/* while (--argc) */
	      }

	if (filecrypt[0] == NUL)	/* outfile not specified */
	      {	sscanf (filename, "%s.%s", filecrypt, after);
		if (OK == strcmp (after, "CRP"))
			strcpy (filecrypt, "TEMP.$$$");
		else strcat (filecrypt, ".CRP");
	      }
	printf("\nencrypted file will be <%s>", filecrypt);
	if (ERROR == (fd_crypt = creat ("TEMP.$$$")))
	      { printf("\ncan not create temporary file");
		continue;	/* while (--argc) */
	      }


/* set up seed for the number generator */
	seed[0] = seed[1] = seed[2] = 0;
	get_seed (key, seed);
	nrand (-1, seed[0], seed[1], seed[2] );
	
	if DEBUG printf("\nfd_in=%d, fd_crypt=%d",fd_in,fd_crypt);
	fine = FALSE;	/* musical end */
	while (!fine)
	      { if (OK > (nread = read (fd_in, cryptbuf, 8)))
		      { printf("\nread returned %d for <%s>",
					      nread,  filename );
			if DEBUG printf("\nfd_in=%d",fd_in);
			ERROR_EXIT
		      }
		for (i=0; i<nread*SECSIZ; i++)
			cryptbuf[i] ^= nrand(1);

		if (nread != write (fd_crypt, cryptbuf, nread))
		      { printf("\nerror writing to temporary file");
			if DEBUG printf("\nfd_crypt=%d",fd_crypt);
			ERROR_EXIT
		      }
		if (nread == 0 || nread != 8)	fine = TRUE;
	      }
	if (ERROR == close (fd_in))
		printf("\nerror closing file <%s>", filename);
	if (ERROR == close (fd_crypt))
	      { puts ("\nerror closing temporary file");
		exit();
	      }
	unlink (filecrypt);
	rename ("TEMP.$$$", filecrypt);
	printf("\n\nEncrypted form of <%s> is in <%s>",
                                 filename,   filecrypt);

      }	/* end while (--argc) */

}	/* end main() */

/*************************************************************
	gets input from keyboard char by char, blanking out
as received. Assumes that "input" is big enough to handle input.
Returns length of input string.
******************************************************************/
int no_echo (input)
char *input;
{
int c;
char *cp;

cp = input;
while (1)
      { if (kbhit())
	      { if (NEWLINE == (*cp = getchar()))
		      { *cp = NUL;
			break;
		      }
		cp++;
		putchar (BACKSPACE);
		putchar (BLANK);
	      }
      }
return (strlen(input));
}

/*************************************************************
	get a seed for the ramnod number generator
"seed" should be a 3-integer array
*************************************************************/
get_seed (key, seed)
char *key;
int *seed;
{
int *ptr;
int i, length;

length = strlen (key);
ptr = key;
for (i=0; i< length/2; i++)
	seed [i%3] +=	*ptr++;
}

/*********************************************************************
 returns index of t in s, ERROR if not found.  Thanx to Kernighan
 and Ritchie, p. 67
*********************************************************************/
int index (s, t)
char *s, *t;
{
int i, j, k;

for (i=0; s[i] != NUL; i++)
      { for (j=i, k=0; t[k] != NUL && s[j]==t[k]; j++, k++)
		;
	if (t[k] == NUL)
		return (i);
      }
return (ERROR);
}
    { for (j=i, k=