/* 29-APRIL-81 */
/* this should be the latest version of the help program */
/* currently set up to read HELP.EDI for Scott's QED */

#include "bdscio.h"

#define MAXHELP 20
#define MAXNAME 15
#define CR 0X0D
#define LF 0X0A
#define NUL'\000'
#define ESC 0X1B

#define TVI_C_SCREEN {putch(ESC); putch('Z'); putch(ESC); putch('\''); }

main(argc,argv)
char **argv;
int argc;
{
struct _buf iobuf;
char hlpnm [MAXHELP] [MAXNAME] ;
char answer [MAXLINE];
char ttt [MAXLINE];
char uuu [MAXLINE];
int i,j,k,l, n;

char crlf[3];

crlf[0]=CR;
crlf[1]=LF;
crlf[2]=NUL;

fopen("HELP.EDI",&iobuf);

TVI_C_SCREEN


	fgets(ttt,&iobuf);
	while (index(ttt,"$$")==-1)
		{
   		  if (argc==1) puts(ttt);
     		  fgets(ttt,&iobuf);
		}
	fgets(ttt,&iobuf);
	i=0;
	while (index(ttt,"$$")==-1)
		{
		  transform(ttt,"$"," ");
		transform(ttt,crlf,"   ");
		  strcpy(hlpnm[i],ttt);
		  i++;
		  fgets(ttt,&iobuf);
		}
	i--;
	n = 3;
	if (argc==1)
	      {
		for(j=0;j<i;j++)
		      {
			printf("\t%-15.15s",hlpnm[j]);
			if (((j+1) %n) == 0)
				puts("\n");
		      }
		puts("\n");
		puts("\n\n\t\t\t");
		gets(ttt);
		k=strlen(ttt);
		for(l=0;l<k;l++) ttt[l]=toupper(ttt[l]);
		strcpy(answer,ttt);
	      }
	else 	{
		strcpy(answer,*++argv);
		}

	for ( j=0; j< (i); j++ )
	      { if ( index( hlpnm[j], answer) != -1)  break;
	      }
	if (j==i)
	      { puts("unrecognized");
		exit();
	      }
	strcpy(ttt,"$");
	strcat(ttt,answer);
	strcpy(answer,ttt);
	fgets(ttt,&iobuf);

	while(index(ttt,answer)==-1)
		if (-1==fgets(ttt,&iobuf)) exit();

	if (-1==fgets(ttt,&iobuf)) exit();
	while(ttt[0]!='$')
	      {
		puts(ttt);
		if (-1==fgets(ttt,&iobuf)) exit();
		if (index(ttt,"$$$")!=-1)
	      	      {
			gets(ttt);
			if (-1==fgets(ttt,&iobuf)) exit();
		      }
	      }
	exit();
}
/* replace search[] in string[] with replace[] */
transform(string, search, replace )
char *string, *search, *replace;
{
	int i,m,x;
	char q[2];

	q[1] = '\0';
	x=0;

	i = strlen(search);	m = strlen(replace);
	if (i<m)	m=i;
	while (q[0] = string[x] )
	     { if(-1 != (i = index(search,q)))
			if ( i<m ) string[x] = replace[i];
		x++;
	      }
	return;
}

index(s,t)
char s[], t[];
{	int i,j,k;
	for (i=0; s[i] != '\0'; i++)
	      { for (j=i, k=0; t[k] != '\0' && s[j]==t[k]; j++,k++);
		if (t[k] == '\0')
			return(i);
	      }
	return(-1);
}

	      { for (j=i, k=0; t[k] != '\0' && s[j]==t[k]; j++,k++);
		if (t[k] == '\0')
			return(i);
	      }
	re