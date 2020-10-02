/*
       HEADER:  150.??;
        TITLE:  Reverse Polish Notation (RPN) Calculator;
  DESCRIPTION:  "A small program which turns your PC into a very expensive, 
                 but convenient(?), pocket calculator.  It operates like the
                 Hewlett-Packard calculators, which use RPN.  It has a 6 value
                 stack, although this could easily be modified.  It is NOT a
                 "resident" program, like the commercial desktop assistant
                 programs.  Commands: "," (clear top), "+", "-", "*", "/",
                 "^" (raise to power), "I" (invert, or 1/x), "N" (negate), "R" 
                 (square root), "L" (log, to base e), "E" (exponential),
                 and "Q" (quit).  Latter values to 1E-7 precision.";
     KEYWORDS:  Calculator, RPN, Reverse Polish Notation;
       SYSTEM:  any with suitable C compiler;
     FILENAME:  HP.C;
     WARNINGS:  "1) User documentation not included.  
                 2) Requires C compiler with double precision real numbers.
                 3) Requires file TRAN.C to compute transcendental functions.";
     SEE-ALSO:  TRAN.C;
    COMPILERS:  any C compiler with double prec. reals;
   REFERENCES:  AUTHORS: unknown; TITLE: "HP.C";
                CITATION: "PC-SIG Disk 50 or 142" 
       ENDREF;
*/
#include "stdio.h"
#include "tran.c"
double pop(),push();
double stack[6];
int sp=0;
main()
{
double dp=1;
int x,i,df=0;
while ((x=getchar()) != 'q' && x != 'Q') {
	if (x>='0' && x<='9') {
	  if (df) dp=10*dp;
	  stack[0]=10*stack[0]+x-'0';
	}
	else if (x=='.') df=1;
	else {
	  stack[0] = stack[0] / dp;
	  dp=1;
	  df=0;
	  if (x==',' && stack[0] == 0) stack[0]=stack[sp];
	  if (stack[0] == 0) stack[0]=pop();
	  switch (x) {
	  case ',' :
	  case '\13' :
	    break;
	  case '+' :
	    stack[0] = stack[0]+pop();
	    break;
	  case '-' :
	    stack[0] = pop()-stack[0];
	    break;
	  case '*' :
	    stack[0] = pop()*stack[0];
	    break;
	  case '/' :
	    stack[0] = pop()/stack[0];
	    break;
	  case '^' :
	    stack[0] = exp( stack[0] * log( pop(0) ) ) ;
	    break;
	  case 'I' :
	  case 'i' :
	    stack[0] = 1/stack[0];
	    break;
	  case 'N' :
	  case 'n' :
	    stack[0] = -stack[0];
	    break;
	  case 'r' :
	  case 'R' :
	    stack[0]=root(stack[0]);
	    break;
	  case 'l' :
	  case 'L' :
	    stack[0]=log(stack[0]);
	    break;
	  case 'e' :
	  case 'E' :
	    stack[0]=exp(stack[0]);
	    break;
	  }
	  push (stack[0]);
	  stack[0]=0;
	  if (x != '\0') {
	  for (i=1;i<=sp;i++) printf("%f  ",stack[i]);
	  printf ("%c\n",x);
	  }
	  }
	  }
}

/* Pop floating from stack */
double pop()
{
if (sp>0) return (stack[sp--]);
else return (0);
}

/* Push floating onto stack */
double push(x)
double x;
{
if (sp<5) stack[++sp] = x;
}
