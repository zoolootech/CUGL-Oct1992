/*
%CC1 $1.C -X -E5000
%CLINK $1 DIO WILDEXP -S
%DELETE $1.CRL 
*/
/*********************************************************************
*                               CAT                                  *
**********************************************************************
*                  COPYRIGHT 1983 EUGENE H. MALLORY                  *
*********************************************************************/
#include "BDSCIO.H"
#include "DIO.H"

main(argc,argv)

int argc;
char **argv;

BEGIN

    int fid,c;
    char fcb[BUFSIZ],doneflag,fname[MAXLINE];

    dioinit(&argc,argv); /* INITIALIZE DIO BUFFERS AND FLAGS */
    wildexp(&argc,&argv);
    
    if (argc == 1 && !DIOIN)
	BEGIN
	    while ((c=getchar()) != EOF) putchar(c);  
	    goto done;
	END
    while (TRUE)
	BEGIN
	    doneflag = TRUE;
	    if (--argc > 0)
		BEGIN
		    strcpy(fname,*++argv);
		    doneflag = FALSE;
		END
	    else if (DIOIN) 
		BEGIN
		    doneflag = getstring(fname);
		    if (strlen(fname) == 0) goto done;
		    fname[strlen(fname)-1] = 0;
		END
	    if (doneflag) goto done;
	    if (strlen(fname) == 0) goto newfile;
	    fid = fopen(fname,fcb);
	    if (fid <= 0)
		BEGIN
		    typef("Unable to open file \'%s\'.\n",fname);
		    goto newfile;
		END
	while (TRUE)
	    BEGIN
		c = getc(fcb);
		if (c == '\r')
		    BEGIN
			c = getc(fcb);
			if (c != '\n')
			    BEGIN
				ungetc(c,fcb);
				c = '\r';
			    END
		    END
		if (c == -1) goto newfile;
		if (c == CPMEOF) goto newfile;
		putchar(c);
	    END
newfile:
    fclose(fcb);
    END
done:
	dioflush();
	exit(0);
END
