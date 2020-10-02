/*
 * mv - move files and directories
 *
 * Usage: mv [-fiv] [-] file file
 *        mv [-fiv] [-] file... dir
 *        mv [-fiv] [-] dir dir
 * Destination can be a drive, a directory, or a filename.
 * Makes only minimal effort to stop you from destroying a file
 * by moving it onto itself.  To move directory trees recursively, use
 * cp -r and rm -r.
 *
 * Flags:
 * -    interpret following arguments starting with '-' as filenames
 * -f   force; don't report errors, override readonly attribute
 * -i   interactive; asks whether to overwrite existing files
 * -v   verbose; display name of each file as it's moved
 *
 * This program is in the public domain.
 * David MacKenzie
 * 6522 Elgin Lane
 * Bethesda, MD 20817
 *
 * Latest revision: 05/08/88
 */

#include <stdio.h>
#include <stat.h>
#include <fcntl.h>
#include <ctype.h>
#ifndef tolower(c)
#define tolower(c) _tolower(c)
#endif

/* Values for the first argument of ftime(). */
#define FT_GET 0		/* Return date/time. */
#define FT_SET 1		/* Set date/time to third argument. */

#define COPYSIZE (1024 * 16)	/* Number of bytes to copy at once. */

#define MAXPATH 125		/* Arbitrary internal limit on path lengths. */

int     fflag = 0;		/* Force success? */
int     iflag = 0;		/* Interactive (query before copying)? */
int     vflag = 0;		/* Verbose (display names as copied)? */

struct stat ssbuf, dsbuf, tsbuf;/* Source, destination, temp. */

_main(argc, argv)
    int     argc;
    char  **argv;
{
    void    usage(), mvdir(), mvfile();
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
		case 'v':
		    vflag = 1;
		    break;
		default:
		    usage();
		    break;
		}
    }

    if (optind == argc || optind == argc - 1)
	usage();

    /* Check type of first source argument. */
    if (dstat(argv[optind], &ssbuf) == 0 && isdir(&ssbuf)) {
	/* Can only move one directory onto one new directory. */
	if (optind != argc - 2) {
	    if (!fflag)
		fprintf(stderr, "%s: Directory rename only\n", argv[optind]);
	    exit(0);
	}
	mvdir(argv[optind], argv[argc - 1]);
    } else {
	if (dstat(argv[argc - 1], &dsbuf) == 0 && !isdir(&dsbuf) &&
	    optind != argc - 2)
	    /* Can't move multiple files onto one file. */
	    usage();
	for (; optind < argc - 1; ++optind)
	    mvfile(argv[optind], argv[argc - 1]);
    }

    exit(0);
}

/*
 * Rename a directory.
 */
void
mvdir(source, dest)
    char   *source;
    char   *dest;
{
    if (prep(source, dest))
	return;
    if (rename(source, dest) == -1)
	/* Problem could be with either source or dest. */
	perr("mv");
}

/*
 * Move a file.
 * Source is a file, dest is either a file (existing or new) or a directory
 * (which must exist).
 */
void
mvfile(source, dest)
    char   *source;
    char   *dest;
{
    long    ftime();
    char   *basename(), *concat(), *Malloc();
    struct stat *statp;
    char   *buf;		/* Copy buffer. */
    int     in, out;		/* File descriptors. */
    int     n;			/* Number of bytes read. */
    int     cperr = 0;		/* Error during copy? */

    if (prep(source, dest))
	return;

    if (dstat(source, &ssbuf) == -1) {
	if (!fflag)
	    fprintf(stderr, "%s: File not found\n", source);
	return;
    }
    if (isdir(&ssbuf)) {
	if (!fflag)
	    fprintf(stderr, "%s: Directory rename only\n", source);
	return;
    }
    statp = &dsbuf;
    if (isdir(&dsbuf)) {
	dest = concat(dest, basename(source));
	if (dstat(dest, &tsbuf) == 0 && isdir(&tsbuf)) {
	    if (!fflag)
		fprintf(stderr, "%s: Is a directory\n", dest);
	    return;
	}
	statp = &tsbuf;
    }
    if (!permit(dest, statp) || !writable(dest, statp))
	return;

    /* Try the simple way first. */
    if (rename(source, dest) != -1)
	return;

    if ((in = open(source, O_RDONLY)) == -1) {
	perr(source);
	return;
    }
    if ((out = open(dest, O_WRONLY | O_CREAT | O_TRUNC)) == -1) {
	perr(dest);
	(void) close(in);
	return;
    }
    buf = Malloc(COPYSIZE);
    while ((n = read(in, buf, COPYSIZE)) > 0)
	if (write(out, buf, n) != n)
	    break;

    (void) free(buf);
    /* Preserve readonly and other modes. */
    if (chmod(dest, ssbuf.st_attr) == -1)
	perr("chmod");
    /* Set date and time of dest to those of source. */
    if (ftime(FT_SET, out, ssbuf.st_mtime))
	perr("ftime");

    if (close(in) == -1 || n < 0) {
	if (!fflag)
	    fprintf(stderr, "%s: Read error\n", source);
	cperr = 1;
    }
    if (close(out) == -1 || n > 0) {
	if (!fflag)
	    fprintf(stderr, "%s: Write error\n", dest);
	cperr = 1;
    }
    if (cperr) {
	(void) unlink(dest);
	return;
    }
    if (fflag && (ssbuf.st_attr & ST_RDONLY))
	(void) chmod(source, ssbuf.st_attr & ~ST_RDONLY);
    if (unlink(source) == -1)
	perr("unlink");
}

/*
 * Common startup for both mvdir and mvfile.
 * Return 0 if ok, 1 if error.
 */
prep(source, dest)
    char   *source, *dest;
{
    char   *strtolower();

    (void) strtolower(source);
    (void) strtolower(dest);
    if (vflag)
	printf("%s\n", source);

    if (!strcmp(source, dest)) {
	if (!fflag)
	    fprintf(stderr, "%s and %s are identical\n", source, dest);
	return 1;
    }
    return 0;
}

/*
 * Determine if overwriting destination file is permissible.
 * Returns 1 if permission granted, 0 if not.
 */
permit(path, statp)
    char   *path;
    struct stat *statp;
{
    int     r;			/* Response character. */
    int     c;			/* One character of input. */

    if (!iflag || fflag)
	return 1;
    /* If it doesn't exist, permission granted. */
    if (statp->st_size == -1L || !isatty(0))
	return 1;
    fprintf(stderr, "overwrite %s? ", path);
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

    /* If it doesn't exist, it's writable. */
    if (statp->st_size == -1L)
	return 1;
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
 * Like stat() but recognizes pseudo-directories:
 * stat() can't find \, ., .., or \., so we check for them manually
 * (\.. doesn't exist under MSDOS). 
 * Sets statp->st_size to -1L if path not found.
 * Returns: 0 = ok, -1 = path not found.
 */
dstat(path, statp)
    char   *path;
    struct stat *statp;
{
    if (isroot(path) || isdot(path)) {
	statp->st_attr = ST_DIRECT;
	return 0;
    } else if (stat(path, statp) == -1) {
	statp->st_size = -1L;
	return -1;
    } else
	return 0;
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
	printf("mv: filename too long\n");
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
Malloc(n)
    unsigned n;
{
    char   *malloc();
    char   *p;

    p = malloc(n);
    if (!p) {
	perr("malloc");
	exit(1);
    }
    return p;
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
    fprintf(stderr, "Usage: mv [-fiv] [-] file file\n");
    fprintf(stderr, "       mv [-fiv] [-] file... dir\n");
    fprintf(stderr, "       mv [-fiv] [-] dir dir\n");
    exit(1);
}
