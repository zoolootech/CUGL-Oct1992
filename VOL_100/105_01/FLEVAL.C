

/* FLEVAL.C  a program to evaluate/test the BDS-C
floating point package.  Entry from keyboard and
printout showing structure of floating point word.
*/
#include "bdscio.h"
main()
{
	char *atof(), op1[5], s[80];
	char *fpmag(),*fpchs(),*fpasg();
	char result1[5],result2[5],result3[5];
	char *gets();
	int atoi();
	int ftoit(), ftoir();
	int nin;
	printf("\n");
	while (TRUE) {
	 printf("\nEnter integer ");
	 gets(s);
	 nin = atoi(s);
	 itof(op1,nin);
	 copyout(op1);
	 printf("\nEnter float ");
	 gets(s);
	 atof(op1,s);
	 copyout(op1);
	printf("\n Truncate %d %x Round %d %x ",ftoit(op1),
		ftoit(op1),ftoir(op1),ftoir(op1));
	fpasg(result1,op1); fpmag(result2,op1); fpchs(result3,op1);
	 printf("\n Assign %e  Mag %e  Chg Sign %e ",result1,
		result2,result3);
			}
}

copyout(oper)
char oper[5];
{	int i;
	printf("\nFLOAT %e hex ",oper);
	for(i=0;i<=4;i++) printf("%X,",oper[i]);
	printf(" dec ");
	for(i=0;i<=4;i++) printf("%D,",oper[i]);
	return;
}

