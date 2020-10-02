#include <bdscio.h>
#include "bdscio+.h"

#define NIOBUFS 1
#include <fio1.h>

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
main(argc,argv)
	int argc;
	char **argv;
	{
	FILE *fp;
	char name[FILENAME], *pfcb;
	unsigned size;
	int i, extents;
#include <fio2.h>

	fp = 0;
	if (argc EQ 2) {
		if (badname(argv[1])) exit(0);
		fp = fopen(argv[1], "r");
		if (!fp) printf("Can't find %s.\n");
	}
	if (!fp) fp = goodfile("Filename? ", name);
	size = filesize(fp);
	extents = (size/128); /* 128 records/extent */
	if (size%128) extents++;
	printf("File size is %u records, thus %d extents.\n",
		size, extents);
	pfcb = fcbaddr(fp->_fd);
	for (i=0; i<extents; i++) {
		seek(fp->_fd, 128*i, 0);
		bdos(33, pfcb);
		showfcb(pfcb);
	}
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
showfcb(f)
	char *f; /* POINTER TO FCB */
	{
	int i, *b, mt;

		printf("  ");
		for(i=1; i <= 11; i++) {
			if (f[i] & 128) printf("!");
			else printf(" ");
		}
		printf("<-- HIGH BITS SET\n");

	printf("%d ",f[0]);
	for(i=1; i <= 11; i++)
		printf("%c",f[i]);
	printf(" Ex%d %d %d RC%3d ",f[12], f[13], f[14], f[15]);
	printf("B");
	b = &f[16];
	for (i=0; i <=7 ; i++)
		printf("%3d/",b[i]);
	printf(" %3d %3d %3d", f[33], f[34], f[35]);
	printf("\n");
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
filesize(fp)
	FILE *fp;
	{

/* FILE MUST BE OPEN FOR READING BEFORE CALLING filesize() */
/* filesize() returns the CP/M record count (128 bytes/record) */
/* variable receiving the record count should be unsigned int */

	char *fcb;
	unsigned *pu;

	fcb = fcbaddr(fp->_fd); /* GET POINTER TO FILE CONTROL BLOCK */
	bdos(35,fcb); /* INSTALL RECORD COUNT IN FILE CONTROL BLOCK */

	if (*(fcb+35) == 1) {
		printf("FATAL ERROR in filesize():\n");
		printf("RECORD COUNT IS 65536;\n");
		exit(0);
	}

	/* GET COUNT OF FILLED SECTORS WRITTEN = TOTAL SECTORS - 1 */
	pu = fcb+33;
	return(*pu);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#include <fio3.h>
i++)
		printf("%c",f[i]);
	