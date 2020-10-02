/*** - the random file code (ry) tester */

#include "a:std.h"		/* bdscio + my stuff */
#include <ry.h>

/*
	the exerciser....
*/

char Name[15];
char Open;
char Pos[4], Apos[15];
int Org;
RFILE *Fp;

main()
{
														/* print menu */
_allocp = NULL;

Name[0] = 0;
Open = NO;

top:
	printf("\f\nwhat to do?\n");
	printf("\n1: creat a random file");
	printf("\n2: open a random file");
	printf("\n3: seek a position");
	printf("\n4: read present position");
	printf("\n5: get a character");
	printf("\n6: put a character");
	printf("\n7: get a line");
	printf("\n8: put a line");
	printf("\n9: close a random file");

	printf("\n\n	- ");
/** go for it */
	switch(getchar()) {			/* get choice */
	case '1':
		makerf();
		break;
	case '2':
		openrf();
		break;
	case '3':
		seekp();
		break;
	case '4':
		readp();
		break;
	case '5':
		gc();
		break;
	case '6':
		pc();
		break;
	case '7':
		gl();
		break;
	case '8':
		pl();
		break;
	case '9':
		closerf();
		break;
	default:
		printf("\n\t\thuh?");
	}
	printf("\nagain? ");
	if (tolower(getchar()) == 'n') exit();
	goto top;
}

makerf()
{
	char mode;
	int sectors;

	if (Open) {
		printf("\nfile (%s) already open!", Name);
		return ERROR;
	}
	printf("\nfile name? ");
	scanf("%s", Name);
	printf("\nmode (r, w, d) ? ");
	mode = tolower(getchar());
	printf("\nbuffer size in sectors ? ");
	scanf("%d", &sectors);
	if ((Fp = rcreat(Name, mode, sectors)) == ERROR) {
		printf("\n ERROR from rfmake!!!");
		return ERROR;
	}
	Open = YES;
	return OK;
}

openrf()
{
	char mode;
	int sectors;

	if (Open) {
		printf("\nfile (%s) already open!", Name);
		return ERROR;
	}
	printf("\nfile name? ");
	scanf("%s", Name);
	printf("\nmode (r, w, d) ? ");
	mode = tolower(getchar());
	printf("\nbuffer size in sectors ? ");
	scanf("%d", &sectors);
	if ((Fp = ropen(Name, mode, sectors)) == ERROR) {
		printf("\n ERROR from rfopen!!!");
		return ERROR;
	}
	Open = YES;
	return OK;
}

closerf()
{
	if (rclose(Fp) == ERROR) {
		printf("\nERROR from rfclose (%s) !!!, Name");
		Open = NO;
		Name[0] = 0;
		return ERROR;
	}
	Open = NO;
	Name[0] = 0;
	return OK;
}

seekp()
{
	printf("\nwhat position do u want ? ");
	scanf("%s", Apos);
	atol(Pos, Apos);
	printf("\nfrom 0:beginning, 1:present pos, or 2:end  (internal form)");
	printf("\nfrom 3:beginning, 4:present pos, or 5:end  (ascii form)");
	printf("\n\t- ");
	scanf("%d", &Org);
	switch (Org) {
	case 0:
	case 1:
		if (lseek(Fp, Pos, Org) == ERROR) {
			printf("\nERROR on lseek (internal form)!");
			return ERROR;
		}
		return OK;
	case 3:
	case 4:
		if (lseek(Fp, Apos, Org) == ERROR) {
			printf("\nERROR on lseek (ascii form)!");
			return ERROR;
		}
		return OK;
	case 2:
	case 5:
		printf("\nseeks from end not supported yet...");
		return ERROR;
	default:
		printf("\nbad origin");
		return ERROR;
	}
}

readp()
{
	printf("\ncurrent file position: %s", ltoa(Apos, ltell(Fp, Pos)));
	return OK;
}

gc()
{
	int c;

	ltoa(Apos, ltell(Fp, Pos));
	if ((c = rgetc(Fp)) == ERROR) {
		printf("\nERROR on rgetc from pos %s", Apos);
		return ERROR;
	}
	printf("\ngot '%c' from position %s", c, Apos);
	return OK;
}

pc()
{
	char c;

	ltoa(Apos, ltell(Fp, Pos));
	printf("\nwhat character ? ");
	scanf("%c", &c);
	if (rputc(c, Fp) == ERROR) {
		printf("\nERROR on rputc at pos %s", Apos);
		return ERROR;
	}
	printf("\nput '%c' at position %s", c, Apos);
	return OK;
}

gs()
{
	char str[MAXLINE];

	ltoa(Apos, ltell(Fp, Pos));
	if (rgets(str, Fp) == ERROR) {
		printf("\nERROR on rgets from pos %s", Apos);
		return ERROR;
	}
	printf("\ngot string '%s' from position %s", str, Apos);
	return OK;
}

ps()
{
	char str[MAXLINE];

	ltoa(Apos, ltell(Fp, Pos));
	printf("\nwhat string ? ");
	scanf("%s", str);
	if (rputs(str, Fp) == ERROR) {
		printf("\nERROR on rputs at pos %s", Apos);
		return ERROR;
	}
	printf("\put string '%s' at position %s", str, Apos);
	return OK;
}

gl()
{
	char str[MAXLINE];

	ltoa(Apos, ltell(Fp, Pos));
	if (rgetl(str, Fp) == ERROR) {
		printf("\nERROR on rgetl from pos %s", Apos);
		return ERROR;
	}
	printf("\ngot line '%s' from position %s", str, Apos);
	return OK;
}

pl()
{
	char str[MAXLINE];

	ltoa(Apos, ltell(Fp, Pos));
	printf("\nwhat line ? ");
	scanf("%s", str);
	strcat(str, "\n");
	if (rputl(str, Fp) == ERROR) {
		printf("\nERROR on rputl at pos %s", Apos);
		return ERROR;
	}
	printf("\put line '%s' at position %s", str, Apos);
	return OK;
}

/***
g()
{
	char block[MAXLINE];

	ltoa(Apos, ltell(Fp, Pos));
	if (rget(Fp, block, num) == ERROR) {
		printf("\nERROR on rget from pos %s", Apos);
		return ERROR;
	}
	printf("\ngot block '%s' from position %s", str, Apos);
	return OK;
}

p()
{
	char str[MAXLINE];

	ltoa(Apos, ltell(Fp, Pos));
	printf("\nwhat string ? ");
	scanf("%s", str);
	if (rputs(str, Fp) == ERROR) {
		printf("\nERROR on rputs at pos %s", Apos);
		return ERROR;
	}
	printf("\put string '%s' at position %s", str, Apos);
	return OK;
}
***/
f (rgetl(str, Fp) == ERROR) {
		printf("\nERROR on rgetl from pos %s"