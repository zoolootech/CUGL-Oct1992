static char *progid = { "fkey.c by Michael Yokoyama"};

#include <stdio.h>

main()
{
	fkey( 4,"cls");
	fkey( 5,"");
	fkey( 7,"");
	fkey( 8,"");
	fkey( 9,"ls");
	fkey(10,"");
}

fkey(key, cmd)
int key;
char cmd[];
{
	if(key < 1 || key > 10 )
		return;
	printf("%c[0;",27);
	printf("%d;",key+58);	
	printf("%c%s%c",34,cmd,34);
	printf(";13p");
}
