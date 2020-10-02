/* eatcom.c:	strip comments from STDIN	*/

#include <stdio.h>
#define START 1
#define MAYBE 2
#define YES 3
#define LEAVING 4
#define QUOTE 5
#define APOST 6

main()
{	int c, state, escape = 0;

	state = START;
	while ((c = getchar()) != EOF)
		switch(state)
		{
			case START:
				if (c != '/')
				{
					putchar(c);
					if (c == '\"')
						state = QUOTE;
					else if (c == '\'')
						state = APOST;
				}
				else
					state = MAYBE;
				break;
			case MAYBE:	
				if (c == '*')
					state = YES;
				else
				{
					putchar('/');
					putchar(c);
					state = START;
				}
				break;
			case YES:
				if (c == '*')
					state = LEAVING;
				break;
			case LEAVING:
				if (c == '/')
					state = START;
				else if (c != '*')
					state = YES;
				break;
			case QUOTE:
				putchar(c);
				if (escape)
					escape = 0;
				else if (c == '\\')
					escape = 1;
				else if (c == '\"')
					state = START;
				break;
			case APOST:
				putchar(c);
				if (escape)
					escape = 0;
				else if (c == '\\')
					escape = 1;
				else if (c == '\'')
					state = START;
		}

	if (state != START)
		printf("Invalid comment syntax!\n");
}
