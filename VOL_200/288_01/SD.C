/*
HEADER:		;
TITLE:		Enhaced Chdir command;
VERSION:	1.2;

DESCRIPTION:	"

	SD (Set Directory): this is a replacement for the MS-DOS
	CD (Chdir: change directory) command.

	SD is upwardly compatible with CD, all options of the CD command
	have been implemented in SD. The only exception is that
	
		SD <drive:><path> 
		
	will switch to the specified drive.
	
	Extended options:	
	
	SD ? or SD /?   Gives a usage (help) message

	SD -            Returns to the last directory used (by SD)
			This makes use of the MS-DOS environment to
			keep the name of the last path in #SD_LAST.

	SD [<drive>:]path       This sets a new drive and path. If no
				drive is given the current drive is used.

	SD <name>       The file SD.DAT, in a directory specified by PATH
			is used to store a list of logical names, and the
			drive & path associated with the name.
			
			V1.1 Added ability to invoke a program after 
			moving to the directory.
			
			BDF Aug. 88
			
			eg:

			123     c:\123 123 
			pas     c:\dev\pas
			c       c:\dev\c

			Thus the command line:
			
				SD C
				
			Will move you to drive C, path \DEV\C. While
			
				SD 123
				
			Will move to drive C, path \123 and start the
			program 123.

	History:

	V1.1	Added abilty to start a program after moving to a named
		logical directory.

		BDF Aug. 1988

	V1.2	Stored the name of the last path used in the environment
		variable SDLAST. This was the original intention but I
		could not find a way to do it, so I used a temp. file to
		hold the name of the last path. However, a back issue of
		Micro Cornucopia (issue 42) I found in a shop, contained
		an articule by Scott Robert Ladd on how to do it (see
		GLOBENV.C). Thank you Scott.
			
		BDF Jan. 1989";

FILENAME:	SD.C;
WARNINGS:	;
SEE-ALSO:	;
AUTHORS:	Brian Farquhar (with thanks to Scott Robert Ladd);
COMPILERS:	Microsoft C v5.1 & QuickC;
			
*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <direct.h>
#include <process.h>
#include <dos.h>

#define TRUE		1			/* Logical true		*/
#define FALSE           0			/* Logical false	*/
#define MATCH		0			/* Match on strcmp()	*/
#define EOL		'\n'			/* End of line char	*/
#define MAX_LINE        255			/* Data file Max. line	*/
#define CDERROR         -1                      /* Error from chdir()   */
#define DATAFILE	"SD.DAT"		/* Logical names file	*/
#define LASTPATH	"SDLAST"		/* Last path 		*/
#define DELIMITERS	" \t"			/* Used in strtok()	*/

extern char *getgenv(char *varname);
extern int   putgenv(char *varname, char *vartext);
extern int   delgenv(char *varname);

/*	Local functions		*/

void help_msg(void);
void prt_cwd(void);
void set_path(char *path);
void save_path(void);
char *get_last(char *path);
void prt_path(char drive);
int  logical_name(char *name);
void search_path(char *file, char *path);

void	main(	int 	argc,
		char    **argv)
{
	char    path[_MAX_PATH];		/* directory path	*/
	char	*path_ptr;

	if (argc == 1)	/* asking for the current path? (SD <cr>)	*/
	{	
		prt_cwd();		/* print the current path	*/
		
		printf("\nLast path\t= %s",get_last(path));
		printf("\n\nUse the '/?' option for help.\n",argv[0]);
	}
	
	/* Asking for the path on another drive? (SD <drive>:)  */
	
	else if (strlen(argv[1]) == 2 && argv[1][1] == ':')
	{
		prt_path(argv[1][0]);		/* path on drive x	*/
		printf("\nLast path\t= %s\n",get_last(path));
	}
		
	/*      did he ask for help? (SD ? or SD /?)   */

	else if (argv[1][0] == '?'|| (strcmp(argv[1],"/?") == MATCH))

		help_msg();

	/*      go back to the last directory? (SD -)   */

	else if (strcmp(argv[1],"-") == MATCH)
	{	
		path_ptr	= get_last(path);
		save_path();
		set_path(path_ptr);
	}
	
	else		/* try to set the given path	*/
	{
		save_path();		/* save the current path	*/
		
		if (!logical_name(argv[1]))
			set_path(argv[1]);
	}

	exit(0);
}

/************************************************************************

	Test to see if the "name" is a logical name in the datafile.
If it is, return a value of TRUE after moving to the associated directory
path. Otherwise return FALSE.

V1.1	Added ability to invoke the program named in the data file.

************************************************************************/

int	logical_name(	char	*name)
{
	char	file[_MAX_PATH];	/* full file spec of DATAFILE	*/
	char	buff[MAX_LINE];		/* line from the DATAFILE	*/
	char    *token;
	FILE	*fp;			/* file pointer for DATAFILE	*/
	char	*prog;			/* program to chain to		*/
	int	c;
	int	i;
						
	search_path(DATAFILE,file);	/* look for the file on PATH	*/

	if (file[0] == NULL)		/* found it?			*/
	{
		printf("Error: can not find the data file '%s'.",DATAFILE);
		return(FALSE);
	}
	
	if ((fp = fopen(file,"r")) == NULL)	/* open the data file	*/
	{
		printf("\nError: unable to open the file '%s'.\n",file);
		return(FALSE);
	}

	i	= 0;
	
	while ((c = fgetc(fp)) != EOF)		/* read the data file	*/
	{
		if (i == MAX_LINE -1)		/* line too long?	*/
		{
			fprintf(stderr,"Maximum of %d characters per line"
					" in '%s'.",MAX_LINE,DATAFILE);
			c	= EOL;
		}
		
		if (c == EOL)
		{
			/*	Now we have a line from the data file,
				use the strtok() function to get the
				logical name on the line, and if it is
				the required name move to the directory
				given by the second token.
				
				V1.1 If a third token is on the line try
				and invoke it as a program.
			*/
			
			buff[i]	= '\0';
			i	= 0;
			token	= strtok(buff,DELIMITERS);

			if (strcmp(token,name) == MATCH) 
			{
				/* get the next token	*/
				
				token	= strtok(NULL,DELIMITERS);
				set_path(token);

				prog	= strtok(NULL,DELIMITERS);

				if (prog != NULL)
					system(prog);
					
				return(TRUE);
			}
		}
		else
		{
			buff[i]	= (char) c;
			i++;
		}
	}

	return(FALSE);
}			

/************************************************************************

	print a help (usage) message.

************************************************************************/

void	help_msg(void)
{
	printf( "\nSD: Set Directory, an enhanced Chdir command\n"
		"\nSD version 1.1:"
		"\tAdded ability to call a program\n"
		"\t\t\twhen moving to a named directory\n"
		"\nUsage:\tSD <CR>\t"
		"\tWith no parameters prints the current path\n"
		"\tSD <drive>:\tPrints the current path on the given drive\n"
		"\tSD /? \t\tPrints this help message\n"
		"\tSD - \t\tReturns to the last directory used (by SD)\n"
		"\tSD [d:]path\tSet the path to the given drive and path\n"
		"\tSD <name> \tMove to the directory in the file SD.DAT\n");
}

/************************************************************************

	Print the C(urrent) W(orking) D(irectory)

************************************************************************/

void	prt_cwd(void)
{
	char    path[_MAX_PATH];

	if (getcwd(path,_MAX_PATH) == NULL)
		perror("Get current working directory error");
	else
		printf("\nPath on %c:\t= %s",path[0],path);
}

/************************************************************************

	set the given drive (optional) & path

************************************************************************/

void	set_path(char *path)
{
	unsigned        curr_drive;
	unsigned        num_drives;
	unsigned	tmp,i;
	
	if (isalpha(path[0]) && path[1] == ':')		/* drive given?	*/
	{
		i	= toupper(path[0]) - 'A' + 1;	/* A=0, B=1...	*/
		tmp	= i;
	
		_dos_getdrive(&curr_drive);
		_dos_setdrive(tmp,&num_drives);
		
		/*      validate the drive      */
		
		_dos_getdrive(&tmp);
		
		if (tmp != i)
		{
			if (tmp != curr_drive)
				_dos_setdrive(curr_drive,&num_drives);
		
			exit(1);
		}
	}
	
	if (chdir(path) == CDERROR)
	{
		printf("Error changing to '%s'.\n",path);
		exit(-1);
	}	
}

/************************************************************************

	Save the current path.

************************************************************************/

void    save_path(void)
{
	char	path[_MAX_PATH];	/* current path to save		*/

	if (getcwd(path,_MAX_PATH) == NULL)
	{
		perror("Get current working directory error");
		exit(-1);
	}
	
	if (putgenv(LASTPATH,path) != 0)
	{
		printf("Error saving the path in the environment table.");
		exit(-1);
	}
}

/************************************************************************

	Get the last path from the LASTPATH environment variable.

************************************************************************/

char	*get_last(char *path)
{
	path	= getenv(LASTPATH);
	return(path);
}

/************************************************************************

	Get and display the path on another drive, and the last path

************************************************************************/

void	prt_path(char drive)
{
	void prt_cwd(void);

	unsigned	curr_drive;	/* current drive 		*/
	unsigned	tmp;		/* temp. drive			*/
	unsigned        num_drives;     /* highest drive No.            */
	unsigned	i;
	
	tmp     = toupper(drive) - 'A' + 1;
	i	= tmp;
	
	_dos_getdrive(&curr_drive);
	_dos_setdrive(tmp,&num_drives);

	/*      validate the drive      */

	_dos_getdrive(&tmp);

	if (tmp != i)
	{
		if (tmp != curr_drive)
			_dos_setdrive(curr_drive,&num_drives);

		exit(-1);
	}

	prt_cwd();      		/* Print path on current drive	*/
	
	_dos_setdrive(curr_drive,&num_drives);	/* re-set the old drive	*/
}

/************************************************************************

	There is a bug in the _searchenv function (I think). If the file
you are looking for is in the current working directory, and that is the
root for that drive, the funtion adds an extra "\".

e.g.	E:\\SDLAST.PTH not E:\SDLAST.PTH.

	This routine traps the bug.

************************************************************************/

void search_path(char 	*file,		/* the file to search for	*/
		 char	*path)		/* the path returned		*/
{
	int     i;

	_searchenv(file,"PATH",path);
	
	if (strlen(path) > 3)		/* check for the \\ error	*/
	{
		if (path[2] == '\\' && path[3] == '\\')
		{
			/* found the "\\" so shift everything 1 left	*/
			
			for (i = 3; i < strlen(path) - 1; i++)
				path[i] = path[i+1];

			path[i] = '\0';
		}
	}
}
