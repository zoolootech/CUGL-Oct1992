/* HEADER: CUG120.06;
   TITLE: DIO45;
   VERSION: 1.45;
   DATE: 00/12/1981;
   DESCRIPTION: "Directed input/output simulation for BDS C.";
   KEYWORDS: pipes,directed i/o;
   SYSTEM: CP/M;
   FILENAME: DIO45.H;
   CRC: E450;
   AUTHORS: Leor Zolman;
   COMPILERS: BDS C;
*/
/*
	External data used by DIO.C for directed I/O simulation:
	(BDSCIO.H must also be #included in the main file.)
*/

/*
#define BUF_CONS 1			/* uncomment if console buffering is
						desired	(see DIO.C)	   */
*/

char _diflag, _doflag;			/* flag if directed I/O being used */
char _pipef, *_pipedest;		/* true if a pipe is being filled  */
char **_savei, **_nullpos;		/* used to remember position in
					   command line when piping 	   */
char _dibuf[BUFSIZ], _dobuf[BUFSIZ];	/* I/O buffers used for direction  */

#ifdef BUF_CONS				/* console buffering data	   */

char _conbuf[MAXLINE + 2];		/* console input buffer used for 
					   non-directed standard input	   */
char *_conbufp;				/* pointer to next character to
						read from console buffer   */
#endif
 handle nested include files to
	a depth of 8.";
   KEYWORDS: C progra