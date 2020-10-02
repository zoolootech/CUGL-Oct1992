/*	Small c compiler CD section	*/
/*	>>>>>>>> start of cc5 <<<<<<<<< */
/* This section contains the expression parser */
#INCLUDE CDEF.H
/*
	Modified	Feb, 1982  to add complement ~ unary operator
	in heir10
	Feb 6,1982	Changed pop()-add() sequences in heir8 and
		heir11 to addstk().
*/
expression()
{
	int lval[2];
	if(heir1(lval))rvalue(lval);
}
heir1(lval)
	int lval[];
{	
	int k,lval2[2];
	k=heir2(lval);
	if (match("="))
		{if(k==0){needlval();return 0;
			}
		if(lval[1])push();
		if(heir1(lval2))rvalue(lval2);
		store(lval);
		return 0;
		}
	else return k;
}
heir2(lval)
	int lval[];
{
	int k,lval2[2];
	k=heir3(lval);
	blanks();
	if(ch()!='|')return k;
	if(k)rvalue(lval);
	while(1)
		{if(match("|"))
			{push();
			if(heir3(lval2)) rvalue(lval2);
			pop();
			or();
			}
		else return 0;
		}
}
heir3(lval)
	int lval[];
{
	int k,lval2[2];
	k=heir4(lval);
	blanks();
	if(ch()!='^')return k;
	if(k)rvalue(lval);
	while(1)
		{if(match("^"))
			{push();
			if(heir4(lval2))rvalue(lval2);
			pop();
			xor();
			}
		else return 0;
		}
	}
heir4(lval)
	int lval[];
	{int k,lval2[2];
	k=heir5(lval);
	blanks();
	if(ch()!='&')return k;
	if(k)rvalue(lval);
	while(1)
		{if(match("&"))
			{push();
			if(heir5(lval2))rvalue(lval2);
			pop();
			and();
			}
		else return 0;
		}
	}
heir5(lval)
	int lval[];
	{int k,lval2[2];
	k=heir6(lval);
	blanks();
	if((streq(line+lptr,"==")==0)&&(streq(line+lptr,"!=")==0))return k;
	if(k)rvalue(lval);
	while(1)
		{if(match("=="))
			{push();
			if(heir6(lval2))rvalue(lval2);
			pop();
			eq();
			}
		else if(match("!="))
			{push();
			if(heir6(lval2))rvalue(lval2);
			pop();
			ne();
			}
		else return 0;
		}
	}
heir6(lval)
	int lval[];
{
	int k,lval2[2];
	k=heir7(lval);
	blanks();
	if((streq(line+lptr,"<")==0)&&(streq(line+lptr,">")==0)&&
		(streq(line+lptr,"<=")==0)&&(streq(line+lptr,">=")==0))return k;
	if(streq(line+lptr,">>"))return k;
	if(streq(line+lptr,"<<"))return k;
if(k)rvalue(lval);
while(1)
	{if(match("<="))
		{push();
		if(heir7(lval2))rvalue(lval2);
		pop();
		if(cptr=lval[0])
			if(cptr[ident]==pointer)
				{ule();
				continue;
				}
		if(cptr=lval2[0])
			if(cptr[ident]==pointer)
				{ule();
				continue;
				}
		le();
		}
	else if(match(">="))
		{push();
		if(heir7(lval2))rvalue(lval2);
		pop();
		if(cptr=lval[0])
			if(cptr[ident]==pointer)
				{uge();
				continue;
				}
		if(cptr=lval2[0])
			if(cptr[ident]==pointer)
				{uge();
				continue;
				}
		ge();
		}
	else if((streq(line+lptr,"<"))&&(streq(line+lptr,"<<")==0))
		{inbyte();
		push();
		if(heir7(lval2))rvalue(lval2);
		pop();
		if(cptr=lval[0])
			if(cptr[ident]==pointer)
				{ult();
				continue;
				}
		if(cptr=lval2[0])
			if(cptr[ident]==pointer)
				{ult();
				continue;
				}
		lt();
		}
	else if((streq(line+lptr,">"))&&(streq(line+lptr,">>")==0))
		{inbyte();
		push();
		if(heir7(lval2))rvalue(lval2);
		pop();
		if(cptr=lval[0])
			if(cptr[ident]==pointer)
				{ugt();
				continue;
				}
		if(cptr=lval2[0])
			if(cptr[ident]==pointer)
				{ugt();
				continue;
				}
		gt();
		}
	else return 0;
	}
}
/* >>>>>>>> start of cc6 <<<<<<<<< */
heir7(lval)
	int lval[];
{
	int k,lval2[2];
	k=heir8(lval);
	blanks();
	if((streq(line+lptr,">>")==0)&&(streq(line+lptr,"<<")==0))return k;
	if(k)rvalue(lval);
	while(1)
		{if(match(">>"))
			{push();
			if(heir8(lval2))rvalue(lval2);
			pop();
			asr();
			}
		else if(match("<<"))
			{push();
			if(heir8(lval2))rvalue(lval2);
			pop();
			asl();
			}
		else return 0;
		}
}
heir8(lval)
	int lval[];
{
	int k,lval2[2];
	k=heir9(lval);
	blanks();
	if((ch()!='+')&&(ch()!='-'))return k;
	if(k)rvalue(lval);
	while(1)
		{if(match("+"))
			{push();
			if(heir9(lval2))rvalue(lval2);
			if(cptr=lval[0])
				if((cptr[ident]==pointer)&&
					(cptr[type]==cint))
					doublereg();
			addstk();
			}
		else if(match("-"))
			{push();
			if(heir9(lval2))rvalue(lval2);
			if(cptr=lval[0])
				if((cptr[ident]==pointer)&&
					(cptr[type]==cint))
					doublereg();
			pop();
			sub();
			}
		else return 0;
		}
}
heir9(lval)
	int lval[];
{
	int k,lval2[2];
	k=heir10(lval);
	blanks();
	if((ch()!='*')&&(ch()!='/')&&(ch()!='%'))return k;
	if(k)rvalue(lval);
	while(1)
		{if(match("*"))
			{push();
			if(heir9(lval2))rvalue(lval2);
			pop();
			mult();
			}
		else if(match("/"))
			{push();
			if(heir10(lval2))rvalue(lval2);
			pop();
			div();
			}
		else if(match("%"))
			{push();
			if(heir10(lval2))rvalue(lval2);
			pop();
			mod();
			}
		else return 0;
		}
}
heir10(lval)
	int lval[];
{
	int k;
	char *ptr;
	if(match("++"))
		{if((k=heir10(lval))==0)
			{needlval();
			return 0;
			}
		if(lval[1])push();
		rvalue(lval);
		inc();
		ptr=lval[0];
		if((ptr[ident]==pointer)&&(ptr[type]==cint))
			inc();
		store(lval);
		return 0;
		}
	else if(match("--"))
		{if((k=heir10(lval))==0)
			{needlval();
			return 0;
			}
		if(lval[1])push();
		rvalue(lval);
		dec();
		ptr=lval[0];
		if((ptr[ident]==pointer)&&(ptr[type]==cint))
			dec();
		store(lval);
		return 0;
		}
	else if (match("-"))
		{k=heir10(lval);
		if(k) rvalue(lval);
		neg();
		return 0;
		}
/*
	complement operator added here
*/
	else if (match("~")) {
		k=heir10(lval);
		if(k) rvalue(lval);
		com();
		return 0;
	     }
	else if(match("*"))
		{k=heir10(lval);
		if(k) rvalue(lval);
		lval[1]=cint;
		if(ptr=lval[0])
			lval[1]=ptr[type];
		lval[0]=0;
		return 1;
		}
	else if(match("&"))
		{k=heir10(lval);
		if(k==0)
			{error("illegal address");
			return 0;
			}
		else if(lval[1])return 0;
		else
			{immed();
			outstr(ptr=lval[0]);
			nl();
			lval[1]=ptr[type];
			return 0;
			}
		}
	else
		{k=heir11(lval);
		if(match("++"))
			{if(k==0)
				{needlval();
				return 0;
				}
			if(lval[1])push();
			rvalue(lval);
			inc();
			ptr=lval[0];
			if((ptr[ident]==pointer)&&(ptr[type]==cint))
				inc();
			store(lval);
			dec();
			if((ptr[ident]==pointer)&&(ptr[type]==cint))
				dec();
			return 0;
			}
		else if(match("--"))
			{if(k==0)
				{needlval();
				return 0;
				}
			if(lval[1])push();
			rvalue(lval);
			dec();
			ptr=lval[0];
			if((ptr[ident]==pointer)&&(ptr[type]==cint))
				dec();
			store(lval);
			inc();
			if((ptr[ident]==pointer)&&(ptr[type]==cint))
				inc();
			return 0;
			}
		else return k;
		}
}
/* >>>>>> start of cc7 <<<<<<< */
heir11(lval)
	int *lval;
{
	int k;
	char *ptr;
	k=primary(lval);
	ptr=lval[0];
	blanks();
	if((ch()=='[')||(ch()=='('))
	while(1)
		{if(match("["))
			{if(ptr==0)
				{error("can't subscript");
				junk();
				needbrack("]");
				return 0;
				}
			else if(ptr[ident]==pointer) rvalue(lval);
			else if(ptr[ident]!=array)
				{error("can't subscript");
				k=0;
				}
			push();
			expression();
			needbrack("]");
			if(ptr[type]==cint)doublereg();
			addstk();
			lval[0]=0;
			lval[1]=ptr[type];
			k=1;
			}
		else if(match("("))
			{if(ptr==0)
				{callfunction(0);
				}
			else if(ptr[ident]!=function)
				{rvalue(lval);
				callfunction(0);
				}
			else callfunction(ptr);
			k=lval[0]=0;
			}
		else return k;
		}
	if(ptr==0)return k;
	if(ptr[ident]==function)
		{immed();
		outstr(ptr);
		nl();
		return 0;
		}
	return k;
	}
primary(lval)
	int *lval;
{
	char *ptr,sname[namesize];
	int num[1],k;
	if(match("("))
		{k=heir1(lval);
		needbrack(")");
		return k;
		}
	if(symname(sname))
		{if(ptr=findloc(sname))
			{getloc(ptr);
			lval[0]=ptr;
			lval[1]=ptr[type];
			if(ptr[ident]==pointer)
				lval[1]=cint;
			if(ptr[ident]==array) return 0;
			else return 1;
			}
		if(ptr=findglb(sname))
			if(ptr[ident]!=function)
				{lval[0]=ptr;
				lval[1]=0;
				if(ptr[ident]!=array) return 1;
				immed();
				outstr(ptr);
				nl();
				lval[1]=ptr[type];
				return 0;
				}
			ptr=addglb(sname,function,cint,0);
			lval[0]=ptr;
			lval[1]=0;
			return 0;
			}
		if(constant(num))
			return(lval[0]=lval[1]=0);
		else
			{error("invalid expression");
			immed();
			outdec(0);
			nl();
			junk();
			return 0;
			}
		}
store(lval)
	int *lval;
{
	if(lval[1]==0)putmem(lval[0]);
	else putstk(lval[1]);
}
rvalue(lval)
	int *lval;
{
	if((lval[0] !=0) && (lval[1] == 0))
		getmem(lval[0]);
	else indirect(lval[1]);
}
test(label)
	int label;
{
	needbrack("(");
	expression();
	needbrack(")");
	testjump(label);
}
constant(val)
	int val[];
{
	if(number(val))
		immed();
	else if(pstr(val))
		immed();
	else if(qstr(val))
		{immed();
		printlabel(litlab);
		outbyte('+');
		}
	else return 0;
	outdec(val[0]);
	nl();
	return 1;
}
number(val)
	int val[];
{
	int k,minus;
	char c;
	k=minus=1;
	while(k)
		{k=0;
		if (match("+"))
			k=1;
		if(match("-")){
			minus=(-minus);
			k=1;}
		}
	if(numeric(ch())==0)return 0;
	while(numeric(ch()))
		{c=inbyte();
		k=k*10+(c-'0');
		}
	if(minus<0) k=-k;
	val[0]=k;
	return 1;
}
pstr(val)
	int val[];
{
	int k;
	char c;
	k=0;
	if(match("'")==0) return 0;
	while((c=gch())!=39)
		k=(k&255)*256 + (c&127);
	val[0]=k;
	return 1;
}
qstr(val)
	int val[];
{
	char c;
	if(match(quote)==0) return 0;
	val[0]=litptr;
	while (ch()!='"')
		{if(ch()==0) break;
		if(litptr>=litmax)
			{error("string space exhausted");
			while(match(quote)==0)
				if(gch()==0) break;
			return 1;
			}
		litq[litptr++]=gch();
		}
	gch();
	litq[litptr++]=0;
	return 1;
}
 ÿ ‰ ÿ ÿ ÿ ÿ ÿ ÿ ÿ ÿ ÿ ÿ ÿ ÿ ÿ ÿ ÿ ÿ ÿ 