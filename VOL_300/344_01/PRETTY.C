

/*
 *      HEADER:         ;
 *      TITLE:          Pretty;
 *      DATE:           10/29/89;
 *      DESCRIPTION:    "Turbo Pascal source code indent utility";
 *      VERSION:        1.0;
 *      FILENAME:       PRETTY.C;
 *      USAGE:          pretty filename.pas [tab width] [-s or /s];
 *
 *      OPTIONS:        tab width = indent increment ( 2 - 8 )
 *                      -s = "use only spaces to indent";
 *
 *      AUTHORS:        Michael Kelly;
 *
 *      INPUT:          filename.pas  (Turbo Pascal source code);
 *      OUTPUT:         filename.prt  (indented Turbo Pascal source code);
 *
 *      COMMENTS:       Tested using TurboC V 1.5 and TubroC V 2.0;
 *
 *      CAVEAT:         "This is not a true parser, for best results put
 *                      reserved words that signify the start of a code
 *                      block at the start of the line";
 */



/*
 *  ***  Pretty  ***
 */
#include <stdio.h>		/* Standard Header Files */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define TOKENS 13		/* Array elements */
#define MAXSTACK 40
#define INBUFSIZE 6144
#define OUTBUFSIZE 8198
int push(void);			/* Function Prototypes */
int pop(void);
int write_spaces(void);
int get_token(void);

enum TOKEN_TYPE {
    BEGIN,REPEAT,CASE,WHILE,WITH,FOR,IF,END,UNTIL,ELSE,\
PROCEDURE,FUNCTION,ENDD,NONE};
enum TOKEN_TYPE z,tok;
enum TOKEN_TYPE compare_tokens(void);
void exit(int status);
void clrscr(void);
void gotoxy(int x,int y);
void show_usage_and_quit(void);
void writeln(void);
void done(void);
void do_switch(void);

int stack[MAXSTACK];		/* External Variables */
int first = 1,use_tabs = 1;
int ndx,nspaces;
char buff[BUFSIZ];
char newbuff[BUFSIZ];
char out_line[BUFSIZ];
char tokens[TOKENS][10] = {
    "begin","repeat","case","while","with","for","if",\
"end","until","else","procedure","function","end."};

FILE *fpin, *fpout;
char *ibuf,*obuf;
int i,x;
char first_word[82] = "";
char out_file[66] = "";

/*
 *  Main
 */

void main(int argc,char *argv[])
{
    char *p;

    /* get input filename and options from command line */

    if(argc > 1) {
	strlwr(argv[1]);	        /* convert file name to lower case */
	if(! strstr(argv[1],".pas"))     /* ".pas" extension required */
	    show_usage_and_quit();
	memccpy(out_file,argv[1],'.',64);
	p = strrchr(out_file,'.');  	/* extract file base name */
	*p = '\0';
	strcat(out_file,".prt");   /* tack ".prt" onto file base name */
    }
    else
	show_usage_and_quit();	/* quit if no filename specified */

    if(argc > 2) {			/* check command line options */
	p = strlwr(argv[2]);
	if(!strcmp(p,"/s") || !strcmp(p,"-s")) {
	    use_tabs = 0;
	    nspaces = 4;	/* use spaces and default tab width */
	}

	else {
	    nspaces = atoi(argv[2]);      /* valid tab width ? */
	    if((nspaces < 2) || (nspaces > 8))
		show_usage_and_quit(); /* quit if not */
	}
    }

    if(argc > 3) {	             /* user tab width and no tabs */
	p = strlwr(argv[3]);
	if(!strcmp(p,"/s") || !strcmp(p,"-s"))
	    use_tabs = 0;
    }


    if(!nspaces) nspaces = 4;     /* if not set, use default tab width */


    fpin = fopen(argv[1],"r");	/* get files ready */
    if(! fpin) {
	fprintf(stderr,"\n\tInput file not found\n");
	exit(1);
    }

    ibuf = (char *) malloc(INBUFSIZE);
    if(!ibuf)  {
	printf("\n\tBuffer allocation error!\n");
	exit(1);
    }

    fpout = fopen(out_file,"w");
    if(! fpout) {
	fprintf(stderr,"\n\tFile creation error\n");
	exit(2);
    }

    obuf = (char *) malloc(OUTBUFSIZE);
    if(!obuf)  {
	printf("\n\tBuffer allocation error!\n");
	exit(1);
    }

    if(setvbuf(fpin,ibuf,_IOFBF,INBUFSIZE))  {
	printf("\n\tRead-file buffer allocation error!\n");
	exit(1);
    }

    if(setvbuf(fpout,obuf,_IOFBF,OUTBUFSIZE))  {
	printf("\n\tWrite-file buffer allocation error!\n");
	exit(1);
    }


    clrscr();
    gotoxy(9,12);
    fprintf(stderr,"Indenting: %s ...\n\n",argv[1]); /* program status */
    fprintf(stderr,"   Output file is: %s\n\n",out_file);


    /* ------------------  File processing Loop  ------------------------- */

    while(! feof(fpin)) {
	if(!get_token()) done();	/* shut down if eof */
	tok =  compare_tokens();
	if(tok != NONE && tok != CASE  && tok != END)
	    first = 0;	/* if keyword, "var" section has passed */
	if(first)
	    fputs(buff,fpout);
	else
	    do_switch();            /* set indent */
    }
    done();		/* close files & quit */
}


/* ------------------  Subordinate Fuctions  --------------------------- */


/*  ---  pad line with spaces or tabs/spaces combination to indent  -- */

int write_spaces()
{
    int i,j;
    char temp[BUFSIZ] = "";


    j = nspaces * x;
    memset(out_line,' ',j);
    out_line[j] = '\0';

    if(use_tabs) {
	i = strlen(out_line) / 8;  	/* number of tabs to use    */
	if(!i) return(1);       /* if less than 8 spaces we're ok  */
	memset(temp,'\t',i);    /* else insert leading tabs       */
	temp[i] = '\0';		  /* null terminate the string   */
	j = strlen(out_line) % 8; /* pad remainder with spaces  */
	memset(&temp[i],' ',j);
	temp[i+j] = '\0';
	strcpy(out_line,temp);	/* tabs/spaces combination complete */
    }

    return(1);		/* done here */

}

/* -----------  save block start position  for alignment  ------------- */

int push()
{
    if(ndx < MAXSTACK) {		/* check for "stack" overflow */
	stack[ndx++] = x;
	return(1);
    }
    else
        return(0);

}

/* --------  retrieve block start position for alignment  ---------- */

int pop()
{
    if((ndx > 0) && (ndx-1 < MAXSTACK)) { /* check "stack" array bounds */
	x = stack[--ndx];
	return(1);
    }
    else
        return(0);

}



int get_token()
{
    char *q;

    out_line[0] = first_word[0] = buff[0] = newbuff[0] = '\0';
    i = 0;
    q = fgets(buff,BUFSIZ,fpin);    /* read a line from the file */
    if(!q) return(0);

    while(isspace(buff[i++]))
	;	                    /* strip leading white space */

    if(i) --i;

    if(!buff[i]) {
	fputs("\n",fpout);	   /* if empty line, copy and get the next */
	if(!get_token()) done();	/* if eof then do exit routine */
	return(2);
    }
    strcpy(newbuff,&buff[i]);  /* newbuff has stripped line */
    sscanf(buff,"%s",first_word);
    q = strtok(first_word,"({;:= ");/* first_word is first token in line */
    strcpy(first_word,q);
    return(1);

}


enum TOKEN_TYPE compare_tokens()
{

    for(z=0;z<TOKENS;z++)
	if(!stricmp(first_word,tokens[z]))
	    return(z);         	   	/* return match */
    return(z);                                      /* or no match */

}


/* ------------ write indented line to file --------------- */

void writeln()
{
    write_spaces();
    strcat(out_line,newbuff);
    fputs(out_line,fpout);
}

/* --------------------  Indent according to keyword  ---------------- */

void do_switch()
{
    int i;

    switch(tok) {
    case IF:     /* determine if single or compound statement */

    case ELSE:

    case FOR:

    case WITH:
    case WHILE:
	writeln();
	x++;
	i = get_token();
	if(!i) done();
	if(i==2) break;
	tok = compare_tokens();
	if(tok == NONE) {
	    writeln();	/* indent single line */
	    x--;
	    break;
	}
	else if(tok == BEGIN || tok == REPEAT || tok == CASE){
	    x--;
	    do_switch();	/* or statement block */
	    break;
	}
	else
	    do_switch();
	break;

    case REPEAT:
    case CASE:
    case BEGIN:
	push();		/* save block start */
	writeln();
	++x;		/* indent 1 tabwidth */
	break;

    case UNTIL:
    case END:
	pop();     	/* align to block start */
	writeln();
	break;

    case PROCEDURE:
    case FUNCTION:
	first = 1;  /*  signal "var" data area */

	x = 0;	  /* & reset indent level to zero */
	ndx = 0;
	memset(stack,0,sizeof stack);
	fputs(newbuff,fpout);
	break;

    case ENDD:
	fputs(newbuff,fpout);
	done();   /* write "end." and shut down */

    case NONE:
	writeln();  /* maintain indent level */
	break;

    default:
	break;	/* unknown condition ? */
    }
}

/* ------------  close files & exit  ------------- */

void done()
{
    fputc(0x1a,fpout);	/* write eof mark & close files */
    fclose(fpout);
    fclose(fpin);
    exit(0);		/* all done */
}

/* -------  identify the program, what it does, and it's usage  ------ */

void show_usage_and_quit()
{
    fprintf(stderr,"\n\t\t\t $$$  Pretty  $$$\n ");
    fprintf(stderr,"\n\tAn auto-indent utility for Pascal Source files.\n");
    fprintf(stderr,"\n\tUsage: pretty filename.pas [tab width] [/s or -s].\n");
    fprintf(stderr,"\n\tAllowed values for tab width are 2 - 8 (default = 4).\n");
    fprintf(stderr,"\n\tIf /s or -s, indent is composed of spaces only.\n");
    fprintf(stderr,"\n\t\t(Default is optimal tabs/spaces).\n");
    fprintf(stderr,"\n\t     Output file will have \".prt\" extension.\n");
    exit(0);
}
