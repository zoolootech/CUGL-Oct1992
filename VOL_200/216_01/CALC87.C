#include <stdio.h>
#include <math.h>
double mem[10];
main()
{
	double a,b,c;
	double getnum(),acos(),asin(),atan(),cos(),sqrt(),tan(),pow();
	char r,getreply();
	int i;
	
	for(;;){
		a=0;
		b=0;
		c=0;
		a=getnum();
		for(;;){
			printf(
"\nEnter your choice:
+ - * / s(Q)uare sq(R)oot (S)in (C)os (T)an arc(s)in arc(c)os arc(t)an
raise-to-the-(P)ower-of (I)nverse-reciprocal (0)-(9) store in memory 1-9
(N)ew (D)isplay-memories (E)rase-memories e(X)it    ");
			r=getreply();
			if (r=='X') exit();
			if (r=='N') break;
			if (r=='E') {
				for (i=0;i<10;i++) mem[i]=0;
				continue;
			}
			if (r=='D') {
				displaymem();
				printf("\n%1.16g\n",a);
				continue;
			}
			if ((r>47)&&(r<58)){
				r-=48;
				mem[r]=a;
				printf("\nMemory %d set to %1.16g\n",r,a);
				continue;
			}
			if (r=='Q') b=a;
			if (r=='*') b=getnum();
			if ((r=='Q')||(r=='*')) printf("\n  %1.16g\nx %1.16g\n= %1.16g\n",a,b,c=a*b);
			if (r=='/') printf("\n  %1.16g\n/ %1.16g\n= %1.16g\n",a,b=getnum(),c=a/b);
			if (r=='+') printf("\n  %1.16g\n+ %1.16g\n= %1.16g\n",a,b=getnum(),c=a+b);
			if (r=='-') printf("\n  %1.16g\n- %1.16g\n= %1.16g\n",a,b=getnum(),c=a-b);
			if (r=='S') printf("\nSin %1.16g\n  = %1.16g\n",a,c=sin(a));
			if (r=='s') printf("\nArcsin %1.16g\n     = %1.16g\n",a,c=asin(a));
			if (r=='C') printf("\nCos %1.16g\n  = %1.16g\n",a,c=cos(a));
			if (r=='c') printf("\nArccos %1.16g\n     = %1.16g\n",a,c=acos(a));
			if (r=='T') printf("\nTan %1.16g\n  = %1.16g\n",a,c=tan(a));
			if (r=='t') printf("\nArctan %1.16g\n     = %1.16g\n",a,c=atan(a));
			if (r=='R') printf("\nSq Root of %1.16g\n         = %1.16g\n",a,c=sqrt(a));
			if (r=='P') printf("\n  %1.16g  to the\n  %1.16g  power\n= %1.16g\n",a,b=getnum(),c=pow(a,b));
			if (r=='I') printf("\nReciprocal of %1.16g\n            = %1.16g\n",a,c=1.0/a); 
			a=c;
		}
	}
}
double getnum()
{
	char s[55],*p,*strchr(),*fgets(),c,getch();
	double n,atof();
	printf("\nEnter a number. Or (M)(n) for memory number 0-9. ");
	fgets(s,51,stdin);
	p=strchr(s,10);
	if (p) *p=0;
	else {
		printf("\n*** WARNING *** --> Entry too long. It has been truncated!\n");
		do c=getch(); 
		while (c!='\n');
	}
	if ((s[0]=='m')||(s[0]=='M'))
		return(n=mem[s[1]-48]);
	return(n=atof(s));
}
displaymem()
{
	int i;
	for (i=0;i<10;i++)
		printf("\nmemory %d = %1.16g",i,mem[i]);
	printf("\n");
}
char replies[29] = "*+-/PIQRSCTNDEXsct0123456789";
getreply()
{
	int i;
	char r;
	for (;;){
		r=getch();
		putchar(r);
		for(i=0;i<28;i++)
			if (r==replies[i]){
				printf("\n");
				return(r);
			}
	}
}
