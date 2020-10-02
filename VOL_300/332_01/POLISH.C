/*----------------------------------------------------*- Fundamental -*-

Facility:		polish

File:			polish.c

Associated files:	- (none)

Description:		This is the RPN calculator taken from the
			C Manual by Kernighan & Ritchie.  It's an
			excellent example of stack implementation.

Notes:			Some changes has been made to the code to
			make it more portable.

Author:			H. Moran (?)

Editor:			Anders Thulin
			Rydsvagen 288
			S-582 50 Linkoping
			SWEDEN

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Edit history :

Vers  Ed   Date        By                Comments
----  ---  ----------  ----------------  -------------------------------
 1.0    0  19xx-xx-xx  H. Moran (?) 
 1.1    1  1988-12-21  Anders Thulin     General cleanup (original did
			                 not compile under K&R)

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*---  Configuration:  ----------------------------------------------

System configuration options:
=============================

  ANSI		ANSI C
  BSD		BSD Unix, SunOS 3.5
  SV2		AT&T UNIX System V.2
  XPG3		X/Open Portability Guide, ed. 3

If you have an ANSI C conformant compiler, define ANSI. If not,
choose the definition that matches your setup best.


Program configuration options:
==============================

Parameter    Original
name         value	Comment

MAXOP	     20		Max nr of chars in a number
MAXVAL      100		Max size of stack

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define	ANSI		1
#define	BSD		0
#define	SV2		0
#define	XPG3		0

#define MAXOP	20
#define MAXVAL 100

/*- End of configurations - - - - - - - - - - - - - - - - - - - - - - - - */

#if ANSI
# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
#endif

#if BSD
# include <ctype.h>
# include <stdio.h>
  extern int atoi();
# define EXIT_SUCCESS	0
#endif

#if SV2
# include <ctype.h>
# include <stdio.h>
  extern int atoi();
# define EXIT_SUCCESS	0
#endif

#if XPG3
# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
#endif

/*----------------------------------------------------------------------
  Local definitions and variables:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define NUMBER '0'
#define TOOBIG '9'

static long val[MAXVAL];		/* Stack	*/
static int  stkpntr = 0;		/* Points to first empty element in stack */

/*----------------------------------------------------------------------
  Local functions:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if __STDC__ != 0
 static void clear(void);
 static int  getop(char *s, int lim);
 static long pop(void);
 static long push(long f);
#else
 static void clear();
 static int  getop();
 static long pop();
 static long push();
#endif


/*----------------------------------------------------------------------

Routine:	clear

Description:	empty the stack

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static void clear ()
{
  stkpntr = 0;
}

/*----------------------------------------------------------------------

Routine:	getop

Description:	Returns the next operand/operator from stdin

		An operand is any (unsigned) number. The separate
		characters are stored in 's', and the value NUMBER
		is returned.

		An operator is everything that is not a number or
		a space.  The operator character is returned as
		value.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static int getop(s, lim)
char s[];		/* character buffer */
int lim;		/* size of character buffer */
{
  int i,c;

  /*  1.  Skip leading spaces:	*/

  while (isspace(c = getchar()))
    ;

  /*  2.  If not a number, then an operator char (or EOF):	*/

  if (!isdigit(c)) {
    return(c);
  }

  /*  3.  Collect a number into s:	*/

  s[0] = c;

  for (i = 1; isdigit(c = getchar()); i++) {
    if (i < lim) {
      s[i] = c;
    }
  }

  if (i < lim) {
    ungetc(c, stdin);
    s[i] = '\0';
    return(NUMBER);
  } else { 			/* buffer overflow */
    while (c != '\n') {		/* skip to eoln		*/
      c = getchar();
    }
    s[lim-1] = '\0';
    return (TOOBIG);
  }
}

/*----------------------------------------------------------------------

Routine:	main

Description:	main program

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

int main()
{
  int  type;		/* type of operand/operator from getop()	*/
  long op2;		/* temporary operand storage */
  char s[MAXOP];	/* number buffer */

  stkpntr = 0;

  puts("polish -- reverse polish calculator");

  while ((type = getop(s,MAXOP)) != EOF) {
    switch (type)  {

      default:
	printf("  Unknown command %c\n", type);
	break;

      case NUMBER:
        (void)push(atoi(s));
	break;

      case '+':
        (void)push(pop() + pop());
        break;

      case '*':
	(void)push(pop() * pop());
	break;

      case '-':
	op2 = pop();
	(void)push(pop() - op2);
	break;

      case '/':
	op2 = pop();
	if (op2 != 0) {
  	  (void)push(pop() / op2);
	} else {
  	  printf("Zero divisor popped\n");
	}
	break;

      case '=':
	printf("        %d\n", push(pop()));
	break;

      case 'c':
	clear();
	break;

      case TOOBIG:
	printf("%.20s ... is too long\n", s);
	break;

    }
  }

  return EXIT_SUCCESS;		/* Never reached */
}

/*----------------------------------------------------------------------

Routine:	pop

Description:	pop a value of the stack and return it as value

		If the stack is empty, print an error message,
		clear the stack (to ensure that stackpointer is 0)
		and return a 0.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static long pop ()
{
  if (stkpntr > 0) {
    return (val[--stkpntr]);
  } else {
    printf("Error: stack empty\n");
    clear ();
    return(0);
  }
}

/*----------------------------------------------------------------------

Routine:	push

Description:	push the argument onto the stack, and return it
		as value.

		If the stack overflows, print an error message,
		clear the stack, and return 0.

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static long push(f)
long f;
{
  if (stkpntr < MAXVAL) {
    return(val[stkpntr++] = f);
  } else {
    printf("Error: stack full\n");
    clear();
    return (0);
  }
}

