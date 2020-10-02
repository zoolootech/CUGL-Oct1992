#include <stdio.h>
#include "config.h"
#include "timer1.h"

FILE *fp;
int fd;
int i;
char buf[BUFSIZ];
static char buf100[] = "1   2   3   4   5   6   7   8   9   10  \
11  12  13  14  15  16  17  18  19  20  21  22  23  24  25 \n";

fp = fopen("100kc", "r");
if (fp == NULL)
	{
	fp = fopen("100kc", "w");
	for (i = 0; i < 1000; ++i)
		fprintf(fp, buf100);
	}
fclose(fp);

fp = fopen("tmpforw", "w");
fclose(fp);
DO_STMT("open, close")
	{
	fd = open("tmpforw", 1);
	close(fd);
	}
OD

fd = open("100kc", 0);
DO_STMT("read-BUFSIZ chars")
	{
	if (read(fd, buf, BUFSIZ) < BUFSIZ)
		{
		lseek(fd, 0L, 0);
		}
	}
OD
close(fd);


fd = open("100kc", 0);
DO_STMT("lseek, read-BUFSIZ")
	{
	lseek(fd, (long)(i = (i + BUFSIZ) & 32767), 0);
	read(fd, buf, BUFSIZ);
	}
OD
close(fd);

fd = open(JUNKFILE, 1);
DO_STMT("write-BUFSIZ chars")
	{
	write(fd, buf, BUFSIZ);
	}
OD
close(fd);


}
