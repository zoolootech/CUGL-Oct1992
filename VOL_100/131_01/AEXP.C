/*
	************************************************************
			ACRL Expression evaluator
	************************************************************

	W. Lemiszki					  9 Jan 1982

	Filename: aexp.c				 BDS C v1.50
*/

#include "acrl.h"

/*
	Scans the next expression from the input line and returns its
	value.  The global 'xrel' is set TRUE if the expression is
	relocatable.  This routine (and expr(), and operand()) assumes
	that the first token has already been scanned.

	expr() takes a precedence argument and scans recursively until
	it finds an operator that has lower precedence.

	operand() scans operands and handles unary operators and
	parenthesized sub expressions.
*/

int expression()
{
	return (expr(0));			/* start with precedence 0 */
}


int expr(lprec)
byte lprec;
{
	byte op, rprec, urel;
	int u, v;

	u = operand();				/* scan an operand */
	while (class == OPERATOR  &&  (rprec = prec(type)) > lprec)
	    {
	    op = type;				/* save operator type */
	    urel = xrel;			/* save reloc attr */
	    scan();				/* next token */
	    v = expr(rprec);			/* get right 'operand' */
	    xrel = opreloc(op, urel, xrel);	/* compute reloc atribute */
	    u = operate(op, u, v);		/* perform operator */
	    }
	return (u);
}




/*
 *	Return the precedence of an operator
 *	------------------------------------
 */
byte prec(op)
byte op;
{
	switch (op)
	    {
	    case '*':
	    case '/':
	    case opMOD:
	    case opSHL:
	    case opSHR:
		return (6);

	    case '+':
	    case '-':
		return (4);

	    case opAND:
		return (2);

	    default:
		return (1);
	    }
}



/*
 *	Compute the relocatability of an operation
 *	------------------------------------------
 */
int opreloc(op, urel, vrel)
byte op;				/* the operator */
byte urel;				/* reloc. atr. of left operand */
byte vrel;				/* reloc. atr. of right operand */
{
	if (!urel && !vrel)			/* both absolute */
		return FALSE;

	if (op == '-' && urel && vrel)		/* rel - rel */
		return FALSE;

	if (op == '+' && !urel && vrel)		/* abs + rel */
		return TRUE;

	if ((op == '+' || op == '-') && urel && !vrel)
		return TRUE;			/* rel +/- abs */

	error('T');				/* all others illegal */
	return FALSE;
}




/*
 *	Perform an operator function
 *	----------------------------
 */
int operate(op, u, v)
byte op;					/* the operator */
int u;						/* left operand */
int v;						/* right operand */
{
	switch (op)
	    {
	    case '+':
		return (u + v);

	    case '-':
		return (u - v);

	    case '*':
		return (u * v);

	    case '/':
		return (u / v);

	    case opAND:
		return (u & v);

	    case opOR:
		return (u | v);

	    case opXOR:
		return (u ^ v);

	    case opMOD:
		return (u % v);

	    case opSHR:
		return (u >> v);

	    case opSHL:
		return (u << v);
	    }
}




/*
 *	Operand Scanning
 *	----------------
 */
int operand()
{
	int v;

	if (type == '-')			/* if '-' operator... */
		class = OPERAND;		/* make it unary minus */

	if (class == IDENT)			/* if identifier... */
		{
		if (type == idUND)		/* check for errors */
			error ('U');		/* undefined */
		if (type == idFUNC)
			error ('X');		/* external error */
		class = OPERAND;		/* then make it ok */
		type = NUMBER;
		}

	if (class == STRING && tokbuf[1] == EOS)    /* if 1 char string... */
		{
		value = *tokbuf;		/* use its ASCII code */
		class = OPERAND;
		type = NUMBER;
		}

	if (class != OPERAND)			/* if not an operand... */
		{
		error('E');			/* expression error */
		return (0);
		}

	switch (type)
	    {
	    case '-':				/* unary - */
		v = -unary();
		break;

	    case opNOT:				/* unary NOT */
		v = ~unary();
		break;

	    case opHIGH:			/* unary HIGH */
		v = unary() >> 8;
		break;

	    case opLOW:				/* unary LOW */
		v = unary() & 0xFF;
		break;



	    case '$':				/* location counter */
		if (!infunc)
		    error('E');			/* expression error */
		v = floc;
		xrel = TRUE;
		scan();
		break;

	    case '(':				/* subexpression */
		scan();
		v = expression();
		if (type != ')')
		    error('B');			/* balance error */
		scan();
		break;

	    default:
		v = value;			/* current token params */
		xrel = reloc;
		scan();
	    }
	return (v);
}



/* Prepare for unary operation */
int unary()
{
	int v;

	scan();
	v = operand();
	if (xrel)			/* unarys illegal on relocs */
		{
		error('T');		/* type error */
		xrel = 0;
		}
	return (v);
}


/*EOF*/
