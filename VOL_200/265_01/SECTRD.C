#include <stdio.h>

#define	MAXSECT		720	/* maximum sectors accessible		*/
#define	SECTSIZE	512	/* OS sector size			*/
#define	EOF_VAL		0xff	/* End of archieve file on disk		*/

static unsigned char	sectbuf[SECTSIZE];	/* must be in DS!	*/

main(argc, argv)
char	*argv[];
{
FILE	*fp;
int	i;
int	strtsect;
int	numsect;

if(argc != 4)
	{
	fprintf(stderr, "Usage: sectrd filenam start-sector number-sectors\n");
	exit(1);
	}
if((fp = fopen(argv[1], "wb")) == NULL)
	{
	fprintf(stderr, "Can't open %s\n", argv[1]);
	exit(1);
	}
strtsect = atoi(argv[2]);
numsect = atoi(argv[3]);
if(strtsect < 0 || strtsect > MAXSECT)
	{
	fprintf(stderr, "start-sector wrong.\n");
	exit(1);
	}
if(numsect < 0 || (numsect + strtsect) > MAXSECT)
	{
	fprintf(stderr, "start-sector wrong.\n");
	exit(1);
	}
for( ; numsect ; ++strtsect, --numsect)
	{
	if(rdsct(strtsect, sectbuf))
		{
		fprintf(stderr, "error reading source disk!\n");
		exit(2);
		}
	for(i = 0 ; i < SECTSIZE ; ++i)
		if(sectbuf[i] == EOF_VAL)
			{
			printf("EOF detected, Byteoffset %d\n", i);
			break;
			}
		else
			fputc(sectbuf[i], fp);
	}
}
