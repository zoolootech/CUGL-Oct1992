/*-------------------------------------------------
03/02/91
You are free to use the code in this file for ANY
purpose whatsoever.
If you can sell it.....go for it.
---------------------
Conrad Thornton
RR1 Box 87C
Downsville,La. 71234
--------------------------------------------------*/
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define ITEMSIZE (sizeof(char) + sizeof(char *))
#define ITSOK	  0
#define ERROR	 -1
#define QUEFULL  -2
#define QUEEMPTY -3

static char *cir_que,*writehead,*readtail,*queend;

int setupque(int);
int quewrite(char *,int);
int queread(char **,int *);
int quekill(void);

static int QUESTART;
static int ITEMSINQUE;
/*-------------------------------------------------*/
int setupque(qsize)
int qsize;
{
	int quesize;

	quesize = qsize * ITEMSIZE;
	cir_que = (char *)malloc(quesize+1);
	if(cir_que == (char *)NULL)
	   return(ERROR);
	memset(cir_que,'\0',quesize);
	readtail  = cir_que;
	writehead = readtail;
	queend	  = readtail+quesize;
	QUESTART  = 1;
	ITEMSINQUE= 0;
	return(ITSOK);
}
/*-------------------------------------------------*/
int quewrite(data,type)
char *data;
int  type;
{
	if(writehead == readtail && ! QUESTART)
	   return(QUEFULL);
	if(writehead == queend) {
	   if(readtail == cir_que)
	      return(QUEFULL);
	   writehead =	cir_que;
	}
	*writehead++ = (char) type;/* ins type and data */
	memcpy(writehead,&data,sizeof(char *));
	writehead += sizeof(char *);
	QUESTART   = 0;
	ITEMSINQUE++;
	return(ITSOK);
}
/*------------------------------------------------------*/
int queovwrite(data,type)
char *data;
int  type;
{
	char *p;
	int  ecode,j;

	if(writehead == queend)
	   writehead = cir_que;
	if(readtail  == queend)
	   readtail  = cir_que;
	if(writehead == readtail && ! QUESTART) {
	   ecode = queread(&p,&j); /* free previous malloc */
	   if(ecode < 0)
	      return(ERROR);
	   free(p);
	   readtail += ITEMSIZE;
	}	      /* now we can write in the old space */
	*writehead++ = (char) type;
	memcpy(writehead,&data,sizeof(char *));
	writehead += sizeof(char *);
	QUESTART   = 0;
	ITEMSINQUE++;
	return(ITSOK);
}
/*--------------------------------------------------------*/
int queread(p,type)
char **p;
int  *type;
{
	if(readtail == writehead)
	   return(QUEEMPTY);
	if(readtail == queend) {
	   if(writehead == cir_que)
	      return(QUEEMPTY);
	   readtail = cir_que;
	}
	*type = (int) *readtail++;
	memcpy(p,readtail,sizeof(char *));
	readtail += sizeof(char *);
	ITEMSINQUE--;
	if(ITEMSINQUE == 0) {
	   readtail  = cir_que;
	   writehead = cir_que;
	   QUESTART  = 1;
	}
	return(ITSOK);
}
/*------------------------------------------------------*/
int quekill()
{
	if(ITEMSINQUE)
	   return(ERROR);
	free(cir_que);
	return(ITSOK);
}
/*-----------------------------------------------------*/
