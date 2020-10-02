/* TLCCMD.C - "The Last Cross-referencer" - Command line routines		*/
/*	Last Modified:	07/07/89											*/

/*
---------------------------------------------------------------------
Copyright (c) 1987-1989, Eduard Schwan Programs [esp] - All rights reserved
TLC (The Last C-Cross-Referencer) and TLP (same, but for Pascal) are
Cross-Reference Generators crafted and shot into the Public Domain by
Eduard Schwan.  The source code and executable program may be freely
distributed as long as the copyright/author notices remain intact, and
it is not used in part or whole as the basis of a commercial product.
Any comments, bug-fixes, or enhancements are welcome.
Also, if you find TLC and it's source code useful, a contribution of
$20 (check/money order) is encouraged!  Hopefully we will all see more
source code distributed!
	Eduard Schwan, 1112 Oceanic Drive, Encinitas, Calif. 92024
---------------------------------------------------------------------
*/

/*
HEADER:		The Last Cross-Referencer;
TITLE:		TLC/TLP - The Last Cross-Referencer;
VERSION:	1.01;

DESCRIPTION: "TLC/TLP.
			Command line routines";

KEYWORDS:	Utility, Cross-reference, C, Pascal, Apple, Macintosh, APW, Aztec;
SYSTEM:		Macintosh MPW, v3.0;
FILENAME:	TLCCMD.C;
WARNINGS:	"Has not yet been ported to MS-DOS.
			Shareware, $20 Check/Money Order suggested.";

SEE-ALSO:	README.TLC,TLCHELP.DOC,TLPHELP.DOC;
AUTHORS:	Eduard Schwan;
COMPILERS:	AZTEC C65 v3.2b, APPLEIIGS APW C v1.0, APPLE MACINTOSH MPW C v3.0;
*/


/*------------------------ include files ------------------------*/

#ifdef AppleIIgs
#include	<shell.h>	/* for filename wildcards */
#endif
#include	<stdio.h>
#include	<errno.h>
#include	"tlc.h"


/*--------------------- external declarations -------------------*/

#include	"tlc.ext"
extern	FILE*	open_text_file();
extern	VOID	close_text_file();
extern	VOID	init_wildcard();
extern	int 	next_wildcard();


/*----------------------- static variables ----------------------*/

static file_list_type* flist_tail = NULL;

#ifdef AppleIIgs
static char   wild_fname[64];
#endif

static char * help_array[] =
{
	"To run The Last CrossReferencer:",
	"   TLC [-f FnameListFile] [-p ParmFile] [-r ResWordFile] [-v]",
	"       [-d] FileName [FileNames..]",
	"where:",
	"   -d                ...Turns on debug messages sent to stderr.",
	"   -f FnameListFile  ...Optional file containing list of source",
	"                        file names to xref (one per line).",
	"   -p ParmFile       ...Optional file containing parameters for",
	"                        tailoring TLC output.",
	"   -r ResWordFile    ...Optional file containing reserved words",
	"                        not to xref.",
	"   -v                ...Turns on progress messages sent to stderr,",
	"                        two '-v's make it even more verbose.",
	"   FileName(s)       ...Source files to xref.",
	"",
	"*** TLC/TLP are shareware utilities ($20 suggested), written by:",
	"*** Eduard Schwan, 1112 Oceanic Drive, Encinitas, Calif. 92024",
	NULL,
};


/*=================[ show_help ]===================*/

static VOID show_help()

	{ /* show_help() */
	char ** help_ptr;

	/*
	start at first string in array of string pointers
	and print each string until a null pointer is found.
	*/
	help_ptr = help_array;
	while (*help_ptr)
		{
		puts(*help_ptr++);
		}
	} /* show_help() */


/*=================[ cmd_error ]===================*/

static VOID cmd_error(msg, argvp, error)
char	*msg;
char	*argvp;
int 	error;

	{ /* cmd_error */
	fprintf(stderr, "Error ");
	if (error != 0)
		fprintf(stderr, "# %d/$%x ", error, error);
	fprintf(stderr, "handling CmdLineArg '%s'\n", argvp);
	fprintf(stderr, "%s\n", msg);
	} /* cmd_error */


/*=================[ add_fname ]===================*/

static VOID add_fname(fname)
char	*fname;

	{ /* add_fname */

	if (verbose > 1)
		fprintf(stderr,"add file # %d:'%s'\n",
				file_rec.num_in_fnames+1,fname);
	if (file_rec.file_list==NULL)
		{ /* first on the list */
/*debug(printf(" added first on list\n");)*/
		file_rec.file_list = (file_list_type*)malloc(sizeof(file_list_type));
		if (file_rec.file_list==NULL)
			{
			fprintf(stderr,"Out of memory adding file # %d:'%s'!\n",
					file_rec.num_in_fnames+1,fname);
			exit(BAD_EXIT);
			}
		flist_tail				= file_rec.file_list;
		flist_tail->file_num	= file_rec.num_in_fnames+1;
		flist_tail->num_lines	= 0;
		flist_tail->next		= NULL;
		}
	else
		{ /* add to end of existing list */
/*debug(printf(" added next on list\n");)*/
		flist_tail->next = (file_list_type*)malloc(sizeof(file_list_type));
		if (flist_tail->next==NULL)
			{
			fprintf(stderr,"Out of memory adding file # %d:'%s'!\n",
					file_rec.num_in_fnames+1,fname);
			exit(BAD_EXIT);
			}
		flist_tail				= flist_tail->next;
		flist_tail->file_num	= file_rec.num_in_fnames+1;
		flist_tail->num_lines	= 0;
		flist_tail->next		= NULL;
		}
	flist_tail->file_name = malloc(strlen(fname)+1);
	if (flist_tail->file_name==NULL)
		{
		fprintf(stderr,"Out of memory adding file # %d:'%s'!\n",
				file_rec.num_in_fnames+1,fname);
		exit(BAD_EXIT);
		}
	strcpy(flist_tail->file_name, fname);
	file_rec.num_in_fnames++;
	} /* add_fname */


/*=================[ read_in_fnames ]===================*/

static VOID read_in_fnames(fp, fname)
FILE	*fp;
char	*fname;

	{ /* read_in_fnames() */
	pos_int line_num;

	if (verbose)
		fprintf(stderr,"Process filenames in '%s'\n",fname);
	line_num = 0;
	while (!feof(fp))
		{
		errno = 0;
		fgets(curr_line, LINE_SIZE, fp);
		line_num++;
/*debug(printf("read_in_fnames:%2d:'%s'",line_num,curr_line);)*/
		if (!feof(fp) && errno != 0)
			{
			cmd_error("Error reading line in -f file",
						fname, errno);
			exit(BAD_EXIT);
			}
		if (strlen(curr_line) > 1 && !feof(fp))
			if (curr_line[0]!=CH_SPACE && curr_line[0]!=CH_COMMENT)
			   {
			   curr_line[strlen(curr_line)-1] = 0; /* zap the <CR> */
			   if (strlen(curr_line) < FNAME_SIZE)
					{
#ifdef AppleIIgs
					init_wildcard(curr_line);
					while (next_wildcard(wild_fname))
						add_fname(wild_fname);
#else
					add_fname(curr_line);
#endif
					}
			   else
				   {
				   fprintf(stderr,
						"FileName too long on line %d of file '%s'\n",
						line_num, fname);
				   exit(BAD_EXIT);
				   }
			   }
		} /*while*/
	} /* read_in_fnames() */


/*=================[ handle_cmd_line ]===================*/

VOID handle_cmd_line(argc,argv)
int 	argc;
char	**argv;

	{ /* handle_cmd_line() */
	FILE	*fp;
	int 	error;

debug(printf("handle_cmd_line:#args=%d\n",argc);)
	/*
	initialize file list tail pointer
	*/
	flist_tail = NULL;

	/*
	handle xref run with no arguments
	*/
	if (argc == 1)
		{
		show_help();
		exit(BAD_EXIT);
		}

	/*
	loop through & handle each command line argument
	*/
	while (--argc)
		{
		/*
		go to next command line parameter (the 1st is the progname)
		*/
		argv++;
		switch (tolower((*argv)[0]))
			{
			case '-':
				switch (tolower((*argv)[1]))
					{

					case 'd': /* turn on debug messages */
						enable_debugs = TRUE;
						break;

					case 'f': /* read file list file */
						if (argc < 2)
							{
							cmd_error("Expected filename after -f",
										*argv, 0);
							exit(BAD_EXIT);
							}
						else
							{ /* open & read fname list file */
							argv++;
							argc--;
							fp=open_text_file(*argv,"r",&error);
							if (error)
								{
								cmd_error("Cannot open -f file",
											*argv, error);
								exit(BAD_EXIT);
								}
							else
								{
								read_in_fnames(fp, *argv);
								close_text_file(fp, *argv);
								}
							} /* else open & read fname list file */
						break;

					case 'p': /* change PARMFILE name */
						if (argc < 2)
							{
							cmd_error("Expected filename after -p",
										*argv, 0);
							exit(BAD_EXIT);
							}
						else
							{
							argv++;
							argc--;
							if (strlen(*argv)+1 > FNAME_SIZE)
								{
								cmd_error("-p filename too long",
											*argv, 0);
								exit(BAD_EXIT);
								}
							else
								{
								strcpy(file_rec.parm_fname, *argv);
								if (verbose > 1)
									fprintf(stderr,
									"set ParmFname to '%s'\n",
									file_rec.parm_fname);
								}
							}
						break;

					case 'r': /* change RESWORDFILE name */
						if (argc < 2)
							{
							cmd_error("Expected filename after -r",
										*argv, 0);
							exit(BAD_EXIT);
							}
						else
							{
							argv++;
							argc--;
							if (strlen(*argv)+1 > FNAME_SIZE)
								{
								cmd_error("-r filename too long",
											*argv, 0);
								exit(BAD_EXIT);
								}
							else
								{
								strcpy(file_rec.resw_fname, *argv);
								if (verbose > 1)
									fprintf(stderr,
									"Set ResWordFname to '%s'\n",
									file_rec.resw_fname);
								}
							}
						break;

					case 'v': /* VERBOSE option on */
						verbose++;
						if (verbose)
							fprintf(stderr, "Verbose option on at level %d\n",verbose);
						break;

					default:
						cmd_error("Unknown option, type TLC ? for help",
									*argv, 0);
						break;
					}; /*switch*/
					break;

				case '?': /* help for the neophyte */
					show_help();
					exit(GOOD_EXIT);
					break; /* never gets here (exit above).. */

				default: /* assume file name */
#ifdef AppleIIgs
					init_wildcard(*argv);
					while (next_wildcard(wild_fname))
						add_fname(wild_fname);
#else
					add_fname(*argv);
#endif
					break;
			}; /*switch*/
		} /*while*/
	} /* handle_cmd_line() */
