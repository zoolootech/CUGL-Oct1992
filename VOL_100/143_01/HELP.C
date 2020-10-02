/*
%CC1 $1.C -X -E5000
%CLINK $1 DIO WILDEXP -N -S
%DELETE	$1.CRL 
*/
/*********************************************************************
*				HELP				     *
**********************************************************************
*		   COPYRIGHT 1983 EUGENE H. MALLORY		     *
*********************************************************************/
#include "BDSCIO.H"
#include "DIO.H"
#define	MENUCOUNT 120
#define	NOCONTINUE 1 /*	Set to 1 if you	want to	exit at	end of section */
char menutext[MENUCOUNT][MAXLINE];
char *menuptrs[MENUCOUNT];
char **menuv,*cprompt,*lprompt;
int menuctr,mindex;
char string[MAXLINE];
char fname[MAXLINE];
char c;
int fd1;
#ifdef C80
FILE *fcb1;
#else
char fcb1[BUFSIZ];
#endif
int str_comp();

main(argc,argv)
char **argv;
int argc;

  BEGIN
    int	i,j,menuflag;
    char *fvector[3];
    char **fvectp;
    char *disk;
    int	diskno,vctr;
    dioinit(&argc,argv);
#ifdef C80
    fcb1 = &fd1;
#endif
    menuflag = 0;
    cprompt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    lprompt = "abcdefghijklmnopqrstuvwxyz";
    if (argc !=	2)
      THEN
	disk = "ABCDEFGHIJKLM";
	vctr = 3;
	diskno = bdos(25,0);
	if (!diskno) vctr = 2;
	fvector[0] = "DUMMY";
	fvector[1] = "?:*.HLP";
	fvector[1][0] =	disk[diskno];
	fvector[2] = "A:*.HLP";
	fvectp = fvector;
	wildexp(&vctr,&fvectp);
#ifdef C80
	qsort(&fvectp[1],vctr-1,2,str_comp);
#else
	qsort(&fvectp[1],vctr-1,2,&str_comp);
#endif
	fvectp++;
	filemenu:
	i = menu(vctr-1,fvectp,cprompt);
	if (i == -1) goto exithelp;
	strcpy(fname,fvectp[i]);
	fd1 = fopen(fname,fcb1);
	if (fd1	== ERROR){
	    typef("HELP: Unable	to open	%s",fname);
	    error("");
	}
	menuflag = 1;
      ENDIF
    else
      THEN
	strcpy(fname,argv[1]);
	strcat(fname,".hlp");
	fd1 = fopen(fname,fcb1);
	if (fd1==ERROR)	
	  THEN
	    strcpy(fname,"a:");
	    strcat(fname,argv[1]);
	    strcat(fname,".hlp");
	    fd1	= fopen(fname,fcb1);
	  ENDIF
	if (fd1==ERROR)	
	  THEN
	    dioflush();
	    typef("HELP: Unable	to find	help for: %s. \n",fname);
	    error("");
	  ENDIF
      ENDIF
    if (!fgets(string,fcb1))
      THEN
	dioflush();
	typef("HELP: Help file %s empty.",fname);
	error("");
      ENDIF
    menuctr = 0;
#ifdef C80
    for	(i=0;i<MENUCOUNT;i++) menuptrs[i] = menutext[i];
#else
    for	(i=0;i<MENUCOUNT;i++) menuptrs[i] = &menutext[i];
#endif
    menuv = &menuptrs[0];
    if (string[0] == ':')
      THEN
	/* Non indexed file, build index */
	do
	  LOOP
	    string[strlen(string)-1] = 0;
	    if (string[0] == ':')
	    strcpy(menutext[menuctr++],string);
	    if (menuctr	== MENUCOUNT)
	    break;
	  ENDLOOP
	while (fgets(string,fcb1));
	fclose(fcb1);
	fopen(fname,fcb1);
	fgets(string,fcb1);
      ENDIF
    else
      THEN
	/* Indexed file, build index */
	do
	  LOOP
	    string[strlen(string)-1] = 0;
	    strcpy(menutext[menuctr++],string);
	    if (!fgets(string,fcb1) || menuctr == MENUCOUNT)
	      THEN
		/* No colons and no index. */
		fclose(fcb1);
		fopen(fname,fcb1);
		typef("\n\014");
		string[0] = ':';
		fgets(&string[1],fcb1);
		helprint(!menuflag);
		if (menuflag) goto filemenu;
		goto exithelp;
	      ENDIF
	  ENDLOOP
	while (string[0] != ':');
      ENDIF
/****************************************************************
*  	Now,	index is built and the first line read		*
*	past the index.						*
****************************************************************/
    again:
    if (menuctr	== 1) /* if only one choice */
      THEN
	helprint(0);
	if (menuflag) goto filemenu;
	else goto exithelp;
      ENDIF
    mindex=menu(menuctr,menuv,lprompt);
    if (mindex == -1 &&	menuflag) goto filemenu;
    if (mindex == -1) goto exithelp;
    while (mindex)
      LOOP
	if (!fgets(string,fcb1))
	  THEN
	    error("HELP: Ran off end of	help file while	searching.");
	  ENDIF
	if (string[0] == ':') mindex--;
      ENDLOOP
    helprint(0);
    fclose(fcb1);
    fopen(fname,fcb1);
    while (fgets(string,fcb1))
      LOOP
	if (string[0] == ':') break;
      ENDLOOP
    goto again;
    exithelp:
#ifndef	C80
    fclose(fcb1);
#endif
    dioflush();
  END

helprint(exitflag)
int exitflag;
  BEGIN
    char c;
    int	lctr,noteof;
    printmore:
    if (DIOOUT)	puts(&string[1]);
    typef("%s",&string[1]);
    lctr = 1;
    while (noteof = fgets(string,fcb1))
      LOOP
	if (string[0] == ':') break;
	lctr++;
	if ((string[0] == '\014' || string[0] == ';' ||	lctr ==	21))
	  THEN
	    lctr = 0;
	    typef("\n(More available, hit CR to	continue, ESC to quit.)\n");
	    c =	bdos(1,0) & 0xff;
	    typef("\n\014");
	    if (c == '\003') exit(0);  	/* CTL C	*/
	    if (c == '\033') goto quit;
	    if (string[0] == '\014' || string[0] == ';') 
	      THEN
		if (DIOOUT) puts(&string[1]);
		typef("%s",&string[1]);
		lctr++;
	      ENDIF
	    else
	      THEN
		if (DIOOUT) puts(string);
		typef("%s",string);
	      ENDIF
	    continue;
	  ENDIF
	if (DIOOUT) puts(string);
	typef("%s",string);
      ENDLOOP
    if (exitflag) goto quit;
    if (!noteof) 
    typef("\n(End of file, CR to return	to menu.)\n");
    else if (NOCONTINUE)
    typef("\n(End of section, CR to return to menu.)\n");
    else
    typef("\n(End of section, CR to continue, ESC to quit.)\n");
    c =	bdos(1,0) & 0xff;
    if (c == '\003') exit(0);  	/* CTL C	*/
    if (c == '\033' || !noteof || NOCONTINUE) 
      THEN
	quit: 
	return(0);
      ENDIF
    typef("\n\014");
    goto printmore;
  END

menu(icounter,strings,prompt)
char **strings,*prompt;
int icounter;
  BEGIN
    int	max,bias,i,j;
    bias = 0;
    lup:
    typef("\n\014");
    if ((icounter-bias)	> 26) max = 26;
    else max = icounter-bias;
    for	(i=0;i<(max+1)/2;i++)
      LOOP
	typef("%c) %s",prompt[i],strings[i+bias]);
	if (i+bias+(max+1)/2 < icounter)
	  THEN
	    for	(j=0;j<=37-strlen(strings[i+bias]);j++)	typef("	");
	    typef("%c) %s\n",
	    prompt[i+(max+1)/2],
	    strings[i+bias+(max+1)/2]);
	  ENDIF
	else typef("\n");
      ENDLOOP
    typef("\n\n");
    if (icounter > bias+26)
      typef("More selections are available.\n");
    typef("Type	selection, ESC to exit,	CR for more, - to backup :");
    c =	bdos(1,0);
    typef("\n\014");
    if (c == '\003') exit(0);  	/* CTL C	*/
    if (c == '\033') return(-1);  	/* ESC   */
    if (c == '\r' && (icounter > bias+26)) bias	+= 26;
    else if (c == '\r' && (icounter <= bias+26)) bias =	0;
    if (c == '-') bias -= 26;
    if (bias ==	-26) bias = icounter-26;
    if (bias < 0) bias = 0;
    if (!isalpha(c)) goto lup;
    for	(i=0;toupper(c)	!= toupper(prompt[i]);i++);
    if (i>=max)	goto lup;
    if (i+bias >= icounter) goto lup;
    return(i+bias);
  END

int str_comp(s,t)
char **s, **t;
  BEGIN
    char *s1, *t1;
    int	i;
    s1 = *s;
    t1 = *t;
    for	(i=0;i<MAXLINE;i++)
      LOOP
	if (t1[i] != s1[i]) 
	return(	s1[i] -	t1[i] );
	if (s1[i] == '\0') return( 0 );
      ENDLOOP
    return( 0);
  END
