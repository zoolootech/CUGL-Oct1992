/*	absolute sector write routine	*/
/*	----------------------------	*/
/* AT&T 3B1 (UNIX PC, 7300) &	 	*/
/* Convergent Techologies S-50		*/
/* version ORG. may have been 		*/
/*	  written by Rainer Gerhards	*/
/*		     Petronellastr. 6	*/
/*		     D-5112 Baesweiler	*/
/*			West Germany	*/
/* MOD. for 3B1 by			*/
/*               T. W. (Bill) Kalebaugh	*/
/*		     Rt 2 Box 3		*/
/*		     Easton, Kansas	*/
/*				66020	*/
/*	Tel. # (913) 773-5732		*/
/*	----------------------------	*/

#include <stdio.h>
#include <sys/gdisk.h>
#include <sys/gdioctl.h>
#include <fcntl.h>

static	struct	gdctl	gdbuf;
struct	gdctl	*gddesc = &gdbuf;

char	*flpy = "/dev/rfp020";


#define SECTSIZE	512

int wrsct(strtsect, sectbuf)
int strtsect;
char	*sectbuf;
{
static int filopen = 0;
static int fd;
int  i=0;
	

if(!filopen)
	fd = open(flpy,O_RDWR);
	if ( fd < 0 ) {
		perror("An error occurred opening floppy drive.\n");
		exit(1);
	}
	else
		filopen = 1;
	gddesc->params.cyls = 40;
	gddesc->params.heads = 2;
	gddesc->params.psectrk = 8;
	gddesc->params.pseccyl = 16;
	gddesc->params.flags = 0;
	gddesc->params.step = 0;
	gddesc->params.sectorsz = 512;
	gddesc->dsktyp = 2;
	if ( ioctl(fd,GDSETA,gddesc) == -1 ) {
	fprintf(stderr, "An error occurred setting the floppy drive.\n");
		exit(1);
	}
lseek(fd, (long) SECTSIZE * strtsect, 0);
return((write(fd, sectbuf, SECTSIZE) == SECTSIZE) ? 0 : 1);
}
