/* ---------------------------------------------------	*/
/*	PRINT - Print a file with header to the printer */
/*		M. Burton	04 July 1983		*/
/*	Written in Computer Innovations C86		*/
/* ---------------------------------------------------	*/
/*		Syntax: 				*/
/*							*/
/*	PRINT filename.typ - Print a file with line	*/
/*				numbers.		*/
/*	PRINT filename.typ /N - Print a file without	*/
/*				line numbers.		*/
/* ---------------------------------------------------	*/

#include "stdio.h"

struct regval		/* Register structure for INT	*/
{
	int ax,bx,cx,dx,si,di,ds,es;
}
struct regval srv, rrv, trv, brv;
int mo, dy, yr;
int hr, mn, sc, hn;

main(argc,argv)
	int argc;	/* Number of args in cmd line	*/
	char *argv[];	/* Args in cmd line		*/
{
	int *fd;	/* File stream pointer		*/
	int *cd;	/* Printer stream pointer	*/
	char *sp;	/* filename.typ pointer 	*/
	int c;		/* Loop variable		*/
	int lineno=1;	/* Line counter 		*/
	int pline=1;	/* Line on page counter 	*/
	int page=1;	/* Page counter 		*/
	char s[232];	/* Line buffer			*/

	srv.ax = (0x2c << 8);
	sysint(0x21,&srv,&brv); /* Get the time 	*/

	if (argc == 1)
	{
		printf("Proper syntax: PRINT filename.typ </N>\007\n");
		goto abort;
	}

	sp = argv[1];
	while ((*sp = toupper(*sp)) != EOS) sp++;
	fd = fopen(argv[1],"r");
	if (fd == 0)
	{
		printf("%s not found\007\n",argv[1]);
		goto abort;
	}

	cd = fopen("PRN:","w");
	if (cd == 0)
	{
		printf("Printer offline\007\n");
		goto abort;
	}

	c = 1;
	prthdr(page,cd,argv[1]);
	while (c != EOS)
	{
		pline = 1;
		while (pline < 57)
		{
			c = fgets(s,232,fd);
			if (c == EOS) goto quit;
			if (argc == 3)
			{
				if (strncmp(argv[2],"/n",2) == 0 || strncmp(argv[2],"/N",2) == 0)
				{
					fprintf(cd,"%s",s);
				}
			}
			else
			{
				fprintf(cd,"%7u  %s",lineno,s);
			}
			lineno++;
			pline++;
		}
		page++;
		fprintf(cd,"\n\014");
		prthdr(page,cd,argv[1]);
	}
quit:
	fprintf(cd,"  \n\014");
	srv.ax = (0x2c << 8);
	sysint(0x21,&srv,&trv); 	/* Get end time */
	hr = (trv.cx >> 8) - (brv.cx >> 8);
	if((mn = (trv.cx & 0xff) - (brv.cx & 0xff)) < 0)
	{
		hr--;
		mn = mn + 60;
	}
	if((sc = (trv.dx >> 8) - (brv.dx >> 8)) < 0)
	{
		mn--;
		sc = sc + 60;
		if (mn < 0)
		{
			hr--;
			mn = mn + 60;
		}
	}
	printf("Finished printing '%s';",argv[1]);
	printf("%5u lines,%5u pages.\n",--lineno,page);
	printf("Total print time %02u:%02u:%02u\n",hr,mn,sc);
	fclose(fd);
	fclose(cd);
abort:	;		/* Dummy line			*/
}

prthdr(p,cc,aa) 	/* Print the page header	*/
	int p, cc, *aa;
{
	srv.ax = (0x2a << 8);
	sysint(0x21,&srv,&rrv);
	mo = (rrv.dx >> 8);
	dy = (rrv.dx & 0xff);
	yr = rrv.cx;
	srv.ax = (0x2c << 8);
	sysint(0x21,&srv,&trv);
	hr = (trv.cx >> 8);
	mn = (trv.cx & 0xff);
	sc = (trv.dx >> 8);
	hn = (trv.dx & 0xff);
	fprintf(cc,"Listing of %s     %02u-%02u-%4u     %02u:%02u:%02u.%02u                   Page %3u\n",aa,mo,dy,yr,hr,mn,sc,hn,p);
}
                                                                     