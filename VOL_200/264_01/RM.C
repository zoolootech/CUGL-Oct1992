/*
 * rm - remove files and directories
 *
 * Usage: rm [-fiv] [-] file...
 *        rm [-fiv] -r [-] file|dir...
 *
 * Flags:
 * -    interpret following arguments starting with '-' as filenames
 * -f   force; don't report errors, override readonly attribute
 * -i   interactive; asks whether to remove each file or directory
 * -r   recursively remove arguments which are directories
 * -v   verbose; display name of each file as it's removed
 *
 * This program is in the public domain.
 * David MacKenzie
 * 6522 Elgin Lane
 * Bethesda, MD 20817
 *
 * Latest revision: 05/10/88
 */

#include <stdio.h>
#include <stat.h>
#include <ctype.h>
#ifndef tolower(c)
#define tolower(c) _tolower(c)
#endif
#include "getdir.h"

#define MAXPATH 125		/* Arbitrary internal limit on path lengths. */

int     fflag = 0;		/* Force success? */
int     iflag = 0;		/* Interactive (query before removing)? */
int     rflag = 0;		/* Remove subdirectories recursively? */
int     vflag = 0;		/* Verbose (display names as removed)? */

struct stat statbuf;

_main(argc, argv)
    int     argc;
    char  **argv;
{
    void    usage(), rm();
    int     optind;		/* Loop index. */

    for (optind = 1; optind < argc && *argv[optind] == '-'; ++optind) {
	if (argv[optind][1] == 0)
	    break;
	else
	    while (*++argv[optind])
		switch (*argv[optind]) {
		case 'f':
		    fflag = 1;
		    break;
		case 'i':
		    iflag = 1;
		    break;
		case 'r':
		    rflag = 1;
		    break;
		case 'v':
		    vflag = 1;
		    break;
		default:
		    usage();
		    break;
		}
    }

    if (optind == argc)
	usage();

    for (; optind < argc; ++optind)
	rm(argv[optind]);

    exit(0);
}

/*
 * Remove file or directory specified by path.
 */
void
rm(path)
    char   *path;
{
    void    rrmdir(), rmfile();
    char   *strtolower();

    (void) strtolower(path);
    if (vflag)
	printf("%s\n", path);

    if (isroot(path) || isdot(path)) {
	fprintf(stderr, "rm: Cannot remove %s\n", path);
	return;
    }
    if (stat(path, &statbuf) == -1) {
	if (!fflag)
	    fprintf(stderr, "%s: File not found\n", path);
	return;
    }
    if (isdir(&statbuf)) {
	if (!rflag) {
	    if (!fflag)
		fprintf(stderr, "rm: %s directory\n", path);
	    return;
	}
	rrmdir(path);
    } else
	rmfile(path);
}

/*
 * Recursively remove a directory.
 * Only used when -r flag is given.
 */
void
rrmdir(path)
    char   *path;
{
    void    rmfile();
    char   *basename(), *concat();
    char    dirbuf[512];	/* Buffer for disk input. */
    struct ffblk *dir = (struct ffblk *) dirbuf;
    char    pathbuf[MAXPATH];	/* Path of globbed dir contents. */
    char   *tail;		/* Tail component of pathbuf. */
    int     i;			/* Loop control. */

    if (!permit(path, 1))
	return;
    /*
     * Save any leading drive and/or path in pathbuf because the MS-DOS
     * globbing routines return only the base of the filenames. 
     */
    (void) strcpy(pathbuf, concat(path, "*.*"));
    tail = basename(pathbuf);

    /* Must reset the disk transfer area because stat() uses it. */
    for (setdta(dirbuf), i = getfirst(pathbuf, FA_ALL); !i;
	setdta(dirbuf), i = getnext(pathbuf, FA_ALL)) {
	if (dir->ff_fname[0] != '.') {
	    (void) strcpy(tail, dir->ff_fname);
	    rm(pathbuf);
	}
    }
    if (rmdir(path) == -1)
	perr(path);
}

/*
 * Remove a file.
 */
void
rmfile(path)
    char   *path;
{
    if (!permit(path, 0) || !writable(path, &statbuf))
	return;
    if (unlink(path) == -1)
	perr(path);
}

/*
 * Determine if overwriting destination file is permissible.
 * Returns 1 if permission granted, 0 if not.
 */
permit(path, isdir)
    char   *path;
    int     isdir;
{
    int     r;			/* Response character. */
    int     c;			/* One character of input. */

    if (!iflag || fflag || !isatty(0))
	return 1;
    fprintf(stderr, isdir ? "remove directory %s? " : "remove %s? ", path);
    fflush(stderr);
    c = r = getchar();
    while (c != '\n' && c != EOF)
	c = getchar();
    return r == 'y' || r == 'Y';
}

/*
 * Return 1 if write permission is granted on the path, 0 otherwise.
 */
writable(path, statp)
    char   *path;
    struct stat *statp;
{
    int     r;			/* Response character. */
    int     c;			/* One character of input. */

    if (statp->st_attr & ST_RDONLY) {
	if (!fflag && isatty(0)) {
	    fprintf(stderr, "override protection for %s? ", path);
	    fflush(stderr);
	    c = r = getchar();
	    while (c != '\n' && c != EOF)
		c = getchar();
	    if (r != 'y' && r != 'Y')
		return 0;
	}
	return chmod(path, statp->st_attr & ~ST_RDONLY) != -1;
    }
    return 1;
}

/*
 * Convert a string to lowercase.  Return the argument.
 */
char   *
strtolower(s)
    char   *s;
{
    char   *t;

    for (t = s; *t; ++t)
	if (isascii(*t) && isupper(*t) && *t != '_')
	    *t = tolower(*t);
    return s;
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
 * Return true if s is an existing directory, false otherwise.
 */
isdir(statp)
    struct stat *statp;
{
    return (statp->st_attr & ST_DIRECT) != 0;
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
	printf("cp: filename too long\n");
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

perr(s)
    char   *s;
{
    if (!fflag)
	perror(s);
}

void
usage()
{
    fprintf(stderr, "Usage: rm [-fiv] [-] file...\n");
    fprintf(stderr, "       rm [-fiv] -r [-] file|dir...\n");
    exit(1);
}
