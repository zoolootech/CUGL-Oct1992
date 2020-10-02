/*	>>>> start of cc3 <<<<<<<<<	*/
/*					*/
#ifndef TRUE	/* see if need include file */
#include <C.DEF>
#endif
/*	Perform a function call		*/
/*					*/
/* called from heir11, this routine will either call	*/
/*	the named function, or if the supplied ptr is	*/
/*	zero, will call the contents of HL		*/
callfunction(ptr)
char *ptr;      /* symbol table entry (or 0) */
{
	int nargs;
	nargs=0;
	blanks();		/* already saw open paren */
	if (ptr==0) push();	/* calling HL */
	while (streq(line+lptr,")") == 0) {
		if (endst()) break;
		expression();	/* get an argument */
		if (ptr == 0) swapstk(); /* don't push addr */
		push();		/* push argument */
		nargs=nargs+2;	/* count args*2 */
		if (match(",") == 0) break;
		}
	needbrack(")");
	if (ptr) call(ptr); else callstk();
	sp=modstk(sp+nargs);    /* clean up arguments */
	}
junk()
{       if(an(inbyte())) while(an(ch())) gch();
	else while(an(ch()) == 0) {
		if(ch() == 0) break;
		gch();
		}
	blanks();
	}
endst()
{
	blanks();
	return ((streq(line+lptr,";") ||(ch() == 0)));
	}
suberror(sname)
char *sname;
{
	char msg[80];

	strcpy(msg,"identifier \"");
	strcat(msg,sname);
	strcat(msg,"\" is not a vaild array base");
	error_prt(msg);
	while(1) {
		if (endst()) return;
		if (ch()==']') {
			gch();
			return;
			}
		gch();
		}
	}

illname()
{
	error("illegal symbol name");
	junk();
	}
undefine(sname)
char *sname;
{
	char msg[80];
	strcpy(msg,"undefine identifier \"");
	strcat(msg,sname);
	strcat(msg,"\"");
	error_print(msg);
	}
/*						*/
/*	written By Mike Bernson 4/81		*/
/*						*/
multidef(sname)
char *sname;
{
	char msg[80];
	strcpy(msg,"already defined symbol or function \"");
	strcat(msg,sname);
	strcat(msg,"\"");
	error_print(msg);
	}
needbrack(str)
char *str;
{
	if (!match(str)) error("missing braket");
	}
needlval()
{
	error("must be lvalue");
	}
findglb(sname)
char *sname;
{
	char *ptr;
	ptr=startglb;
	while(ptr!=glbptr) {
		if (astreq(sname,ptr,namemax)) return ptr;
		ptr=ptr+symsiz;
		}
	return 0;
	}
findloc(sname)
char *sname;
{
	char *ptr;
	ptr=startloc;
	while(ptr!=locptr) {
		if(astreq(sname,ptr,namemax))return ptr;
		ptr=ptr+symsiz;
		}
	return 0;
	}
/* Test if next input string is legal symbol name */
symname(sname)
char *sname;
{
	int k;char c;
	blanks();
	if (!alpha(ch())) return 0;
	k=0;
	while(an(ch())) sname[k++]=gch();
	sname[k]=0;
	return 1;
	}
/* Return next avail internal label number */
getlabel()
{
	return (++nxtlab);
	}
/* Print label at start of line */
sprintlabel(label)
int label;
{
	printlabel(label);
	col();
	nl();
	}
/* Print specified number as label */
printlabel(label)
int label;
{
	outstr("@");
	outdec(label);
	}
/* Test if given character is alpha */
alpha(c)
char c;
{
	return((c>='a' && c<='z') ||
		(c>='A' && c<='Z') || c=='_');
	}
/* Test if given character is numeric */
numeric(c)
char c;
{
	return(c>='0' && c<='9');
	}
/* Test if given character is alphanumeric */
an(c)
	char c;
{       return(alpha(c) || numeric(c));
	}
/* Print a carriage return and a string only to console */
pl(str)
char *str;
{
	putch('\r');
	while (*str) putch(*str++);
	}
addwhile(ptr)
	int ptr[];
{
	int k;
	if (wqptr==wqmax) {
		error("too many active whiles");
		return;
		}
	k=0;
	while (k<wqsiz) *wqptr++ = ptr[k++];
	}
delwhile()
{
	if (readwhile()) wqptr=wqptr-wqsiz;
	}
readwhile()
{
	if (wqptr==wq) {
		error("no active while");
		return 0;
		}
	else return wqptr-wqsiz;
	}
ch()
{
	return line[lptr];
	}
nch()
{
       if(ch() == 0) return 0;
	else return(line[lptr+1]);
	}
gch()
{       if(ch() == 0) return 0;
	else return line[lptr++];
	}
kill()
{       lptr=0;
	line[0]=0;
	}
inbyte()
{
	while (!line[lptr]) {
		if (eof) return 0;
		inline();
		preprocess();
		}
	return gch();
	}
inchar()
	{
	if (!line[lptr]) inline();
	if (eof) return 0;
	return (gch());
	}
inline()
{
	int k;
	char *unit;
	while(1) {
		kill();
		if (input==0)openin();
		if (eof) return;
		if (input2) {
			unit=finp2;
			line2++;
			}
		else {
			unit=finp;
			line1++;
			}
		while((k=getc(unit)) != ERROR) {
			if((k=='\n') || (lptr>=linemax)
				|| (k==26)) break;
			if (k != '\r') line[lptr++]=k;
			}
		line[lptr]=0;   /* append null */
		if(k<=0 || k==26) {
			if (input2) {
				close(input2);
				input2=0;
				}
			else {
				close(input);
				input=0;
				}
			}
		if(lptr) {
			if(ctext && cmode) {
				comment();
				tab();
				outstr(line);
				nl();
				}
			lptr=0;
			return;
			}
		}
	}
zs,j;
	char c;
	j=zs=0;
	k=10000;
	if (number<0) {
		num