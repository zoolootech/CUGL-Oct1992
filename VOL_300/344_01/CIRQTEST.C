/*----------------------------------------------
Test code for cirque.c
You are free to use any or all for any purpose
whatsoever.
--------------------
Conrad Thornton
-----------------------------------------------*/
#include <stdio.h>
#include <malloc.h>
#include <string.h>

extern int setupque(int);
extern int quewrite(char *,int);
extern int queread(char **,int *);
extern int quekill(void);

typedef struct {
 char name[50];
 int  agelie;
 }xrs;
/*----------------------------------------------*/
main(argc,argv)
int  argc;
char *argv[];
{
       int  j,k,ecode,snum;
       char buff[50],*gets();

       if(argc < 2) {
	  printf("Usage: program-name Queue-Size\n");
	  exit(1);
       }
       snum = atoi(argv[1]);
       if((ecode = setupque(snum)) != 0) {
	   printf("Cannot malloc for Que\n");
	   exit(1);
       }
       printf("Queue Size = %d\n",snum);
       while(1) {
	  ecode = 0;
	  printf("1=Ins-1-Record 2=Read-All-Data 3=Quit ");
	  j = getch();
	  printf("\n");
	  if(j == '3') {
	     if((ecode = quekill()) != 0) {
		printf("\nTHE QUEUE IS NOT EMPTY. CANNOT KILL IT\n");
		printf("SELECT 2=Read-All-Data  then 3=Quit\n\n");
	     }
	     else
		break;
	  }
	  else if(j == '2')
	     ecode = read_em();
	  else if(j == '1') {
	     printf("\n1=char string  2=ints(6)  3=struct ");
	     k = getch();
	     k -= 48;
	     printf("\n");
	     switch(k)
	     {
	     case 1: printf("Enter String ..Limit 48 chars\n");
		     gets(buff);
		     buff[48]=NULL;
		     ecode = insert(buff,snum,1);
		     break;
	     case 2: printf("Enter seed number\n");
		     gets(buff);
		     snum = atoi(buff);
		     ecode = insert(buff,snum,2);
		     break;
	     case 3: printf("Enter int value for struct\n");
		     gets(buff);
		     snum = atoi(buff);
		     printf("Enter string for name field..Limit 48 chars\n");
		     gets(buff);
		     buff[48]=NULL;
		     ecode = insert(buff,snum,3);
	     }
	  }
	  if(ecode != 0)
	     printf("Error: Errcode = < %d >\n",ecode);
       }
}
/*-------------------------------------------------------------*/
int insert(str,val,key)
char *str;
int  val,key;
{
       int  x,z,ecode;
       char *pc,*getptc();
       int  *pi,*px,*getpti();
       xrs  *pxrs;

       ecode = 0;
       switch(key)
       {
       case 1: if((pc = getptc(str)) == (char *)NULL) {
		  printf("Could not malloc for string\n");
		  ecode = -4;
	       }
	       else if((x = quewrite(pc,1)) != 0) {
		  printf("Quewrite failed on string load\n");
		  ecode = -5;
	       }
	       break;
       case 2: if((pi = getpti(6)) == (int *)NULL) {
		  printf("Could not malloc for 6 int's\n");
		  ecode = -6;
	       }
	       else {
		  z  = val;
		  px = pi;
		  x = 0;
		  while(x < 6) {   /* just assigns nums for test */
		     *px = z;
		     ++px;
		     ++x;
		     ++z;
		  }
		  if((x = quewrite((char *)pi,2)) != 0) {
		     printf("Quewrite failed on int load\n");
		     ecode = -7;
		  }
	       }
	       break;
       case 3: pxrs = (xrs *) malloc(sizeof(xrs));
	       if(! pxrs) {
		  printf("Could not malloc for struct\n");
		  ecode = -8;
	       }
	       else {
		  strcpy(pxrs->name,str);
		  pxrs->agelie = val;
		  if((x = quewrite((char *)pxrs,3)) != 0) {    /* xrs type 3 */
		     printf("Quewrite failed on struct load\n");
		     ecode = -9;
		  }
	       }
       }
       return(ecode);
}
/*-----------------------------------------------------------------*/
int read_em() /* This temp fun ALWAYS returns QUEEMPTY (-3) at end */
{
       int  x,z,ecode;
       char *pc,*getptc();
       int  *pi,*px,*getpti();
       xrs  *pxrs;

       do {
	  ecode = queread(&pc,&x);
	  if(ecode >= 0) {		      /* queue is not empty */
	     if(x == 1) {		    /* it's our char string */
		printf("String = %s\n",pc);
		free(pc);
	     }
	     else if(x == 2) {			   /* its our int's */
		pi = px = (int *)pc;
		x = 0;
		while(x < 6) {
		   printf("%d ",*px);
		   ++x;
		   ++px;
		}
		printf("\n");
		free(pi);
	     }
	     else if(x == 3) {		      /* it's our structure */
		pxrs = (xrs *) pc;
		printf("Name = %s Age = %d\n",pxrs->name,pxrs->agelie);
		free(pxrs);
	     }
	  }
	  else {
	     printf("Ecode returned val = %d\n",ecode);
	     break;
	  }
       }while(1);
       return(ecode);
}
/*------------------------------------------------------------------*/
char *getptc(str)
char *str;
{
	int  ln;
	char *p;

	ln = strlen(str);
	if(ln == 0)
	   return((char *)NULL);
	if((p = (char *)malloc(ln+1)) != (char *)NULL)
	   memcpy(p,str,ln+1);
	return(p);
}
/*-----------------------------------------------------------------*/
int *getpti(many)
int many;
{
	int *pix;

	pix = (int *)malloc(many*sizeof(int));
	return(pix);
}
/*-----------------------------------------------------------------*/
