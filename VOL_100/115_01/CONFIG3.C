/* Screen editor configurator:  general utilities
 *
 * Source:  config3.c
 * Version: Jan. 10, 1981.
 */

/* convert upper case to lower case */

tolower(c) char c;
{
	if ((c>='A')*(c<='Z')) {
		return(c+32);
	}
	else {
		return(c);
	}
}

/* return: is first token in args a number ? */
/* return value of number in *val            */

number(args,val) char *args; int *val;
{
char c;
	c=*args++;
	if ((c<'0')+(c>'9')) {
		return(NO);
	}
	*val=c-'0';
	while (c=*args++) {
		if ((c<'0')+(c>'9')) {
			break;
		}
		*val=(*val*10)+c-'0';
	}
	return(YES);
}

/* convert character buffer to numeric */

ctoi(buf,index) char *buf; int index;
{
int k;
	while ( (buf[index]==' ') +
		(buf[index]==TAB) ) {
		index++;
	}
	k=0;
	while ((buf[index]>='0')*(buf[index]<='9')) {
		k=(k*10)+buf[index]-'0';
		index++;
	}
	return(k);
}


/* return maximum of m,n */

max(m,n) int m,n;
{
	if (m>=n) {
		return(m);
	}
	else {
		return(n);
	}
}

/* return minimum of m,n */

min(m,n) int m,n;
{
	if (m<=n) {
		return(m);
	}
	else {
		return(n);
	}
}

/* put decimal integer n in field width >= w */

putdec(n,w) int n,w;
{
char chars[10];
int i,nd;
	nd=itoc(n,chars,10);
	i=nd;
	while (i++<w) {
		putchar(' ');
	}
	i=0;
	while (i<nd) {
		putchar(chars[i++]);
	}
}

/* convert integer n to character string in str */

itoc(n,str,size) int n; char *str; int size;
{
int absval;
int len;
int i,j,k;
	absval=abs(n);
	/* generate digits */
	str[0]=0;
	i=1;
	while (i<size) {
		str[i++]=(absval%10)+'0';
		absval=absval/10;
		if (absval==0) {
			break;
		}
	}
	/* generate sign */
	if ((i<size)&(n<0)) {
		str[i++]='-';
	}
	len=i-1;
	/* reverse sign, digits */
	i--;
	j=0;
	while (j<i) {
		k=str[i];
		str[i]=str[j];
		str[j]=k;
		i--;
		j++;
	}
	return(len);
}

/* return absolute value of n */

abs(n) int n;
{
	if (n<0) {
		return(-n);
	}
	else {
		return(n);
	}
}

                                                                                                                                