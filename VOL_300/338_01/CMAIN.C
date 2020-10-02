#include        <stdio.h>
#include        "c.h"
#include        "expr.h"
#include        "gen.h"
#include        "cglbdec.h"

/*
 *	68000 C compiler
 *
 *	Copyright 1984, 1985, 1986 Matthew Brandt.
 *  all commercial rights reserved.
 *
 *	This compiler is intended as an instructive tool for personal use. Any
 *	use for profit without the written consent of the author is prohibited.
 *
 *	This compiler may be distributed freely for non-commercial use as long
 *	as this notice stays intact. Please forward any enhancements or questions
 *	to:
 *
 *		Matthew Brandt
 *		Box 920337
 *		Norcross, Ga 30092
 */

char            infile[20],
                listfile[20],
                outfile[20];
extern TABLE    tagtable;
int		mainflag, echo;
extern int      total_errors;

main(argc,argv)
int     argc;
char    **argv;
{
		printf("\n68000 C Compiler r2.00 for UNIX. Copyright Mathew Brandt 1984/85/86\n");
		printf("Adpated to MSDOS by B Brown, October 1989.\n");
	   if( argc == 1 )
	   {
			printf("\n\n usage: cc68k -options sourcefile.c\n\n");
			exit(0);
	   }
	   while(--argc) {
                if( **++argv == '-')
                        options(*argv);
                else if( openfiles(*argv)) {
						printf("Source file : %s\n", infile);
						printf("Asm file    : %s\n", outfile);
						printf("Listfile    : %s\n", listfile);
						lineno = 0;
                        initsym();
                        cgetch();
                        getsym();
                        compile();
                        summary();
                        release_global();
                        closefiles();
                        }
                }
}

int	options(s)
char	*s;
{
	s++;
	if( *s == 'E' || *s == 'e' )  echo = 1;  else echo = 0;
	return 0;
}

int     openfiles(s)
char    *s;
{       char    *p;
        int     ofl, lfl;
        strcpy(infile,s);
        strcpy(listfile,s);
        strcpy(outfile,s);
        makename(listfile,".lis");
		makename(outfile,".asm");
        if( (input = fopen(infile,"r")) == 0) {
                printf(" cant open %s\n",infile);
                return 0;
                }
        ofl = creat(outfile,-1);
        if( ofl < 0 )
                {
                printf(" cant create %s\n",outfile);
                fclose(input);
                return 0;
                }
        if( (output = fdopen(ofl,"w")) == 0) {
                printf(" cant open %s\n",outfile);
                fclose(input);
                return 0;
                }
        if( (list = fopen(listfile,"w")) == 0) {
                printf(" cant open %s\n",listfile);
                fclose(input);
                fclose(output);
                return 0;
                }
        return 1;
}

makename(s,e)
char    *s, *e;
{       while(*s != 0 && *s != '.')
                ++s;
        while(*s++ = *e++);
}

summary()
{       printf("\n -- %d errors found.",total_errors);
        fprintf(list,"\f\n *** global scope symbol table ***\n\n");
        list_table(&gsyms,0);
        fprintf(list,"\n *** structures and unions ***\n\n");
        list_table(&tagtable,0);
}

closefiles()
{       fclose(input);
		fprintf(output, "\n*LIBRARY ROUTINES FOR LONG MULTIPLY AND SWITCH STATEMENT FOLLOW\n\n");
		fprintf(output, "*  c%lmul  long signed multiply\n");
		fprintf(output, "*  multiplies two long operands on the stack\n");
		fprintf(output, "*  and returns the result on the stack with no garbage.\n");
		fprintf(output, "*  global  clmul\n");
		fprintf(output, "clmul: \n");
		fprintf(output, "     movem.l d0/d1/d2/d3/,-(a7)          ;save registers\n");
		fprintf(output, "     move.l  20(a7),d0                   ;get parameter 1\n");
		fprintf(output, "     move.w  d0,d2\n");
		fprintf(output, "     move.w  d0,d1\n");
		fprintf(output, "     ext.l   d1\n");
		fprintf(output, "     swap    d1\n");
		fprintf(output, "     swap    d0\n");
		fprintf(output, "     sub.w   d0,d1\n");
		fprintf(output, "     move.w  26(a7),d0             ;get msw of parameter 2\n");
		fprintf(output, "     move.w  d0,d3\n");
		fprintf(output, "     ext.l   d3\n");
		fprintf(output, "     swap    d3\n");
		fprintf(output, "     sub.l   24(a7),d3             ;subtract lsw of parameter 2\n");
		fprintf(output, "     muls    d0,d1\n");
		fprintf(output, "     muls    d2,d3\n");
		fprintf(output, "     add.w   d1,d3\n");
		fprintf(output, "     muls    d2,d0\n");
		fprintf(output, "     swap    d0\n");
		fprintf(output, "     sub.w   d3,d0\n");
		fprintf(output, "     swap    d0\n");
		fprintf(output, "     move.l  d0,24(a7)\n");
		fprintf(output, "     move.l  16(a7),20(a7)          ;move return address\n");
		fprintf(output, "     movem.l (a7)+,d0/d1/d2/d3/     ;restore registers\n");
		fprintf(output, "     add.w   #4,a7                  ;adjust stack\n");
		fprintf(output, "     rts\n");
		fprintf(output, "\n\n*       c%switch - execute c switch statement\n");
		fprintf(output, "*       the switch table is encoded as follows:\n");
		fprintf(output, "*               long    label1,case1\n");
		fprintf(output, "*               long    label2,case2\n");
		fprintf(output, "*               long    label3,case3\n");
		fprintf(output, "*               ... for all cases\n");
		fprintf(output, "*               long    0,defaultcase\n");
		fprintf(output, "*\n*       the case variable is passed in d0\n");
		fprintf(output, "*       global  c%switch\n");
		fprintf(output, "cswitch:\n");
		fprintf(output, "        move.l  (a7)+, a0              ;get table address\n");
		fprintf(output, "csw1:\n");
		fprintf(output, "        move.l  (a0)+, a1              ;get a label\n");
		fprintf(output, "        move.l  a1, d1                 ;test it for default\n");
		fprintf(output, "        beq     csw2                   ;jump if default case\n");
		fprintf(output, "        cmp.l   (a0)+,d0               ;see if this case\n");
		fprintf(output, "        bne     csw1                   ;next case if not\n");
		fprintf(output, "        jmp     (a1)                   ;jump to case\n");
		fprintf(output, "csw2:\n");
		fprintf(output, "        move.l  (a0), a0              ;get default address\n");
		fprintf(output, "        jmp     (a0)                   ;jump to default case\n\n");

		fprintf(output,"\n\tEND\n\n");
		fclose(output);
        fclose(list);
}

