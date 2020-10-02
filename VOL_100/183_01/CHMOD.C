
 /***********************************************************************/
 /* chmod - exercise CHMOD DOS function call (#43)                      */
 /***********************************************************************/
 /* (c) copyright Jerry M. Carlin 8/14/1983                             */
 /* permission given to use or distribute so long as no money charged.  */
 /* If questions, call 415-680-7590.                                    */
 /***********************************************************************/

 /*#define DEBUG 1*/

#include "stdio.h"

#define CARRY  1        /* 8086 flags carry bit */
#define FCALL  0x21     /* function call int per DOS 2.0 manual pg D-2 */

#define CHMOD  0x43     /* DOS 2.0 Change File Mode pg D-39 */
#define SETDTA 0x1A     /* Set Disk Transfer Address pg D-26 */
#define FFIRST 0x4E     /* DOS 2.0 Find First File pg D-49 */
#define FNEXT  0x4F     /* DOS 2.0 Find Next File pg D-50 */

 /* file attributes from DOS 2.0 manual page C-4 */

#define RO     01       /* read only */
#define HIDE   02       /* hidden file */
#define SYS    04       /* system file */
#define VOLLBL 08       /* special volume label - in first 11 bytes */
#define SUBDIR 0x10     /* defines a subdirectory */
#define ARCHV  0x20     /* archive bit */

char *index();

struct regval {
        unsigned ax,bx,cx,dx,si,di,ds,es;
} iregs, oregs, fileregs;

struct segreg {
        unsigned cs,ss,sds,ses;
} sr;

main(ac,av)
int ac;
char *av[];
{
        char attr[9];           /* file attributes */
        int flags;              /* 8086 flag register */
        char buff[128];         /* disk I/O buffer */
        char fullname[128];     /* full file name including drive and dir */
        char ddprefix[120];     /* drive and directory info */
        int i;

        bdos(SETDTA,buff);      /* Set Data Transfer Address */
        iregs.ax = 17152;       /* set ah = 43 */
        segread(&sr);           /* get stack & data seg regs */
        iregs.ds = sr.sds;
        iregs.es = sr.ses;
        fileregs.ds = iregs.ds;
        fileregs.es = iregs.es;
        fileregs.ax = 0x4E00;                   /* find first */
        fileregs.cx = HIDE | SYS | SUBDIR;      /* allow any file */

        if (strcmp(av[1],"help") == 0 || strcmp(av[1],"?") == 0 || ac == 1)
        {
                printf("\nchmod [-[r][h][s][a]] filename.ext ");
                printf("[filename.ext...]\n");
                printf("\nUse the '-' option to set file modes. Omit");
                printf(" the '-' option for mode display.\nA dash by ");
                printf(" itself (chmod - filename) sets normal mode.\n\n");
                printf("Settable flags:\n  r = read only\n");
                printf("  h = hidden\n  s = system");
                printf("\n  a = archive\n\nAny flag not set will be reset.");
                printf(" This means that if you do not, \nfor example, set");
                printf(" read-only the file will be write enabled.\n\n");
                printf("DOS global filename chars '*' and '?' may be used.\n");
                printf("\nMode display includes 'w' for write ok and ");
                printf("'d' for a subdirectory.\n\n");
                exit(0);
        }
        if (av[1][0] == '-')    /* set mode */
        {
                iregs.ax |= 1;  /* set update bit */
                iregs.cx = 0;
                if (index(av[1],'r') != 0 || index(av[1],'R') != 0)
                        iregs.cx |= RO;
                if (index(av[1],'h') != 0 || index(av[1],'H') != 0)
                        iregs.cx |= HIDE;
                if (index(av[1],'s') != 0 || index(av[1],'S') != 0)
                        iregs.cx |= SYS;
                if (index(av[1],'a') != 0 || index(av[1],'A') != 0)
                        iregs.cx |= ARCHV;
#ifdef DEBUG
                printf("fun code =%x\n",iregs.ax);
                printf("set mode =%x\n",iregs.cx);
#endif
                ++av;   /* skip over set mode word */
                --ac;
                while (--ac > 0)
                {
                        fileregs.dx = *++av;
                        flags = sysint(FCALL,&fileregs,&oregs);
                        if (oregs.ax)
                        {
                                perror(fileregs.dx,oregs.ax);
                                continue;
                        }
                        buildp(*av,ddprefix);
                        while (oregs.ax == 0)
                        {
                                strcpy(fullname,ddprefix);
                                strcat(fullname,buff+30);
                                iregs.dx = fullname;
                                flags = sysint(FCALL,&iregs,&oregs);
                                if (flags & CARRY)
                                        perror(iregs.dx,oregs.ax);
                                oregs.ax = bdos(FNEXT,0) & 0xff;
                        }
                }
                exit(0);
        }

 /**********************************/
 /* else report on current setting */
 /**********************************/

        while (--ac > 0)
        {
                fileregs.dx = *++av;
                flags = sysint(FCALL,&fileregs,&oregs);   /* Find First */
                if (oregs.ax)
                {
                        perror(fileregs.dx,oregs.ax);
                        continue;
                }
                i = 0;
                buildp(*av,ddprefix);
                while (i == 0)
                {
                        strcpy(fullname,ddprefix);
                        strcat(fullname,buff+30);       /* see pg D-49 */
                        iregs.dx = fullname;            /* next file */
                        flags = sysint(FCALL,&iregs,&oregs);
#ifdef DEBUG
                        printf("file     = %s\n",iregs.dx);
                        printf("err code = %x\n",oregs.ax);
                        printf("8086flags= %x\n",flags);
                        printf("mode byte= %d\n",oregs.cx);
#endif
                        if (flags & CARRY)
                        {
                                perror(iregs.dx,oregs.ax);
                                break;
                        }
                        if (oregs.cx & RO)
                                attr[0] = 'r';
                        else
                                attr[0] = 'w';
                        if (oregs.cx & HIDE)
                                attr[1] = 'h';
                        else
                                attr[1] = '-';
                        if (oregs.cx & SYS)
                                attr[2] = 's';
                        else
                                attr[2] = '-';
                        if (oregs.cx & SUBDIR)
                                attr[3] = 'd';
                        else
                                attr[3] = '-';
                        if (oregs.cx & ARCHV)
                                attr[4] = 'a';
                        else
                                attr[4] = '-';
                        attr[5] = '\0';
                        printf("%s %s\n",attr,iregs.dx);
                        i = bdos(FNEXT,0) & 0xff;  /* Find Next */
                }
        }
        exit(0);
}

 /*****************************************************************/
 /* print error - see error return table DOS 2.0 Manual page D-14 */
 /*****************************************************************/

perror(s,code)
char *s;
unsigned code;
{
        printf("chmod: File %s",s);
        switch(code) {
        case 2:
        case 18: /* perror called with 18 if bad filename entered */
                printf(" not found.\n");
                break;
        case 3:
                printf(". Path not found.\n");
                break;
        case 5:
                printf(". Access denied.\n");
                break;
        default:
                printf(". DOS error code %u returned.\n",code);
        }
        return;
}

 /********************************************************/
 /* build prefix str containing drive and directory info */
 /* needed since FFIRST DTA does not store this info     */
 /********************************************************/

buildp(fullname,ddprefix)
char *fullname; /* full path name (input) */
char *ddprefix; /* drive and directory info */
{
#ifdef DEBUG
        char *savep;
        savep = ddprefix;
#endif
        while ((index(fullname,':') != 0) || (index(fullname,'\\') != 0))
                *ddprefix++ = *fullname++;
        *ddprefix = '\0';
#ifdef DEBUG
        printf("Prefix=%s\n",savep);
#endif
}


fix++ = *fullname++;
        *ddprefix = '\0';
#ifdef DEBUG
        pri