/*
	****************************************************
			FPUT Copy Utility
	****************************************************

	W. Lemiszki				 24 Oct 1983

	Filename: arglist.c			 BDS C v1.50

	Copies a list of files into a destination user/drive.
 */

#include <bdscio.h>			/* Standard Definitions */

char dstdrive[20];			/* destination user/dr:		*/
char *memory;				/* ptr to free memory 		*/
unsigned msects;			/* size of memory buffer	*/



/*
 *	M A I N
 *	-------
 */
main(argc, argv)
int argc;  char *argv[];
{
	char lbuf[MAXLINE];			/* argument line buffer	*/
	char *gets();

	puts("FPUT  v2.1\n\n");
	if (argc < 3)
		{
		puts("\7Usage: fput [u/]d: <ufn> @<ufn> ...\n");
		exit();
		}

	if (get_drive(argv[1]) == ERROR)	/* scan dest. drive */
		{
		puts("\7Bad user/drive: argument.\n");
		exit();
		}

	memory = sbrk(128);			/* get 1st chunk of memory */
	msects = 1;
	while (sbrk(128) != ERROR)		/* allocate it all */
		++msects;

	proclist(argc-2, argv+2);		/* process the cmd args */
}




/*
 *	Process a list of args
 *	----------------------
 */
proclist(argc, argv)
int argc;  char *argv[];
{
	int i;

	for (i=0; i<argc; ++i)			/* do each arg */
		if (argv[i][0] == '@')
			procfile(++argv[i]);	/* get from file */
		    else
			fput(argv[i]);		/* do it */
}



/*
 *	Process arguments from a file
 *	-----------------------------
 */
procfile(arg)
char *arg;
{
	char fname[20], iobuf[BUFSIZ], lbuf[MAXLINE];

	if (arg[0] == EOS)			/* default ? */
		strcpy(fname, "FILES.CAT");
	    else
		strcpy(fname, arg);
	if (index(fname, '.') == NULL)		/* if no extention */
		strcat(fname, ".CAT");

	putchar('@');				/* echo name */
	putlj(fname, 17);

	if (fopen(fname, iobuf) == ERROR)
		return err(" does not exist.");
	putchar('\n');

	msects -= 10;				/* leave stk space for iobuf */
	while (fgets(lbuf, iobuf) != NULL)	/* until EOF */
		procline(lbuf);
	msects += 10;				/* restore memory */

	putchar('\n');
	fclose(iobuf);
}



/*
 *	Process a line of arguments
 *	---------------------------
 */
procline(line)
char *line;
{
	int argc;  char *argv[20];
	char *ptr;

	for (ptr=line;  *ptr = toupper(*ptr);  ++ptr)
		if (*ptr == ';'  ||  *ptr == '\n')	/* comment ? */
			break;
	*ptr = EOS;					/* terminate */
	argc = parseargs(line, argv);
	proclist(argc, argv);
}


/* Parse a string up into args */
int parseargs(str, argv)
char *str, *argv[];
{
	int argc;
	char d1, d2;			/* delimiters */
	char c;

	argc = 0;
	do
		{
		while ((c = *str) == ' '  ||  c == '\t')
			++str;				/* skip whitespace */

		if (c == '"'  ||  c == '\'')		/* quoted string ? */
			{
			d1 = d2 = c;
			++str;				/* skip quote */
			}
		    else
			{
			d1 = ' ';
			d2 = '\t';
			}

		if (c)					/* != EOS */
			argv[argc++] = str;		/* save ptr */

		while ((c = *str)  &&  c != d1  &&  c != d2)
			++str;			/* scan til delim or EOS */

		*str++ = EOS;			/* terminate arg */
		} while (c);			/* scan entire string */

	return argc;
}




/* Print an error msg */
int err(msg)
char *msg;
{
	puts(msg);
	puts("\7\n");		/* terminate and beep */
	return ERROR;
}


/* Put a left justified string */
putlj(str, width)
char *str;
int width;
{
	while (width--)
		if (*str)
			putchar(*str++);
		    else
			putchar(' ');
}


/* Check for wildcard characters */
haswild(arg)
char *arg;
{
	return ((index(arg, '?') || index(arg,'*'))  ?  ERROR  :  OK);
}


/* Find a char in a string */
char *index(str, c)
char *str, c;
{
	while (*str)
		{
		if (c == *str)
			return (str);
		++str;
		}
	return NULL;
}




/*
 *	Scan the destination user/drive
 *	-------------------------------
 *	(Only looks at last char.)
 */
int get_drive(arg)
char *arg;
{
	char c;

	strcpy(dstdrive, arg);			/* move into buffer */
	if ((c = arg[strlen(arg) - 1]) == '/'  ||  (c == ':'))
		return OK;
	return ERROR;
}




/*
 *	Copy a file to 'dstdrive'
 *	------------------------
 */
fput(srcname)
char *srcname;				/* source filename */
{
	char dstname[20];			/* dst filename buf */
	char *ptr;

	putlj(srcname, 18);			/* print the arg */
	if (haswild(srcname))
		return (err("is ambiguous."));

	ptr = srcname;				/* make dstname from srcname */
	if (index(ptr, '/'))			/* != NULL */
		ptr = index(ptr, '/');		/* drop user number */
	if (index(ptr, ':'))			/* != NULL */
		ptr = index(ptr, ':');		/* drop drive spec */

	strcpy(dstname, dstdrive);		/* init with drive spec. */
	strcat(dstname, ptr);			/* add base filename */
	fcopy(srcname, dstname);		/* copy the file */
}




/*
 *	Copy 'srcname' to 'dstname'
 *	---------------------------
 *	Uses a temporary output file, and renames only after
 *	successful completion.
 */
int fcopy(srcname, dstname)
char *srcname, *dstname;
{
	char tmpname[20];			/* temporary output filename */
	int srcfd, dstfd;			/* file descriptors */

	maketemp(dstname, tmpname);		/* generate temp filename */

	if ((srcfd = open(srcname, 0)) == ERROR)
		return (err("does not exist."));

	if ((dstfd = creat(tmpname)) == ERROR)
		{
		close(srcfd);
		return (err("opened, but can't create temporary."));
		}

	if (fcpy(srcfd, dstfd) == ERROR)	/* move the data */
		{
		close(srcfd);
		fabort(dstfd);
		return ERROR;
		}

	close(srcfd);
	close(dstfd);
	unlink(dstname);
	if (rename(tmpname, dstname) == ERROR)
		{
		unlink(tmpname);
		return (err("copied, but can't rename temporary."));
		}

	puts("copied to ");
	puts(dstname);
	putchar('\n');
	return (OK);
}




/* Make a temporary filename */
maketemp(name, buf)
char *name;				/* base name */
char *buf;				/* destination buffer */
{
	char *index();

	strcpy(buf, name);			/* name to buffer */
	if (index(buf, '.'))			/* != NULL */
		*index(buf, '.') = EOS;		/* drop the extension */
	strcat(buf, ".$F$");			/* add temp ext */
}



/* Transfer Data */
int fcpy(srcfd, dstfd)
int srcfd, dstfd;
{
	int n;

	while (n = read(srcfd, memory, msects))		/* 0 is EOF */
		{
		if (n == ERROR)
			return (err("contains unreadable data."));
		if (write(dstfd, memory, n) != n)
			return (err("ok, but error writing. (disk full?)"));
		}
	return (OK);
}


/*EOF*/
