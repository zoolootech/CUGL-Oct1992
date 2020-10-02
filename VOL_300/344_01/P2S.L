/*
* File: P2S.L
*
* Program: 
*		 		P2S
*
*		 printf() to streams i/o converter
*
* Description:
*
* Converts printf() calls to << operators
* Uses Lex to find printf's, fprintf's and sprintf's in C source
* and then uses p2s() call to convert the printf arguments to C++ streams
* formatted I/O.
* Does handle things like %6.2f (width and precision flags)
* by generating the appropriate streams manipulators - setw()
* and setprecision().
* Does NOT handle things like %-6.2f (yet) since there are no manipulators
* for left and right justification (if you decide to enhance this to
* handle this or anything else please submit back to CUG or me)
*
* stdin reads inoriginal C source, converted source goes to stdout.
*
* Usage: 
*
* P2S < {.c file with printfs} > {file with streams i/o}
* Options:
*	f - don't convert fprintf()'s
*	p - don't convert printf()'s
*	s - don't convert sprintf()'s
*	i - converts by doing 
*		#ifdef __cplusplus
*		  ... streams code ..
*		#else
*		  ... old printf code ...
*		#endif
*	    to maintain backward compatibility with C (if desired)
*
* Example:
*
* For example to only convert fprintf calls and maintain C compatibility
* the invocation is:
*
* 	p2s p s i < foo.c >foo.cpp
*	 	
*	
* Construction:
*
* Just flex (or lex) this file, and compile the resultant C source. 
* MKP2S.BAT shows how to make program with Flex and Turbo C.
*
* Restrictions:
* 
* You may use this program without restriction as a tool in your own 
* development efforts. 
* You MAY NOT redistribute either the source or executables, in modified
* or unmodified form, for any amount of money or along with any other 
* commercial package. You must include this copyright in any program
* which uses this source and you may not distribute such programs 
* commercially. No restrictions apply to any programs which you converted
* using this tool (using the tool is unrestricted, distributing it or using
* its source is).
* 
* Copyright (c) 1990. Adam Blum, Retrieval Systems Corporation,(703)-524-4534
*/

%{
#include<stdio.h>
#include<ctype.h>
/* prototypes */
int p2s(char *s1,char *s2);
char *prs(char *p,char *s);
char *getfmt(char *p,int *prec,int *width);
void strrepl(char *s,char *srch,char *repl);

#define QUOTE 34

/* variables */
int ifdef=0,sprt=1,fprt=1,prt=1,i;

%}

%%
/*		PATTERNS			*/

#define.*\n	{ECHO;}
printf\([^\;]*\)\; {
		if(prt)
			p2sprt(yytext,yyleng,yytext+7);
		else {
			ECHO;
		}
		return 1;}
fprintf\([^\;]*\)\; {
		if(fprt)
			p2sprt(yytext,yyleng,yytext+8);
		else{
			ECHO;
		}
		return 1;}

sprintf\([^\;]*\)\; {
		if(sprt)
			p2sprt(yytext,yyleng,yytext+8);
		else {
			ECHO;
		}
		return 1;}
.|\n	{ECHO;return 1;}
%%
/*		FUNCTIONS			*/
main(int argc,char **argv)
{
	for(i=0;i<argc;i++){
		switch(toupper(argv[i][0])){
		case 'I':ifdef=1;break;
		case 'S':sprt=0;break;
		case 'F':fprt=0;break;
		case 'P':prt=0;break;
		}
	}		
	fprintf(stderr,	       
"printf to C++ streams converter, (c) 1990, Adam Blum, Retrieval Systems Corp.\n");
	fprintf(stderr,
"Usage: p2s <{source with printf calls} >{output with streams operators}\n");
	for(;;){
		if(yylex()<1)break;
	}
}

ctparens(char *s,int n)
{
	int i,ct=0;
	for(i=0;i<n;i++)	
		if(s[i]=='(')
			ct++;
		else if(s[i]==')')
			ct--;
	return ct;
}

/* P2SPRT
* Print the converted strings
*/
p2sprt(char *base,int n,char *s)
{
	char s1[256],s2[1024];
	strncpy(s1,s,n);
	s1[n]=0;
	p2s(s1,s2);	
	if(ifdef)
		printf("\n#ifdef __cplusplus\n%s;\n#else\n%s\n#endif\n",s2,base);
	else
 		printf("\n/* was %s*/\n%s;",base,s2);
}


/* P2S
* Convert printf arguments to formatted streams I/O
*/
p2s(char *s1,char *s2)
{
	char srch[16],arg[256],*p=s1;
	int replno=0,width,prec,i;
	/* grab the filename first, and pass it through
	 untranslated. streams opening and closing of
	 file will have to be translated manually */
	for(;isspace(*p);p++)
		;
	if(*p==QUOTE){
		strcpy(s2,"cout\0");
	}
	else if(!strncmp(p,"stderr",6)){
		strcpy(s2,"cerr\0");
		/* read up to first quote mark for formatting string */
		for(;*p!='"'&&*p;p++)
			;

	}
	else{
		p=prs(p,s2);
		/* stream operator directed to file stream */
		/* read up to first quote mark for formatting string */
		for(;*p!='"'&&*p;p++)
			;
	}
	if(!*p)return 0; /* not really a printf call */
	p++; /* skip over quote otherwise*/


	for(;;){
		/* read up to a formatting code (starts with "%")
		but skip any "%%"s and treat them as single "%" */
		for(	i=0;
			(*p!='%'||*(p+1)=='%')	&&
			(*p!=QUOTE||*(p-1)=='\\');
			p++,i++
		)
		{
			arg[i]=*p;
			if(*p=='%'&&*(p+1)=='%')
				p++;
		}
		if(i){ /* if there is a literal */
			arg[i]=0;
			sprintf(s2+strlen(s2),"<<%c%s%c\0",QUOTE,arg,QUOTE);
		}
		if(*p=='%'){
			p=getfmt(p,&width,&prec);
			if(width>=0)
				sprintf(s2+strlen(s2),
					"<<setw(%d)",width);
			if(prec>=0)
				sprintf(s2+strlen(s2),
					"<<setprecision(%d)",prec);
			sprintf(s2+strlen(s2),"<<(_%d_)",replno++);
		}
		if(*p==QUOTE)break;
	}

	/* read up to and beyond first comma before printf arguments */
	for(;*p!=',';p++)
		;
	p++;  /* skip over comma */

	replno=0;  /* set number back to create search and replace strings */
	for(;;){
		p=prs(p,arg);
		sprintf(srch,"_%d_",replno++);
		strrepl(s2,srch,arg);
		if(*p==')'||!*p)break;
	}
	return 1;
}


/* GETFMT
* extracts width and precision number from printf formatting code
*/
char *getfmt(char *p,int *width,int *prec)
{
	char *period,*last;
	if(*p++!='%')
		return 0; /* wasnt even a formatting code */
	/* find last char of fmt code (the alphabetic char) */
	for(last=p;!isalpha(*last);last++)
		;
	if(last==p){ /* if its the next one after %, no width or prec */
		*prec=-1;
		*width=-1;
		return last+1; 
	}
	period=strchr(p,'.'); 
	if(period==p) /* no width if . next to % */
		*width=-1;
	else	      
		*width=atoi(p);
	*prec=period?atoi(period+1):-1; /* no precision if didnt find period */
	return last+1;
}

/*
* PRS
* Parse out field s from buffer pointed to by p, moving p to next field
* field ends on comma, null, or right paren that ends the first left paren
* scope in field, or is out of scope of any parens
*/
char *prs(char *p,char *s)
{
	int i,parct=0;
	for(i=0;*p!=','&&*p;p++,i++){
		/* we are sensitive to paren nesting level:
		 * if we reach a right paren and we have seen as many or fewer
    	         * left parens so far, we have reached end of field 
		 */
		if(*p=='(')parct++;
		if(*p==')'){
			if(--parct<0)break;
		}
		s[i]=*p;
	}
	s[i]=0;
	if(!*p||p==')')return p;
	p++;
	return p;
}

void strrepl(s,srch,repl)
char *s,*srch,*repl;
{
	char near *found;
	char temp[1024];int ofs;
	for(;;){
		found=(char near *)strstr(s,srch);
		if(found){
			ofs=(int)((char near *)found-(char near *)s);
			strncpy(temp,s,ofs);
			temp[ofs]=0;
			strcat(temp,repl);
			strcat(temp,s+ofs+strlen(srch));
			strcpy(s,temp);
		}
		else
			break;
	}
}

