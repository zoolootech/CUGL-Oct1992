/*
%CC1 $1.C -X -E5000
%CLINK $1  -S
%DELETE	$1.CRL 
*/
/*********************************************************************
*				if				     *
**********************************************************************
*		   COPYRIGHT 1983 EUGENE H. MALLORY		     *
*********************************************************************/
#include "BDSCIO.H"
#define	BCOUNT 120
char fname[MAXLINE];
int fd1;
FILE fcb1;
char barray[BCOUNT][128];

main(argc,argv)
char **argv;
int argc;

BEGIN
    int	i,bctr,flag,ifcount;
/********************************************************************
    		EVALUATE ARGUMENT EXPRESSION
********************************************************************/
    if (argc ==	1) goto	false;
    if (argc ==	2 && strequ(argv[1],"NOT")) goto true;
    if (argc ==	3 && strequ(argv[1],"NOT")) goto false;
    if (strequ(argv[1],"EXISTS"))
	BEGIN
	    if (argc !=	3)
		abort("IF: SYNTAX: if EXISTS fid.");
	    fd1	= open(argv[2],	READ);
	    if (fd1 == -1) goto	false;
	    close(fd1);
	    goto true;
	END
    if (strequ(argv[1],"NOTEXISTS"))
	BEGIN
	    if (argc !=	3)
		abort("IF: SYNTAX: if NOTEXISTS	fid.");
	    fd1	= open(argv[2],	READ);
	    if (fd1 == -1) goto	true;
	    close(fd1);
	    goto false;
	END
    if (argc ==	4)
	BEGIN
	    if (strequ(argv[2],"EQUAL"))
		if (strequ(argv[1],argv[3])) goto true;
		else goto false;
	END
    if (argc ==	2 && strequ(argv[1],"EQUAL")) goto true;
    if (argc ==	3 && strequ(argv[1],"EQUAL")) goto false;
    if (argc ==	3 && strequ(argv[2],"EQUAL")) goto false;
    if (argc ==	2 && strequ(argv[1],"NOTEQUAL")) goto false;
    if (argc ==	3 && strequ(argv[1],"NOTEQUAL")) goto true;
    if (argc ==	3 && strequ(argv[2],"NOTEQUAL")) goto true;
    if (argc ==	4)
	BEGIN
	    if (strequ(argv[2],"NOTEQUAL"))
		if (strequ(argv[1],argv[3])) goto false;
		else goto true;
	END
    abort("IF: SYNTAX: See help	file for correct forms.");
    
/*********************************************************************/
    
false:		/* Scan off to find endif	*/
/*  printf("	.\n    .\n    .\n  ENDIF"); */
    
    fd1	= open("A:$$$.SUB", READWRITE);
    if (fd1 == -1)
	BEGIN
	    abort("IF: Unable to open A:$$$.SUB.");
	END
    else
	BEGIN
	    bctr = 0;
	    do
		BEGIN
		    if (bctr ==	BCOUNT)
			abort("IF: A:$$$.SUB too big.");
		    flag = read(fd1,barray[bctr++],1);
		    if (flag ==	-1)
			abort("IF: Unable to read A:$$$.SUB.");
		END
	    while (flag);
	    bctr--;
	    close(fd1);
	    unlink("A:$$$.SUB");
	END
    
    ifcount = 1;
    
    while (bctr--)
	BEGIN
	    if (toupper(barray[bctr][1]) == 'E'
		&&
		toupper(barray[bctr][2]) == 'N'
		&&
		toupper(barray[bctr][3]) == 'D'
		&&
		toupper(barray[bctr][4]) == 'I'
		&&
		toupper(barray[bctr][5]) == 'F')
		    ifcount -= 1;
	    if (toupper(barray[bctr][1]) == 'I'
		&&
		toupper(barray[bctr][2]) == 'F')
		    ifcount += 1;
#ifdef C80
	    printf("--%s\n",barray[bctr][1]);
#else
	    printf("--%s\n",&barray[bctr][1]);
#endif
	    if (ifcount	== 0) break;
	END
	    
    fd1	= creat("A:$$$.SUB");
    if (fd1 == -1)
	    abort("IF: Unable to recreate A:$$$.SUB.");
    for	(i=0;i<bctr;i++)
	BEGIN
	    flag = write(fd1,barray[i],1);
	    if (flag !=	1)
		abort("IF: Unable to write A:$$$.SUB.");
	END
    close(fd1);
true: 
#ifdef C80
    exit(0);
#else
    bios(1);
#endif
END

/********************************************************************/
/*			  ERROR					    */
/********************************************************************/
abort(string)
char *string;
    BEGIN
	unlink("A:$$$.SUB");
	error(string);
    END

int strequ(s,t)
char *s, *t;
BEGIN
    int	i;
	for (i=0;i<MAXLINE;i++)
	    BEGIN
		if (t[i] != s[i]) return(0);
		if (s[i] == '\0')  return(1);
	    END
	return(1);
END

