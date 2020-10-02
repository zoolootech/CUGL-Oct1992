/* UNIX(TM) version	*/
/* vi: set ai : */
#include <stdio.h>

#define	SECTSIZE	512

int	rdsct(strtsect, sectbuf)
int	strtsect;
char	*sectbuf;
{
static int	filopen = 0;
static FILE	*fp;

if(!filopen)
	if((fp = fopen("/dev/dsk/fd", "r")) == NULL)
		{
		perror("error opening floppy drive");
		exit(1);
		/*NOTREACHED*/
		}
	else
		filopen = 1;
fseek(fp, (long) SECTSIZE * strtsect, 0);
return((fread(sectbuf, sizeof(char), SECTSIZE, fp) == SECTSIZE) ? 0 : 1);
}
