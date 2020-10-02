/* ritman printer set-up program */
#include "stdio.h"
#define SI '\17'	/* compressed */
#define DC2 '\22'	/* cancel */
#define ESC '\33'	/* escape */
#define CE '\105'	/* esc e = emphasized */
#define CF '\106'	/* esc f cancels */
#define CG '\107'	/* esc g = double strike */
#define CH '\110'	/* esc h cancels */

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
	printf("Riteman printer setup program\n\n");
	printf("Enter number(s) of options and then press ENTER\n\n");
	printf("A...Pica (10 cpi)      B...Elite (12 cpi)\n");
	printf("C...Skip over perfs    D...Cancel perf skip\n");
	printf("E...Double width       F...Cancel double width\n");
	printf("G...Compressed print   H...Cancel compressed\n");
	printf("I...Emphasized print   J...Cancel emphasized\n");
	printf("K...Double strike      L...Cancel double strike\n");
	printf("M...Proportional print N...Cancel proportional\n");
	printf("O...Unidirectional     P...Cancel\n");
	printf("Q...Italic char set    R...Cancel italics\n");
	printf("S...Half speed print   T...Full speed print\n");
	printf("U...reset to power-on defaults\n\n");
	printf("Choice ==>");
	bufptr = fgets(buf,buflen,stdin);
	prn = fopen("PRN:","w");
	if(prn == NULL) abort("Can\'t open the printer");
	while(*bufptr != EOS){
j = fputc(*bufptr,stderr);
j = fputc('\n',stderr);
		switch(*bufptr){
			case 'A':
				j = fputc(DC2,prn);
				j = fputc('\120',prn);
				break;
			case 'B':
				j = fputc(ESC,prn);
				j = fputc('\115',prn);
				break;
			case 'C':
				j = fputc(ESC,prn);
				j = fputc('\116',prn);
				j = fputc('\6',prn);
				break;
			case 'D':
				j = fputc(ESC,prn);
				j = fputc('\117',prn);
				break;
			case 'E':
				j = fputc(ESC,prn);
				j = fputc('\127',prn);
				j = fputc('\1',prn);
				break;
			case 'F':
				j = fputc(ESC,prn);
				j = fputc('\127',prn);
				j = fputc('\0',prn);
				break;
			case 'G':
				j = fputc(SI,prn);
				break;
			case 'H':
				j = fputc(DC2,prn);
				break;
			case 'I':
				j = fputc(ESC,prn);
				j = fputc(CE,prn);
				break;
			case 'J':
				j = fputc(ESC,prn);
				j = fputc(CF,prn);
				break;
			case 'K':
				j = fputc(ESC,prn);
				j = fputc(CG,prn);
				break;
			case 'L':
				j = fputc(ESC,prn);
				j = fputc(CH,prn);
				break;
			case 'M':
				j = fputc(ESC,prn);
				j = fputc('\160',prn);
				j = fputc('\1',prn);
				break;
			case 'N':
				j = fputc(ESC,prn);
				j = fputc('\160',prn);
				j = fputc('\0',prn);
				break;
			case 'O':
				j = fputc(ESC,prn);
				j = fputc('\125',prn);
				j = fputc('\1',prn);
				break;
			case 'P':
				j = fputc(ESC,prn);
				j = fputc('\125',prn);
				j = fputc('\0',prn);
				break;
			case 'Q':
				j = fputc(ESC,prn);
				j = fputc('\64',prn);
				break;
			case 'R':
				j = fputc(ESC,prn);
				j = fputc('\65',prn);
				break;
			case 'S':
				j = fputc(ESC,prn);
				j = fputc('\163',prn);
				j = fputc('\1',prn);
				break;
			case 'T':
				j = fputc(ESC,prn);
				j = fputc('\163',prn);
				j = fputc('\0',prn);
				break;
			case 'U':
				j = fputc(ESC,prn);
				j = fputc('\100',prn);
				break;
			       }
		++bufptr;
	}
	i = close(prn);
}
