/*
	Program to copy a file replacing:
		spaces with tabs and spaces 'entab'
		or tabs and spaces with spaces 'detab'
	optional fourth argument allows the tab stop modulus
	to be user specified instead of 8.
*/

/*
	Macros for constant definitions
*/

#define ERROR -1	/* error flag returned by buffered i/o routines */
#define EOFF -1		/* end of file marker returned by getc() */
#define EOF 0x1A	/* CP/M's end file char for ascii files */
#define NOFILE -1	/* no such file indication given by fopen() */
#define TAB 0x09	/* tab character */
#define MAXLINE 79	/* max column number in a line */

/*
	Argument vector indices
*/

#define XFER_TYPE 1	/* entab or detab transfer */
#define FROM_FILE 2
#define TO_FILE   3
#define MODULUS   4

/*
	main to open the files for entab() or detab()
	and handle invocation errors.
*/

main(argc,argv)
	int argc;
	char *argv[];
	{
	int fdin,fdout;
	char inbuf[134],outbuf[134];

	if(argc != 4 && argc != 5) {
	  printf("Correct invocation form is:\n\n");
	  printf("VARTABS <detab or entab> <from file> <to file>");
	  printf(" [<tab stop modulus>]\n");
	  }
	else if( (fdin = fopen(argv[FROM_FILE],inbuf)) == NOFILE )
	  printf("No such file %s\n",argv[FROM_FILE]);
	else if( (fdout  = fcreat(argv[TO_FILE],outbuf)) == ERROR )
	  printf("Can't open %s\n",argv[TO_FILE]);
	else if( argc == 4) {
	  if( tolower(*argv[XFER_TYPE]) == 'e' )
	    entab(inbuf,outbuf,8);
	  else if( tolower(*argv[XFER_TYPE]) == 'd')
	    detab(inbuf,outbuf,8);
	  else
	    printf("What !!\n");
	  }
	else {
	  if( tolower(*argv[XFER_TYPE]) == 'e' )
	    entab(inbuf,outbuf,atoi(argv[MODULUS]));
	  else if( tolower(*argv[XFER_TYPE]) == 'd' )
	    detab(inbuf,outbuf,atoi(argv[MODULUS]));
	  else
	    printf("What !!\n");
	  }
	exit();
	}

/*
	procedure detab --copy file to file translating tabs to spaces
*/

detab(filein,fileout,modulo)
	char filein[];	/* the input file buffer */
	char fileout[];	/* the output file buffer */
	int  modulo;		/* modulus for tab stops */
	{
	int col;		/* column counter */
	char tabs[MAXLINE];	/* vector of tab_stops */
	int c;		/* 1 char buffer */

	settab(tabs,MAXLINE,modulo);	/* init the tab_stops to mod modulo cols */
	col = 0;
	while( (c = getc(filein)) != EOFF )
	  if( c == TAB ) {
	    do {
	putchr(' ',fileout);
	col++;
	} while( ! tabpos(col,tabs,MAXLINE) );
	    }
	  else if( c == '\n' ) {
	    putc(c,fileout);
	    col = 0;
	    }
	  else {
	    putc(c,fileout);
	    col++;
	    }
	if( fflush(fileout) == ERROR)
	  err_exit("output file flush error");
	return;
	}	/* end detab */


/*
	procedure entab --copy file to file translating
			spaces to tabs and spaces
*/

entab(filein,fileout,modulo)
	char filein[];	/* the input file buffer */
	char fileout[];	/* the output file buffer */
	int  modulo;		/* modulus for tab stops */
	{
	int col,newcol;	/* column counters */
	char tabs[MAXLINE];	/* vector of tab_stops */
	int c;		/* 1 char buffer */

	settab(tabs,MAXLINE,modulo);	/* init the tab_stops to mod modulo */
	col = 0;
	while(1) {
	  newcol = col;
	  while( (c = getc(filein)) == ' ' ) { /* collect blanks */
	    newcol++;
	    if( tabpos(newcol,tabs,MAXLINE) ) {
	putchr(TAB,fileout);
	col = newcol;
	}	/* end if */
	    }		/* end while */
	  for(; col<newcol; col++)
	    putchr(' ',fileout);
	  if(c == ERROR)
	    err_exit("error in reading file\n");
	  putchr(c,fileout);
	  if(c == EOF)
	    break;
	  if( c == '\n' )
	    col = 0;
	  else
	    col++;
	  }	/* end while(1) */
	if( fflush(fileout) == ERROR )
	  err_exit("error in flushing output buffer\n");
	return;
	}	/* end entab */

/*
	procedure -- putchr write a char to an output file and
		error exit with message if error occurrs
*/

putchr(c,buf)
	char c,buf[];
	{
	int stat;

	stat = putc(c,buf);
	if(stat == ERROR) err_exit("error during buffered write\n");
	return;
	}


/* procedure err_exit -- print error message and exit to CP/M */

err_exit(msg)
	char *msg;
	{
	printf("%s",*msg);
	exit();
	}

/*
function tabpos -- return true if past end of MAXLINE or at a tab_stop
		false otherwise
*/

tabpos(col,tabs,max)
	int col,max;
	char tabs[];
	{
	return ( col >max || tabs[col] );
	}

/* procedure settab -- initialize the array of tab_stops to mod 8 values */

settab(tabs,max,modulo)
	int max,modulo;
	char tabs[];
	{
	int i;

	for(i=0; i<=max; i++)
	  tabs[i] = (i % modulo == 0);
	return;
	}
                                                                    