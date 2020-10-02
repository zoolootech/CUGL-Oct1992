/*
 * chmod - change file modes
 * Usage: chmod +|-ahrs file...
 *
 * Attributes that can be added (+) or removed (-):
 * a    archive
 * h    hidden
 * r    read only
 * s    system
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

_main(argc, argv)
    int     argc;
    char  **argv;
{
    int     optind;		/* Index into argv. */
    int     errs = 0;		/* # of files couldn't chmod. */
    int     mask;		/* Modes to change. */

    if (argc < 3)
	usage();

    mask = parseattr(argv[1]);

    for (optind = 2; optind < argc; ++optind)
	errs += chmodfile(argv[optind], mask, *argv[1]);

    exit(errs);
}

/*
 * Perform the operation on the modes of the file.
 * Return 0 if ok, 1 if error.
 */
chmodfile(file, mode, op)
    char   *file;
    int     mode;		/* Mode(s) to change in stat format. */
    char    op;			/* Operation, + or -. */
{
    struct stat sbuf;

    if (stat(file, &sbuf) == -1) {
	fprintf(stderr, "%s: File not found\n", file);
	return 1;
    }
    if (op == '+')
	sbuf.st_attr |= mode;
    else
	sbuf.st_attr &= ~mode;
    if (chmod(file, sbuf.st_attr) == -1) {
	perror(file);
	return 1;
    }
    return 0;
}

/*
 * Return a stat-compatible mask for the attributes specified in the
 * string.  Also, exit if the string is not a valid attribute specifier.
*/
parseattr(s)
    char   *s;
{
    int     mask = 0;

    if (s[0] != '+' && s[0] != '-' || s[1] == 0)
	usage();

    for (++s; *s; ++s)
	switch (*s) {
	case 'r':
	    mask |= ST_RDONLY;
	    break;
	case 'h':
	    mask |= ST_HIDDEN;
	    break;
	case 's':
	    mask |= ST_SYSTEM;
	    break;
	case 'a':
	    mask |= ST_ARCHIV;
	    break;
	default:
	    usage();
	}
    return mask;
}

usage()
{
    fprintf(stderr, "Usage: chmod +|-ahrs file...\n");
    exit(1);
}
