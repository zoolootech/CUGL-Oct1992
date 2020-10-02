#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <fcntl.h>
#include <GRADIO.h>
#include <GRADVAR.h>

#define TBUFSIZ 16384   /* Text buffer size */
#define MAX_LPP 100     /* Max. number of line per page */

#define FALSE 0
#define TRUE 1
#define ERROR (-1)

int TEXT_EOF=0, REMAIN=0;
char TBUF[TBUFSIZ+1], *PAGEPTR;
char *PAGE[MAX_LPP];

/* global variables init. to default value */
int SWWIDTH=792;        /* width of output, length of paper */
int SWHEIGHT=960;       /* height of output, width of paper */
int VERT_DEN=0, HORZ_DEN=4;  /* vertical and horizontal density */
int LINE_SPACING=11;    /* number of pixels between 2 base lines */
int TOPMARG=36, BOTMARG=36;     /* top and bottom margin */
int LEN=1, LINEPERPAGE;
char fontname[64]="S6X8D.fon";  /* default font name */

int textfile;

int sw_move();

main(argc,argv)
int argc;
char *argv[];
{
    char ch;
    char **tpage, *ptr;
    int ret, frame1, linepos, loop, linecnt, fontnu, tlcnt;

    /* Interpret the options if any */

    argv++;
    while (argc-- > 1 && *(*argv)++=='-') {
        if (**argv) {
            ch= *(*argv)++;
            if (ch!='f')
                ret=atoi(*argv);
        } else {
            ch=0;
        }
        switch(ch) {
        case 'H':
            SWHEIGHT=ret;
            break;
        case 'L':
            if (ret>0) LEN=ret;
            break;
        case 'W':
            SWWIDTH=ret;
            break;
        case 'b':
            BOTMARG=ret;
            break;
        case 'd':
            HORZ_DEN=ret & 0x0f; 
            break;
        case 'f':
            strcpy(fontname,*argv);
            break;
        case 's':
            LINE_SPACING=ret;
            break;
        case 't':
            TOPMARG=ret;
            break;
        case 'v':
            VERT_DEN=ret ? 0x10 : 0;
            break;
        default:
            fprintf(stderr,"Unknow option -%c ** IGNORED **\n",ch);
            break;
        }
        argv++;
    }

    /* Any command line argument remain ? */
    if (argc != 1) {
        /* no more or too much */
        fprintf(stderr,"Usage: swprt [options] textfile\n");
        exit(1);
    }
    (*argv)--;

    /* Open text file */
    if ((textfile=open(argv[0],O_RDONLY | O_BINARY)) == ERROR) {
        fprintf(stderr,"swprt: text file not found\n");
        exit(1);
    }
    LINEPERPAGE=(SWHEIGHT-TOPMARG-BOTMARG)/LINE_SPACING;
    GRADinit();         /* Initialize GRAD environemnt */
    if ((fontnu=LoadFont(fontname)) == 0) {
        fprintf(stderr,"Font file %s not found\n",fontname);
        cleanup(1);
    }
    SelectFont(fontnu);
    SPACING_FUNC=sw_move;
/*    setgraph(); */
    frame1=CreateFrame(SWHEIGHT-TOPMARG, SWWIDTH);
    SelectFrame(frame1);

    while ((linecnt=readpage()) > 0) {  /* while more to print */
        for (loop=LEN; loop>0; loop--) {   /* loop LEN times for each page */
            SetOrg(0,(loop-LEN)*SWWIDTH);
            tpage=PAGE;
            linepos=SWHEIGHT-TOPMARG-LINE_SPACING;   /* starting line pos */
            tlcnt=linecnt;
            while (tlcnt--) {
                WriteStr(TOP_LEFT,DOWN,linepos,0,*tpage++,0,0);
                linepos-=LINE_SPACING;
            }
            PrintFrame(HORZ_DEN+VERT_DEN,(char **) NULL,0,12,0);
/*            getch(); */
            PlotType(1);        /* erase the frame */
            HorzLine(0,0,20000,20000);
            PlotType(0);
        }
        ptr=TBUF;       /* move remaining data to begining of buffer */
        for (loop=REMAIN; loop>0; loop--) {
            *ptr++ = *PAGEPTR++;
        }
        PAGEPTR=TBUF;
    }
    close(textfile);
    cleanup(0);
/*    settext(); */
} 
    

/* read next page of text data */
readpage()
{
    int temp1, linenu, temp2, inkline;

    inkline=0;
    if (!TEXT_EOF) {
        temp1=TBUFSIZ-REMAIN;
        if ((temp2=read(textfile, &TBUF[REMAIN], temp1)) != temp1) {
            TEXT_EOF=TRUE;
        }
        REMAIN+=temp2;
    }
    linenu=0;
    if (REMAIN != 0) {
        PAGEPTR=TBUF;
        for (; (linenu<LINEPERPAGE) && (REMAIN>0); ) {
            PAGE[linenu]=PAGEPTR;
            linenu++;
            for (; REMAIN>0 ;) {
                while (REMAIN-- > 0) {
                    if (*PAGEPTR==12) {         /* is it a FORM FEED ? */
                        *PAGEPTR++='\0';        /* yes, end of page */
                        return(linenu);
                    }
                    if (*PAGEPTR==0) {
                        *PAGEPTR++ = 0x80; /* change 0 in text file to 0x80 */
                        continue;
                    }
                    if (*PAGEPTR++ == '\r')     /* is it a carriage return ? */
                        break;                  /* yes, end of search */
                    inkline=linenu;
                }
                if ((REMAIN>0) && (*PAGEPTR=='\n')) {   /* new line ? */
                    *(PAGEPTR-1)='\0';  /* end of line */
                    *PAGEPTR++;
                    REMAIN--;
                    break;
                }
            }
        }
        if (REMAIN<=0) {        /* any data remain in the buffer ?*/
            linenu=inkline;     /* No more! */
            REMAIN=0;
            *PAGEPTR='\0';
        }
    }
    return(linenu);
}

int sw_move(curx, cury, w, h, remaining)
int *curx, *cury, w, h;
char *remaining;
{
    *cury+=h;   /* increase to next position */
    return(*cury+ORGY > WINY2);  /* if cury > upper limit of window */
                                 /* no need to continue             */
}

