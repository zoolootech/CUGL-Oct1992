/*
TITLE:		ATOF;
DATE:		2/9/88;
DESCRIPTION:	"atof() and strtod()";
VERSION:	1;
KEYWORDS:	ATOF, STRTOD;
FILENAME:	ATOF.C;
WARNINGS:	"Probably causes rounding errors.";
SEE-ALSO:	
SYSTEM:		MS-DOS;
COMPILERS:	Aztec C86;
AUTHORS:	Dan Schechter;
 */

/* Public domain source code from KITTENSOFT by Dan Schechter.
Aztec C v 4.1b has a bug in atof() which is scheduled to be fixed in the
next release. Aztec C does not have strtod(). The following is intended to
be a TEMPORARY work-around for atof() and a BETTER-THAN-NOTHING strtod().
These functions will probably introduce minor rounding errors. 
Otherwise I think they work like their "real" counterparts. */

#include <stdio.h>

main()
{
	char *r,s[200];
	double d,atof(),strtod();
	
	for(;;){
		printf("?:");
		gets(s);
		if (s[0]==0) exit(0);
		d=strtod(s,&r);
		printf("%.15g\n%s\n",d,r);
	}
}


double atof(s)
char *s;
{
	double strtod();
	
	return( strtod(s,(char *)0) );
}

#define STRTODLEN 100

double strtod(s,p)
char *s,**p;
{
	char 	is[STRTODLEN], /* string to hold the integer portion */
		fs[STRTODLEN], /* string to hold the fractional portion */
		es[STRTODLEN]; /* string to hold the exponent portion */
	double 	ii,		/* the integer part as a double */
		fi,		/* the fractional part as a double */
		efac;		/* the factor represented by the 
				exponent portion of the string. */
	int 	ei,		/* the power of 10 */
		flen,		/* the length of the string containing 
				the decimal fraction */
		sign=1,		/* sign=0 if the number is < 0. */
		esign=1;	/* esign=0 if the exponent is < 0 */
	char *_digitcopy();
	double atoi_d();
	
	is[0]=fs[0]=es[0]= '\0';
	while((*s==' ')||(*s=='\t')) s++;   /* disregard leading white space */
	
	switch (*s){
		case '-': sign = 0;
		case '+': s++;
	}
	
	if ((*s)&&(*s != '.')) s= _digitcopy(s,is);
	if (*s == '.') s= _digitcopy(++s,fs);
	if ((*s == 'e')||(*s == 'E')) {
		switch (*(++s)){
			case '-': esign = 0;
			case '+': s++;
		}
		s= _digitcopy(s,es);
	}
	ii= atoi_d(is);
	fi= atoi_d(fs);
	ei= esign ? atoi(es) : -atoi(es);
	
	if (p) *p=s;
	
	efac=1.0;
	if (ei>0) for(;ei;ei--) efac *= 10.0;   /* This probably introduces */
	if (ei<0) for(;ei;ei++) efac /= 10.0;	/* rounding errors.         */
	
	flen= strlen(fs);
	while(flen--) fi /= 10.0;		/* Ditto. */
	
	if (!sign) efac = -efac;
	return ( (ii + fi) * efac );
}

double atoi_d(s)
char *s;
{
	double n=0.0;
	for (;*s;s++){
		n *= 10.0;
		n += *s - 48;
	}
	return(n);
}
 
char *_digitcopy(from,to)
char *from,*to;
{
	
	while((*from>='0')&&(*from<='9')) (*to++)=(*from++);
	*to=0;
	return(from);
}
