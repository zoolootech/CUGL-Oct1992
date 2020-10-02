/*addmoney will add dollars and cents which are presented in an array
which is 6 unsigned integers. The first integer is the dollars of the 
first number, the second integer the cents of the first number, the 
third integer the dollars of the second number, and the fourth integer 
the cents of the second number. The answer, which properly checks for
overflow of the cents, has the dollars in the fifth integer and cents in 
the sixth integer. This routine will only add money, it will not subtract!
The maximum value permitted in the answer (and obviously in either of the
values is $65535.99. There are no error checks. */

addmoney(money)
unsigned money[];
{
	money[4] = money[0] + money[2];
	money[5] = money[1] + money[3];
	if (money[5] > 99)
	{
		money[5] -= 100;
		money[4]++;
	}
	return;    
}

/* chkdate will check an ASCII string to see if it is a proper date. A 
ptoper date is one which has a number of 1 to 31 followed by a correct 
3-letter code. If an improper date is found, chkdate returns -1; else
it returns the day of year where JAN 1 is a 1 and every month has 31
days. */

chkdate(dd)
char dd[];
{
	int i,j,k;
	char dt[36];

	k = atoi(dd);
	if (k < 1 || k > 31)
		return(-1);
	i = 0;
	while(isdigit(dd[i++]));
	for (j=0,i--;j<3;j++,i++)
		dd[i] = toupper(dd[i]);
	i -=3;
	movmem("JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC",dt,36);
	for(j = 0; j < 36; j+=3)
		if(dt[j]==dd[i]&&dt[j+1]==dd[i+1]&&dt[j+2]==dd[i+2])
			return(((j/3)*31) + k);
	return(-1);
}

/*comma checks a buffer (resbuf) at an index (j) for the nth comma (nbr).
If it encounters an end-of-line(CR) or slash it returns a -1. Else it
returns the index to the first character after the comma.*/

comma(resbuf,j,nbr)
char resbuf[];
int j;
int nbr;
{
	int i;
	for (i = 0; i < nbr; i++)
	{
		while (resbuf[j] != ',')
		{
			if (resbuf[j] == 13 || resbuf[j] == 92)
				return(-1);
			j++;
		}
		j++;
	}
	return(j);
}
/*count counts the number of characters in the buffer (resbuf) from the 
index (j) up to either the first comma,first slash, first end-of-line (CR),
or end of buffer (control Z). It does not count blanks in the total. The 
total number of characters is returned as an integer.*/

count(resbuf,j)
char resbuf[];
int j;
{
	int i;
	for(i=0;resbuf[j]!=','&&resbuf[j]>26&&resbuf[j]!=92;j++) 
	if(resbuf[j] != ' ')
		i++;
	return(i);
}

/*count1 counts the number of characters in the buffer (resbuf) from the
index (j) up to either the first comma,first slash, first end-of-line (CR),
or end of buffer (control Z). It counts blanks in the total. The total number 
of characters is returned as an integer. */

count1(resbuf,j)
char resbuf[];
int j;
{
	int i;
	for (i=0;resbuf[j]!=','&&resbuf[j]>26&&resbuf[j]!=92;i++,j++);
	return(i);
}

/*error will print the alarm in the array (ss) and then hang until any 
character is typed on the console. there are no error conditions or exit
parameters*/

error(ss)
char ss[];
{

	printf("%s",ss);
	bdos(1);
}

/*findline will look for the first line of the buffer (resbuf) starting at the 
index (j) which has a first character match with the character in flag. Since 
findline first looks for the beginning of the nextline it is necessary to have
a special case for j in order to get the very first line. Therefore if j is
negative then the first character of the first line is also checked. If a match
is found then the index to the character which matches is returned, else a -1
is returned indicating no match found.*/

findline(resbuf,j,flag)
char resbuf[];
int j;
char flag;
{
	if(j < 0)
		if(resbuf[0] == flag)
			return(0);
	while((j = nextline(resbuf,j)) != -1)
		if (toupper(resbuf[j]) == flag)
			return(j);
	return(j);
}

/*getal is a special function used to retrieve airline information from
the airline file. It matches the two characters in al against the first 2 
characters of 32 byte groupings in the airline file. If it finds a match
it returns the 32 byte array in buf1 and a exit parameter of zero. If it
cannot find a match it returns -1.*/

getal(al,buf1,fd)
char al[];
char buf1[];
int fd;
{
	int i,j;
	for (i=0;;i++)
	{
		rwblk(fd,buf1,0,1,i);
		for(j=0;j<128;j+=32)
		{
			if((buf1[j] =='Z')&&(buf1[j+1]=='Z'))
				return(-1);
			if((buf1[j]==al[0])&&(buf1[j+1]==al[1]))
			{
				for (i=0;i<32;i++,j++)
					buf1[i] = buf1[j];
				return(0);
			}
		}
	}
}

/*getcit is a special routine which matches a set of 3-byte city codes (plus
a string-ending zero) against a file on disk. The city codes are in the 
array city, and the number of entries is in count. The disk file has a 
16-byte array for each city which is moved into buf2. If all the cities 
are found then a zero is returned, else a -1 is returned*/

getcit(city,buf1,buf2,count,fd)
char city[];
char buf1[];
char buf2[];
int count;
int fd;
{
	int i,j,k,m,n;
	n = 0;
	for (i=0;;i++)
	{	
		rwblk(fd,buf1,0,1,i);
		for(j=0;j<128;j+=16)
		{
			for (k=0;city[k] != 26;k+=4)
			{
				if(stgcmp("ZZZ",&buf1[j],3))
					return(-1);
				if(stgcmp(&city[k],&buf1[j],3))
				{	
         				movmem(&buf1[j],&buf2[k * 4],16);
					n++;   
					if(n==count)       
						return(0);
				}
			}
		}
	}
}

/*getdate will retrieve the date from the MPM function TOD and tranlate it 
into a set of ASCII characters in the array date. It checks to see if the 
TOD function has ever been run and if not prints an alarm to the operator
and exits back to MPM. Else it returns the month as 2 digits followed by
the day as 2 digits, followed by the last 2 digits of year. getdate does
proper checks for leap year and different size months.*/

getdate(date)
char date[];
{
int temp[3];
int day_tab[2][13];
int i,j,k;
int leap;
int year;
int yearday;
int month;
int day;

bdos (155,temp);
if(temp[0] == 761)
{
	printf("No TOD entered \n");
	exit(0);
}
for (yearday = temp[0],year = 2;;)
{
if ((yearday - 365) < 0)
	break;
yearday -= 365;
year++;
if ((yearday - 365) < 0)
	break;
yearday -=365;
year++;
if ((yearday - 366) < 0)
	break;
yearday -=366;
year++;
if ((yearday - 365) < 0)
	break;
yearday -=365;
year++;
}
initw(day_tab,"0,31,28,31,30,31,30,31,31,30,31,30,31");
initw(&day_tab[1][0],"0,31,29,31,30,31,30,31,31,30,31,30,31");
leap = year%4 == 0;
for (i = 1;yearday > day_tab[leap][i];i++)
	yearday -= day_tab[leap][i];
itoa2(i,&date[0]);
itoa2(yearday,&date[2]);
year += 76;
itoa2(year,&date[4]);
return;
}
/*itoa2 is a little function used by getdate to convert an integer which must
be between 0 and 99 to 2 ASCII characters*/

itoa2(n,s)
char s[2];
int n;
{
s[0] = n/10 + '0';
s[1] = n%10 + '0';
}

/*getfare is a specialized function used for computing base airline fares
from gross fares. It receives dollars and cents as 2 integers and divides 
them by the fraction represented by DIVA and DIVB (currently 1.05). The
answer is output as ASCII characters in the array out1 with 4 digits of
dollars,a decimal point and 2 digits of cents. Leading zeroes are turned 
into blanks.*/

#define DIVA 1050
#define DIVB 105
getfare(dollars,cents,out1)
int dollars;
int cents;
char out1[];
{
	int i;
	int out[7];
	for(i=0;i<7;i++)
	{
		out[i] = dollars/DIVA;
		dollars = dollars%DIVA;
		dollars = dollars*10;
	}
	for (i=4;i<7;i++)
	{
		cents = cents*10;
		out[i] = out[i] + (cents/DIVB);
		cents = cents%DIVB;
	}
	out[6] += 5;
	for(i=6;i>=0;i--)
		if(out[i] > 9)
		{
			out[i] -= 10;
			out[i-1]++;
		}

	for(i=0;out[i] == 0;i++)
		out[i] +=47;
	for(i=0;i<6;i++)
		out1[i] = out[i] + 48;
	out1[6] = out1[5];
	out1[5] = out1[4];
	out1[4] = '.';
}

/*getsec will get a sector or group of sectors from the file represented 
by fdd. The file must previously have been opened and it must have been
initialized such that the number of sectors in the file are represented 
by a single bit in the first 4 sectors of the file. Since the bit map 
for allocation is already allocated itself the first 4 bits must be set 
to 1 for this scheme to work. The function assumes that the file is 3568 
sectors long (the size of the largest file in my computer system at 
double density) so this constant should be adjusted when used on other
systems. The input parameters permit the allocation of either 1,2,or
4 sector sizes. This is controlled via opt where 1 = 1,2 = 2, and anything 
else = 4. It is possible to allocate several groups of sectors and this
is controlled by nbr. Thus to allocate 15 2-sector groupings set nbr to
15 and opt to 2. All sectors are allocated on even boundaries. i.e the 
first sector of a 4-sector allocation is always evenly divisible by itself
(After all 0 is the first sector) Because it is necessary to read the bit
map to allocate it is necessary to provide a 512-byte buffer for the getsec
function to use to input it from disk. Finally, the array addr will contain 
a list of the sectors which have been allocated. If a multi-sector allocation
is requested then the first of the 2 or 4 sectors is provided. Notice you
cannot mix sector sizes in a single call to getsec. If you want to check the
out-of-sectors condition I suggest you set zeros in addr and check for an 
answer of zero.*/

#define FILESIZE 446            /*3568/8*/
getsec(nbr,addr,fdd,opt,buf)
int nbr;
int addr[];
int fdd;
int opt;
char buf[];
{
	int i,j,k;
	char mask;
	int count;
	rwblk(fdd,buf,0,4,0);
	k = 0;
	for(i=0;i<FILESIZE;i++)
	{
		if(buf[i] == 0377)
			continue;
		if(opt <= 1)
		{
			mask = 0200;
			count = 1;
		}
		else if(opt == 2)
		{
			mask = 0300;
			count = 2;
		}
		else
		{
			mask = 0360;
			count = 4;
		}
		for(j=0;j<8;j+=count)
		{
			if(((buf[i]<<(j))&mask) == 0)
			{
				addr[k++]=(i<<3) + (j);
				buf[i] = buf[i] | (mask >> j);
				if(k == nbr)
				{
					rwblk(fdd,buf,1,4,0);
					return(0);
				}
			}
		}
	}
}

/*itoa is right out of the bible. I don't know why Leor never included it*/

itoa(n,s)        /*convert n to chacracters in s */
char s[];
int n;
{
	int i, sign;

	if ((sign = n) < 0)   /* record sign */
		n = -n;       /* make n positive */
	i = 0;
	do        /* generate digits in reverse order */
	{
		s[i++] = n%10+'0';  /* get next digit */
	}
	while ((n /= 10) > 0);  /*delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

/*lineemp will take a set of ASCII characters from a terminal buffer of
a size indicated by the integer size and called crtbuf, and compress them
into an ASCII buffer called resbuf which has CR LF and terminates with a 
control Z. The terminal buffer is checked for blank lines from the back 
end and they are completely eliminated. lineemp returns the index to the
control Z which provides the size of the disk buffer.*/

lineemp(resbuf,crtbuf,size)
char resbuf[];
char crtbuf[];
int size;
{
	int i,j,k,m;
	int lastline;

	for (i = size-1;i>0;i--)
		if(crtbuf[i] != ' ')
			break;
	lastline = i/80;
	for(i = k = m = 0;i<=lastline;i++,k+=80)
	{
		for(j = 80;j>0;j--)
			if(crtbuf[k+j-1] != ' ')
			break;
		movmem(&crtbuf[k],&resbuf[m],j);
		m = m + j;
		resbuf[m++] = 13;
		resbuf[m++] = 10;
	}
	resbuf[m] = 26;
	return(m);
}

/*linefull is the opposite of lineemp. It takes a typical text file from 
disk and puts it into a terminal buffer,throwing out the CR and LF and 
setting all unused characters to blanks. Caution! linefull cannot use
tabs.*/

linefull(resbuf,crtbuf)
char *resbuf;
char *crtbuf;
{
	char *line;
	char c;

	line = crtbuf;
	while((c = *resbuf++) != 26)
	{
		if(c == 13)
		{
			resbuf++;
			crtbuf = line = line + 80;
			continue;
		}
		*crtbuf++ = c;
	}
}

/*list will output a buffer of characters to the list device using the CPM
BDOS 5 function. The number of characters is specified by nbr. This function
is necessary because some idiot at Digital Research required a $ to terminate
the printer output and therefore you can't print any buffers with a $ in 
them*/

list(stadd,nbr)
char stadd[];
int nbr;
{
	int i;
	for ( i = 0; i < nbr; i++)
	{
		if (stadd[i] == '_')
			stadd[i] = ' ';
		bdos (5,stadd[i]);
	}
}

/*nextline searches an ASCII buffer for the beginning of the next line. If
it finds at least one more line it exits with an integer pointing to the
first character of that line. Else it exits with a -1.  j specifies the 
offset into the buffer to start the search. */

nextline(resbuf,j)
char resbuf[];
int j;
{
	while (resbuf[j] != 13) 
	{
		if (resbuf[j] == 26)
			return (-1);
		j++;
	}
	j++;
	if (resbuf[++j] == 26)
		return (-1);
	return(j);
}

/*percent will take the dollars in array money[0] and the cents in array
money[1], multiply it by the percent value in array money[2], and leave
the answer as unsigned integers with dollars in array money[3], and cents
in array money[4]. Proper rounding is used and cents are guaranteed to be
less then 100. Caution! Do not use with negative numbers. */

percent(money)
unsigned money[];
{
	unsigned i,j;
	i = money[0] * money[2];
	j = money[1] * money[2];
	j = j + 50;
	j = j/100;
	money[3] = i/100;
	money[4] = (i%100) + j;
	if (money[4] > 99)
	{
		money[4] -= 100;
		money[3] ++;
	}
	return;
}

/*putcity will move 3 characters from array s1 and put them in array s2. It
will then add a zero to array s2*/

putcity(s1,s2)
char *s1;
char *s2;
{
	int i;
	for (i=0;i<3;i++)
		*s2++ = *s1++;
	*s2 = 0;
}

/*putsec is the opposite of getsec. It will release sectors back to the system.
It has similar options to getsec and like getsec it cannot mix options. If 
putsec gets a sector number to release which is out of range or if the sector
to be released is not currently set it will exit with a -1 and display an 
alarm on the terminal*/

putsec(nbr,addr,fdd,opt,buf)
int nbr;
int addr[];
int fdd;
int opt;
char buf[];
{
	int i,j,k;
	char mask;
	rwblk(fdd,buf,0,4,0);
	if(opt <= 1)
		mask = 0200;
	else if(opt == 2)
		mask = 0300;
	else
		mask = 0360;
	for(i=0;i < nbr;i++)
	{
		if(addr[i] < FILESIZE * 8)
		{
			j = addr[i] >> 3;
			k = addr[i] & 7;
			if(((buf[j] << k) & mask) == mask)
			{
				buf[j] =buf[j] & ~(mask >> k);
				continue;
			}
		}
		printf("release err\n");
		i = -1;
		break;
	}
	rwblk(fdd,buf,1,4,0);
	return(i);
}

/*reverse is another routine from the bible*/

reverse(s)             /*reverse string s in place */
char s[];
{
	int c,i,j;
	
	for(i=0,j=strlen(s)-1;i<j;i++,j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

/*rwblk combines read,write,and seek into one function. If you tend to use
seek + read or seek + write a lot it will save you space. The parameters 
are fdd - the file descriptor, buf - the data buffer to read or write from,
rw - read/write flag 0 = read, nbc - number of sectors, sbc - starting 
sector.*/

rwblk(fdd,buf,rw,nbc,sbc)
int fdd;
char buf[];
int rw;
int nbc;
int sbc;
{
	int i;
	if((i == seek(fdd,sbc,0)) != -1)
		if(rw == 0)
			while((i = read(fdd,buf,nbc)) != -1)
				return(i);
		else 
			while((i = write(fdd,buf,nbc)) != -1)
				return(i);
	printf("disk error");
	return(i);
}
/*stgcmp is similar to strcmp. It does not require a zero to end the 
string. Instead it has a character count as the parameter nbr. stgcmp
will compare upper and lower case characters as equals and does not do
an arithmetic comparison. The exit parameter is either 1 (TRUE) for 
compare or zero (FALSE) for no compare.*/

stgcmp(rr,ss,nbr)
char rr[];
char ss[];
int nbr;
{
	int i;
	
	for(i = 0;i < nbr; i++)
		if(toupper(rr[i]) != toupper(ss[i]))
			return(0);
	return(1);
}

/*submoney is identical to admoney except it subtracts rather than adds. 
Since the money in array money[0] and money[1] is subtracted from the 
money in array money[2] and array money[3] it is very important to make 
sure that array money[2] and money[3] are larger than array money[0] and
money[1]. Array money[4] contains the dollar answer and array money[5]
contains the cents answer. */

submoney(money)
unsigned money[];
{
	money[4] = money[2] - money[0];
	if (money[3] < money[1])
	{
		money[3] +=100;
		money[4]--;
	}
	money[5] = money[3] - money[1];
	return(0);
}
