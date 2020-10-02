/* UNIX(TM) version	*/
/* vi: set ai : */
#include <stdio.h>

#define	SECTSIZE	512

int	wrsct(strtsect, sectbuf)
int	strtsect;
char	*sectbuf;
{
static int	filopen = 0;
static FILE	*fp;

if(!filopen)
	if((fp = fopen("/dev/fd048", "w")) == NULL)
		{
		fprintf(stderr, "error opening floppy drive!\n");
		exit(1);
		/*NOTREACHED*/
		}
	else
		filopen = 1;
fseek(fp, (long) SECTSIZE * strtsect, 0);
return((fwrite(sectbuf, sizeof(char), SECTSIZE, fp) == SECTSIZE) ? 0 : 1);
}
