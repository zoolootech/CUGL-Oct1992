/*

This utility copies a record from a named file to the printer. Records
may be selected by random within the file. References outside the bounds
of the file result in the read buffer contents remaining unchanged and
thus printing identically again.

The print format is:
	top line ASCII
	second line high order hex nibble
	third line   low order hex nibble

The record is printed in two segments to accomodate 8 inch paper. 
arguments are suppled during execution.
*/
#include "bdscio.h"
#define input 0

main()

{
char buffer[324];
char fname[20];
int fd;

for (;;){
	printf("FILE DESCRIPTOR--");
	if (scanf("%s\n",fname)==0) break;
	fd=open(fname,input);
	do_sector(fd,buffer);
	close(fd);
	}
}

do_sector(fdes,buf)

int fdes;
char buf[];
{
int i;
int sect;
char instr[135];

for(;;){
	printf("RECORD--");
	if (scanf("%d\n",&sect)==0) break;
	seek(fdes,sect,0);
	read(fdes,buf,1);
	trxln(buf,sect);
	fprintf(2,"\n");
	}
}
/*
			trxln

This routine translates the line in buf, printing it as ascii 
when reasonable and always printing a hex version. The line is
printed in two segments, one containing a record number and
50 elements, the second containing the balance.
*/

trxln(buf,sect)

char *buf;
{
int loval;
int hival;
int i,j;

for(i=0;i<3;i++){
	for(j=0;j<128;j++){
		if (!(i+j)) fprintf(2,"RECORD %d\n",sect);
		switch(i){
			case 0:
				if((buf[j]<127)&&(buf[j]>31))
				   putc(buf[j],2);
				else putc('.',2);
				break;
			case 2:
				loval=buf[j];
				loval=loval & 0x0f;
				loval=asciihex(loval);
				putc(loval,2);
				break;
			case 1:
				hival=buf[j];
				hival=hival & 0xf0;
				hival=hival>>4;
				hival=asciihex(hival);
				putc(hival,2);
				break;
			}
		if(j==60){
			fprintf(2,"\n");
			if(i<2){i++;j=-1;}
			else i=0;
			}
		if(j==127){
			fprintf(2,"\n");
			if (i<2){
				j=60;
				i++;
				}
			}
		}
	}
}
/*
			asciihex

This function accepts a value between 0 and 15 and returns
the ascii equivalent
*/

char asciihex(inval)

int inval;
{
if(inval<10) inval+=0x30;
else inval+=0x37;
return inval;
}
