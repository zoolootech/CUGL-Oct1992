/*
 *	absolute sector write program
 *	-----------------------------
 *
 *	written by	Rainer Gerhards
 *			Petronellastr. 6
 *			D-5112 Baesweiler
 *			West Germany
 */

#include <stdio.h>

#define	MAXSECT		720	/* maximum sectors accessible		*/
#define	SECTSIZE	512	/* OS sector size			*/

static char		sectbuf[SECTSIZE];	/* must be in DS!	*/

main(argc, argv)
char	*argv[];
{
FILE	*fp;
int	strtsect;
int	i;
int	numsect;

if(argc != 3)
	{
	fprintf(stderr, "Usage: sectwr filenam start-sector\n");
	exit(1);
	}
if((fp = fopen(argv[1], "rb")) == NULL)
	{
	fprintf(stderr, "Can't open %s", argv[1]);
	exit(1);
	}
strtsect = atoi(argv[2]);
if(strtsect < 0 || strtsect > MAXSECT)
	{
	fprintf(stderr, "start-sector wrong.\n");
	exit(1);
	}
for(numsect = 0 ; !feof(fp) ; ++numsect)
	{
	for(i = 0 ; i < SECTSIZE ; ++i)
		sectbuf[i] = fgetc(fp);		/* ignore EOF	*/
	if(wrsct(strtsect, sectbuf))
		{
		fprintf(stderr, "error writing source disk!\n");
		exit(2);
		}
	++strtsect;
	}
printf("%d sectors written.\n", numsect);
}
