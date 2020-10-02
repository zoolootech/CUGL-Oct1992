
/*  ENGLISH.C   no mod for V 1.43  */
#include "advent.h"

/*
	Analyze a two word sentence
*/
english()
{

	char *msg;
	int type1,type2,val1,val2;

	verb=object=motion=0;
	type2=val2=-1;
	type1=val1=-1;
	msg="bad grammar...";
	getin();
	if(!analyze(word1,&type1,&val1))
		return 0;
	if(type1==2 && val1==SAY) {
		verb=SAY;
		object=1;
		return 1;
	}
	if(strcmp(word2,""))
		if(!analyze(word2,&type2,&val2))
			return 0;
	/* check his grammar */
	if(type1==3) {
		rspeak(val1);
		return 0;
	}
	else if(type2==3) {
		rspeak(val2);
		return 0;
	}
	else if(type1==0) {
		if(type2==0) {
			printf("%s\n",msg);
			return 0;
		}
		else
			motion=val1;
	}
	else if(type2==0)
		motion=val2;
	else if(type1==1) {
		object=val1;
		if(type2==2)
			verb=val2;
		if(type2==1) {
			printf("%s\n",msg);
			return 0;
		}
	}
	else if(type1==2) {
		verb=val1;
		if(type2==1)
			object=val2;
		if(type2==2) {
			printf("%s\n",msg);
			return 0;
		}
	}
	else
		bug(36);
	return 1;
}


/*
		Routine to analyze a word.
*/
analyze(word,type,value)
char *word;
int *type,*value;
{
	int wordval,msg,i;

	/* make sure I understand */
	if((wordval=vocab(word,0))==-1) {
		switch(rand()%3) {
		case 0:
			msg=60;
			break;
		case 1:
			msg=61;
			break;
		default:
			msg=13;
		}
		rspeak(msg);
		return(0);
	}
	*type = wordval/1000;
	*value = wordval%1000;
	return 1;
}

/*
	routine to get two words of input
	and convert them to lower case
*/
getin()
{
	char *cptr,*bptr;
	char linebuff[65];

	putchar(0x80+'>');
	word1[0]=word2[0]='\0';
	bptr=linebuff;
	gets(linebuff);
	skipspc(&bptr);
	getword(&bptr,word1);
	if(!*bptr)
		return;
	while(!isspace(*bptr)) {
		if(!*bptr++)
			return;
	}
	skipspc(&bptr);
	getword(&bptr,word2);
}

/*
	Routine to extract one word from a buffer
*/
getword(buff,word)
char **buff,*word;
{
	int i;

	for(i=0;i<WORDSIZE;++i) {
		if(!**buff || isspace(**buff)) {
			*word='\0';
			return;
		}
		*word++=tolower(*(*buff)++);
	}
	*--word='\0';
}

/*
	Routine to skip spaces
*/
skipspc(buff)
char **buff;
{
	while(isspace(**buff)) ++(*buff);
}
