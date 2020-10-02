/*
%CC1 $1.C -X -E5000
%CLINK $1 DIO WILDEXP -S
%DELETE	$1.CRL 
*/
/*********************************************************************
*				HEAD				     *
**********************************************************************
*		   COPYRIGHT 1983 EUGENE H. MALLORY		     *
*********************************************************************/
#include "BDSCIO.H"
#include "DIO.H"

int str_comp();

main(argc,argv)

int argc;
char **argv;

BEGIN

int fid,c,i,pl;
char fcb[BUFSIZ],doneflag,fname[MAXLINE],string[MAXLINE];
int ii,jj,optionerr;
char *ss;

dioinit(&argc,argv); /*	INITIALIZE DIO BUFFERS AND FLAGS */

/*********************************************************************
***		  ARGUMENT PROCESSING				   ***
*********************************************************************/

	pl = 10;
	optionerr = FALSE;
	
	for (ii=argc-1;ii>0;ii--)
	    if (argv[ii][0] == '-')
		BEGIN
		    for	(ss = &argv[ii][1]; *ss	!= '\0';)
			BEGIN
			    switch (toupper(*ss++))
				BEGIN
				    case 'L':
					pl= atoi(ss);
					break;
				    case 'H':
					optionerr = TRUE;
					break;
				    default:
					typef("HEAD: Illegal option %c.\n"
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
		typef("HEAD:  Legal options are:\n");
		typef("-Ln    Length of	head to	output.\n");
		exit(0);
	    END
	    
	if (pl < 1) error("HEAD: Length	 less than 1.");
	
/*********************************************************************
***		     END OF ARGUMENT PROCESSING			   ***
*********************************************************************/
wildexp(&argc,&argv);

#ifdef C80
qsort(&argv[1],argc-1,2,str_comp);
#else
qsort(&argv[1],argc-1,2,&str_comp); 
#endif
	    
while (TRUE)
    BEGIN
	doneflag = TRUE;
	if (--argc > 0)
	    BEGIN
		strcpy(fname,*++argv);
		doneflag = FALSE;
	    END
	else if	(DIOIN)	
	    BEGIN
		doneflag = getstring(fname);
		if (strlen(fname) == 0)	goto done;
		fname[strlen(fname)-1] = 0;
	    END
	if (doneflag) goto done;
	if (strlen(fname) == 0)	goto newfile;
	fid = fopen(fname,fcb);
	if (fid	<= 0)
	    BEGIN
		printf("Unable to open file \'%s\'.\n",fname);
		goto newfile;
	    END
	printf("FILE: %s\n\n",fname);
	for (i=1;i<=pl;i++)
	    BEGIN
		if (fgets(string,fcb)) puts(string);
		else break;
	    END
	printf("\n\n");
newfile:
    fclose(fcb);
    END
done:
	dioflush();
	exit(0);
END

int str_comp(s,t)
char **s, **t;
BEGIN
    char *s1, *t1;
    int	i;
    s1 = *s;
    t1 = *t;
	for (i=0;i<MAXLINE;i++)
	    BEGIN
		if (t1[i] != s1[i]) 
		    return s1[i] - t1[i];
		if (s1[i] == '\0')  return 0;
	    END
	return 0;
END
