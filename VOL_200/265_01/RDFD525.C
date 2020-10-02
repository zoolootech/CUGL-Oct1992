/*
*
*	rdfd525 - ReaD Floppy Disk, (5 & 1/4 inch).
*
*	This routine is used as an input filter when a floppy disk can
*	not be read directly.
*
*	This program attempts to read the entire disk, but it will block
*	if there are 5120 unread bytes in a pipeline. When a program
*	like cpio (1) detects its eof and terminates, this program will
*	receive a hangup signal and exit without reading the entire disk.
*
*	The routine will facilitate reading in the following situations:
*		a: The floppy has been formatted with double density
*		   (40 tracks) instead of quad density (80 tracks).
*		b: The floppy was written on a single-sided drive.
*		c: The floppy was written to use only eight of the nine
*		   available sectors on each track.
*
*	The program runs under UNIX V.
*	The hardware supports a format of nine, 512 byte sectors per 
*	track. IBM high capacity floppies, written 80 tracks of 15 
*	sectors, are not supported.
*	The diskette controller must support the track density specified.
*
*	The following options can be specified:
*		[-q] -Quad track density. Default is double density.
*		[-s] -Single sided. Default is double sided.
*		[-8] - Eight sectors per track. Default is nine.
*
*	The pathname of the floppy disk, eg. /dev/rfd0, may be
*	specified on the command line. Otherwise it is assumed to be
*	stdin. Extraneous garbage at the end of the command is an error.
*
*	Example: rdfd525 /dev/rfd0 > temp will place the contents
*	of a double-sided, IBM-compatible diskette in the floppy drive
*	specified by /dev/rfd0 into the file named temp.
*
*	NOTE: Since a full track of a 5 1/4" floppy is only 4608 bytes
*	it is probably safe to assume that there are no controllers
*	without full-track buffering. Thus, it is unlikely that an
*	interleave factor other than 1 exists for this medium. In any
*	case it is not supported.
*
* c 1988 John E Van Deusen, PO BOX 9387, BOISE ID 83707, (208) 343-1865.
*	All rights transfered to the C Users Group.
*/

#include <stdio.h>
#include <fcntl.h>

#define SECTOR_SIZE	512
#define CYLINDERS	40
#define SECTOR_COUNT	9

void exit();
long lseek();
void perror();

extern int errno;
extern char *optarg;
extern int optind;

main (argc, argv)
int argc;
char *argv[];
{
int	index, 
	head, 
	ier=0, 
	nread;
short	n_heads=2, 
	sectors=SECTOR_COUNT, 
	cylinders=CYLINDERS;
char	track[SECTOR_SIZE*SECTOR_COUNT];

while ((index = getopt(argc, argv, "qs8")) != EOF)
	switch (index) {
	case 'q': cylinders = CYLINDERS * 2;
	          break;
	case 's': n_heads = 1;
		  break;
	case '8': sectors = 8;
		  break;
	case '?': 	/* unknown */
	default : ier++;
		  break;
	};
if (ier)
	{ 
	(void) fprintf (stderr, "%s:usage [-q][-s][-8] [fl_dev]\n", argv[0]);
	exit(-1);
	}
switch (argc - optind) {
case 0: /* Input device not specified */
	break;
case 1: /* Input device specified */
	if (freopen(argv[optind], "r", stdin) == NULL)
		{
		(void) fprintf (stderr, "%s: error in opening %s for input. ",
					argv[0], argv[optind]);
		perror ("");
		exit (-1);
		}
	break;
default: (void) fprintf (stderr, "%s: extraneous argument(s). \n",argv[0]);
	 exit (-1);
	/* NOTREACHED */
	break;
}
for (index = 0; index < cylinders; index++) {
	for (head = 0; head < n_heads; head++) {
		if ((nread = read (0, track, (unsigned)sectors * SECTOR_SIZE)) == -1)
		{
		(void)fprintf (stderr, "%s: can't read track %d.", argv[0], index);
		perror ("");
		exit (errno);
		}
		if (nread != sectors * SECTOR_SIZE)
		{
		(void)fprintf (stderr, "%s: input device must be a floppy disk.\n", argv[0]);
		exit (-1);
		}
		if (sectors != SECTOR_COUNT)
		/* Skip the unused 9th sector */
			if (lseek (0, SECTOR_SIZE, 1) == -1)
			{
			(void) fprintf (stderr, "%s: can not seek track %d. ", argv[0], index+1);
			perror ("");
			exit (errno);
			}
		if ( (ier = write(1, track, (unsigned) nread)) != nread)
		{
		(void) fprintf (stderr, "%s: can not write track %d. ", argv[0], ier);
		if (ier == -1)
			perror("");
		else
			(void) fprintf (stderr, "Return count = %d unexpected.\n", argv[0], ier);
		exit (-1);
		}
	} /* for end */
	if (n_heads == 1)	/* skip the track for head 2 */
		if (lseek (0, sizeof(track), 1) == -1)
		{
		(void) fprintf (stderr, "%s: can not seek track %d.", argv[0], index + 1);
		perror("");
		exit (errno);
		}
	} /* for end */
} /* main end */
