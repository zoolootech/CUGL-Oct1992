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

/*      variable initialization         */

enum e_gt { nogen, bytegen, wordgen, longgen };
enum e_sg { noseg, codeseg, dataseg };

extern int echo;

int	       gentype = nogen;
int	       curseg = noseg;
int        outcol = 0;

struct oplst {
        char    *s;
        int     ov;
        }       opl[] =
		{       {"move",op_move}, {"move",op_moveq}, {"add",op_add},
                {"add",op_addi}, {"add",op_addq}, {"sub",op_sub},
                {"sub",op_subi}, {"sub",op_subq}, {"and",op_and},
                {"or",op_or}, {"eor",op_eor}, {"muls",op_muls},
                {"divs",op_divs}, {"swap",op_swap}, {"beq",op_beq},
                {"bhi",op_bhi}, {"bhs",op_bhs}, {"blo",op_blo},
                {"bls",op_bls}, {"mulu",op_mulu}, {"divu",op_divu},
                {"bne",op_bne}, {"blt",op_blt}, {"ble",op_ble},
                {"bgt",op_bgt}, {"bge",op_bge}, {"neg",op_neg},
                {"not",op_not}, {"cmp",op_cmp}, {"ext",op_ext},
                {"jmp",op_jmp}, {"jsr",op_jsr}, {"rts",op_rts},
                {"lea",op_lea}, {"asr",op_asr}, {"asl",op_asl},
                {"clr",op_clr}, {"link",op_link}, {"unlk",op_unlk},
				{"bra",op_bra}, {"movem",op_movem}, {"pea",op_pea},
                {"cmp",op_cmpi}, {"tst",op_tst}, {"dc",op_dc},
                {0,0} };

putop(op)
int     op;
{       int     i;
        i = 0;
        while( opl[i].s )
                {
                if( opl[i].ov == op )
                        {
                        fprintf(output,"\t%s",opl[i].s);
						if( echo )
							fprintf(stdout,"\t%s",opl[i].s);
						return;
                        }
                ++i;
                }
        printf("DIAG - illegal opcode.\n");
}

putconst(offset) /* put a constant to the output file. */
struct enode    *offset;
{       switch( offset->nodetype )
                {
                case en_autocon:
                case en_icon:
                        fprintf(output,"%d",offset->v.i);
						if( echo )
							fprintf(stdout, "%d",offset->v.i);
						break;
                case en_labcon:
						fprintf(output,"L%04d",offset->v.i);
						if( echo ) fprintf(stdout,"L%04d",offset->v.i);
                        break;
                case en_nacon:
						fprintf(output,"%s",offset->v.p[0]);
						if( echo ) fprintf(stdout,"%s",offset->v.p[0]);
                        break;
                case en_add:
                        putconst(offset->v.p[0]);
						fprintf(output,"+");
						if(echo) fprintf(stdout, "+");
                        putconst(offset->v.p[1]);
                        break;
                case en_sub:
						putconst(offset->v.p[0]);
						fprintf(output,"-");
						if( echo ) fprintf(stdout,"-");
                        putconst(offset->v.p[1]);
                        break;
                case en_uminus:
						fprintf(output,"-");
						if(echo) fprintf(stdout,"-");
                        putconst(offset->v.p[0]);
                        break;
                default:
                        printf("DIAG - illegal constant node.\n");
                        break;
                }
}

putlen(l) /* append the length field to an instruction. */
int     l;
{       switch( l )
                {
                case 0:
                        break;  /* no length field */
                case 1:
                        fprintf(output,".b");
						if( echo ) fprintf(stdout,".b");
						break;
                case 2:
						fprintf(output,".w");
						if(echo) fprintf(stdout,".w");
                        break;
                case 4:
						fprintf(output,".l");
						if(echo) fprintf(stdout,".l");
                        break;
                default:
                        printf("DIAG - illegal length field.\n");
                        break;
                }
}

putamode(ap) /* output a general addressing mode. */
struct amode    *ap;
{       switch( ap->mode )
                {
                case am_immed:
						fprintf(output,"#");
						if(echo) fprintf(stdout,"#");
                case am_direct:
                        putconst(ap->offset);
                        break;
                case am_areg:
						fprintf(output,"A%d",ap->preg);
						if( echo) fprintf(stdout,"A%d",ap->preg);
                        break;
                case am_dreg:
						fprintf(output,"D%d",ap->preg);
						if(echo) fprintf(stdout,"D%d",ap->preg);
						break;
                case am_ind:
						fprintf(output,"(A%d)",ap->preg);
						if(echo) fprintf(stdout,"(A%d)",ap->preg);
						break;
                case am_ainc:
						fprintf(output,"(A%d)+",ap->preg);
						if(echo) fprintf(stdout,"(A%d)+",ap->preg);
						break;
                case am_adec:
						fprintf(output,"-(A%d)",ap->preg);
						if(echo) fprintf(stdout,"-(A%d)",ap->preg);
						break;
                case am_indx:
						putconst(ap->offset);
						fprintf(output,"(A%d)",ap->preg);
						if(echo) fprintf(stdout,"(A%d)",ap->preg);
						break;
                case am_xpc:
                        putconst(ap->offset);
						fprintf(output,"(PC,D%d)",ap->preg);
						if(echo) fprintf(stdout,"(PC,D%d)",ap->preg);
						break;
                case am_indx2:
                        putconst(ap->offset);
						fprintf(output,"(A%d,D%d.l)",ap->preg,ap->sreg);
						if(echo) fprintf(stdout,"(A%d,D%d.l)",ap->preg,ap->sreg);
						break;
                case am_indx3:
                        putconst(ap->offset);
						fprintf(output,"(A%d,A%d.l)",ap->preg,ap->sreg);
						if(echo) fprintf(stdout,"(A%d,A%d.l)",ap->preg,ap->sreg);
						break;
                case am_mask:
                        put_mask(ap->offset);
                        break;
                default:
                        printf("DIAG - illegal address mode.\n");
                        break;
                }
}

put_code(op,len,aps,apd) /* output a generic instruction. */
struct amode    *aps, *apd;
int             op, len;
{       if( op == op_dc )
		{
		switch( len )
			{
			case 1: fprintf(output,"\tdc.b");
					if(echo) fprintf(stdout,"\tdc.b");
					break;
			case 2: fprintf(output,"\tdc.w");
					if( echo ) fprintf(stdout,"\tdc.w");
					break;
			case 4: fprintf(output,"\tdc.l");
					if( echo ) fprintf(stdout,"\tdc.l");
					break;
			}
		}
	else
		{
		putop(op);
        	putlen(len);
		}
        if( aps != 0 )
                {
				fprintf(output,"\t");
				if(echo) fprintf(stdout,"\t");
				putamode( aps );
                if( apd != 0 )
				{
					fprintf(output,",");
					if(echo) fprintf(stdout,",");
					putamode( apd );
					}
                }
		fprintf(output,"\n");
		if(echo) fprintf(stdout,"\n");
}

put_mask(mask)  /* generate a register mask for restore and save. */
int     mask;
{
	unsigned int i, position = 15;
	static int mswitch = 0;
	char *reglist[] = { "A7","A6","A5","A4","A3","A2","A1","A0",
						"D7","D6","D5","D4","D3","D2","D1","D0" };
	i = mask;
	if( mswitch == 1) position = 0;
	while( i >= 1 ) {
		if( (i & 1) == 1 ) fprintf(output, "%s/", reglist[position]);
		i = i >> 1;
		if( mswitch == 1 ) position++; else position--;
	}
	if( mswitch == 1) mswitch = 0; else mswitch = 1;
}

putreg(r) /* generate a register name from a tempref number. */
int     r;
{
	if( r < 8 )
	{
		fprintf(output,"D%d",r);
		if(echo) fprintf(stdout,"D%d",r);
	}
	else
	{
		fprintf(output,"A%d",r - 8);
		if(echo) fprintf(stdout,"A%d",r-8);
	}
}

gen_strlab(s)  /* generate a named label. */
char    *s;
{
	fprintf(output,"%s",s);
	if( echo ) fprintf(stdout,"%s",s);
}

put_label(lab)  /* output a compiler generated label. */
int     lab;
{
	fprintf(output,"L%04d\n",lab);
	if(echo) fprintf(stdout,"L%04d\n",lab);
}

genbyte(val)
int     val;
{       if( gentype == bytegen && outcol < 30) {
				fprintf(output,",%d",val & 0x00ff);
				if(echo) fprintf(stdout,",%d",val & 0x00ff);
                outcol += 4;
                }
        else    {
                nl();
				fprintf(output,"\tDC.B\t%d",val & 0x00ff);
				if( echo) fprintf(stdout,"\tDC.B\t%d",val & 0x00ff);
				gentype = bytegen;
                outcol = 19;
                }
}

genword(val)
int     val;
{       if( gentype == wordgen && outcol < 29) {
                fprintf(output,",%d",val & 0x0ffff);
				if(echo) fprintf(stdout,",%d",val & 0x0ffff);
				outcol += 6;
                }
        else    {
                nl();
				fprintf(output,"\tDC.W\t%d",val & 0x0ffff);
				if(echo) fprintf(stdout,"\tDC.W\t%d",val & 0x0ffff);
				gentype = wordgen;
                outcol = 21;
                }
}

genlong(val)
int     val;
{       if( gentype == longgen && outcol < 28) {
                fprintf(output,",%d",val);
				if(echo) fprintf(stdout,",%d",val);
				outcol += 10;
                }
        else    {
                nl();
				fprintf(output,"\tDC.L\t%d",val);
				if(echo) fprintf(stdout,"\tDC.L\t%d",val);
				gentype = longgen;
                outcol = 25;
                }
}

genref(sp,offset)
SYM     *sp;
int     offset;
{       char    sign;
        if( offset < 0) {
                sign = '-';
                offset = -offset;
                }
        else
                sign = '+';
        if( gentype == longgen && outcol < 55 - strlen(sp->name)) {
                if( sp->storage_class == sc_static)
				{
					fprintf(output,",L%04d%c%d",sp->value.i,sign,offset);
					if(echo) fprintf(stdout,",L%04d%c%d",sp->value.i,sign,offset);
				}
                else
				{
						fprintf(output,",%s%c%d",sp->name,sign,offset);
						if(echo) fprintf(stdout,",%s%c%d",sp->name,sign,offset);
				}
                outcol += (11 + strlen(sp->name));
                }
        else    {
                nl();
                if(sp->storage_class == sc_static)
				{
					fprintf(output,"\tlong\tL%04d%c%d",sp->value.i,sign,offset);
					if(echo) fprintf(stdout,"\tlong\tL%04d%c%d",sp->value.i,sign,offset);
				}
                else
				{
					fprintf(output,"\tlong\t%s%c%d",sp->name,sign,offset);
					if(echo) fprintf(stdout,"\tlong\t%s%c%d",sp->name,sign,offset);
				}
                outcol = 26 + strlen(sp->name);
                gentype = longgen;
                }
}

genstorage(nbytes)
int     nbytes;
{       nl();
		fprintf(output,"\tDS  \t%d\n",nbytes);
		if(echo) fprintf(stdout,"\tDS  \t%d\n",nbytes);
}

gen_labref(n)
int     n;
{       if( gentype == longgen && outcol < 58) {
				fprintf(output,",L%04d",n);
				if( echo ) fprintf(stdout,",L%04d",n);
				outcol += 6;
                }
        else    {
                nl();
				fprintf(output,"\tlong\tL%04d",n);
				if(echo) fprintf(stdout,"\tlong\tL%04d",n);
				outcol = 22;
                gentype = longgen;
                }
}

int  stringlit(s) /* make s a string literal and return it's label number. */
char    *s;
{       struct slit     *lp;
        ++global_flag;          /* always allocate from global space. */
        lp = xalloc(sizeof(struct slit));
        lp->label = nextlabel++;
        lp->str = litlate(s);
        lp->next = strtab;
        strtab = lp;
        --global_flag;
        return lp->label;
}

dumplits() /* dump the string literal pool. */
{       char            *cp;
        while( strtab != 0) {
                cseg();
                nl();
                put_label(strtab->label);
                cp = strtab->str;
                while(*cp)
                        genbyte(*cp++);
                genbyte(0);
                strtab = strtab->next;
                }
        nl();
}

nl()
{       if(outcol > 0) {
				fprintf(output,"\n");
				if(echo) fprintf(stdout,"\n");
                outcol = 0;
                gentype = nogen;
                }
}

cseg()
{       if( curseg != codeseg) {
                nl();
				fprintf(output,"\n*\ttext\n");
				if(echo) fprintf(stdout,"\n*\ttext\n");
                curseg = codeseg;
                }
}

dseg()
{       if( curseg != dataseg) {
                nl();
				fprintf(output,"*\tdata\t2\n");
				if(echo) fprintf(stdout,"*\tdata\t2\n");
                curseg = dataseg;
                }
}

