/*
%CC1 $1.C -X -E5000
%CLINK $1 DIO  -S
%DELETE	$1.CRL 
*/
/*********************************************************************
*				TEE				     *
**********************************************************************
*		   COPYRIGHT 1983 EUGENE H. MALLORY		     *
*********************************************************************/
#include "BDSCIO.H"
#include "DIO.H"
    
char *version;
    
main(argc,argv)

int argc;
char **argv;

BEGIN

int fid,c;
char fcb[BUFSIZ],teeflag,fname[MAXLINE];

    dioinit(&argc,argv); /* INITIALIZE DIO BUFFERS AND FLAGS */
    version = "Version 3.0";
    
    if (argc >=	2)
	BEGIN
	    strcpy(fname,*++argv);
	    teeflag = TRUE;
	    fid	= fcreat(fname,fcb);
	    if (fid <= 0)
		BEGIN
		    error("TEE:	Unable to create file \'%s\'.",fname);
		END
	END
    else
	BEGIN
	    teeflag = FALSE;
	END

    while ((c=getchar()) != EOF) 
	BEGIN
	    putchar(c);
	    if (teeflag) 
	    BEGIN
		if (c == '\n') putc('\r',fcb);
		putc(c,fcb);
	    END
	END
    putc(CPMEOF,fcb);
    fflush(fcb);
    fclose(fcb);
    dioflush();
    exit(0);
END
