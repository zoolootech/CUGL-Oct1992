/* fmttbl -  Sam Yanuck
 * Accumulate space and time data for sample codes, each run
 * recorded in a file.
 */
#include <stdio.h>
#define TIMEF "time.tbl"
#define SPACEF "space.tbl"
char fprefix[20] = "";
char foutname[64] = "";

main(ac, av)
	int ac;
	char *av[];
	{
	short i, nskip, j;
	short lastrow;
	static char space[5][100][7] = {0};
	static char time[5][100][13] = {0};
	static char sample[100][21] = {0};
	FILE *fpin, *fpout;

	if (strcmp(av[1], "-p") == 0)
		{
		strcpy(fprefix, av[2]);
		nskip = 2;
		}
	else
		nskip = 0;
	for (i = 1; i + nskip < ac; ++i)
		{
		fpin = fopen(av[i+nskip], "r");		/* open input files sequentially */
#if 0
printf("fname=<%s>\n", av[i+nskip]);
#endif
		if (fpin == NULL) 			     
			error("can't open", av[i+nskip]);
 
		while (getc(fpin) != '\n')	   /* bypass input of col. headings */
			;						    
		for (j = 0;
			0 < fscanf(fpin,
			"%20c%*2c%6c%*2c%12c", sample[j], space[i-1][j], time[i-1][j]);
			++j)
			{
#if 0
printf("sample=<%20s> space=<%6s> time=<%12s>\n",
	sample[j], space[i-1][j], time[i-1][j]);
#endif
			while (getc(fpin) != '\n')	 /* bypass remaining whitespace */
				;						    
			}
		lastrow = j-1;						/* set array boundary */
		fclose(fpin);						
		}
 
	strcpy(foutname, fprefix);
	strcat(foutname, TIMEF);
	fpout = fopen(foutname, "w");		/* print sample code, then */
	if (fpout == NULL) 				/* time from each input file */
		error("can't open (write)", foutname);  
	for (j = 0; j <= lastrow; ++j)
		{
		fprintf(fpout, "%20s  ", sample[j]);
		for (i = 1; i < ac-nskip; ++i)
			fprintf(fpout, "%10.10s ", time[i-1][j] + 2);
		fprintf(fpout, "\n");
		}
	fclose(fpout);

	strcpy(foutname, fprefix);
	strcat(foutname, SPACEF);
	fpout = fopen(foutname, "w");		/* same structure as above */
	if (fpout == NULL)               
		error("can't open", foutname);
	for (j = 0; j <= lastrow; ++j)
		{
		fprintf(fpout, "%20s  ", sample[j]);
		for (i = 1; i < ac-nskip; ++i)
			fprintf(fpout, "%10.10s ", space[i-1][j]);
		fprintf(fpout, "\n");
		}
	fclose(fpout);
	
	}
