/* ibm printer set-up program */
#include "stdio.h"
#define SI '\17'	/* 17 cpi */
#define DC2 '\22'	/* 10 cpi */
#define ESC '\33'	/* escape */
#define C0 '\60'	/* esc 0 = 8 lpi */
#define C1 '\61'	/* esc 1 = 7/72 lpi */
#define C2 '\62'	/* esc 2 = 6 lpi */
#define CE '\105'	/* esc e = emphasized */
#define CF '\106'	/* esc f cancels */
#define CG '\107'	/* esc g = enhanced */
#define CH '\110'	/* esc h cancels */
#define CW '\127'       /* esc w double width */
#define CX '\130'	/* esc x 1/esc x 0 start/stop corresp */

extern FILE *fopen();
extern char *fgets();
extern int fclose(), fputc();
FILE *prn;
int i, j, buflen=15;
char buf[15], *bufptr;
main()
{
	for(i=1;i<=24;++i)
		printf("\n");  /* clear screen */
	printf("Okidata pc setup program\n\n");
	printf("Enter number(s) of options and then press ENTER\n\n");
	printf("1...10 CPI (default)\n2...17 CPI\n");
	printf("3...6 LPI (default)\n4...8 LPI\n");
	printf("5...7/72 LPI\n6...Emphasized print\n");
	printf("7...Enhanced print\n8...Correspondence print\n");
	printf("9...Double wide\n0...reset all defaults\n\n");
	printf("Choice ==>");
	bufptr = fgets(buf,buflen,stdin);
	prn = fopen("PRN:","w");
	if(prn == NULL) abort("Can\'t open the printer");
	while(*bufptr != EOS){
j = fputc(*bufptr,stderr);
j = fputc('\n',stderr);
		switch(*bufptr){
			case '1':
				j = fputc(DC2,prn);
				break;
			case '2':
				j = fputc(SI,prn);
				break;
			case '3':
				j = fputc(C2,prn);
				break;
			case '4':
				j = fputc(ESC,prn);
				j = fputc(C0,prn);
				break;
			case '5':
				j = fputc(ESC,prn);
				j = fputc(C1,prn);
				break;
			case '6':
				j = fputc(ESC,prn);
				j = fputc(CE,prn);
				break;
			case '7':
				j = fputc(ESC,prn);
				j = fputc(CG,prn);
				break;
			case '8':
				j = fputc(ESC,prn);
				j = fputc(CX,prn);
				j = fputc(C0,prn);
				break;
			case '9':

				j = fputc(ESC,prn);
				j = fputc(CW,prn);
				j = fputc(C1,prn);
				break;

			case '0':
				j = fputc(DC2,prn);
				j = fputc(ESC,prn);
				j = fputc(C2,prn);
				j = fputc(ESC,prn);
				j = fputc(CX,prn);
				j = fputc(C0,prn);
				j = fputc(ESC,prn);
				j = fputc(CF,prn);
				j = fputc(ESC,prn);
				j = fputc(CH,prn);
				j = fputc(ESC,prn);
				j = fputc(CW,prn);
				j = fputc(C0,prn);
				break;
		}
		++bufptr;
	}
	i = close(prn);
}
