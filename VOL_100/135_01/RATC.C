/*
	ratc.c---rational version of the VLI math package
	for BDSc.  Usage is as in example...

	by Hugh S. Myers

	build:
		n>cc ratc
		n>clink ratc vli math

	4/1/84
	4/9/84
*/

#include <bdscio.h>
#define MAX 256

main()
{
	char s1[MAX],s2[MAX];
	int i;

	forever {
		puts("? ");
		getline(s1,MAX);
		puts("? ");
		getline(s2,MAX);
		printf("s1+s2= %s\n",radd(s1,s2));
		printf("s1-s2= %s\n",rsub(s1,s2));
		printf("s1*s2= %s\n",rmul(s1,s2));
		printf("s1/s2= %s\n",rdiv(s1,s2));
	}
}

char *radd(t1,t2)
char *t1,*t2;
{
	char s1[MAX],s2[MAX];
	int i;

	strcpy(s1,t1);
	strcpy(s2,t2);
	i=scale(s1,s2);
	strcpy(s1,addl(s1,s2));
	insertch(s1,i,'.');
	return s1;
}

char *rsub(t1,t2)
char *t1,*t2;
{
	char s1[MAX],s2[MAX];
	int i;

	strcpy(s1,t1);
	strcpy(s2,t2);
	i=scale(s1,s2);
	strcpy(s1,subl(s1,s2));
	insertch(s1,i,'.');
	return s1;
}

char *rmul(t1,t2)
char *t1,*t2;
{
	char s1[MAX],s2[MAX];
	int i;

	strcpy(s1,t1);
	strcpy(s2,t2);
	i=scale(s1,s2);
	strcpy(s1,mull(s1,s2));
	insertch(s1,i+i,'.');
	return s1;
}

char *rdiv(t1,t2)
char *t1,*t2;
{
	char s1[MAX],s2[MAX];
	int i,j;

	strcpy(s1,t1);
	strcpy(s2,t2);
	i=scale(s1,s2);
	j=max(strlen(s1),strlen(s2));
	pad(s1,j);
	strcpy(s1,divl(s1,s2));
	insertch(s1,j,'.');
	return s1;
}

scale(s1,s2)
char *s1,*s2;
{
	int i,j;
	i=norm(s1);
	j=norm(s2);
	if(i>j) {
		pad(s2,i-j);
		j=i;
	}
	if(j>i) {
		pad(s1,j-i);
		i=j;
	}
	return i;
}

norm(s)
char s[];
{
	int i;

	if((i=israt(s))<0) {
		pad(s,1);
		return 1;
	}
	deletech(s,i);
	if(i==strlen(s))
		pad(s,1);
	return(strlen(s,1)-i);
}

israt(s)
char s[];
{
	int i;

	for(i=0;i<strlen(s);i++)
		if(s[i]=='.') {
			return i;
		}
	return -1;
}

deletech(s,n)
char s[];
int n;
{
	for(;n<strlen(s);n++)
		s[n]=s[n+1];
}

insertch(s,n,ch)
char s[],ch;
int n;
{
	int i;

	if(n<strlen(s)) {
		n = strlen(s)-n;
		for(i=strlen(s)+1;i>n;i--)
			s[i]=s[i-1];
		s[n]=ch;
		return;
	}
	if(n>strlen(s))
		prefix(s,n-strlen(s),'0');
	prefix(s,1,'.');
	return;		
}

pad(s,n)
char *s;
int n;
{
	for(;n>0;n--)
		strcat(s,"0");
}

prefix(t,n,ch)
char *t,ch;
int n;
{
	char s[MAX];
	int i;

	for(i=0;n>0;n--,i++)
		s[i]=ch;
	s[i]='\0';
	strcat(s,t);
	strcpy(t,s);
	return;
}