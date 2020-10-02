/********************************************************
 *                      C M E N U                       *
 *                                                      *
 *         A compiler for menu driven programs          *
 *                   by Robert Ramey                    *
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
    printf("#include menu.h\n");
    printf("extern char hist[];\n");

    atable = symmk(0, MAXMENUS * 4);
    if(!atable){
        fprintf(stderr, "Not enough memory");
        exit(1);
    }
    symadd(atable,"menu");
    symadd(atable,"action");
    symadd(atable,"hist");
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
        c = ' ';
        mptr = *(MLINE **)symdat(mtable, current);
        while(mptr = mptr->link){
            /* check for continuation lines */
            if(mptr->mtext[1] != '.') continue;
            if(1 < mksym(mptr->mtext, s1, s2, s3)
            && !symlkup(atable, s2)){
                if(c == ' ')
                    printf("extern int");
                putchar(c);
                printf("%s()",s2);
                symadd(atable, s2);
                c = ',';
            }
        }
        if(c != ' ')
            printf(";\n");

        /* generate menu data structures */
        mptr = *(MLINE **)symdat(mtable, current);
        if(mindex)
            printf("static ");
        printf("struct {\n");
        printf("\tchar *question = \"");
        for(;;){
            printf("%s",mptr->mtext);
            mptr = mptr->link;
            if(mptr->mtext[1] != '.')
                printf("\\n\\\n");
            else{
                printf("\";\n");
                break;
            }
        }
        printf("\tMLINE m[] = ");
        c = '{';
        do{
            switch(mksym(mptr->mtext,s1,s2,s3)){
            case 3:
            case 2:
                printf("%c\n\t\t{\"%s\",%s,%s}"
                , c, s1, s2, s3);
                break;
            case 1:
                printf("%c\n\t\t{\"%s\"",c,s1);
                sfix[0] = mptr->mtext[0];
                sfix[1] = NULL;
                strmov(strmov(cbuffer, current),sfix);
                if(symlkup(mtable, cbuffer)){
                    printf(",menu,&m%s}",cbuffer);
                }
                else
                    printf(",action,&hist}");
            }
            c = ',';
        }
        while(mptr = mptr->link);
        printf(";\n\t};\n\tint eos = NULL;\n");
        printf("} m%s;\n", current);
    }
    while(mindex--);
}
/************************************************************
mksym - separate action routine name and argument.
*************************************************************/
int
mksym(instring, sa)
char *instring, *sa;
{
    char c, *cptr,*s, **sb, tstring[MAXSIZE];
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
        if(**sb == NULL)
            strmov(*sb, "0");
    }
    sb = &sa + 2;
    if(isalpha(**sb)){
        strmov(strmov(tstring,"&"),*sb);
        strmov(*sb, tstring);
        *sb = tstring;
    }
    return i;
}
