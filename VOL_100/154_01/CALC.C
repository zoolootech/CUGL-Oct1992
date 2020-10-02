/*
	calc.c:		simple display accumulator for quick,
				on-screen calculations.

	It accepts simple commands of the form  "x opr",
	where "x" is a double, and "opr" is one of:

		s ==>	reset accumulator with number
		+ ==>	add number to accumulator
		- ==>	subtract number from accumulator
		* ==>	multiply accumulator by number
		/ ==>	divide accumulator by number (checks for 0)
		q ==> 	end session (or ^Z)

	Example:	10 s		(10.0000)
				5 +			(15.0000)
				3 /			( 3.0000)
				0 e			(exit)

-----
	(c) Chuck Allison, 1985
-----

*/

#include <stdio.h>

main()
{
	double 	accum = 0.0,
			x;
	char	line[80],
			opr;
	int		argc;

	while (gets(line) != NULL)
	{
		argc = sscanf(line," %lf%1s",&x,&opr);
		if (argc != 2)
		{
			/* ..invalid entry (probably backwards).. */
			printf("? try again ..\n");
			continue;
		}
		switch(opr)
		{
			case '+':
				accum += x;
				break;
			case '-':
				accum -= x;
				break;
			case '*':
				accum *= x;
				break;
			case '/':
				if (x != 0)
					accum /= x;
				else
					printf("? division by 0: ");
				break;
			case 's':
			case 'S':
				accum = x;
				break;
			case 'q':
			case 'Q':
				exit(0);
			default :
				printf("? invalid operator: ");
		}
		printf("%11.4f %c\n =%11.4f\n",x,opr,accum);
	}
}
