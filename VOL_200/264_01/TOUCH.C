/*
 * touch - set time of files to current time
 *
 * Usage: touch [-c] file...
 * Exit status is # of files that couldn't be touched.
 *
 * Flags:
 * -c	don't try to create file if it doesn't already exist
 *
 * This program is in the public domain.
 * David MacKenzie
 * 6522 Elgin Lane
 * Bethesda, MD 20817
 *
 * Latest revision: 04/23/88
 */

#include <time.h>
#include <fcntl.h>

long    timep[2];		/* two elements containing current time */

_main(argc, argv)
    int     argc;
    char  **argv;
{
    void    usage();
    int     trycreat = 1;	/* try to create nonexistant file? */
    int     optind;
    int     errs = 0;		/* # of files we couldn't touch */

    (void) time(&timep[0]);
    timep[1] = timep[0];

    for (optind = 1; optind < argc && *argv[optind] == '-'; ++optind)
	while (*++argv[optind])
	    if (*argv[optind] == 'c')
		trycreat = 0;
	    else
		usage();

    if (optind == argc)
	usage();

    for (; optind < argc; ++optind)
	errs += touch(argv[optind], trycreat);

    exit(errs);
}

/*
 * Update the time of file.
 * Return 0 if ok, 1 if error.
 */

touch(file, trycreat)
    char   *file;
    int     trycreat;
{
    int     fd;

    if (utime(file, timep) == -1 &&
	(!trycreat || (fd = open(file, O_CREAT)) == -1)) {
	perror(file);
	return 1;
    }
    (void) close(fd);
    return 0;
}

void
usage()
{
    printf("Usage: touch [-c] file...\n");
    exit(1);
}
