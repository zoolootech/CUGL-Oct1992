   
/********************************************************/
/*							*/
/*	expression handler 				*/
/*							*/
/*	table below show precedence of operator		*/
/*							*/
/********************************************************/
/*	operator	* routine	* associativity	*/
/********************************************************/
/*	| ++ -- - * &	* heir10	* left to right	*/
/********************************************************/
/*	* / %		* heir9		* rigth to left	*/
/********************************************************/
/*	+ -		* heir8		* left to right	*/
/********************************************************/
/*	<< >>		* heir7		* left to right	*/
/********************************************************/
/*	< <= > >=	* heir6		* left to right	*/
/********************************************************/
/*	== !=		* heir5		* left to rigth */
/********************************************************/
/*	&		* heir4		* left to rigth */
/********************************************************/
/*	^		* heir3		* left to right */
/********************************************************/
/*	|		* heir2		* left to right */
/********************************************************/
/*	&&		* heir1e	* left to right */
/********************************************************/
/*	||		* heir1d	* left to right */
/********************************************************/
/*	=  -= += *= %=  * heir1 	* left to right */
/*	/= &= ^= |=	*		*		*/
/********************************************************/
#ifndef TRUE	/* check to see if need include file */
#include <C.DEF>
#endif
expression()
{
        int lval[lvalsize],status;
	status=FALSE;
	rvalue(lval,heir1(lval,&status));
	return status;
	}
heir1(lval,status)
int lval[];
int *status;
{
        int k;
	char *ptr;
        k=heir1d(lval,status);
        if (ch() !=  '=' && nch() != '=') return k;
	*status=FALSE;
	switch(ch()) {

		/* just an equal */
		case '=' :
			gch();
			equal(lval,k);
			break;

		/* add value then store */
		case '+' :
		case '-' :
			gch();gch();
			equal_exp(lval,k);
			size_adjust(lval);
			if (ch() == '-') sub(); else add();
			k=1;
			break;

		case '*' :
			gch();gch();
			equal_exp(lval,k);
			mult();
			k=1;
			break;

		case '/' :
			gch();gch();
			equal_exp(lval,k);
			div();
			k=1;
			break;


		case '%' :
			gch();gch();
			equal_exp(lval,k);
			mod();
			k=1;
			break;


		case '&' :
			gch();gch();
			equal_exp(lval,k);
			and();
			k=1;
			break;

	
		case '^' :
			gch();gch();	
			equal_exp(lval,k);
			xor();
			k=1;
			break;
		
		case '|' :
			gch();gch();
			equal_exp(lval,k);
			or();
			k=1;
			break;

		default:
			return k;
		}
	store(lval,k);
	return 0;
	}
/*							*/
/*	written by Mike Bernson 1/81			*/
/*							*/
heir1d(lval,status)
int lval[];
int *status;
{
	int k,lval2[lvalsize];
	int label;
	k=heir1e(lval,status);
	blanks();
	if (ch() != '|' || nch() != '|') return k;
	rvalue(lval,k);
	label=getlabel();
	while(1) {
		if (match("||")) {
			truejump(label,*status);
			*status=TRUE;
			rvalue(lval2,heir1e(lval2,status));
			}
		else {
			sprintlabel(label);
			return 0;
			}
		}
	}
/*					*/
/*	written by Mike Bernson 1/81	*/
/*					*/
heir1e(lval,status)
int lval[];
int *status;
{
	int label;
	int k,lval2[lvalsize];
	k=heir2(lval,status);
	blanks();
	if (ch() != '&' || nch() != '&') return k;
	rvalue(lval,k);
	label=getlabel();
	while(1) {
		if (match("&&")) {
			testjump(label,*status);
			*status=TRUE;
			rvalue(lval2,heir2(lval2,status));
			}
		else {
			sprintlabel(label);
			return 0;
			}
		}
	}
heir2(lval,status)
int lval[];
int *status;
{
	int k,lval2[lvalsize];
	k=heir3(lval,status);
	blanks();
	if (ch() != '|' || nch() =='|' || nch()== '=') return k;
	rvalue(lval,k);
	while(1) {
		if (match("|")) {
			push();
			rvalue(lval2,heir3(lval2,status));
			or();
			*status=TRUE;
			}
		else return 0;
		}
	}
heir3(lval,status)
int lval[];
int *status;
{
	int k,lval2[lvalsize];
 	k=heir4(lval,status);
	blanks();
	if (ch() != '^' || nch() == '=') return k;
	rvalue(lval,k);
	while(1) {
		if (match("^")) {
			push();
			rvalue(lval2,heir4(lval2,status));
			xor();
			*status=TRUE;
			}
		else return 0;
		}
	}
heir4(lval,status)
int lval[];
int *status;
{
	int k,lval2[lvalsize];
	k=heir5(lval,status);
	blanks();
	if (ch() != '&' || nch()=='&' || nch() == '=') return k;
	rvalue(lval,k);
	while(1) {
		if (match("&")) {
			push();
			rvalue(lval2,heir5(lval2,status));
			and();
			*status=TRUE;
			}
		else return 0;
		}
	}
heir5(lval,status)
int lval[];
int *status;
{
	int k,lval2[lvalsize];
	k=heir6(lval,status);
	blanks();
	if ((streq(line+lptr,"==") ==0) &&
                (streq(line+lptr,"!=") == 0)) return k;
        rvalue(lval,k);
        while(1) {
		if (match("==")) {
			push();
			rvalue(lval2,heir6(lval2,status));
			eq();
			*status=TRUE;
			}
		else if (match("!=")) {
			push();
			rvalue(lval2,heir6(lval2,status));
			ne();
			*status=TRUE;
			}
		else return 0;
		}
	}
heir6(lval,status)
int lval[];
int *status;
{
	int k,lval2[lvalsize];
	k=heir7(lval,status);
	blanks();
	if ((streq(line+lptr,"<") == 0) &&
		(streq(line+lptr,">") == 0) &&
		(streq(line+lptr,"<=") == 0) &&
		(streq(line+lptr,">=") == 0)) return k;
	if (streq(line+lptr,">>")) return k;
	if (streq(line+lptr,"<<")) return k;
	rvalue(lval,k);
	while(1) {
		if (match("<=")) {
			push();
			rvalue(lval2,heir7(lval2,status));
			*status=TRUE;
			if (cptr=lval[0])
				if(cptr[ident]==pointer) {
					ule();
					continue;
					}
			if (cptr=lval2[0])
				if(cptr[ident]==pointer) {
					le();
					continue;
					}
			le();
			}
		else if (match(">=")) {
			push();
			rvalue(lval2,heir7(lval2,status));
			*status=TRUE;
			if (cptr=lval[0])
				if (cptr[ident]==pointer) {
					uge();
					continue;
					}
			if (cptr=lval2[0])
				if (cptr[ident]==pointer) {
					uge();
					continue;
					}
			ge();
			}
		else if ((streq(line+lptr,"<")) &&
			(streq(line+lptr,"<<")==0)) {
				inbyte();
				push();
				rvalue(lval2,heir7(lval2,status));
				*status=TRUE;
				if (cptr=lval[0])
					if (cptr[ident]==pointer) {
					ult();
					continue;
					}
				if(cptr=lval2[0])
					if (cptr[ident]==pointer) {
						ult();
						continue;
						}
			lt();
			}
		else if ((streq(line+lptr,">")) &&
			(streq(line+lptr,">>") == 0)) {
				inbyte();
				push();
				rvalue(lval2,heir7(lval2,status));
				*status=TRUE;
			if (cptr=lval[0])
				if (cptr[ident]==pointer) {
					ugt();
					continue;
					}
			if (cptr=lval2[0])
				if (cptr[ident]==pointer) {
					ugt();
					continue;
					}
			gt();
			}
		else return 0;
		}
	}

	if (isbyte(k)) call("@stacks"); else call("@stackl");
	defvalue(k);
	return newsp;
	}
/*	Do