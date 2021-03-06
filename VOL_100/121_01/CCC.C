/*
HEADER: CUG 121.??;

    TITLE:	ccc - a command driver for BDS C;
    VERSION:	1.0;
    DATE:	01/11/86;
    DESCRIPTION: "Ccc is a command driver for BDS C.  It takes global flags,
		and one or more sets of local flags plus filenames.
		It builds a submit file to compile all the files with an
		extension of .c, and link the output with the remaining
		files, using the l2 linker.";
    KEYWORDS:	command, driver;
    SYSTEM:	CP/M;
    FILENAME:	CCC.C;
    WARNINGS:	"Copyright (c) 1981, Mike W. Meyer.
		The link step uses the L2 linker; this has the problem that
		you can't get to the L2 overlay stuff currently (the -m, -org,
		-ovl flags are passed to the compiler).
		By changing the appropriate #define, it is possible to use
		CLINK instead; however, this will probably make things
		worse, due to conflicts with the -o, -d, and -r flags.
		One minor problem is that flags to the C compiler must not
		have a space between themselves and their argument.";
    AUTHORS:	Mike W. Meyer;
    COMPILERS:	BDS-C 1.50;
*/

/*
 *	Examples:
 *		compile pencil & printer, externals at 6100, linking in others
 *			'ccc -e6100 pencil.c printer.c driver -l crayon queue'
 *
 *		same thing, done wrong:
 *			'ccc -e 6100 ' etc.
 */
#include <bdscio.h>

#define LINKER		"l2"
#define COMPILER	"cc"
#define SUBFILE		"cc.sub"

main(argc, argv) char **argv; {
	char *tap, subfile[BUFSIZ], temp[MAXLINE], submit ;
	char linkline[MAXLINE], comline[MAXLINE], comflags[MAXLINE] ;
	int in, defdisk ;

	if (argc < 2 || argv[1][0] == '?') {
		puts("usage: ccc [flags] file [flags] [file [flags]] [*]\n") ;
		exit(0) ;
		}
	unlink(SUBFILE) ;			/* a safety measure */
	if (fcreat(SUBFILE, subfile) == ERROR)
		barf("Can't creat the submit file!\n") ;
	defdisk = ERROR; submit = TRUE; *comline = *comflags = NULL ;
	strcpy(linkline, LINKER) ;
	while (tap = *++argv, --argc)
		if (*tap == '-')
			switch (tap[1]) {
				/* the compiler flags */
				case 'P': case 'A': case 'D': case 'M':
				case 'E': case 'O': case 'R': case 'S':
					strpad(*comline ? comline : comflags,
						tap) ;
					break ;
				/* the ccc flag */
				case 'N': submit = FALSE ;
					  break ;
				/* the linker flags */
				default: strpad(linkline, tap) ;
				}
		else {
			if (defdisk == ERROR)
				defdisk = tap[1] == ':' ? *tap : FALSE ;
			if (index(tap, ':') == ERROR && defdisk)
				sprintf(temp, "%c:%s",	defdisk, tap) ;
			else
				strcpy(temp, tap) ;
			if ((in = index(temp, '.')) != ERROR) {
				if (*comline)
					fprintf(subfile, "%s %s\n",
							comline, comflags) ;
				sprintf(comline, "%s %s", COMPILER, temp) ;
				temp[in] = NULL ;
				}
			strpad(linkline, temp) ;
			}
	if (*comline) fprintf(subfile, "%s %s\n", comline, comflags) ;
	fprintf(subfile, "%s\n", linkline) ;
	if (submit) fprintf(subfile, "era %s\n%c", SUBFILE, CPMEOF) ;
	else putc(CPMEOF, subfile) ;
	fflush(subfile) ;
	fclose(subfile) ;
	if (submit && execl("SUBMIT", "CC", 0))
		barf("Uh... Boss, I need a submit.\n") ;
	else printf("Your submit file is named %s.\n", SUBFILE) ;
	}

index(string, element) char *string, *element; {
	char *other ;

	other = string ;
	while (*other != element)
		if (!*other++) return ERROR ;
	return other - string ;
	}

strpad(dest, source) char *dest, *source; {

	while (*dest++)
		;
	dest[-1] = ' ' ;
	strcpy(dest, source) ;
	}
                             