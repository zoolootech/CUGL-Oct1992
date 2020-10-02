/*
%CC1 $1.C -X -E5000
%CLINK $1 DIO WILDEXP -S
%DELETE $1.CRL 
*/
/*********************************************************************
*                               BATCH                                *
**********************************************************************
*                  COPYRIGHT 1983 EUGENE H. MALLORY                  *
*********************************************************************/
#include "BDSCIO.H"
#ifndef C80
#include "DIO.H"
#endif

#define BCOUNT 120

char string[MAXLINE];
char fname[MAXLINE];
char c;
int fd1,fd2;
char fcb1[BUFSIZ],fcb2[BUFSIZ];
char barray[BCOUNT][MAXLINE];
char buffer[128];
int bctr,flag;
int str_comp();

main(argc,argv)
char **argv;
int argc;

BEGIN
    int i,j,menuflag;
    char *fvector[3];
    char **fvectp;
    char *disk;
    int diskno,vctr,len;
    
    dioinit(&argc,argv);
    
    if (DIOIN)
	BEGIN
	    bctr = 0;
	    while (!getstring(string))
		BEGIN
		    len = strlen(string);
		    string[len-1] = 0;
		    linecopy(barray[bctr++],string,argv,argc,0); 
		    if (bctr == BCOUNT)
			error("BATCH: Batch file too big.");
		END
	    goto writesub;
	END


    if (argc >= 2 && argv[1][0] == '/')
	BEGIN
	    bctr = 0;
	    typef("*");
	    while (gets(string))
		BEGIN
		    if (string[0] == 0) goto writesub;
		    linecopy(barray[bctr++],string,argv,argc,1); 
		    if (bctr == BCOUNT)
			error("BATCH: Batch file too big.");
		    typef("*");
		END
	    goto writesub;
	END
	
    if (argc < 2)
	BEGIN
	    disk = "ABCDEFGHIJKLM";
	    vctr = 3;
	    diskno = bdos(25,0);
	    if (!diskno) vctr = 2;
	    fvector[0] = "DUMMY";
	    fvector[1] = "?:*.BAT";
	    fvector[1][0] = disk[diskno];
	    fvector[2] = "A:*.BAT";
	    fvectp = fvector;
	    wildexp(&vctr,&fvectp);
#ifdef C80
	    qsort(&fvectp[1],vctr-1,2,str_comp); 
#else
	    qsort(&fvectp[1],vctr-1,2,&str_comp); 
#endif
	    fvectp++;
filemenu:
	    i = menu(vctr-1,fvectp);
	    if (i == -1) goto exitbatch;
	    strcpy(fname,fvectp[i]);
	    fd1 = fopen(fname,fcb1);
	    if (fd1 == ERROR)
		error("BATCH: Unable to open %s",fname);
	END
    else
	BEGIN
	    strcpy(fname,argv[1]);
	    strcat(fname,".BAT");
	    fd1 = fopen(fname,fcb1);
	    if (fd1==ERROR) 
		BEGIN
		    strcpy(fname,"a:");
		    strcat(fname,argv[1]);
		    strcat(fname,".BAT");
		    fd1 = fopen(fname,fcb1);
		END
	    if (fd1==ERROR) 
		BEGIN
		    dioflush();
		    error("BATCH: Unable to find: %s. \N",fname);
		END
	END
	
    if (!fgets(string,fcb1))
	BEGIN
	    dioflush();
	    error("BATCH: Batch file %s empty.",fname);
	END
	
    bctr = 0;
    do
	BEGIN
	    len = strlen(string);
	    string[len-1] = 0;
	    linecopy(barray[bctr++],string,argv,argc,1); 
	    if (bctr == BCOUNT)
		error("BATCH: Batch file too big.");
	END
    while (fgets(string,fcb1));
	
/*	SUBSTITUTED ARGUMENTS ARE NOW IN BARRAY */    

writesub:    
    fd2 = open("A:$$$.SUB",READWRITE);
    if (fd2 == -1)
	BEGIN
	    fd2 = creat("A:$$$.SUB");
	END
    else
	BEGIN
	    do
		BEGIN
#ifdef C80
		    flag = seek(fd2,0,2);	/* goto end of file */
			/* This had to be done since C80 read only 
			indicates when beyond EOF and it does so by 
			returning an error flag. This accomplishes 
			the same thing but isn't very elegant. Sorry. */
#else
		    flag = read(fd2,buffer,1);
#endif
		    if (flag == -1)
			error("BATCH: Unable to append to A:$$$.SUB.");
		END
	    while (flag);
	END
    
    /* Now $$.sub should be opened correctly */
    
    while (bctr--)
	BEGIN
	    for (i=0;i<128;i++) buffer[i] = 0x1a;
	    strcpy(&buffer[1],barray[bctr]);
	    buffer[0] = strlen(barray[bctr]);
	    i = write(fd2,buffer,1);
	    if (i != 1)
		error("BATCH: Unable to write A:$$$.SUB.");
	END
    close(fd2);
    
exitbatch:
    dioflush();
    bios(1);
END

menu(icounter,strings)
char **strings;
int icounter;
BEGIN
int max,bias,i,j;
char *prompt;
    prompt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    bias = 0;
loop:
    typef("\n\014");
    if ((icounter-bias) > 26) max = 26;
    else max = icounter-bias;
    for (i=0;i<(max+1)/2;i++)
	BEGIN
	    typef("%c) %s",prompt[i],strings[i+bias]);
	    if (i+bias+(max+1)/2 < icounter)
		BEGIN
		    for (j=0;j<=37-strlen(strings[i+bias]);j++) typef(" ");
		    typef("%c) %s\n",
			prompt[i+(max+1)/2],
			strings[i+bias+(max+1)/2]);
		END
	    else typef("\n");
	END
    typef("\n\n");
    if (icounter > bias+26)
	typef("More selections are available.\n");
    typef("Type selection, ESC to exit, CR for more, - to backup :");
    c = bdos(1,0);
    typef("\n\014");
    if (c == '\033') return -1;
    if (c == '\r' && (icounter > bias+26)) bias += 26;
    else if (c == '\r' && (icounter <= bias+26)) bias = 0;
    if (c == '-') bias -= 26;
    if (bias < 0) bias = 0;
    if (!isalpha(c)) goto loop;
    for (i=0;toupper(c)!=prompt[i];i++);
    if (i>=max) goto loop;
    if (i+bias >= icounter) goto loop;
    return i+bias;
END

int str_comp(s,t)
char **s, **t;
BEGIN
    char *s1, *t1;
    int i;
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

linecopy(dest,source,argv,argc,bias)
char *dest,*source,**argv;
int argc,bias;
BEGIN
int i;
char c,tempstr[MAXLINE];
    if (*source == '*') *source = ';';
    while (c = *source++)    
	BEGIN
	    if (c == '$' && isdigit(*source))
		BEGIN
		    i = *source-'0'+bias;
		    if (i < argc)
			BEGIN
			    strcpy(dest,argv[i]);
			    dest += strlen(argv[i]);
			END
		    source++;
		END
	    else if (c == '$' && *source == '*')
		BEGIN
		    for (i=1+bias;i<argc;i++)
			BEGIN
			    if (i < argc)
				BEGIN
				    strcpy(dest,argv[i]);
				    strcat(dest," ");
				    dest += strlen(argv[i])+1;
				END
			END
		    source++;
		END
	    else if (c == '$' && *source == '#')
		BEGIN
		    sprintf(tempstr,"%d",argc-bias);
		    strcpy(dest,tempstr);
		    dest += strlen(tempstr);
		    source++;
		END
	    else if (c == '$' && *source == '$')
		BEGIN
		    source++;
		    *dest++ = c;
		END
	    else if (c == '^' && isalpha(*source))
		BEGIN
		    *dest++ = toupper(*source)-0x40;
		    source++;
		END 
	    else
		BEGIN
		    *dest++ = c;
		END
	END
    *dest = 0;
END

