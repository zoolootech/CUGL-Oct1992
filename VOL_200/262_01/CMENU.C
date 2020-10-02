/********************************************************
 *              CMENU (Assembler Version)               *
 *                                                      *
 *        A compiler for menu driven programs           *
 *                  by Robert Ramey                     *
 ********************************************************/
#include "stdio.h"
#include "symdef.h"

#define QUOTE   '\''    /* character used for quote by assembler */
#define MAXDEPTH 10 /* Maximum number of digits in menu code */
#define MAXLINE 128 /* Maximum length of any line in menu */
#define MAXMENUS 50 /* Maximum number of menus */
#define MAXSIZE 11  /* Space reserved for a symbol */

SYMBOLTABLE *mtable;    /* Accumulates menus */
SYMBOLTABLE *atable;    /* Stores external symbols */

typedef struct mline {
    struct mline *link;
    char mtext[2];
    } MLINE;


char *marray[MAXMENUS];
int mindex;
int strcmp();
char *strmov();

main(argc,argv)
int argc;
char *argv[];
{
    moat(2000);
    mtable = symmk(sizeof(MLINE *), MAXMENUS);
    if(!mtable){
        fprintf(stderr, "Not enough memory");
        exit(1);
    }
    load();
    qsort(marray, mindex + 1, strcmp);
    generate(); /* generate c code for menus */
}
load(){
    char line[MAXLINE];
    MLINE *last, *tptr; /* last element allocated in menu list */
    int i, errflag;
    
    errflag = FALSE;

    /* load nul symbol corresponding to root menu */
    marray[mindex = 0] = symadd(mtable, "");
    last = NULL;
    while(gets(line)){
        if(isspace(line[0])){
            if(errflag) continue;
            for(i = 1;isspace(line[i]);++i);
            tptr =
            malloc(sizeof(char *) + strlen(line + i) + 1);
            if(last)
                last->link = tptr;
            else
                *(MLINE **)
                symdat(mtable,marray[mindex]) = tptr;
            last = tptr;
            last->link = NULL;
            strmov(last->mtext, line + i);
        }
        else{
            if(symlkup(mtable, line)){
                fprint(stderr,"%s menu repeated\n");
                errflag = TRUE;
            }
            else{
                marray[++mindex] =symadd(mtable, line);
                last = NULL;
                errflag = FALSE;
            }
        }
    }
}
generate(){
    MLINE *mptr;
    char sfix[2],c, cbuffer[MAXDEPTH], *current, *next;
    char s1[MAXLINE], s2[MAXSIZE], s3[MAXSIZE];
    int litcount, i;

    /* preamble */
    printf(";Compiled by CMENU\n");
    printf("\t.HD64\n");
    printf("\tDSEG\n");
    printf("\tPUBLIC\tM?\n");
    printf("\tEXTRN\tMENU?,ACTION?,HIST?\n");

    atable = symmk(0, MAXMENUS * 4);
    if(!atable){
        fprintf(stderr, "Not enough memory");
        exit(1);
    }
    symadd(atable,"MENU?");
    symadd(atable,"ACTION?");
    symadd(atable,"HIST?");
    i = 0;
    next = marray[mindex];
    do{
        current = next;
        if(mindex){
            next = marray[mindex - 1];
            if(strncmp(next, current, strlen(current) - 1))
                printf("menu %s has no father\n"
                    ,current);
        }

        /* generate external statements for action routines */
        c = '\t';
        mptr = *(MLINE **)symdat(mtable, current);
        while(mptr = mptr->link){
            /* check for continuation lines */
            if(mptr->mtext[1] != '.') continue;
            if(1 < mksym(mptr->mtext, s1, s2, s3)
            && !symlkup(atable, s2)){
                if(c == '\t')
                    printf("\tEXTRN");
                putchar(c);
                printf(s2);
                symadd(atable, s2);
                c = ',';
            }
        }
        if(c != '\t')
            putchar('\n');

        /* generate menu data structures */
        litcount = i;
        mptr = *(MLINE **)symdat(mtable, current);
        printf("M%s?:\tDW\tL%d?\n",current,++i);
        while(mptr = mptr->link){
            /* check for continuation lines */
            if(mptr->mtext[1] != '.') continue;
            printf("\tDW\tL%d?,",++i);
            switch(mksym(mptr->mtext,s1,s2,s3)){
            case 3:
            case 2:
                printf("%s,%s\n", s2, s3);
                break;
            case 1:
                sfix[0] = mptr->mtext[0];
                sfix[1] = NULL;
                strmov(strmov(cbuffer, current),sfix);
                if(symlkup(mtable, cbuffer)){
                    printf("MENU?,M%s?\n",cbuffer);
                }
                else
                    printf("ACTION?,HIST?\n");
            }
        }
        printf("\tDW\t0\n");

        /* generate literals */
        i = litcount;
        mptr = *(MLINE **)symdat(mtable, current);
        printf("L%d?:",++i);
        while(mptr){
            printf("\tDB\t\'");
            qout(mptr->mtext);
            mptr = mptr->link;
            /* figure in continuation lines */
            if(mptr == NULL)
                printf("',0\n");
            else
            if(mptr->mtext[1] == '.')
                printf("',0\nL%d?:",++i);
            else
                printf("',%d,%d\n", CR, LF);
        }
    }
    while(mindex--);
    printf("\tEND\n");
}
/**************************************************************
qout - output a quoted string. QUOTE character gets special
treatment.
***************************************************************/
qout(s)
char *s;
{
    char c;
    while(c = *s++){
        switch(c){
        case QUOTE:
            printf("%c,%d,%c",QUOTE,QUOTE,QUOTE);
            break;
        case ';':
            return;
        default:
            putchar(c);
        }
    }
}
int
mksym(instring, sa)
char *instring, *sa;
{
    char c, *cptr,*s, **sb;
    int i, j;

    sb = &sa;
    s = *sb++;
    cptr = ";()";
    c = *cptr++;
    i = 0;
    while(i < 3){
        if(*instring == NULL){
            ++i;
            *s = NULL;
            break;
        }
        if(*instring == c){
            ++i;
            *s = NULL;
            s = *sb++;
            c = *cptr++;
            ++instring;
        }
        else
            *s++ = *instring++;
    }
    for(j = i;j < 3;++j)
        *sb++ = "0";
    sb = &sa + 1;
    for(j = 1;j < 3;++j, ++sb){
        if(isalpha(**sb)){
            strupr(*sb);
            strcat(*sb, "?");
        }
        else
        if(**sb == NULL)
            strmov(*sb, "0");
    }
    return i;
}
     }
        if(*in