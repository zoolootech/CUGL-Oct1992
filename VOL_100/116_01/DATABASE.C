
/*  programs in DATABASE.C    no changes for V 1.43   */
#include "advent.h"

/*
	Routine to fill travel array
	for a given location.
*/
gettrav(loc)
int loc;
{
	int opt;
	char atrav[20];

	swchfd(3);
	fseek(dbuff,idx3[(loc-1)/10],0);
	rdskip(dbuff,'#',(loc-1)%10+1,0);
	rdskip(dbuff,'\n',1,0);
	for(opt=0;opt<MAXTRAV;++opt) {
		rdupto(dbuff,'\n',0,atrav);
		if(atrav[0]=='#'){
			travel[opt].tdest=-1;
			return;
		}
		travel[opt].tcond=0;
		initw(&travel[opt],atrav);
	}
	bug(33);
}

/*
	Function to scan a file up to a specified
	point and either print or return a string.
*/
rdupto(buff,delim,print,string)
char*buff,delim,print,*string;
{
	int c;

	while((c=getc(buff))!=delim) {
		if(c==-1)
			bug(35);
		if(c==EOF)
			return 0;
		if(c=='\015')
			continue;
		if(print)
			putchar(c);
		else
			*string++=c;
	}
	if (!print)
		*string='\0';
	return 1;
}

/*
	Function to read a file skipping
	a give character a specified number
	of times, with or without repositioning
	the file.
*/
rdskip(buff,x,n,rewind)
char *buff,x,rewind;
int n;
{
	int c;

	if(rewind)
		if(fseek(buff,0,0)==-1)
			bug(31);
	while(n--)
		while((c=getc(buff))!=x)
			if(c==-1 || c==EOF)
				bug(32);
}

/*
	routine to seek in buffered file
*/
struct buf {
	int bfd;
	int nleft;
	char *nextp;
	char buff[128];
};

fseek(iobuf,offset,code)
struct buf *iobuf;
int offset;
char code;
{
	int i;

	if (code) {
		for (i=0;i<offset;++i)
			if(getc(iobuf)==-1)
				return(-1);
	}
	else {
		if(seek(iobuf->bfd,offset/128,0)==-1)
			return(-1);
		iobuf->nleft = 0;
		for (i=0;i<offset%128;++i)
			if(getc(iobuf)==-1)
				return(-1);
	}
}

/*
	Routine to switch fd's in a
	file buffer to allow buffer sharing
*/
swchfd(n)
int n;
{
	int *fdptr;

	fdptr = dbuff;
	*fdptr = fd[n];
}

/*
	Routine to request a yes or no answer
	to a question.
*/
yes(msg1,msg2,msg3)
int msg1,msg2,msg3;
{
	char answer[80];

	if(msg1)
		rspeak(msg1);
	putchar('>');
	gets(answer);
	if(tolower(answer[0])=='n') {
		if(msg3)
			rspeak(msg3);
		return 0;
	}
	if(msg2)
		rspeak(msg2);
	return 1;
}

/*
	Print a random message from database 6
*/
rspeak(msg)
int msg;
{
	if (msg == 54) {
		printf("ok.\n");
		return;
	}
	swchfd(6);
	fseek(dbuff,idx6[(msg-1)/10],0);
	rdskip(dbuff,'#',(msg-1)%10+1,0);
	rdskip(dbuff,'\n',1,0);
	rdupto(dbuff,'#',1,0);
}

/*
	Routine to print the message
	for an item in a given state.
*/
pspeak(item,state)
int item,state;
{
	swchfd(5);
	rdskip(dbuff,'#',item,1);
	rdskip(dbuff,'/',state+2,0);
	rdupto(dbuff,'/',1,0);
}

/*
	Print the long description of a location
*/
desclg(loc)
int loc;
{
	swchfd(1);
	fseek(dbuff,idx1[(loc-1)/10],0);
	rdskip(dbuff,'#',(loc-1)%10+1,0);
	rdskip(dbuff,'\n',1,0);
	rdupto(dbuff,'#',1,0);
}

/*
	Print the short description of a location
*/
descsh(loc)
int loc;
{
	swchfd(2);
	fseek(dbuff,idx2[(loc-1)/10],0);
	rdskip(dbuff,'#',(loc-1)%10+1,0);
	rdskip(dbuff,'\n',1,0);
	rdupto(dbuff,'#',1,0);
}

/*
	routine to look up a vocabulary word.
	word is the word to look up.
	val  is the minimum acceptable value,
		if != 0 return %1000
*/
vocab(word,val)
char *word;
int val;
{
	char vword[WORDSIZE];
	int v;
	char *wptr;
	int wval;

	wval = 0;
	wptr = fastverb;
	while (*wptr) {
		if (!strcmp(word,wptr)) {
			v = fastvval[wval];
			if (!val)
				return v;
			if (val <= v)
				return v%1000;
		}
		while (*wptr++);
		++wval;
	}
	/* reposition file */
	swchfd(4);
	if(fseek(dbuff,fastvseek,0)==-1)
		bug(21);
	while(1) {
		/* glom onto a word */
		if(!rdupto(dbuff,',',0,vword))
			return(-1);
		/* compare words */
		if (!strcmp(word,vword)){
			if(!rdupto(dbuff,'\n',0,vword))
				bug(30);
			v=atoi(vword);
			if(!val)
				return v;
			if(val<=v)
				return v%1000;
		}
		rdskip(dbuff,'\n',1,0);
	}
}

/*
	Utility Routines
*/

/*
	Routine to test for darkness
*/
dark()
{
	return(!(cond[loc] & LIGHT) &&
		(!prop[LAMP] ||
		!here(LAMP)));
}

/*
	Routine to tell if an item is present.
*/
here(item)
int item;
{
	return(place[item]==loc || toting(item));
}

/*
	Routine to tell if an item is being carried.
*/
toting(item)
int item;
{
	return(place[item]==-1);
}

/*
	Routine to tell if a location causes
	a forced move.
*/
forced(atloc)
int atloc;
{
	return(cond[atloc]==2);
}

/*
	Routine true x% of the time.
*/
pct(x)
int x;
{
	return(rand()%100 < x);
}

/*
	Routine to tell if player is on
	either side of a two sided object.
*/
at(item)
int item;
{
	return(place[item]==loc || fixed[item]==loc);
}

/*
	Routine to destroy an object
*/
dstroy(obj)
int obj;
{
	move(obj,0);
}

/*
	Routine to move an object
*/
move(obj,where)
int obj,where;
{
	int from;

	from = (obj<MAXOBJ) ? place[obj] : fixed[obj];
	if(from>0 && from<=300)
		carry(obj,from);
	drop(obj,where);
}

/*
	Juggle an object
	currently a no-op
*/
juggle(loc)
int loc;
{
}

/*
	Routine to carry an object
*/
carry(obj,where)
int obj,where;
{
	if(obj<MAXOBJ){
		if(place[obj]==-1)
			return;
		place[obj]=-1;
		++holding;
	}
}

/*
	Routine to drop an object
*/
drop(obj,where)
int obj,where;
{
	if(obj<MAXOBJ) {
		if(place[obj]==-1)
			--holding;
		place[obj]=where;
	}
	else
		fixed[obj-MAXOBJ]=where;
}

/*
	routine to move an object and return a
	value used to set the negated prop values
	for the repository.
*/
put(obj,where,pval)
int obj,where,pval;
{
	move(obj,where);
	return((-1)-pval);
}
/*
	Routine to check for presence
	of dwarves..
*/
dcheck()
{
	int i;

	for(i=1;i<(DWARFMAX-1);++i)
		if(dloc[i]==loc)
			return i;
	return 0;
}

/*
	Determine liquid in the bottle
*/
liq()
{
	int i,j;
	i=prop[BOTTLE];
	j=-1-i;
	return liq2(i>j ? i : j);
}

/*
	Determine liquid at a location
*/
liqloc(loc)
int loc;
{
	if(cond[loc]&LIQUID)
		return liq2(cond[loc]&WATOIL);
	else
		return liq2(1);
}

/*
	Convert  0 to WATER
		 1 to nothing
		 2 to OIL
*/
liq2(pbottle)
int pbottle;
{
	return (1-pbottle)*WATER+(pbottle>>1)*(WATER+OIL);
}

/*
	Fatal error routine
*/
bug(n)
int n;
{
	printf("Fatal error number %d\n",n);
	exit();
}
