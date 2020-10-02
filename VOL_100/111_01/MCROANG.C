/*
HEADER:		;
TITLE:		MicroAngelo;
VERSION:	1.0;
DATE:		02/28/1982;

DESCRIPTION:	"Creates new character sets for the MicroAngelo graphics
		board.  Compilation requires MCROANG.H.";

KEYWORDS:	MicroAngelo, graphics;
SYSTEM:		CP/M-80;
FILENAME:	MCROANG.C;
SEE-ALSO:	MCROANG.H;
AUTHORS:	Joe Shannon;
COMPILERS:	BDS C;
*/
/***********************************************************************

Version 1.0, 2/28/1982:
	This program is released to public domain with the
	request that author credit be retained in the source.
	Joe Shannon, 566 Rock Road, State College, PA 16801
	(814) 238-0029

***********************************************************************/

#include <BDSCIO.H>
#include "MCROANG.H"
#define	MBASX	290
#define MBASY	60
#define	ABASEC	17
#define	ABASER	8
#define FNC	38
#define	FNR	50
#define PROMPTC	38
#define	PROMPTR	0
#define ON	0
#define	OFF	1
#define	COMP	2
#define MWIDTH	120
#define MHIGTH	240
#define	ALPHABAS	0xF940
#define NOBYTS	1536

main()
{
	chrset();
}

chrset()
{
	clear();
	setac(0,0);
	printf("\t\t\t");
	alphmode(AMOD_UL);
	printf("Modify Alternate Character Set\n");
	alphmode(0);
	printf("\n\n\tCommand Set\t\t\t\t    Motion Commands\n\n");
	printf("F- Set File Name for I/O\t\t\t\t7  8  9\n");
	printf("L- Load File to MicroAngelo\t\t\t\t  \\|/\n");
	printf("S- Save File from MicroAngelo\t\t\t\t4 - - 6\n");
	printf("A- ASCII Character to work on\t\t\t\t  /|\\\n");
	printf("E- Exit function\t\t\t\t\t1  2  3\n");
	printf("C- Clear the Character\t\t\t\t  5 = Compliment Point\n");
	printf("D- Default Char Set (PAC II only)\n");

	setac(ABASEC-1,ABASER);
	printf("      HIGH BYTE");
	setac(ABASEC,ABASER);
	alphmode(AMOD_UL);
	printf("   0 1 2 3 4 5 6 7");
	alphmode(AMOD_CU);
	setac(ABASEC+1,ABASER);
	printf("0|");
	setac(ABASEC+2,ABASER);
	printf("1|");
	setac(ABASEC+3,ABASER);
	printf("2|");
	setac(ABASEC+4,ABASER);
	printf("3|");
	setac(ABASEC+5,ABASER);
	printf("4|");
	setac(ABASEC+6,ABASER);
	printf("5|");
	setac(ABASEC+7,ABASER);
	printf("6|");
	setac(ABASEC+8,ABASER);
	printf("7|");
	setac(ABASEC+9,ABASER);
	printf("8|");
	setac(ABASEC+10,ABASER);
	printf("9|");
	setac(ABASEC+11,ABASER);
	printf("A|");
	setac(ABASEC+12,ABASER);
	printf("B|");
	setac(ABASEC+13,ABASER);
	printf("C|");
	setac(ABASEC+14,ABASER);
	printf("D|");
	setac(ABASEC+15,ABASER);
	printf("E|");
	setac(ABASEC+16,ABASER);
	printf("F|");

	drwadisp(); /* Display the alternate char set */

	setac(FNC,FNR);
	printf("FILENAME :-");

	/* outline chart */
	nvector(ON,MBASX-1,MBASY-1,MBASX+121,MBASY-1);
	vector(ON,MBASX+121,MBASY+241);
	vector(ON,MBASX-1,MBASY+241);
	vector(ON,MBASX-1,MBASY-1);

 	modify();
	alphmode(0);
}


/* main program loop */
modify()
{
char	filnam[13];
	strcpy(filnam,"            ");
char	c,chr;
int	curx,cury;

	chr=curx=cury=0;
	pltdbl(chr); /* display the char double size */
	loadchr(chr); /* get the char from M/A and display it */
	togcur(curx,cury); /* turn on cursor */

while((c=tolower(bios(3)))!='e')
	switch(c) {
		case 'f':
			getnam(filnam);
			break;
		case 'l':
			lodfil(filnam);
			togcur(curx,cury); /* turn on cursor */
			drwadisp(); /* Display the alternate char set */
			pltdbl(chr); /* display the char double size */
			loadchr(chr); /* get the char from M/A and display it */
			togcur(curx,cury); /* turn on cursor */
			break;
		case 's':
			savfil(filnam);
			break;
		case 'a':
			togcur(curx,cury);
			chr=asciichr();
			togcur(curx,cury);
			break;
		case 'c':
			togcur(curx,cury);
			clrchr(chr);
			togcur(curx,cury);
			break;
		case 'd':
			togcur(curx,cury);
			send8(0x9B);
			drwadisp();
			pltdbl(chr);
			loadchr(chr);
			togcur(curx,cury);
			break;
		case '1':
			togcur(curx,cury);
			if(cury>0) cury--;
			if(curx>0) curx--;
			togcur(curx,cury);
			break;
		case '2':
			togcur(curx,cury);
			if(cury>0) cury--;
			togcur(curx,cury);
			break;
		case '3':
			togcur(curx,cury);
			if(curx<5) curx++;
			if(cury>0) cury--;
			togcur(curx,cury);
			break;
		case '4':
			togcur(curx,cury);
			if(curx>0) curx--;
			togcur(curx,cury);
			break;
		case '5':
			togcur(curx,cury);
			compl(chr,curx,cury);
			togcur(curx,cury);
			break;
		case '6':
			togcur(curx,cury);
			if(curx<5) curx++;
			togcur(curx,cury);
			break;
		case '7':
			togcur(curx,cury);
			if(cury<11) cury++;
			if(curx>0) curx--;
			togcur(curx,cury);
			break;
		case '8':
			togcur(curx,cury);
			if(cury<11) cury++;
			togcur(curx,cury);
			break;
		case '9':
			togcur(curx,cury);
			if(cury<11) cury++;
			if(curx<5) curx++;
			togcur(curx,cury);
			break;
		default:
			break;
}
}


/* draw the alternate alpha set */
drwadisp()
{
int	c,r;
	for(c=0;c<16;c++){
		for(r=0;r<8;r++){
			setac(ABASEC+c+1,ABASER+3+(2*r));
			plotachr((r*16)+c);
		}
	}
}

/* plot alternate char at the current alpha position */
plotachr(c)
char	c;
{
	send8(CHAR_MOD);
	send8(0x80);
	send8(GCUR_ALP);
	send8(CHAR_PLT);
	send8(c);
}

/* plot the char at the double size position */
pltdbl(c)
char	c;
{
	setgc(MBASX+56,MBASY+250);
	send8(CHAR_MOD);
	send8(0x84);
	send8(CHAR_PLT);
	send8(c);
	send8(CHAR_MOD);
	send8(0);
	setac(36,50);
	printf("ASCII - %02x",c);
}

getbyt(chr,yloc)
int	yloc;
char	chr;
{
int	add;
	add=(ALPHABAS+(chr*12)+(11-yloc));
	send8(MEMY_EXM);
	send16(1);
	send16(add);
	return(get8());
}

depbyt(b,chr,yloc)
int	b,yloc;
char	chr;
{
int	add;
	add=(ALPHABAS+(chr*12)+(11-yloc));
	send8(MEMY_DEP);
	send16(1);
	send16(add);
	send8(b);
}


/* gets a charactor from MicroAngelo & draw matrix */
loadchr(c)
char	c;
{
int	add,i;
	region(OFF,MBASX,MBASY,MBASX+MWIDTH,MBASY+MHIGTH);
	for(i=0;i<12;i++) loadbyt(c,i);
}

/* draws one byte of the matrix */
loadbyt(chr,i)
int	i;
char	chr;
{
char	bit;
int	j,k;

	k=getbyt(chr,i);
	bit=32;
	for(j=0;j<6;j++){
		if( k & bit)	markcmp(j,i);
		bit=bit>>1;
	}
}

/* compliments the mark defined by base coordinates  y=0-11 x=0-7 */
markcmp(x,y)
int	x,y;
{
int	baddx,baddy;
	baddy=(y*20)+MBASY+2;
	baddx=(x*20)+MBASX+2;
	region(COMP,baddx,baddy,baddx+16,baddy+16);
}


/* clear a whole char for fresh start */
clrchr(chr)
char	chr;
{
int	i;
	for(i=0;i<12;i++) depbyt(0,chr,i);
	pltdbl(chr);
	loadchr(chr);
	setac(ABASEC+1+(chr%16),ABASER+3+(2*(chr/16)));
	plotachr(chr);
}

/* toggle the graphic cursor ON/OFF */
togcur(curx,cury)
int	curx,cury;
{
int	addrx,addry;
	addrx=MBASX+(curx*20);
	addry=MBASY+(cury*20);
	nvector(COMP,addrx,addry+10,addrx+20,addry+10);
	nvector(COMP,addrx+10,addry,addrx+10,addry+20);
}

/* compliment the grafic loc and adjust the rest */
compl(chr,curx,cury)
int	curx,cury;
char	chr;
{
char	byt,mask;
	markcmp(curx,cury);
	byt=getbyt(chr,cury);
	byt^=(32>>curx);
	depbyt(byt,chr,cury);
	pltdbl(chr);
	setac(ABASEC+1+(chr%16),ABASER+3+(2*(chr/16)));
	plotachr(chr);
}

prompt(frmt)
{
	clrprompt();
	printf("\7\7\7");
	setac(PROMPTC,PROMPTR);
	printf(frmt);
}

getnam(fnam)
char	*fnam;
{
int	i;
char	*ptr;
	ptr=fnam;
	prompt("Enter File Name - ********");
	setac(PROMPTC,18);
	for(i=0;i<8;i++) if((*ptr++=toupper(getchar()))== '\n') break;
	*--ptr= 0;
	strcat(fnam,".CHR");
	clrprompt();
	setac(FNC,FNR+13);
	printf("               ");
	setac(FNC,FNR+13);
	printf("%8s",fnam);
	printf("             ");
}

asciichr()
{
char	chr,ch,cl;
while(1){
	prompt("Enter new character in HEX - **");
	setac(PROMPTC,29);
	if(ishexlo(tolower(ch=getchar())))
		if(ishex(tolower(cl=getchar()))) break;
}
	ch=htodec(ch);
	cl=htodec(cl);
	chr=(ch*16)+cl;
	pltdbl(chr);
	loadchr(chr);
	clrprompt();
	return(chr);
}

clrprompt()
{
	setac(PROMPTC,PROMPTR);
	printf("                                               ");
}

ishex(c)
char	c;
{	if(isdigit(c)) return(1);
	if((c<'a')||(c>'f')) return(0);
	return(1);
}

ishexlo(c)
char	c;
{	if(isdigit(c)) if(c<'8') return(1);
	return(0);
}

htodec(c)
char	c;
{
	if(isdigit(c)) return(c-'0');
	if(isalpha(c)) return((c-'a')+10);
}

savfil(fnam)
char	*fnam;
{
char	c,e,buff[BUFSIZ];
int	i,fd;
	if(*fnam==' '){
		prompt("No File Name Specified **");
		return;
	}
	if(fd=fcreat(fnam,buff)==ERROR){
		prompt("FILE ERROR ! ");
		return;
	}
	prompt(" Writing file - ");
	send8(MEMY_EXM);
	send16(NOBYTS);
	send16(ALPHABAS);
	for(i=0;i<NOBYTS;i++){
			c=get8();
			if(e!=ERROR) e=(putc(c,buff));
			}
	if(e==ERROR) prompt(" Write ERROR- disk prob full ");
	fflush(buff);
	if(fclose(buff)==ERROR) prompt("ERROR Closing file ");
	else(prompt(" File Written -- "));
}

lodfil(fnam)
char	*fnam;
{
char	buff[BUFSIZ];
int	i,fd;
	if(*fnam==' '){
		prompt("No File Name Specified **");
		return;
	}
	if(fd=fopen(fnam,buff)==ERROR){
		prompt("FILE ERROR ! ");
		return;
	}
	prompt(" Reading file - ");
	send8(MEMY_DEP);
	send16(NOBYTS);
	send16(ALPHABAS);
	for(i=0;i<NOBYTS;i++) send8(getc(buff));
	if(fclose(buff)==ERROR) prompt("ERROR Closing file ");
	else(prompt(" File Loaded -- "));
}
/*----------------SEND 8 BIT QUANTITY---------------*/
send8(BYTE)
char	BYTE;
{	while (inp(STATPORT) & SENDBIT);
	outp(DATAPORT,BYTE);
	return;
}

/*----------------SEND 16 BIT QUANTITY--------------*/
send16(INTEGER)
int   INTEGER;
{	send8(INTEGER>>8);
	send8(INTEGER);
	return;
}

/*---------------GET 8 BIT QUANTITY-----------------*/
get8()
{	while((inp(STATPORT) & RECVBIT)==0);
	return(inp(DATAPORT));
}

/*-------------CLEAR GRAFICS SCREEN-----------------*/
clear()
{	send8(SCRN_CLR);
	return;
}

/*---------------SET THE GRAFICS CURSOR-------------*/
setgc(XLOC,YLOC)
int  XLOC,YLOC;
{	send8(GCUR_SET);
	sendxy(XLOC,YLOC);
	return;
}

/*----------------SET THE ALPHA CURSOR--------------*/
setac(XLOC,YLOC)
int  XLOC,YLOC;
{	send8(ACUR_SET);
	send8(XLOC);
	send8(YLOC);
	return;
}

/*-----------------VECTOR FUNCTIONS-----------------*/
vector(MODE,XLOC,YLOC)
int  MODE,XLOC,YLOC;
{	switch(MODE){
	case 1:send8(VECT_OFF);
		break;
	case 2:send8(VECT_CPL);
		break;
	case 3:send8(RVEC_ON);
		break;
	case 4:send8(RVEC_OFF);
		break;
	case 5:send8(RVEC_CPL);
		break;
	default:
		send8(VECT_ON);
		break;
	}
	sendxy(XLOC,YLOC);
	return;
}

/*-------NEW VECTOR FUNCTIONS-----------------------*/
nvector(MODE,X1LOC,Y1LOC,X2LOC,Y2LOC)
int	MODE,X1LOC,Y1LOC,X2LOC,Y2LOC;
{	setgc(X1LOC,Y1LOC);
	vector(MODE,X2LOC,Y2LOC);
	return;
}

/*--------------REGION FUNCTIONS--------------------*/
region(MODE,X1LOC,Y1LOC,X2LOC,Y2LOC)
int  MODE,X1LOC,Y1LOC,X2LOC,Y2LOC;
{	switch(MODE){
	case 1:send8(REG_OFF);
		break;
	case 2:send8(REG_CPL);
		break;
	case 3:send8(RREG_ON);
		break;
	case 4:send8(RREG_OFF);
		break;
	case 5:send8(RREG_CPL);
		break;
	default:
		send8(REG_ON);
		break;
	}
	sendxy(X1LOC,Y1LOC);
	sendxy(X2LOC,Y2LOC);
	return;
}

/*--------------SET ALPHA MODE----------------------*/
alphmode(MODE)
char MODE;
{	send8(AMOD_SET);
	send8(MODE);
	return;
}

/*--------------SEND XY COORDINATE PAIR-------------*/
sendxy(XLOC,YLOC)
int	XLOC,YLOC;
{	send16(XLOC);
	send16(YLOC);
	return;
}
