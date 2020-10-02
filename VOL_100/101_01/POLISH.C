/*  This is the RPN calculator taken from the C Manual
by Kernighan & Ritchie.  It's an excellent example of stack
implementation. It works for signed numbers. For unsigned
numbers, just change the d in the "=" case print statement
to a u. Example of use:
1 2 - 4 5 + * =  -9
*/

#define MAXOP 20
#define NUMBER '0'
#define TOOBIG '9'
int stkpntr;

main ()
{
	stkpntr = 0;
	int type;
	char s[MAXOP];
	int op2, atoi(), pop(), push();

	while (type = getop(s,MAXOP))
		switch (type)  {
		case NUMBER:
			push(atoi(s));
			break;
		case '+':
			push(pop() + pop());
			break;
		case '*':
			push(pop() * pop());
			break;
		case '-':
			op2 = pop();
			push(pop() - op2);
			break;
		case '/':
			op2 = pop();
			if (op2 != 0)
				push(pop() / op2);
			else
				printf("Zero divisor popped\n");
			break;
		case '=':
			printf("\t%d\n", push(pop()));
			break;
		case 'c':
			clear();
			break;
		case TOOBIG:
			printf("%.20s ... is too long\n", s);
			break;
		default:
			printf("  Unknown command %c\n", type);
			break;
		}
}
#define MAXVAL 100
int val[MAXVAL];
int push (f)
int f;
{
	if (stkpntr < MAXVAL)
		return(val[stkpntr++] = f);
	else  {
		printf("Error: stack full\n");
		clear ();
		return (0);
	}
}
int pop ()
{
	if (stkpntr > 0)
		return (val[--stkpntr]);
	else  {
		printf("Error: stack empty\n");
		clear ();
		return(0);
	}
}
clear ()
{
	stkpntr = 0;
}
getop(s,lim)
char s[];
int lim;
{
	int i,c;
	while ((c = getchar()) == ' ' || c == '\t' || c == '\n')
		;
	if  (c < '0' || c > '9')
		return(c);
	s[0] = c;
	for (i = 1; (c = getchar()) >= '0' && c <= '9' ; i++)
		if (i < lim)
			s[i] = c;
	if (i < lim) {
		ungetch(c);
		s[i] = '\0';
		return(NUMBER);
	} else { 
		while (c != '\n')
			c = getchar();
		s[lim-1] = '\0';
		return (TOOBIG);
	}
}
