/*	>>>>> start of c9 <<<<<<	*/
#ifndef	TRUE	/* check to see if include file is needed */
#include <C.DEF>
#endif

/*	Add the primary and secondary registers	*/
/*	(results in primary) */
add()
{
	pop();
	ol("dad\td");
	}
/*	Subtract the primary register from secondary */
/*	(results in primary) */
sub()
{
	ccall("@sub");
	}
/*	Multiply the primary and secondary registers */
/*	(results in primary) */
mult()
{
	ccall("@mult");
	}
/*	Divide the secondary register by the primary */
/*	(quotient in primary, remainder in seconday) */
div()
{
	ccall("@div");
	}
/*	Compute remainder (mod) of seconday by primary */
/*		(remainder in primary, quotient ient in secondary) */
mod()
{
	div();
	swap();
	}
/* Inclusive 'or' the primary and the secondary registers */
/*	(results in primary) */
or()
{
	ccall("@or");
	}
/* Exxclusive 'or' then primary and the secondary registers */
/*	(results in primary)	*/
xor()
{
	ccall("@xor");
	}
/*	'and' the primary and secondary registers */
/*	(results in primary) */
and()
{
	ccall("@and");
	}
/*	Arithmetic shift right the secondary register number of */
/*		times in primary (results in primary ) */
asr()
{
	ccall("@asr");
	}
/*	arithmetic left shift the secondary register number of */
/*		times in primary (results in primary) */
asl()
{
	ccall("@asl");
	}
/*	From two's complement of primary register */
neg()
{
	call("@neg");
	}
/*	form one's complement of primary register */
com()
{
	call("@com");
	}
pre_inc(lval)
int *lval;
{
	call("@preinc");
	inc_def(lval);
	}
post_inc(lval)
int *lval;
{
	call("@postinc");
	inc_def(lval);
	}
pre_dec(lval)
int *lval;
{
	call("@predec");
	inc_def(lval);
	}
post_dec(lval)
int *lval;
{
	call("@postdec");
	inc_def(lval);
	}
inc_def(lval)
int *lval;
{
	char *ptr;
	int value;

	ptr=lval[0];
	if (ptr[ident] == pointer && ptr[indcnt] != lval[1]) value=0x80;
	else value=0;
	value=value | data_size(lval);
	if (ptr[type] == cint && ptr[ident] != pointer) value=value | 0x80; 
	defbyte();
	outhex(value);
	nl();
	}
 
/*	following are the conditional operators */
/*	they compare the seconday register against the primary */
/*	and put a leteral 1 in the primary if the condition is */
/*	true otherwise the clear the primary register */
/*	change to not condition */
nlogical()
{
	call("@nlog");
	}
/*	test for logical and */
land()
{
	ccall("@land");
	}
/*	test for logical or */
lor()
{
	ccall("@lor");
	}
/*	test for equal */
eq()
{
	ccall("@eq");
	}
/*	test for not equal */
ne()
{
	ccall("@ne");
	}
/*	test for less than (signed) */
lt()
{
	ccall("@lt");
	}
/*	tet for less than or equal to (signed) */
le()
{
	ccall("@le");
	}
/*	test for greater than (signed) */
gt()
{
	ccall("@gt");
	}
/*	test for greater than or or equal (signed) */
ge()
{
	ccall("@ge");
	}
/*	test for less than (unsigned) */
ult()
{
	ccall("@ult");
	}
/*	Test for lees than or equal to (unsigned) */
ule()
{
	ccall("@ule");
	}
/*	test for greater than (unsigned) */
ugt()
{
	ccall("@ugt");
	}
/*	test for greater than or equal to (unsigned) */
uge()
{
	ccall("@uge");
	}

