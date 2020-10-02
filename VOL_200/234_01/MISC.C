/*
  HEADER: CUG	 nnn.nn;
  TITLE:	 XDIR - Hard Disk Manager
  VERSION:	 1.0 for IBM-PC
  DATE: 	 Apr 03, 1987
  DESCRIPTION:	 Hard Disk Manager for IBM PC
  KEYWORDS:	 Hard Disk Manager Dump Directory
  SYSTEM:	 IBM-PC and Compatiables
  FILENAME:      misc.c
  WARNINGS:	 None
  CRC:		 N/A
  SEE-ALSO:	 HDIR.DOC and XDIR.DOC
  AUTHOR:	 Mike Blakley 15645 SW 82 Cir Ln #76, Miami, Fl 33193
  COMPILERS:	 ECO-C
  REFERENCES:	 XDIR.DOC
*/
#include <stdio.h>
/*
  clear

*/
void clear(dest,len,val)
char *dest;
int  len;
int  val;
{
     char *cp;
     cp = dest;
     while (len--) *cp++ = (char) val;

}


blockmv(dest,src,len)
char *dest, *src;
int  len;
{
     while (len--) *dest++ = *src++;
}

/*
writestr.c
*/
void writestr(buffer)
char *buffer;
{
     static int i;
     while (i=(int)*buffer++) putchar(i);
}
