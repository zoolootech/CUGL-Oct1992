/*      >>>>>> start of cc6 <<<<<<      */
#ifndef	TRUE	/* see if need include file */
#include <C.DEF>
#endif

heir7(lval,status)
int lval[];
int *status;
{
	int k,lval2[lvalsize];
	k=heir8(lval,status);
	blanks();
	if (!streq(line+lptr,">>") &&
		!streq(line+lptr,"<<")) return k;
	rvalue(lval,k);
	while(1) {
		if (match(">>")) {
			push();
			k=heir8(lval2);
			rvalue(lval2,k);
			asr();
			*status=FALSE;
			}
		else if (match("<<")) {
			push();
			k=heir8(lval2);
			rvalue(lval2,k);
			asl();
			*status=FALSE;
			}
		else return 0;
		}
	}
heir8(lval,status)
int lval[];
int *status;
{
	int k,lval2[lvalsize];
	k=heir9(lval,status);
	blanks();
	if ((ch() != '+' && ch() != '-') || nch() == '=') return k;
	rvalue(lval,k);
	while(1) { 
		switch(ch()) {

			/* add last item to next item */
			case '+' :
				push();
				gch();
				k=heir9(lval2,status);
				rvalue(lval2,k);
				size_adjust(lval);
				add();
				*status=FALSE;
				break;

			/* subtract next item from current item */
			case '-' :
				push();
				gch();
				k=heir9(lval2,status);
				rvalue(lval2,k);
				size_adjust(lval);
				sub();
				*status=FALSE;
				break;

			/* operator not "+" or "-" */
			default :
				return 0;
			}
		}
	}
heir9(lval,status)
int lval[];
int *status;
{
	int k,lval2[lvalsize],temp;

	temp=0;
	k=heir10(lval,&temp,status);
	blanks();
	if ((ch() != '*' && ch()!='/' && ch() != '%') 
		|| nch() == '=') return k;
	rvalue(lval,k);
	while(1) {
		if (match("*")) {
			push();
			k=heir10(lval2,&temp,status);
			rvalue(lval2,k);
			mult();
			*status=FALSE;
			}
		else if (match("/")) {
			push();
			k=heir10(lval2,&temp,status);
			rvalue(lval2,k);
			div();
			*status=FALSE;
			}
		else if (match("%")) {
			push();
			k=heir10(lval2,&temp,status);
			rvalue(lval2,k);
			mod();
			*status=FALSE;
			}
		else return 0;
		}
	}
heir10(lval,count,status)
int lval[];
int *count;
int *status;
{
	int k,lval2[lvalsize];
	char *ptr,flag;

	if(match("++")) {
		if (!(k=heir10(lval,count,status))) {
			needlval();
			return LOADED;
			}
		lval[1] += *count;
		address(lval,k);
		pre_inc(lval);
		return LOADED;
		}
	else if (match("--")) {
		if (!(k=heir10(lval,count,status))) {
			needlval();
			return LOADED;
			}
		lval[1] += *count;
		address(lval,k);
		pre_dec(lval);
		*status=TRUE;
		return LOADED;
		}
	else if (match("-")) {
		if (isdigit(nch())) constant();
		else {
			k=heir10(lval,count,status);
			rvalue(lval,k);
			neg();
			*status=TRUE;
			}
		return LOADED;
		}	
	else if (match("*")) {
		++*count;
		k=heir10(lval,count,status);
		rvalue(lval,k);
		lval[1] += *count;
		*count=0;
		*status=FALSE;
		return ADDRESS;
		}
	else if (match("&")) {
		k=heir10(lval,count,status);
		*status=FALSE;
		if (k == 0 || lval[0]) {
			error("illegal address");
			return LOADED;
			}
		else if (lval[1]) return LOADED;
			else {
				immed();
				outstr(ptr=lval[0]);
				nl();
				return LOADED;
				}
		}
	else if (match("!")) {
		rvalue(lval,heir10(lval,count,status));
		nlogical();
		*status=TRUE;
		return LOADED;
		}
	else {
		k=heir11(lval,status);
		movmem(lval,lval2,lvalsize*2);
		lval2[1] += *count;
		if (match("++")) {
			if(k == LOADED) {
				needlval();
				return LOADED;
				}
			address(lval,k);
			post_inc(lval);
			*status=TRUE;
			return LOADED;
			}
		else if (match("--")) {
			if (k == LOADED) {
				needlval();
				return LOADED;
				}
			address(lval,k);
			post_dec(lval);
			*status=TRUE;
			return LOADED;
			}
		else return k;
		}
	}
k;
		}
	}

		else return k;