/*
HEADER:		;
TITLE:		copy cpio to file
VERSION:	

DESCRIPTION:	Copies 8" SSSD CPM cpio format disk on drive C to a file

KEYWORDS:	cpio, archive
SYSTEM:		CP/M-80, V3.1;
FILENAME:	CP3SCTRD.C;
WARNINGS:	Requires Version 3.x of CP/M.
		Tested only on unbanked system, but uses banked calls
		interleave not compatible with UNIX (TM) cpio

SEE-ALSO:	SNAIDER.C, GERHARDS.C
AUTHORS:	Tim Prince (adapted from CUG program)
COMPILERS:	Eco-C v2.48
*/

#include <stdio.h>

/* CP/M+ version recreated 16.VII.88 - TCP
**	Tim Prince
**	Chandler Software
**	4456 W. Maple Rd.
**	Birmingham MI 48010-1923 */
#define DRIVEA	0
#define DRIVEC	2
#define SECTORS	26	/* 8" SSSD 128 byte sectors */
#define TRACKS	77
#define	MAXSECT	(TRACKS*SECTORS-1) /* maximum sectors accessible	*/
#define	SECTSIZE	128	/* OS sector size			*/
#define	EOF_VAL		0xe5	/* Past End of archive file on disk	*/

static unsigned char	sectbuf[SECTSIZE];	/* must be in DS!	*/

main(argc, argv)int argc;
char	*argv[];
{
FILE	*fp;
int	i,strtsect,numsect;

if(argc != 4)	{
	fprintf(stderr, "Usage: sectrd filenam start-sector number-sectors\n");
	exit(1);
	}
if((fp = fopen(argv[1], "wb")) == NULL)	{
	fprintf(stderr, "Can't open %s\n", argv[1]);
	exit(1);
	}
if((strtsect= atoi(argv[2])) < 0 || strtsect > MAXSECT||
	(numsect= atoi(argv[3])) < 0 || (numsect + strtsect) > MAXSECT)	{
	fprintf(stderr, "start-sector wrong.\n");
	exit(1);
	}
for( ; numsect ; ++strtsect, --numsect)	{
	printf ("\n\nSector %d\n\n", strtsect);
	if(rdsct(strtsect, sectbuf))	{
		fprintf(stderr, "error reading source disk!\n");
		exit(2);
		}
	for(i = 0 ; i < SECTSIZE ; ++i)
		if(sectbuf[i] == EOF_VAL){
			printf("EOF detected, Byteoffset %d\n", i);
			goto CLOSE;
			}
		else	{
			fputc(sectbuf[i], fp);
			putchar(sectbuf[i]);
			}
	}
CLOSE:
fclose(fp);
}

char **_bdos(); /* may be int or pointer */
struct {	/* CP/M+ bios call parameter block */
	char funct;
	char areg;
	unsigned bcreg;
	char * dereg;
	char * hlreg;	}pb;

char bios(fn,bfr) int fn,bfr;{
	pb.funct=fn;
	pb.bcreg=bfr; /* may be int or (char *) */
	(void)_bdos(50,&pb);
	return pb.areg;}

char **seldsk(drive) int drive;{
	pb.funct=9;
	pb.bcreg=drive;
	return _bdos(50,&pb);}

int sectran(sec,trans) unsigned sec; char *trans;{
	pb.funct=16;
	pb.bcreg=sec;
	pb.dereg=trans;
	return (int)_bdos(50,&pb);}

int rdsct(strtsect,sectbuf) int strtsect; char *sectbuf;{
	int retval;
	bios(10,strtsect/SECTORS);	/* track */
	bios(11,sectran(strtsect%SECTORS,*seldsk(DRIVEC))); /* sector */
	bios(12,sectbuf);		/* buffer */
	retval=(bios(13,NULL)!=0);	/* read sector */
	(void)seldsk((int)*(char *)4);	/* drive last selected by BDOS */
	return retval;
}
