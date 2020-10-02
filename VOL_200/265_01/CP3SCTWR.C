/*
HEADER:		;
TITLE:		cpio "dump" for CP/M+
VERSION:	

DESCRIPTION:	copies a cpio archive file to an 8" SSSD CPM disk in
		drive C

KEYWORDS:	cpio, archive
SYSTEM:		CP/M-80, V3.1
FILENAME:	CP3SCTWR.C
WARNINGS:	Requires Version 3.x of CP/M.  Overwrites disk in crive C,
		without checking contents.  Interleave incompatible with
		UNIX (TM)

SEE-ALSO:	SNAIDER.C
AUTHORS:	Tim Prince (modified from Gerhards PC-DOS version)
COMPILERS:	Eco-C, v2.48

 *	absolute sector write program
 *	-----------------------------
 *
 *	written by	Rainer Gerhards
 *			Petronellastr. 6
 *			D-5112 Baesweiler
 *			West Germany
**
**	CP/M+ version for Eco-C by
**			Tim Prince
**			4456 W. Maple Rd.
**			Birmingham MI 48010-1923
 */

#include <stdio.h>

#define DRIVEA	0
#define DRIVEC	2
#define SECTORS	26
#define	TRACKS	77
#define	MAXSECT	(TRACKS*SECTORS-1) /* maximum sectors accessible	*/
#define	SECTSIZE	128	/* OS sector size			*/

static char		sectbuf[SECTSIZE];	/* must be in DS!	*/

main(argc, argv)int argc;
char	*argv[];
{
FILE	*fp;
int	strtsect,i,numsect;

if(argc != 3)	{
	fprintf(stderr, "Usage: sectwr filenam start-sector\n");
	exit(1);
	}
if((fp = fopen(argv[1], "rb")) == NULL)	{
	fprintf(stderr, "Can't open %s", argv[1]);
	exit(1);
	}
if((strtsect= atoi(argv[2])) < 0 || strtsect > MAXSECT)	{
	fprintf(stderr, "start-sector wrong.\n");
	exit(1);
	}
for(numsect = 0 ; !feof(fp) ; ++numsect)	{
	for(i = 0 ; i < SECTSIZE ; ++i)
		sectbuf[i] = fgetc(fp);		/* ignore EOF	*/
	if(wrsct(strtsect, sectbuf))	{
		fprintf(stderr, "error writing source disk!\n");
		exit(2);
		}
	++strtsect;
	}
printf("%d sectors written.\n", numsect);
}

char **_bdos(); /* returns pointer or unsigned, depending on function */
struct {	/* CP/M+ bios call parameter block */
	char funct;
	char areg;
	unsigned bcreg;
	char *dereg;
	char *hlreg;	}pb;

char bios(fn,val) int fn,val;{
	pb.funct=fn;
	pb.bcreg=val; /* may be int or pointer */
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

int wrsct(strtsect,sectbuf) int strtsect; char *sectbuf;{
	int retval;
	bios(10,strtsect/SECTORS);	/* track */
	bios(11,sectran(strtsect%SECTORS,*seldsk(DRIVEC))); /* sector */
	bios(12,sectbuf);		/* buffer */
	retval=(bios(14,NULL)!=0);	/* write sector */
	(void)seldsk((int)*(char *)4);	/* reset to match last BDOS select */
	return retval;
}
