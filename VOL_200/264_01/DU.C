/*
 * du - summarize disk usage
 *
 * Displays the number of kilobytes used by all files and, recursively,
 * directories within each specified directory or filename.
 *
 * Usage: du [-as] [path...]
 *
 * Flags:
 * -a   display an entry for all files
 * -s   display only the grand total
 *
 * Default action is to display an entry for each directory only.
 * Default path is ".".
 *
 * This program is in the public domain.
 * David MacKenzie
 * 6522 Elgin Lane
 * Bethesda, MD 20817
 *
 * Latest revision: 05/03/88
 */

#include <ctype.h>
#ifndef tolower
#define tolower(s) _tolower(s)
#endif
#include "getdir.h"

/* For dynamically allocating space to store contents of each directory. */
#define ENTRIES_INCR 25

/*
 * kbytes(b)
 *     int b;
 * Macro to return the number of kilobytes (rounded up) taken by a given
 * number of bytes.
 */
#define kbytes(b) ((b) / 1024 + ((b) % 1024 != 0))

/* Shortened format for array in memory, to save space */
struct sablk {
    char    sa_attrib;
    unsigned long sa_fsize;
    char    sa_fname[13];
};

char    dirbuf[512];		/* Buffer for disk input. */

int     allfiles = 0;		/* -a flag. */
int     summary = 0;		/* -s flag. */

char   *
malloc(), *realloc();

_main(argc, argv)
    int     argc;
    char  **argv;
{
    int     optind;		/* Loop index. */

    for (optind = 1; optind < argc && *argv[optind] == '-'; ++optind) {
	while (*++argv[optind]) {
	    switch (*argv[optind]) {
	    case 'a':
		allfiles = 1;
		break;
	    case 's':
		summary = 1;
		break;
	    default:
		printf("Usage: du [-as] [path...]\n");
		exit(1);
		break;
	    }
	}
    }

    if (optind == argc)
	(void) dudir(".", 1);
    else
	for (; optind < argc; ++optind)
	    (void) dudir(argv[optind], 1);

    exit(0);
}

/*
 * Calculate usage for one directory.  Calls itself recursively
 * on subdirectories.  Returns number of kilobytes taken by the files
 * and, recursively, directories in path.
 */

dudir(path, root_call)
    char   *path;		/* Can contain wildcards. */
    char    root_call;		/* Was this invocation from main()? */
{
    char   *basename();
    struct sablk *initarray(), *saveent();
    struct ffblk *dir = (struct ffblk *) dirbuf;
    struct sablk *save_array;	/* Array of dir entries. */
    int     last_entry = -1;	/* Last valid entry of save_array. */
    char    tmppath[125];	/* For tacking various things onto path. */
    char   *tail;		/* For isolating leading dirs in path. */
    int     kb;			/* Temporary for calculation. */
    int     total_kbytes = 0;	/* Size of this directory. */
    int     i;			/* Loop index. */

    /*
     * The following kludge is necessary because the MS-DOS globbing routines
     * (getfirst and getnext) don't recognize any of these directories on
     * their own. 
     */
    (void) strcpy(tmppath, path);
    if (isroot(tmppath)) {
	(void) strcat(tmppath, "*.*");
	kb = dudir(tmppath, 0);
	if (!summary || root_call)
	    printf("%d\t%s\n", kb, path);
	total_kbytes += kb;
    } else if (isdot(tmppath)) {
	(void) strcat(tmppath, "\\*.*");
	kb = dudir(tmppath, 0);
	if (!summary || root_call)
	    printf("%d\t%s\n", kb, path);
	total_kbytes += kb;
    } else {
	save_array = initarray();

        setdta(dirbuf);
        
	for (i = getfirst(tmppath, FA_ALL); !i; i = getnext(tmppath, FA_ALL))
	    save_array = saveent(save_array, dir, ++last_entry);

	/* Overwrite any last element of path (e.g. "*.*") with the name. */
	tail = basename(tmppath);

	for (i = 0; i <= last_entry; ++i) {
	    if ((save_array[i].sa_attrib & FA_DIREC)) {
		if (save_array[i].sa_fname[0] != '.') {
		    (void) strcpy(tail, save_array[i].sa_fname);
		    (void) strcat(tmppath, "\\*.*");
		    kb = dudir(tmppath, 0);
		    if (!summary || root_call) {
			*tail = 0;
			printf("%d\t%s%s\n", kb, tmppath,
			save_array[i].sa_fname);
		    }
		    total_kbytes += kb;
		}
	    } else {
		/* Normal file. */
		kb = kbytes(save_array[i].sa_fsize);
		if (allfiles) {
		    *tail = 0;
		    printf("%d\t%s%s\n", kb, tmppath, save_array[i].sa_fname);
		}
		total_kbytes += kb;
	    }
	}
	(void) free(save_array);
    }

    return total_kbytes;
}

struct sablk *
initarray()
{
    struct sablk *save_array;

    save_array = (struct sablk *)
	malloc(sizeof(struct sablk) * ENTRIES_INCR);
    if (!save_array) {
	perror("malloc");
	exit(1);
    }
    return save_array;
}

struct sablk *
saveent(save_array, dir, last_entry)
    struct sablk *save_array;
    struct ffblk *dir;
    int     last_entry;
{
    char   *normalize();

    if (last_entry > 0 && last_entry % ENTRIES_INCR == 0) {
	save_array = (struct sablk *)
	    realloc(save_array, sizeof(struct sablk) *
	    (last_entry + ENTRIES_INCR));
	if (!save_array) {
	    perror("realloc");
	    exit(1);
	}
    }
    save_array[last_entry].sa_attrib = dir->ff_attrib;
    save_array[last_entry].sa_fsize = dir->ff_fsize;
    (void) strcpy(save_array[last_entry].sa_fname, normalize(dir->ff_fname));
    return save_array;
}

/*
 * Return true if the path has the form "d:" or "\" or "d:\".
 */
isroot(p)
    char   *p;
{
    int     i = strlen(p);

    return i == 2 && p[1] == ':' ||
	!strcmp(p, "\\") ||
	i == 3 && !strcmp(p + 1, ":\\");
}

/*
 * Return true if the path has the form:
 * ".." or "d:.." or "." or "d:." or "\." or "d:\.".
 */
isdot(p)
    char   *p;
{
    int     i = strlen(p);

    return !strcmp(p, "..") ||
	i == 4 && !strcmp(p + 1, ":..") ||
	!strcmp(p, ".") ||
	i == 3 && !strcmp(p + 1, ":.") ||
	!strcmp(p, "\\.") ||
	i == 4 && !strcmp(p + 1, ":\\.");
}

/*
 * Convert uppercase letters to lowercase, and non-graphic characters to '?'.
 * Return the argument.
 */

char   *
normalize(s)
    char   *s;
{
    char   *t;

    for (t = s; *t; ++t)
	if (!isascii(*t) || !isgraph(*t))
	    *t = '?';
	else if (isupper(*t) && *t != '_')
	    /* Aztec C's ctype thinks that isupper('_') is true . . . */
	    *t = tolower(*t);
    return s;
}
