#include "config.h"
#include "timer1.h"

extern double t_minsam;
FILE *fp;
int i;
char buf[BUFSIZ];
static char buf100[] = "1   2   3   4   5   6   7   8   9   10  \
11  12  13  14  15  16  17  18  19  20  21  22  23  24  25 \n";

t_minsam = 2e6;	/* get at least 2 sec for each sample */

fp = fopen("100kc", "r");
if (fp == NULL)
	{
	fp = fopen("100kc", "w");
	for (i = 0; i < 1000; ++i)
		fprintf(fp, buf100);
	}
fclose(fp);

DO_STMT("fopen, fclose")
	{
	fp = fopen("tmpforw", "w");
	fclose(fp);
	}
OD

fp = fopen("100kc", "r");
DO_STMT("fgetc")
	{
	if (fgetc(fp) == EOF)
		{
		fclose(fp);
		fp = fopen("100kc", "r");
		}
	}
OD
fclose(fp);

fp = fopen("100kc", "r");
DO_STMT("fgets-100chars")
	{
	if (fgets(buf, 100, fp) == NULL)
		{
		fclose(fp);
		fp = fopen("100kc", "r");
		}
	}
OD
fclose(fp);

fp = fopen(JUNKFILE, "w");
DO_STMT("fprintf-%d")
	{
	fprintf(fp, "%d\n", 12345);
	}
OD
fclose(fp);

fp = fopen(JUNKFILE, "w");
DO_STMT("fprintf-%d %d")
	{
	fprintf(fp, "%d %d\n", 12345, 12345);
	}
OD
fclose(fp);

fp = fopen(JUNKFILE, "w");
DO_STMT("fputc")
	{
	fputc('\n', fp);
	}
OD
fclose(fp);

fp = fopen(JUNKFILE, "w");
DO_STMT("fputs-100 chars")
	{
	fputs(buf100, fp);
	}
OD
fclose(fp);

fp = fopen("100kc", "r");
DO_STMT("fread-100chars")
	{
	if (fread(buf, 1, 100, fp) < 100)
		{
		fclose(fp);
		fp = fopen("100kc", "r");
		}
	}
OD
fclose(fp);

fp = fopen("100kc", "r");
DO_STMT("fread-BUFSIZ chars")
	{
	if (fread(buf, 1, BUFSIZ, fp) < BUFSIZ)
		{
		fclose(fp);
		fp = fopen("100kc", "r");
		}
	}
OD
fclose(fp);

fp = fopen("100kc", "r");
DO_STMT("fscanf-1 int")
	{
	if (fscanf(fp, "%d", &i) < 1)
		{
		fclose(fp);
		fp = fopen("100kc", "r");
		}
	}
OD
fclose(fp);

fp = fopen("100kc", "r");
DO_STMT("fscanf-2 ints")
	{
	if (fscanf(fp, "%d%d", &i, &i) < 2)
		{
		fclose(fp);
		fp = fopen("100kc", "r");
		}
	}
OD
fclose(fp);

fp = fopen("100kc", "r");
DO_STMT("fseek, fgetc")
	{
	fseek(fp, (long)(i = (i + 1000) % 30000), 0);
	fgetc(fp);
	}
OD
fclose(fp);

fp = fopen(JUNKFILE, "w");
DO_STMT("fwrite-100 chars")
	{
	fwrite(buf100, 1, 100, fp);
	}
OD
fclose(fp);

fp = fopen(JUNKFILE, "w");
DO_STMT("fwrite-BUFSIZ chars")
	{
	fwrite(buf100, 1, BUFSIZ, fp);
	}
OD
fclose(fp);

DO_STMT("sprintf-1 int")
	{
	sprintf(buf, "%d", 12345);
	}
OD

DO_STMT("sprintf-2 ints")
	{
	sprintf(buf, "%d%d", 12345, 12345);
	}
OD

DO_STMT("sscanf-1 int")
	{
	sscanf("12345", "%d", &i);
	}
OD

DO_STMT("sscanf-2 ints")
	{
	sscanf("12345 12345", "%d %d", &i, &i);
	}
OD

}
