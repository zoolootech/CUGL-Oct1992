#include <stdio.h>
#include <gds.h>
#include <ctype.h>
#include <GRADENV.h>

/*==============================================================*
 *                                                              *
 *    This file contains the program for Draw.                  *
 *    This is a long but simple program so it does not have     *
 *        a lot of comment.                                     *
 *                                                              *
 *==============================================================*/

#define MAXNUMMARK 10

int CURX=0, CURY=0;

struct markpt {
    int x,y;
} MARKER[MAXNUMMARK];   /* markers */

struct func_entry {
    union {
        char name[2];   /* command name */
        int value;
    } func;
    int numparm;
};

static char *str;
static int rvflag = 0;  /*reverse flag */

static struct func_entry FUNTABLE[] =
  { {'R','S', 0}, {'L','F', 1}, {'R','T', 1}, {'U','P', 1},
    {'D','N', 1}, {'U','L', 1}, {'U','R', 1}, {'D','L', 1},
    {'D','R', 1}, {'J','R', 2}, {'J','A', 2}, {'W','R', 0},
    {'E','R', 0}, {'R','V', 0}, {'N','W', 0}, {'H','L', 2},
    {'V','L', 2}, {'S','O', 0}, {'S','Y', 1}, {'A','S', 1},
    {'M','K',-1}, {'W','N', 2}, {'D','Y', 1}, {'C','I', 1},
    {'E','L', 2}, {'B','X', 4}, {'F','C', 1}, {'F','E', 2},
    {'R','E', 2}, {'P','U', 0}, {'E','S', 0}, {'E','O', 1},
    {'F','I', 0} };

#define NUMFUNC sizeof(FUNTABLE)/sizeof(struct func_entry)
#define MAXNUMPARM 4    /* max. number of parameter for all commands */

Draw(drawstr,parm1,parm2)
char *drawstr;
int parm1,parm2;
{
    static int draw_arcspec=0xffff;

    int parm[MAXNUMPARM];
    unsigned int cmd;
    int move,parmcnt,chg,loop,delay,up,drawslot;
    char ch;
    register int chgx,chgy;

    str=drawstr;
    MARKER[0].x=parm1;          /* put parm1 and parm2 into marker 0 */
    MARKER[0].y=parm2;
    chg=1;
    if ((drawslot=EnvSave(0)) == 0) {   /* allocate a environment slot */
        graderror(4,16);
    }

    skipspace();        /* skip over initial spaces if any */
    while (*str != NULL) {
        move=1;
        cmd=toupper(str[0])+((toupper(str[1])) << 8);
        str+=2;
        if (*str == '$') {      /* is there a DON'T MOVE indicator */
            move=0;
            str++;
        }
        skipspace();
        parmcnt=0;
        while(nextvalue(&parm[parmcnt++])) {    /* read parameters */
            skipspace();
            if (*str != ',') {
                parmcnt++;
                break;
            }
            str++;
            skipspace();
        }
        parmcnt--;
        for (loop=NUMFUNC-1; loop>=0; loop--) {
            if (FUNTABLE[loop].func.value==cmd) break;
        }
        if (loop<0) {
            graderror(2,17,cmd>>8,cmd&0xff);
            while (*str != NULL && !isalpha(*str)) str++;
            continue;
        }
        /* test whether correct number of parameters is supplied or not */
        if ((FUNTABLE[loop].numparm!=parmcnt) && (FUNTABLE[loop].numparm>=0)) {
            graderror(2,18,cmd>>8,cmd&0xff);
            continue;
        }
        up=chgx=chgy=0;
        switch (loop) { /* perform required function */
        case 0: /* RS -- reset */
            ResetWin();
            CURX=ORGX;
            CURY=ORGY;
            SetOrg(0,0);
            break;
        case 1: /* LF -- left */
            chgx = -parm[0];
            goto drawline;
        case 2: /* RT -- right */
            chgx = parm[0];
            goto drawline;
        case 3: /* UP -- up */
            chgy = -parm[0];
            goto drawline;
        case 4: /* DN -- down */
            chgy = parm[0];
            goto drawline;
        case 5: /* UL -- up-left */
            chgx = -parm[0];
            chgy = -parm[0];
            goto drawline;
        case 6: /* UR -- up-right */
            chgx = parm[0];
            chgy = -parm[0];
            goto drawline;
        case 7: /* DL -- down-left */
            chgx = -parm[0];
            chgy = parm[0];
            goto drawline;
        case 8: /* DR -- down-right */
            chgx = parm[0];
            chgy = parm[0];
            goto drawline;
        case 9: /* JR -- jump relative */
            chgx = parm[0];
            chgy = parm[1];
drawline:
            if (chg && !up) {
                if (rvflag) Dot(CURX,CURY);
                Line(CURX,CURY,CURX+chgx,CURY+chgy);
            }
            if (CUR_PLOT==2) rvflag=1;
            break;
        case 10: /* JA -- jump absolute */
            chgx = parm[0]-CURX;
            chgy = parm[1]-CURY;
            goto drawline;
        case 11: /* WR -- write */
            rvflag=chg=0;
            goto settype;
        case 12: /* ER -- erase */
            chg=1;
            rvflag=0;
            goto settype;
        case 13: /* RV -- reverse */
            chg=2;
settype:
            PlotType(chg);
            chg=1;
            break;
        case 14: /* NW -- nowrite */
            chg=0;
            break;
        case 15: /* HL -- horizontal line */
            HorzLine(CURX,CURY,parm[0],parm[1]);
            chgx=parm[0];
            break;
        case 16: /* VL -- vertical line */
            VertLine(CURX,CURY,parm[0],parm[1]);
            chgy=parm[0];
            break;
        case 17: /* SO -- set origin */
            RelOrg(CURX,CURY);
            CURX=CURY=0;
            break;
        case 18: /* SY -- set style */
            SetStyle(parm[0]);
            break;
        case 19: /* AS -- arc specification */
            draw_arcspec=parm[0];
            break;
        case 20: /* MK -- mark location */
            if ((parmcnt!=1) && (parmcnt!=3)) {
                graderror(2,18,cmd>>8,cmd&0xff);
                continue;
            }
            if ((parm[0]>=MAXNUMMARK) || (parm[0]<0)) {
                graderror(2,20,parm[0]);
            }
            if (parmcnt==1) {
                MARKER[parm[0]].x=CURX+ORGX;
                MARKER[parm[0]].y=CURY+ORGY;
            } else {
                MARKER[parm[0]].x=parm[1]+ORGX;
                MARKER[parm[0]].x=parm[2]+ORGY;
            }
            break;
        case 21: /* WN -- set window */
            SetWin(CURX,CURY,parm[0],parm[1]);
            break;
        case 22: /* DY -- delay */
            for (loop=parm[0]; loop>0; loop--) 
                for (delay=1000; delay>0; delay--);
            break;
        case 23: /* CI -- circle */
            Arc1(CURX,CURY,parm[0],draw_arcspec);
            break;
        case 24: /* EL -- ellipse */
            Earc1(CURX,CURY,parm[0],parm[1],draw_arcspec);
            break;
        case 25: /* BX -- box */
            Box(CURX,CURY,parm[0],parm[1],parm[2],parm[3]);
            break;
        case 26: /* FC -- filled circle */
            FillCircle(CURX,CURY,parm[0]);
            break;
        case 27: /* FE -- filled ellipse */
            FillEllipse(CURX,CURY,parm[0],parm[1]);
            break;
        case 28: /* RE -- rectangle */
            Rectangle(CURX,CURY,parm[0],parm[1]);
            break;
        case 29: /* UP pen for next command */
            up=2;
            break;
        case 30: /* Save enviroment */
            EnvSave(drawslot);
            break;
        case 31: /* Load environment */
            EnvRsto(drawslot,KEEP_SLOT | parm[0]);
            break;
        case 32: /* Fill */
            SolidFill(CURX,CURY);
            break;
        default:        /* impossible */
            graderror(10,10,__FILE__,__LINE__);
        }
        if (up) up--;   /* still pen up ? */
        if (move) {
            CURX+=chgx;
            CURY+=chgy;
        }
        ch=*str;
        while ((ch != NULL) && !isalpha(ch)) ch=*(++str);
    }
EnvRsto(drawslot,0x7fff); /* Keep all except the slot */
}

/* read next value from the draw string */
static int nextvalue(result)
int *result;
{
    char ch,ch2,ch3;
    int temp,negative;

    ch = *str;
    if ((ch=='%') || (ch == '&')) {  /* marker indicator ? */
        ch3= *(++str);
        if (!isdigit(ch3)) {
            graderror(2,19);
            return(0);
        }
        ch2=*(++str);
        if ((ch2=='X') || (ch2=='x')) {
            *result=MARKER[ch3-'0'].x - ORGX;
            if (ch=='&') *result-=CURX;
        } else if ((ch2=='Y') || (ch2=='y')) {
            *result=MARKER[ch3-'0'].y - ORGY;
            if (ch=='&') *result-=CURY;
        } else
            return(0);
        str++;
        return(1);
    }
    temp=negative=0;
    if (ch=='-') {
        str++;
        negative=1;
    } else if (ch=='+') {
        str++;
    }
    ch= *str;
    if ((ch=='0') && (toupper(str[1])=='X')) { /* Hexadecimal ? */
        str+=2;
        ch= *str;
        while (isxdigit(ch)) {  /* convert to decimal */
            temp=(temp<<4)+(ch<='9'?ch-'0':(ch&0x0f)+9);
            ch= *(++str);
        }
        goto goodret;
    }
    if (isdigit(ch)) {
        do {
            temp=(((temp<<2)+temp)<<1)+ch-'0';
            ch= *(++str);
        } while (isdigit(ch));
goodret:
        *result= negative ? -temp : temp;
        return(1);
    }
    return(0);
}

/* skip until a non-space character */
static skipspace()
{
    char ch;

    ch= *str;
    while (isspace(ch))
        ch= *(++str);
}

