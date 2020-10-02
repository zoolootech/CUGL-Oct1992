/*
	CPRINT, Print a (fairly) neat listing of a c program

	Programmed by Andrew L. Bender, M. D.
	June 24, 1984 - Version 1.0
	June 26, 1984 - Version 1.1
		Fix some bugs in string processing
	July 3, 1984 - Version 1.2
		Allow for printer file device.
		Count source statements

	Permission is hereby given to freely distribute this
	program but it may not be sold.  This program may be
	modified by anyone as long as this notice remains.
	The purpose of this program was to test the CI86
	optimizing c compiler.  It does a clean compilation
	of this program which tests most features of the
	c language as described by Kernighan and Ritchie in
	their book "The C Programming Language."
	CI-86 is a trademark of Computer Innovations,
	Tinton Falls NJ for their implementation of the full
	c programming language as described in Kernighan and 
	Ritchie.

	The listing contains two columns on the left:
		column 1 is the "nesting level" of compound
		statements and column 2 is a rough idea of
		what the line number will be.  It is actually
		a count of "\n" characters.  The heading on
		each page gives the name of the file being
		printed and the date and time it was created.

	If braces, brackets or parenthesis are unbalanced
	this will be reported at the end of the listing.
	A complete syntactical analysis of the program
	would require more effort but would be worth it as
	an exercise of much value in learning the c language
	because of the facility and fluency needed to write
	such a checker.
	A program checker "lint" is quite through in its
	analysis of c programs and is available on most
	UNIX systems.
*/
#include "local.h"
#define	LPP	58
#define	TAB	3
#define	CHPL	74
/* LPP=lines per page
		TAB = number of spaces to indent for a tab
		CHPL = characters to print on each line (74 is very tight)
			because the line and level count take up 8 columns
*/
unsigned int hour,minute,second,month,day,year;
int pagect, level, tabct, lines, stmnt, linect;
typedef int void;
typedef int boolean;
struct regval { int ax, bx, cx, dx, si, di, ds, es; };
struct regval callreg, retreg;
unsigned char title[60];
boolean comment, quote, squote, instrng;
unsigned char lineimage[520];
unsigned char fmt[]=
	{"\fListing of file: %s      Catalogued on %d/%d/%d At %d:%02d:%02d\
           Page  %d \n\n"};
unsigned char output[60]={"PRN:"};

/* top of page routine */

void toppage(pr)
FILE *pr;
{
	fprintf(pr,fmt,title,month,day,year,hour,minute,second,pagect);
	linect=1;
	pagect++;
}

/* main program */

main(argc,argv)
int argc;
unsigned char *argv[];
{
	int i,j,kk,jj,n;
	unsigned char buf[600], fname [254];
	unsigned char k;
	int *pb;
	int lbrace, rbrace, lparen, rparen, lbrack, rbrack;
	extern FILE *fopen();
	extern FILE *fclose();
	extern char *fgets();
	FILE *prn, *fd;
	fprintf(stderr,"C Program Listing Version 1.2\n");
	if (argc<2)
		{
		fprintf(stderr,"Type In Filename To Print: ");
		gets(buf,254,stdin);	/*get user file name if not on control card*/
		pb=index(buf,'\n');
		*pb='\0';
		}
	else strcpy(buf,argv[1]);	/*copy control stmnt string to user bfr*/
	makefnam(buf,".c",fname);	/*convert file name to filename.c*/
	fprintf(stderr,"Listing Of File: %s \n",fname);
	if (argc==3) strcpy(output,argv[2]);	/*copy prt file name to output*/
	prn=fopen(output,"w");	/*open printer*/
	if(prn==NULL) {
		fprintf(stderr,"Assignment of print device: %s failed.",output);
		exit(1);
		}
	fd=fopen(fname,"r");	/*open program file to list*/
	if (fd==NULL) {
		fprintf(stderr,"Cannot Open File: %s \n",fname);
		exit(1);	/*error exit*/
		}
	linect = pagect = 1;	/*set page & line count to one*/
	stmnt = j = rbrack = lbrack = rbrace = lbrace = rparen = lparen = 0;
	callreg.bx =*fd;	/*get file handle*/
	callreg.ax = 0x5700 ; /*get file data  */
	sysint21(&callreg,&retreg);	/*fetch file date and time*/

/* n.b. -> ci86 c documentation is a bit confusing here.
 the structure is naturally a memory area so it conflicts
 in that sense with the register lo-hi multiplexing */

	month=(retreg.dx & 0x01E0)>>5;
	day=retreg.dx & 0x001F;
	year=((retreg.dx >>9)& 0x00EF)+80;
	hour=(retreg.cx >>11) & 0x001F;
	minute=(retreg.cx & 0x07E0)>>5;
	second=(retreg.cx & 0x001F)<<1;	/* multiply by 2 */ 
	strcpy(title,fname);
	comment=squote=instrng=quote=FALSE;
	toppage(prn);	/*take top of page action*/
	while(fgets(lineimage,519,fd)!=NULL){
		jj=strlen(lineimage);
		for(i=0;k=lineimage[i],k!='\0'&& i<jj;i++){
			kk=k;
			if (kk=='\n')
				{
				lines++;
				}
			if (comment==FALSE && instrng==FALSE) {
/* switch is entered when processing outside a comment
or string.  If in a comment or string (either " or ') then
the if statement drops thru to the else at the
conclusion of the loop to turn processing back on
In the case of alphanumerics the loop is totally ignored.
*/
				if (isalnum(k)==0){ /* don't switch on alphanumerics */
				switch (kk)	{
			case '{':
				level++;
				lbrace++;
				break;
			case '}':
				rbrace++;
				level--;
				break;
			case '(':
				lparen++;
				break;
			case ')':
				rparen++;
				break;
			case '[':
				lbrack++;
				break;
			case ']':
				rbrack++;
				break;
			case '/':
				comment= lineimage[i+1]=='*';
				break;
			case ';':
				stmnt++;
				break;
			case '\'':
				quote=!squote;
				instrng=quote|squote;
				break;
			case '"':
				squote= !quote;
				instrng= squote|quote;
				break;
					}
				}
			}
			else { /*if comment or instrng == TRUE*/
				if (comment==TRUE && k=='*' && lineimage[i+1]=='/')
					comment=FALSE;
				if (quote==TRUE && k=='\'' &&
				(lineimage[i-1] != '\\' ||
				(lineimage[i-2]=='\\' && lineimage[i-1]=='\\')))
					quote=FALSE;
				if (squote==TRUE && k=='"') squote=FALSE;
				instrng = quote | squote;
			}
		if (k=='\t') for(n=j;buf[j++]=' ',j<n+TAB;);
		else buf[j++]=k;
		if (j>CHPL || k == '\n'){
			if (k != '\n') 
				buf[j++]='\n';
			buf[j]='\0';
			fprintf(prn,"%2d%4d: %s",level,lines,buf);
			j=0;
			if (linect++>LPP) toppage(prn);
			}
		}
	}
fclose(fd);
if (level==0 && rbrack==lbrack && rparen==lparen)
	{
	fprintf(prn,"\n%d Statements in source code.  \
Braces, brackets and parenthesis are balanced.\n",stmnt);
	}
else
	{
	fprintf(prn,"\nUnbalanced pairs { %d, } %d, [ %d, ] %d, ( %d, ) %d\n",
	lbrace,rbrace,lbrack,rbrack,lparen,rparen);
fclose(prn);
	}
}
