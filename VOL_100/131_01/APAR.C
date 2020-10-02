/*
	************************************************************
				ACRL Parser
	************************************************************

	W. Lemiszki					  9 Jan 1982

	Filename: apar.c				 BDS C v1.50
*/

#include "acrl.h"


parse()
{
	xrel = prepost = ltyp = specflag = FALSE;	/* init for stmt */
	codelen = 0;
	errchar = ' ';

L1:	scan();					/* scan 1st token */
	if (class == EOL)
		return;				/* empty line */

	if (class == IDENT)			/* if identifier scanned... */
		{
		labsav();			/* save label params */
		scan();				/* and scan next */
		}

	if (infunc  &&  type == ':')		/* label definition */
		{
		speclist(floc);
		enter(LOCAL, idLAB+RELBIT, floc);
		goto L1;			/* go parse rest of line */
		}
	else if (class == PSEUDO)
		pseudo_op();
	else if (infunc && class == MACHINE)
		instruction();

	if (class != EOL)			/* check last token */
		error('Q');			/* extra garbage */
	if (ltyp)
		error('L');			/* label not used */
}




/*
 *	Process a pseudo op
 *	-------------------
 */
pseudo_op()
{
	switch (type)
	    {
	    case psINCL:
		include();
		break;

	    case psEQU:
		equset(idEQU);
		break;

	    case psSET:
		equset(idSET);
		break;

	    case psFUNC:
		function();
		break;

	    case psFEND:
		fend();
		break;

	    case psEND:
		endstmt();
		break;

	    case psDB:
		db();
		break;

	    case psDW:
		dw();
		break;

	    case psDS:
		ds();
		break;
	    }
}




/*
 *	Process INCLUDE statement
 *	-------------------------
 */
include()
{
	if (incflag)				/* if already including... */
		{
		error('N');			/* nest error */
		return;
		}
	scan();					/* scan filename string */
	if (class == STRING)
	    {
	    if (fopen(tokbuf, incfile) == ERROR)
		fatal("Can't include %s\n",tokbuf);
	    linesav = linecnt;			/* save line # */
	    linecnt = 0;
	    incflag = TRUE;
	    scan();
	    }
	  else
	    error('Q');				/* no filename present */
}



/*
 *	Process EQU or SET statement
 *	----------------------------
 */
equset(idtyp)
byte idtyp;
{
	scan();
	speclist(value = expression());		/* list the value */
	enter(infunc ? LOCAL : GLOBAL, idtyp + (xrel ? RELBIT : 0), value);
}




/*
 *	Process FUNCTION definition
 *	---------------------------
 */
function()
{
	int xfn;

	if (infunc)				/* if already in a function */
		{
		error('N');			/* nest error */
		return;
		}

	if (++numfunc > 64)
		fatal("Too many functions (64 max)\n");

	speclist(lc);				/* list output file address */
	enter(FUNCS, idFUNC, numfunc);		/* add to function table */
	infunc = TRUE;

	scan();
	xfn = 0;				/* external funcs needed */
	while (class == IDENT)			/* for each ext function... */
		{
		labsav();			/* save identifier params */
		enter(LOCAL, idFUNC, ++xfn);	/* into local table */
		nameout(lnam);			/* output name to codebuf */
		scan();
		if (type == ',')		/* eat the comma */
			scan();
		}
	code(0);				/* end of external funcs */

	codew(flen[numfunc]);			/* output body length */
	floc = 0;
	rellen = 0;

	if (xfn)				/* if any ext funcs */
		{
		code(0xC3);			/* jmp around vectors */
		codew(floc = 3*(xfn+1));
		relbuf[rellen++] = 1;		/* this jmp is reloc */
		while (xfn--)
			{
			code(0xC3);		/* output empty jmps */
			codew(0);
			}
		}

	faddr[numfunc] = lc;			/* save crl file addr */
	prepost = PRE;
}




/*
 *	Process FEND statement
 *	----------------------
 */
fend()
{
	int i;

	if (!infunc)				/* must be in a function */
		{
		error('N');			/* nest error */
		return;
		}
	scan();					/* scan EOL */
	speclist(flen[numfunc] = floc);		/* save (and list) func len */

	codew(rellen);				/* output rellocation list */
	for (i=0; i<rellen; )
		codew(relbuf[i++]);

	infunc = FALSE;
	prepost = POST;
}



/*
 *	Process END statement
 *	---------------------
 */
endstmt()
{
	if (incflag)			/* illegal in include files */
		{
		error('N');		/* nest error */
		return;
		}
	endflag = TRUE;
	if (infunc)			/* illegal inside a function */
		{
		error('N');		/* nest error */
		fend();			/* fake a FEND */
		}
	speclist(lc);			/* list the output file addr */
	scan();				/* scan the EOL */
}




/*
 *	Process DB statement
 *	--------------------
 */
db()
{
	char *ptr;

	if (!infunc)
	    return;				/* Q error */
	do
	    {
	    scan();
	    if (class == STRING && tokbuf[1])	/* if more than 1 char */
		{
		for (ptr = tokbuf; *ptr; )	/* copy to code buffer */
		    code(*ptr++);
		scan();
		}
	      else
		code(expr8());			/* get 8 bit expression */
	    } while (type == ',');
}



/*
 *	Process DW statement
 *	--------------------
 */
dw()
{
	if (!infunc)
	    return;				/* Q error */
	do
	    {
	    scan();
	    codew(expr16());
	    } while (type == ',');
}




/*
 *	Process DS statement
 *	--------------------
 */
ds()
{
	int v;

	if (!infunc)
		return;
	speclist(floc);				/* list the address */
	scan();
	v = expression();
	if (xrel)
		{
		error('T');			/* must not be reloc. */
		xrel = FALSE;
		v = 0;
		}
	floc += v;				/* adjust */
	lc += v;				/* generate 0's */
}




/* SUPPORT ROUTINES */


/* Save label parameters */
labsav()
{
	char *strsave();

	if (type == idUND)			/* if undefined */
		lnam = strsave(tokbuf);		/* save the name */
	    else
		{
		labid = id;			/* else save entry ptr */
		lnam = labid->nam;
		}
	ltyp = type + (reloc ? RELBIT : 0);
}



/* Define a user identifier */
enter(level, type, value)
int level;
byte type;
int value;
{
	if (ltyp == 0)				/* no label */
		error('L');
	else if (ltyp == idUND)			/* undefined ? */
		install(level, lnam, IDENT, type, value);
	else if ((~RELBIT & type) == idSET && (~RELBIT & ltyp) == idSET)
		{
		labid->val = value;		/* new 'set' value */
		labid->typ = type;		/* maybe new RELBIT */
		}
	else if (!pass || type != ltyp)
		{
		error('M');			/* multiply defined */
		labid->typ = idMUL;		/* flag in table */
		}
	else if (value != labid->val)
		error('P');			/* phase error */
	ltyp = 0;				/* clear label flag */
}




/* Statement error handling */
error(c)
char c;
{
	if (errchar != ' ')			/* keep 1st error */
		return;
	errchar = c;
	++errcnt;				/* count it */
}



/* Set special value to list in address field */
speclist(v)
int v;
{
	specflag = TRUE;
	specval = v;
}



/* Byte to code buffer */
code(datum)
byte datum;
{
	codebuf[codelen++] = datum;
}



/* Word to code buffer */
codew(word)
addr word;
{
	code(word & 0xFF);
	code(word >> 8);
}




/* Scan an 8 bit expression */
expr8()
{
	int v;

	v = expression();
	if (xrel)
		error('T');			/* must be absolute */
	if (v >= -256  &&  v < 256)
		return (v);
	error('V');				/* out of range */
	return (v & 0xFF);
}



/* Scan an address expression */
expr16()
{
	int v;

	if (class == IDENT && type == idFUNC)	/* external function */
		{
		v = value * 3;			/* its vector address */
		scan();
		xrel = TRUE;
		if (class == OPERATOR)		/* no expressions allowed */
			error('X');
		}
	    else
		v = expression();

	if (xrel)				/* add to reloc list */
		relbuf[rellen++] = floc + codelen;

	return (v);
}


/*EOF*/
