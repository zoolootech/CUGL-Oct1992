/*
HEADER:		;
TITLE:		MXprint;
VERSION:	1.0;
DATE:		02/28/1982;

DESCRIPTION:	"Prints files on Epson MX-80 or MX-100 printers, with
		optional headers and page numbers at the top of each page.
		Will print in normal, emphasized, or condensed modes.

		Compilation requires MXPRINT.H.";

KEYWORDS:	Print, utility, MX80, MX100;
SYSTEM:		CP/M-80;
FILENAME:	MXPRINT.C;
SEE-ALSO:	MXPRINT.H, MXPRINT.DOC;
AUTHORS:	Joe Shannon;
COMPILERS:	BDS C;
*/
/***************************************************************************

Version 1.0, 2/28/1982:
	This program is released to public domain with the request that
	author credit be retained in the source.
	Joe Shannon, 566 Rock Road, State College, PA 16801
	(814) 238-0029

***************************************************************************/

#include	<BDSCIO.H>
#include	"MXPRINT.H"

#define	FULL	60	/* full page */
#define	FORM	0x0C	/* formfeed */
#define LF	0x0A	/* ASCII linefeed */
#define CR	0x0D	/* ASCII carriage return */
#define	QEOF	0x1A	/* QED end of file mark */
#define PNTWIDTH 80	/* normal printer line MAX */

char	rbuff[BUFSIZ];
char	filename[20];
char	fflag,eflag,lflag,cflag,nflag,pflag,iflag;
int	linecount,pageno,maxwidth,frelength;

main(argc,argv)
int	argc;
char	**argv;
{
int	i,s,c,charcnt;

	maxwidth=PNTWIDTH;
	_allocp = NULL;
	fflag=eflag=lflag=cflag=nflag=pflag=0;
	linecount=1;
	pageno = 0;
	if(argc<2) exit();
	strcpy(filename,argv[1]);
	printf("%s\n",filename);
	if(fopen(filename,rbuff)==-1){
		printf("Bad filename **");
		exit();
	};
	while(--argc > 1 && argv[argc][0] == '-')
		for(i=1;s=argv[argc][i];i++)
			switch (tolower(s)){
			case	'e':	eflag++ ; break;/* emphasized */
			case	'l':	lflag++ ; break;/* large      */
			case	'c':	cflag++ ; break;/* condensed  */
			case	'p':	pflag++ ; break;/* page mode  */
			case	'n':	nflag++ ; break;/* line #s    */
			case	'f':	fflag++ ; break;/* free form  */
			default:
				printf("illegal option **");
				exit();
			}
	pntint();
	if(eflag) emppnt();
	if(lflag) lrgpnt();
	if(cflag) smlpnt();
	if(fflag) freform();

	nextpg();
	if(nflag) fprintf(2,"%3d: ",linecount);
	charcnt=0;
	while(1){
		c=getc(rbuff);
		if((c==EOF)||(c==CPMEOF)||(c==QEOF))	break;
		if(c!=CR){
			putc(c,2);
			printf("%c",c);
			if(++charcnt > maxwidth){
				++linecount;
				charcnt=0;
			}
		}
		else{
			++linecount;
			if((linecount % FULL)== 0) nextpg();
			if(lflag) lrgpnt();
			if(nflag) fprintf(2,"%3d: ",linecount);
			if((fflag)&&(linecount>=frelength)) frepause();
			charcnt=0;
		}
		if((c==CR)&& lflag ) lrgpnt();
	};
	pntint();
	if(fflag) pedon();
	exit();
}
nextpg(){
	if(++pageno > 1) fprintf(2,"%c",FORM);
	if(pflag){
	fprintf(2,"FILE: %s\t\t\t\t\t\tPage #%d\n",filename,pageno);
	fprintf(2,"----------------------------------------");
	fprintf(2,"----------------------------------------\n\n");
	}
	else if(!(fflag)) fprintf(2,"\n\n\n");
	if(lflag) lrgpnt();
	return;
}
freform(){
char	inp;
	inp=0;
	frelength=FULL;
	pedoff();
	while((inp < '3') || (inp > '4')){
		printf("\n FREE FORM MODE ");
		printf("\n\n\t Choose option - \n");
		printf("\t\t1- set maximum column width in chars.\n");
		printf("\t\t2- set maximum page length in lines.\n");
		printf("\t\t3- CONTINUE\n");
		printf("\t\t4- QUIT PROGRAM\n");

		inp=getchar();
		if(inp=='1') setcol();
		if(inp=='2') setlen();
		if(inp=='3') break;
		if(inp=='4'){pntint();
			if(fflag) pedon();
			exit();
		}
	}
	while(inp != 'y'){
		printf("\nIs the printer turned on and the paper ");
		printf("in place ??");
		inp=getchar();
	}
}
setcol(){
int	i,size;
	i=0;
	while(i != 1 ){
		printf("\n\tENTER maximum column width - ");
		i=scanf("%d",&size);
	}
	maxwidth=size;
	colsiz(size);
}
setlen(){
int	i;
	i=0;
	while(i != 1 ){
		printf("\n\tENTER maximum page length in lines - ");
		i=scanf("%d",&frelength);
	}
}

frepause(){
char	inp;
	inp=0;
	while((inp<'1') || (inp>'3')){
		prbell();	/* sound printer bell */
		printf("\n*** MAXIMUM LENGTH WARNING ***\n");
		printf("\n\n\tChoose option - ");
		printf("\n\n\t\t1- print one more line on page.");
		printf("\n\t\t2- a new page is now in printer.");
		printf("\n\t\t3- QUIT the program.\n\n");
		inp=getchar();
	}
	if(inp=='1') return;
	if(inp=='2'){ linecount=0;
			return;
	}
	if(inp=='3'){pntint();
			if(fflag) pedon();
			exit();
		}
}
