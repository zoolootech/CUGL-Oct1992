/*
%CC1 $1.C -X -E5000
%CLINK $1 DIO 
%DELETE $1.CRL 
*/
/*********************************************************************
*                               2UP                                  *
**********************************************************************
*                  COPYRIGHT 1983 EUGENE H. MALLORY                  *
*	MODIFIED FOR EITHER C80 OR BDS/C COMPILER BY H. ECKERSON    *
*			4/27/85					     *
*********************************************************************/

#include "BDSCIO.H"
#INCLUDE "DIO.H"

#define MAXW 132

char doneflag,string[MAXLINE];
char array[90][MAXW+2];
int ncol,col,len,hold,margin,space,width;
int bm,tm;

main(argc,argv)

int argc;
char **argv;

  BEGIN
    int i,j,k,firstflag;
    dioinit(&argc,argv);  	/* INITIALIZE DIO BUFFERS AND FLAGS */

/*********************************************************************
***               ARGUMENT PROCESSING                              ***
*********************************************************************/

    int ii,jj,optionerr;
    char *ss;
    hold = ncol = space = margin = col = len = optionerr = 0;
    bm = tm = 0;
    for (ii=argc-1;ii>0;ii--)
    if (argv[ii][0] == '-')
      THEN
	for (ss = &argv[ii][1]; *ss != '\0';)
	  LOOP
	    switch (toupper(*ss++))
	      BEGIN
	      case 'L':
		len = atoi(ss);
		break;
	      case 'C':
		col= atoi(ss);
		break;
	      case 'M':
		margin = atoi(ss);
		break;
	      case 'T':
		tm = atoi(ss);
		break;
	      case 'B':
		bm = atoi(ss);
		break;
	      case 'S':
		space = atoi(ss);
		break;
	      case 'N':
		ncol= atoi(ss);
		break;
	      case 'W':
		hold = TRUE;
		break;
	      case 'H':
		optionerr = TRUE;
		break;
	      default:
		typef("2UP: Illegal option %c.\n", *--ss);
		ss++;
		optionerr = TRUE;
		break;
	      END
	    while (isdigit(*ss)) ss++;
	  ENDLOOP
	for (jj=ii;jj<(argc-1);jj++) argv[jj] = argv[jj+1];
	argc--;
      ENDIF
    if (optionerr) 
      THEN
	typef("2UP:   Legal options are:\n");
	typef("-Ln    Page length.\n");
	typef("-Cn    Column width .\n");
	typef("-Mn    Margin.\n");
	typef("-Tn    Top margin.\n");
	typef("-Bn    Bottom margin.\n");
	typef("-W     Wait at the bottom of each page.\n");
	typef("-Sn    Space between columns.\n");
	typef("-Nn    Number of columns.\n");
	exit(0);
      ENDIF
    if (ncol == 0) ncol = 2;
    if (space == 0) space = 0;
    if (len == 0) len = 66;
    if (col == 0) col = (80-margin-space*(ncol-1))/ncol;
    if (len > 88) error("2UP: Too long a page, 88 lines is maximum.");
    if ((margin+ncol*col+(ncol-1)*space) > MAXW) 
    error("2UP: Total width exceeds 132.");
    len = len-tm-bm;
    if (len < 1)
    error("2UP: Insufficient length.");

/*********************************************************************
***                  END OF ARGUMENT PROCESSING                    ***
*********************************************************************/
/*********************************/
/*	MAIN LOOP                */
/*********************************/
    firstflag = TRUE;
    doneflag = FALSE;
    width = margin + ncol*col + (ncol-1)*space;
    do
      LOOP
	for (i=1;i<=len;i++)
	  LOOP
	    for (j=1;j<=MAXW+1;j++)
	    array[i][j] = ' ';
	  ENDLOOP
	for (k=0;k<ncol;k++)
	  LOOP
	    for (i=1;i<=len;i++)
	      LOOP
		if (!doneflag) 
		  THEN
		    doneflag = getstring(string);
		    if (k == 0 
		    && i == 1&& doneflag) goto done;
		    if (k == 0 
		    && i == 1 && hold && !firstflag)THEN
			typef("Hit return to continue.\n");
			while (!bdos(6,0xff));
		      ENDIF
		    firstflag = FALSE;
		    if (!doneflag) 
		      THEN
			strmov(&array[i][margin+k*col+k*space+1],
			string,col);
		      ENDIF
		  ENDIF
	      ENDLOOP
	  ENDLOOP
	for (i=1;i<=tm;i++) puts("\n");
	for (i=1;i<=len;i++)
	  LOOP
	    for (j=MAXW+1;j>0;j--)
	    if (array[i][j] != ' ' || j == 1)
	      THEN
		array[i][j+1] = 0;
		break;
	      ENDIF
	    puts(&array[i][1]);
	    puts("\n");
	  ENDLOOP
	for (i=1;i<=bm;i++) puts("\n");
      ENDLOOP
    while (doneflag == FALSE);
    done:;
    dioflush();
  END

strmov(s1,s2,n)
char *s1,*s2;
int n;
  BEGIN
    int i;
    char c;
    for (i=1;i<=n;i++)
      LOOP
	c = *s2;
	s2++;
	if ((c == 0) || (c == '\n')) 
	  THEN
	    for (i;i<=n;i++) s1++;
	    return(0);
	  ENDIF
	*s1 = c;
	s1++;
      ENDLOOP
  END
