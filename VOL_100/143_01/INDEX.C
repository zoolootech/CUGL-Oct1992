/*
%CC1 $1.C -X -E5000
%CLINK $1 DIO  -S
%DELETE	$1.CRL 
*/
/*********************************************************************
*				INDEX				*
**********************************************************************
*		   COPYRIGHT 1983 EUGENE H. MALLORY		     *
*********************************************************************/

#include "BDSCIO.H"
#include "DIO.H"

main(argc,argv)

char **argv;
int argc;

BEGIN  
    int	 i,j,n,k,flag,skip;
    int	col;
    char string[MAXLINE];
    char nlin[MAXLINE];
    char key[MAXLINE];
    char ntext[MAXLINE];
    char olin[MAXLINE];
    int	number[200];

    int	ii,jj,optionerr;
    char *ss;

    dioinit(&argc,argv);

/*********************************************************************
***		  ARGUMENT PROCESSING				   ***
*********************************************************************/

	col = 60;
	optionerr = 0;
	
	for (ii=argc-1;ii>0;ii--)
	    if (argv[ii][0] == '-')
		BEGIN
		    for	(ss = &argv[ii][1]; *ss	!= '\0';)
			BEGIN
			    switch (toupper(*ss++))
				BEGIN
				    case 'C':
					col= atoi(ss);
					break;
				    case 'H':
					optionerr = TRUE;
					break;
				    default:
					typef("INDEX: Illegal option %c.\n"
						,*--ss);
					ss++;
					optionerr = TRUE;
					break;
				END
			    while (isdigit(*ss)) ss++;
			END
		    for	(jj=ii;jj<(argc-1);jj++) argv[jj] = argv[jj+1];
		    argc--;
		END
	

	if (optionerr) 
	    BEGIN
		typef("INDEX: Legal options are:\n");
		typef("-Cn    Column width.\n");
		exit(0);
	    END
	    
	if (col	< 12) error("INDEX: Column less	than 12.");
	
/*********************************************************************
***		     END OF ARGUMENT PROCESSING			   ***
*********************************************************************/
	    

    key[0] = 0;
    nlin[0] = 0;
    i =	0;
    
    do
	BEGIN
	    flag = getstring(string);
	    if (!flag && strlen(string)	< 9)
		error("INDEX: Input line too short.");
	    string[strlen(string)-1] = 0; /* remove new	line */
	    if (!flag && strequ(key,&string[8]))
		BEGIN
		    n =	atoi(string);
		    if (i == 0 || n != number[i-1]) number[i++]	= n;
		END
	    else 	/* NEW	KEY, PRINT LAST	INDEX LINE(S) */
		BEGIN
		    if (i > 0)
		    BEGIN
		      sprintf(olin,"%s	",key);
		      nlin[0] =	0;
		      for (j=0;j<i;j++)
			BEGIN
				if (j <	i-1)
				    sprintf(ntext,"%d,",number[j]);
				else
				    sprintf(ntext,"%d",number[j]);
				if (strlen(olin) 
				    + strlen(ntext)
				    + strlen(nlin)
				    < col)
					strcat(nlin,ntext);
				else
				    BEGIN
					puts(olin);
					skip = col-strlen(olin)-strlen(nlin);
					for (k=1;k<=(skip);k++)
					    if (k == 1 ||
						k == skip ||
						olin[0]	== ' ')
						putchar(' ');
					    else
						putchar('.');
					puts(nlin);
					puts("\n");
					strcpy(olin,"	 ");
					strcpy(nlin,ntext);
				    END
			END
		      puts(olin);
		    skip = col-strlen(olin)-strlen(nlin);
		    for	(k=1;k<=(skip);k++)
			if (k == 1 ||
			    k == skip ||
			    olin[0] == ' ')
			    putchar(' ');
			else
			    putchar('.');
		      puts(nlin);
		      puts("\n");
		    END
		    if (!flag)
			strcpy(key,&string[8]);
		    i =	0;
		    number[i++]	= atoi(string);
		END
	END
    while (!flag);
    
    dioflush();
END  


/*****************************************************/
/*	STREQU - STRING EQUAL	(IGNORES CASE)	       */
/*	RETURNS:	   1 if	EQUAL		       */
/*			   0 if	NOT EQUAL	       */
/*****************************************************/

strequ(s1,s2)
char *s1,*s2;
BEGIN
    while (toupper(*s1++) == toupper(*s2++)) 
	    if (!*s1 &&	!*s2) return (1);
    return (0);
END
