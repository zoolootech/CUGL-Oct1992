/* Test program for dynamic memory allocation leak trace tool.  By Mike
   Schwartz, 3-20-87. */

#include "btree.h"
#include <stdio.h>

main()
{
	int size, n;
	extern int MalTraceEnbld;
	char buf[10];
	char *p, *malloc();
	extern BTREE MalBtree;

	MalTraceEnbld = 0;	/* Only turn on malloc info printing/checking
				   for direct calls (not for prints, etc.) */
	do {
		fputs("m/f/p/b: ", stdout);
		gets(buf);
		switch (buf[0]) {
			case 'm':	/* interactive malloc */
				fputs("\t# bytes: ", stdout);
				scanf("%d", &size);
				gets(buf);	/* throw away CR */
				MalTraceEnbld = 1;
				p = malloc(size);
				MalTraceEnbld = 0;
				printf("\tmalloc returned x%x\n", p);
				break;

			case 'f':	/* interactive free */
				fputs("\taddr: ", stdout);
				scanf("%x", &p);
				gets(buf);	/* throw away CR */
				MalTraceEnbld = 1;
				free(p);
				MalTraceEnbld = 0;
				break;

                        case 'p':	/* print pending (not yet freed)
                                           mallocs.  If n > 0, print (at
                                           most) the first n entries.  If n
                                           == 0, print all entries.  If n <
                                           0, print (at most) the last -n
                                           entries */
				fputs("\tnumber of mallocs to print: ", stdout);
				scanf("%d", &n);
				gets(buf);	/* throw away CR */
				MalTraceEnbld = 1;
				PMal(n);
				fflush(stdout);
				MalTraceEnbld = 0;
				break;
			
			case 'b':	/* print btree of pending mallocs */
				puts("btree of pending mallocs:");
				ShowTree(MalBtree, 0);
				break;

			default:
				puts("Invalid command; choose one of:");
				puts("\tm to malloc");
				puts("\tf to free");
				puts("\tp to print pending (not yet freed) mallocs");
				puts("\t(b to print the btree associated with pending mallocs)");
				break;
		}
	} while(1);
}
