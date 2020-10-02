/*
 * ls - list contents of directory
 *
 * Usage: ls [-alrstxzAR1] [path...]
 *
 * Flags:
 * -a   list all files, including hidden and system files, ".", and ".."
 * -l   long listing form (extra information)
 * -r   reverse order of sorting
 * -s   display size of each file in kilobytes, and total for each directory
 * -t   sort by time/date (latest first)
 * -x	sort by extension
 * -z	sort by size
 * -A   list all files except "." and ".."
 * -R	list subdirectories recursively
 * -1	display 1 entry per line of short form
 *
 * This program is in the public domain.
 * David MacKenzie
 * 6522 Elgin Lane
 * Bethesda, MD 20817
 *
 * Latest revision: 05/07/88
 */

#include <ctype.h>
#ifndef tolower
#define tolower(s) _tolower(s)
#endif
#include "getdir.h"

/* Arbitrary internal limit on filename path length. */
#define MAXPATH 125

/* For dynamically allocating space to store contents of each directory. */
#define ENTRIES_INCR 25

/*
 * isdir(e)
 *     struct sablk *e;
 */
#define isdir(e) ((e)->sa_attrib & FA_DIREC)

/*
 * kbytes(b)
 *     int b;
 * Macro to return the number of kilobytes (rounded up) taken by a given
 * number of bytes.
 */
#define kbytes(b) ((b) / 1024 + ((b) % 1024 != 0))

/* Shortened format for array in memory, to save space. */
struct sablk {
    char    sa_attrib;
    unsigned sa_ftime;
    unsigned sa_fdate;
    unsigned long sa_fsize;
    char   *sa_fname;
};

/*
 * Linked list of names of directories to list recursively (automatically
 * initialized to NULL).
 */
struct dirs {
    char   *dirs_name;
    struct dirs *dirs_next;
}      *dirlist;

char    dirbuf[512];		/* Buffer for disk input. */

char   *monthtab[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

int     aflag = 0;		/* Display . and .. */
int     lflag = 0;		/* Long listing form. */
int     rflag = 0;		/* Reverse order of sort. */
int     sflag = 0;		/* Display file sizes. */
int     tflag = 0;		/* Sort by time/date. */
int     xflag = 0;		/* Sort by extension. */
int     zflag = 0;		/* Sort by size. */
int     Aflag = 0;		/* Display hidden and system files. */
int     Rflag = 0;		/* Display subdirectories recursively. */
int     oneflag = 0;		/* One entry per line in short listing. */

int     listingargs;		/* Boolean for sort comparison function. */

char   *
Malloc(), *Realloc();

main(argc, argv)
    int     argc;
    char  **argv;
{
    void    listfiles(), listdir();
    int     entcmp();
    struct sablk *initarray(), *saveent();
    char   *basename(), *index();
    struct dirs *dirp;
    struct ffblk *dir = (struct ffblk *) dirbuf;
    struct sablk *save_array;	/* Array of dir entries for sorting. */
    int     last_entry = -1;	/* Last valid entry of save_array. */
    int     last_normal;	/* Last non-directory entry. */
    char    pathbuf[MAXPATH];	/* Path of globbed arguments. */
    char   *tail;		/* Tail component of pathbuf. */
    int     optind;		/* Loop index. */

    for (optind = 1; optind < argc && *argv[optind] == '-'; ++optind) {
	while (*++argv[optind]) {
	    switch (*argv[optind]) {
	    case 'a':
		aflag = Aflag = 1;
		break;
	    case 'l':
		lflag = 1;
		break;
	    case 'r':
		rflag = 1;
		break;
	    case 's':
		sflag = 1;
		break;
	    case 't':
		tflag = 1;
		break;
	    case 'x':
		xflag = 1;
		break;
	    case 'z':
		zflag = 1;
		break;
	    case 'A':
		Aflag = 1;
		break;
	    case 'R':
		Rflag = 1;
		break;
	    case '1':
		oneflag = 1;
		break;
	    default:
		printf("Usage: ls [-alrstxzAR1] [path...]\n");
		exit(1);
		break;
	    }
	}
    }

    setdta(dirbuf);		/* Set disk transfer area. */

    if (optind == argc)
	argv[argc++] = ".";

    save_array = initarray();
    for (; optind < argc; ++optind) {
	/*
	 * Normally, when we say getfirst("dir") or getnext("dir"), where
	 * "dir" is the name of a directory, they return "dir" back to us
	 * along with its attributes, by which we discover that it has the
	 * FA_DIREC attribute.  However, for certain pseudo-directories
	 * (which are real, regular directories on Unix), if we say, for
	 * instance, getfirst("\"), we get nothing back - no matches. To work
	 * around that problem, we have this special test for the problem
	 * pseudo-directories. 
	 */
	if (isroot(argv[optind]) || isdot(argv[optind])) {
	    (void) strcpy(dir->ff_fname, argv[optind]);
	    dir->ff_attrib = FA_DIREC;
	    dir->ff_ftime = dir->ff_fdate = 0;
	    dir->ff_fsize = 0L;
	    save_array = saveent(save_array, dir, ++last_entry);
	} else if (!getfirst(argv[optind], FA_ALL)) {
	    /*
	     * Copy any leading drive and/or path into pathbuf because the
	     * MS-DOS globbing routines return only the base of the
	     * filenames. 
	     */
	    (void) strcpy(pathbuf, argv[optind]);
	    tail = basename(pathbuf);

	    do {
		if (dir->ff_fname[0] != '.' &&
		    (Aflag ||
		    (dir->ff_attrib & (FA_HIDDEN | FA_SYSTEM)) == 0)) {
		    (void) strcpy(tail, dir->ff_fname);
		    (void) strcpy(dir->ff_fname, pathbuf);
		    save_array = saveent(save_array, dir, ++last_entry);
		}
	    } while (!getnext(argv[optind], FA_ALL));
	} else if (index(argv[optind], '*') || index(argv[optind], '?')) {
	    printf("No match.\n");
	    exit(1);
	}
    }
    listingargs = 1;
    qsort((char *) save_array, last_entry + 1, sizeof(struct sablk),
	entcmp);

    /* Find last non-directory entry. */
    for (last_normal = last_entry;
	last_normal >= 0 && isdir(&save_array[last_normal]);
	--last_normal);

    /* First the regular listing... */
    listfiles(save_array, last_normal);

    /* ...then the directories. */
    if (last_normal < last_entry) {
	if (last_normal++ >= 0)
	    putchar('\n');
	for (;;) {
	    listdir(save_array[last_normal].sa_fname, last_entry > 0);
	    /*
	     * We avoid a lot of memory waste by not using recursive function
	     * calls to implement recursive listing. 
	     */
	    while (dirlist) {
		/* Chop head of list off and show it. */
		dirp = dirlist;
		dirlist = dirp->dirs_next;
		putchar('\n');
		/* This call might add a new head to the list! */
		listdir(dirp->dirs_name, 1);
		free(dirp->dirs_name);
		free((char *) dirp);
	    }
	    if (++last_normal > last_entry)
		break;
	    putchar('\n');
	}
    }
    exit(0);
}

/*
 * Display contents of one directory.
 */
void
listdir(name, label)
    char   *name;		/* Name of directory to display. */
    int     label;		/* Diplay the directory's name:? */
{
    int     entcmp();
    void    listfiles(), addsubdir();
    char   *concat(), *savestr();
    struct sablk *save_array;
    int     last_entry;
    int     total_kbytes;
    int     i;			/* Loop index. */

    total_kbytes = getdir(name, &save_array, &last_entry);

    if (label)
	printf("%s:\n", name);
    if (lflag || sflag)
	printf("total %d\n", total_kbytes);
    if (last_entry == -1)
	return;

    listingargs = 0;
    qsort((char *) save_array, last_entry + 1, sizeof(struct sablk),
	entcmp);
    listfiles(save_array, last_entry);

    if (Rflag)
	for (i = last_entry; i >= 0; --i)
	    if (isdir(&save_array[i]) && save_array[i].sa_fname[0] != '.')
		/* Add entry to list of subdirectories. */
		addsubdir(concat(name, save_array[i].sa_fname));
    for (i = 0; i <= last_entry; ++i)
	free(save_array[i].sa_fname);
    free(save_array);
}

/*
 * Read contents of directory into array and return size in kilobytes.
 */
getdir(name, psave_array, plast_entry)
    char   *name;
    struct sablk **psave_array;
    int    *plast_entry;
{
    char   *concat();
    struct sablk *initarray(), *saveent();
    struct ffblk *dir = (struct ffblk *) dirbuf;
    struct sablk *save_array;	/* Array of dir entries for sorting. */
    int     last_entry = -1;	/* Last valid entry of save_array. */
    int     total_kbytes = 0;	/* Ignored if sflag and lflag not given. */
    char   *nametmp;
    int     i;			/* Temporary. */

    save_array = initarray();
    nametmp = concat(name, "*.*");

    for (i = getfirst(nametmp, FA_ALL); !i; i = getnext(nametmp, FA_ALL))
	if ((aflag || dir->ff_fname[0] != '.') &&
	    (Aflag || (dir->ff_attrib & (FA_HIDDEN | FA_SYSTEM)) == 0)) {
	    save_array = saveent(save_array, dir, ++last_entry);
	    if (sflag || lflag)
		total_kbytes += kbytes(dir->ff_fsize);
	}
    if (last_entry == -1) {
	(void) free(save_array);
	*psave_array = (void *) 0;
    } else
	*psave_array = save_array;
    *plast_entry = last_entry;
    return total_kbytes;
}

void
listfiles(save_array, last_entry)
    struct sablk *save_array;
    int     last_entry;
{
    if (last_entry < 0)
	return;

    if (lflag)
	longlist(save_array, last_entry);
    else
	shortlist(save_array, last_entry);
}

/*
 * List files in verbose format.
 */
longlist(save_array, last_entry)
    struct sablk *save_array;
    int     last_entry;
{
    int     i;

    for (i = 0; i <= last_entry; ++i)
	longent(&save_array[i]);
}

/*
 * List files in table format.
 */
shortlist(save_array, last_entry)
    struct sablk *save_array;
    int     last_entry;
{
    int     width = 0;		/* Length of longest filename. */
    int     columns = 0;	/* # of columns to show. */
    int     lines = 0;		/* # of rows to show. */
    int     w;			/* Temporary. */
    int     x;			/* Temporary. */
    int     i;			/* Loop index. */
    int     j;			/* Loop index. */

    if (oneflag)
	columns = 1;
    else {
	/*
	 * Find the length of the longest filename.  Add one character for
	 * the '\' at the ends of directories. 
	 */
	for (i = 0; i <= last_entry; ++i)
	    if ((w = strlen(save_array[i].sa_fname) +
		    (isdir(&save_array[i]) != 0)) > width)
		width = w;
	/* Round off to next tab stop and adjust for kbytes field. */
	width = (width & ~7) + 8 + sflag * 5;
	if (width > 80)
	    columns = 1;
	else
	    columns = 80 / width;
    }

    lines = (last_entry + columns) / columns;
    for (i = 0; i < lines; ++i) {
	for (j = 0; j < columns; ++j) {
	    x = j * lines + i;
	    shortent(&save_array[x]);

	    if (x + lines > last_entry) {
		putchar('\n');
		break;
	    }
	    w = strlen(save_array[x].sa_fname) + sflag * 5 +
		(isdir(&save_array[x]) != 0);
	    while (w < width) {
		w = (w & ~7) + 8;
		putchar('\t');
	    }
	}
    }
}

/*
 * Long listing of one file.
 */
longent(ent)
    struct sablk *ent;
{
    int     month;

    if (sflag)
	printf("%4d ", kbytes(ent->sa_fsize));

    month = ((ent->sa_fdate >> 5) & 15) - 1;
    if (month < 0 || month > 11)
	month = 11;

    /* Aztec C scrambles the minutes if this is 1 printf. */
    printf("%c%c%c%c%c %9lu %s %02u 19%02u ",
	ent->sa_attrib & FA_DIREC  ? 'd' : '-',
	ent->sa_attrib & FA_SYSTEM ? 's' : '-',
	ent->sa_attrib & FA_HIDDEN ? 'h' : '-',
	ent->sa_attrib & FA_RDONLY ? 'r' : '-',
	ent->sa_attrib & FA_ARCH   ? 'a' : '-',
	ent->sa_fsize,
	monthtab[month],	/* Month. */
	ent->sa_fdate & 31,	/* Day. */
	((ent->sa_fdate >> 9) & 127) + 80);	/* Year. */
    printf("%2u:%02u %-13s\n",
	(ent->sa_ftime >> 11) & 31,	/* Hour. */
	(ent->sa_ftime >> 5) & 63,	/* Minute. */
	ent->sa_fname);
}

/*
 * Display short form of file.
 */
shortent(ent)
    struct sablk *ent;
{
    if (sflag)
	printf("%4d ", kbytes(ent->sa_fsize));

    printf("%s", ent->sa_fname);
    if (isdir(ent))
	putchar('\\');
}

struct sablk *
initarray()
{
    return (struct sablk *)
    Malloc(sizeof(struct sablk) * ENTRIES_INCR);
}

struct sablk *
saveent(save_array, dir, last_entry)
    struct sablk *save_array;
    struct ffblk *dir;
    int     last_entry;
{
    char   *normalize(), *savestr();

    if (last_entry > 0 && last_entry % ENTRIES_INCR == 0)
	save_array = (struct sablk *)
	    Realloc(save_array, sizeof(struct sablk) *
	    (last_entry + ENTRIES_INCR));

    save_array[last_entry].sa_attrib = dir->ff_attrib;
    save_array[last_entry].sa_ftime = dir->ff_ftime;
    save_array[last_entry].sa_fdate = dir->ff_fdate;
    save_array[last_entry].sa_fsize = dir->ff_fsize;
    save_array[last_entry].sa_fname = savestr(normalize(dir->ff_fname));
    return save_array;
}

/*
 * Return file stuck on to the end of dir.
 */
char   *
concat(dir, file)
    char   *dir, *file;
{
    static char catbuf[MAXPATH];
    int     i;

    if (strlen(dir) + strlen(file) + 2 > sizeof(catbuf)) {
	printf("ls: filename too long\n");
	exit(1);
    }
    if (!strcmp(dir, "") || !strcmp(dir, ".")) {
	(void) strcpy(catbuf, file);
    } else {
	(void) strcpy(catbuf, dir);
	i = strlen(dir) - 1;
	if (dir[i] != '\\' && dir[i] != ':' && file[0] != '\\')
	    (void) strcat(catbuf, "\\");
	strcat(catbuf, file);
    }
    return catbuf;
}

char   *
savestr(s)
    char   *s;
{
    char   *t;

    t = Malloc(strlen(s) + 1);
    (void) strcpy(t, s);
    return t;
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

/*
 * Comparison routine for qsort().
 */

entcmp(e1, e2)
    struct sablk *e1, *e2;
{
    char   *index();
    int     result;

    /* Command line argument directories are sorted to the end. */
    if (listingargs) {
	if (isdir(e1) && !isdir(e2))
	    return 1;
	else if (!isdir(e1) && isdir(e2))
	    return -1;
	/*
	 * Either both or neither are command line directories; do regular
	 * sort. 
	 */
    }
    if (zflag)
	result = e1->sa_fsize < e2->sa_fsize ? -1 :
	    e1->sa_fsize > e2->sa_fsize ? 1 :
	    strcmp(e1->sa_fname, e2->sa_fname);
    else if (tflag)
	result = e1->sa_fdate < e2->sa_fdate ? 1 :
	    e1->sa_fdate > e2->sa_fdate ? -1 :
	    e1->sa_ftime < e2->sa_ftime ? 1 :
	    e1->sa_ftime > e2->sa_ftime ? -1 :
	    strcmp(e1->sa_fname, e2->sa_fname);
    else if (xflag) {
	char   *ex1 = index(e1->sa_fname, '.');
	char   *ex2 = index(e2->sa_fname, '.');

	result = !ex1 && ex2 ? -1 :
	    ex1 && !ex2 ? 1 :
	    ex1 && ex2 ? strcmp(ex1, ex2) :
	    strcmp(e1->sa_fname, e2->sa_fname);
    } else
	result = strcmp(e1->sa_fname, e2->sa_fname);
    return rflag ? -result : result;
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
 * Return a pointer to the base of path p, e.g. with any drive
 * and leading path removed.
 */
char   *
basename(p)
    char   *p;
{
    char   *tail;

    for (tail = p; *p; ++p)
	if (*p == ':' || *p == '\\')
	    tail = p + 1;
    return tail;
}

char   *
Malloc(n)
    unsigned n;
{
    char   *malloc();
    char   *p;

    p = malloc(n);
    if (!p) {
	perror("malloc");
	exit(1);
    }
    return p;
}

char   *
Realloc(p, n)
    char   *p;
    unsigned n;
{
    char   *realloc();

    p = realloc(p, n);
    if (!p) {
	perror("realloc");
	exit(1);
    }
    return p;
}

/*
 * Add directory d to list of subdirectories to list recursively.
 */
void
addsubdir(d)
    char   *d;
{
    char   *savestr();
    struct dirs *dirp;

    /* Make the new entry the head of the list. */
    dirp = (struct dirs *) Malloc(sizeof(struct dirs));
    dirp->dirs_name = savestr(d);
    dirp->dirs_next = dirlist;
    dirlist = dirp;
}
