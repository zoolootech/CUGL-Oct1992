/* MS OS/2 (TM) version    written by Rainer Gerhards */
/* vi: set ai : */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#define  INCL_BASE
#include <os2.h>

#define	SECTSIZE	512

int	rdsct(strtsect, sectbuf)
int	strtsect;
char	*sectbuf;
{
static int	filopen = 0;
static int	fh;
int		actcode;
static long	ifsize = 100;
USHORT		ret;
ULONG		dummy;
unsigned	byterd;

if(!filopen)
	{
	ret = DosOpen("A:", &fh, &actcode, (long) ((void far *) &ifsize), 0,
		      0x01, 0x8090, 0l);
	if(ret != 0)
		{
		fprintf(stderr, "Can't open floppy A: - OS error %d!\n", ret);
		exit(1);
		/*NOTREACHED*/
		}
	else
		filopen = 1;
	}
ret = DosChgFilePtr(fh, (long) SECTSIZE * strtsect, 0, &dummy);
if(ret == 0)
	ret = DosRead(fh, sectbuf, SECTSIZE,  &byterd);
return(((ret == 0) && (byterd == SECTSIZE)) ? 0 : 1);
}
