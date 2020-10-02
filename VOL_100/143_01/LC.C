
/*
%CC1 $1.C -X -E5000
%CLINK $1 DIO  -S
%DELETE	$1.CRL 
*/
/*********************************************************************
*				LC				     *
**********************************************************************
*		   COPYRIGHT 1983 EUGENE H. MALLORY		     *
*********************************************************************/
#include "BDSCIO.H"
#include "DIO.H"

main(argc,argv)

char **argv;
int argc;

BEGIN  
    int	 c,nl,wc,cc,inword;
    dioinit(&argc,argv);
    
/*********************************************************************
***		  ARGUMENT PROCESSING				   ***
*********************************************************************/
#define	LC 0
#define	WC 1
#define	CC 2

int ii,jj,optionerr,option;
char *ss;
	option = optionerr = 0;

	for (ii=argc-1;ii>0;ii--)
	    if (argv[ii][0] == '-')
		BEGIN
		    for	(ss = &argv[ii][1]; *ss	!= '\0';)
			BEGIN
			    switch (toupper(*ss++))
				BEGIN
				    case 'L':
					option = LC;
					break;
				    case 'W':
					option = WC;
					break;
				    case 'C':
					option = CC;
					break;
				    case 'H':
					optionerr = TRUE;
					break;
				    default:
					typef("LC: Illegal option %c.\n"
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
		typef("LC:    Legal options are:\n");
		typef("-L     Line count.\n");
		typef("-W     Word count.\n");
		typef("-C     Character	count.\n");
		exit(0);
	    END	

/*********************************************************************
***		     END OF ARGUMENT PROCESSING			   ***
*********************************************************************/
    
    inword = wc	= cc = nl = 0;
    if (option == LC)
    BEGIN
    while ((c=getchar()) != EOF) if (c == '\n')	nl++;
    printf("%d\n",nl);
    END
    else if (option == WC)
    BEGIN
    inword = FALSE;
    wc = 0;
    while ((c=getchar()) != EOF) 
	BEGIN
	    if (!isalpha(c) && inword)
		BEGIN
		    inword = FALSE;
		    wc++;
		END
	    else if (isalpha(c))
		BEGIN
		    inword = TRUE;
		END
	END
    printf("%d\n",wc);
    END
    else if (option == CC)
    BEGIN
    while ((c=getchar()) != EOF) cc++;
    printf("%d\n",cc);
    END
    dioflush();
END  
