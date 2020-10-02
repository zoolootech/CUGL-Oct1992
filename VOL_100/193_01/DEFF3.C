/*-----------------------------------------------------------------*/
/*  LIBRARY FUNCTIONS  ==>  written by Phil Cogar                  */
/*  For BDS C under CP/M 2.2 using the "BDOS" Function             */
/*                                                                 */
/*  This file contains the following functions -                   */
/*                                                                 */
/*       CGET                CPUT           DIRECTC                */
/*       CLRSCREEN           LISTC          GET_IOBYTE             */
/*       SET_IOBYTE          PRT_STR        READ_STR               */
/*       CON_STAT            GET_CPM        RESET_DSK              */
/*       SELECT_DSK          OPEN_FILE      CLOSE_FILE             */
/*       SEARCH_FIRST        SEARCH_NEXT    CREATE_FILE            */
/*       ERASE_FILE          READ_SEQ_SEC   WRITE_SEQ_SEC          */
/*       GET_DEFAULT         SET_DMA        MAKE_RO                */
/*       SET_ATTRIBUTES      USER_ID        BITCOUNT               */
/*       BINARY              SHELL_SORT     REVERSE                */
/*       ITOA                INDEX          PRINT_DEC              */
/*       SWAP_POINTERS       ISALNUM                               */
/*                                                                 */
/*  all of which are described in DEFF3.TXT and are                */
/*  contained in the linking file  ==>  DEFF3.CRL                  */
/*  Copyright 1986 - Cogar Computer Services Pty. Ltd.             */
/*-----------------------------------------------------------------*/
#include "pec.h"	/* definitions required                    */
/*=================================================================*/
/*            CGET                                                 */
/*  Read Console Byte  ==>  CP/M Function No. 1                    */
/*  Note will read any BYTE value from the console                 */

cget()
{
	bdos(READ,0);
}
/*-----------------------------------------------------------------*/
/*            CPUT                                                 */
/*  Write Console Byte  ==> CP/M Function No. 2                    */
/*  Note will send any BYTE value to the console                   */

cput(c)
int c;
{
	bdos(WRITE,c);
}
/*-----------------------------------------------------------------*/
/*            CLRSCREEN                                            */
/*  Clear screen function for Hazeltine Esprit terminal            */

clrscreen()
{
	cput(126);
	cput(28);
}
/*-----------------------------------------------------------------*/
/*            LISTC                                                */
/*  List byte to line printer                                      */

listc(c)
int c;
{

	bdos(LIST, c);
}
/*-----------------------------------------------------------------*/
/*            DIRECTC                                              */
/*  Use direct console IN/OUT  ==>  CP/M function No. 6            */
/*  Note:     DUTY = IN = 0FFH                                     */
/*                 = OUT = character to be printed to screen       */
/*  Returns either the input character or Reg. A = 0               */
/*-----------------------------------------------------------------*/

#define	IN 255	/* Character in function */

directc(DUTY)
int DUTY;
{
	bdos(DCIO,DUTY);
}
/*-----------------------------------------------------------------*/
/*            GET_IOBYTE                                           */
/*  Get IOBYTE setting                                             */
/*  Returns the current IOBYTE setting                             */
/*-----------------------------------------------------------------*/

get_iobyte()
{
	bdos(IOBYTE,0);
}
/*-----------------------------------------------------------------*/
/*            SET_IOBYTE                                           */
/*  Sets the IOBYTE                                                */
/*  Doesn't return anything                                        */
/*-----------------------------------------------------------------*/

set_iobyte(IOB)
short IOB;	/* new IOBYTE setting */
{
	bdos(SET_IOBYTE,IOB);
}
/*-----------------------------------------------------------------*/
/*            PRT_STR                                              */
/*  Print a "$" terminated string to the console                   */
/*  Doesn't return anything                                        */
/*-----------------------------------------------------------------*/

prt_str(BUFF)
char *BUFF;	/* pointer to string buffer */
{
	bdos(PRT_STRING,BUFF);
}
/*-----------------------------------------------------------------*/
/*            READ_STR                                             */
/*  Read a "RETURN" terminated string from the console             */
/*  Note:     The CR doesn't form part of the string.   As written */
/*  ----      this function terminates the string with a null.     */
/*-----------------------------------------------------------------*/

#define BUFF_LEN	134	/* maximum number of characters */

read_str(BUFF)
short *BUFF;
{
	short n;
	for(n = 0; n <= BUFF_LEN; n++)
	{
		BUFF[n] = '\0';	/* Initialise string with NULL's */
	}
	
	BUFF[0] = BUFF_LEN;	/* must tell CP/M buffer size */

	bdos(READ_STRING,BUFF);
}
/*-----------------------------------------------------------------*/
/*            CON_STAT                                             */
/*  Read the console status  ==>  CP/M Function No. 11             */
/*  Returns NULL if no character waiting, else returns 0FFH        */
/*-----------------------------------------------------------------*/

con_stat()
{
	bdos(CON_STATUS,0);
}
/*-----------------------------------------------------------------*/
/*            GET_CPM                                              */
/*  Get CP/M Version Number                                        */
/*  Number is returned in HL with the details -                    */
/*                                                                 */
/*     H = 00H for CP/M  or H = 01H for MP/M                       */
/*     L = 00H for all releases prior to 2.0                       */
/*     L = 20H for release 2.0, 21H for release 2.1, 22H for       */
/*         release 2.2, and so on.                                 */
/*-----------------------------------------------------------------*/

get_cpm()
{
	bdos(CPM_NUM,0);
}
/*-----------------------------------------------------------------*/
/*            RESET_DSK                                            */
/*  Reset the disk system                                          */
/*  Typically used after a disk is changed                         */
/*  Does not return anything                                       */
/*-----------------------------------------------------------------*/

reset_dsk()
{
	bdos(RESET,0);
}
/*-----------------------------------------------------------------*/
/*            SELECT_DSK                                           */
/*  Select logical disk  ==>  CP/M Function No. 14                 */
/*  This makes the nominated disk the default disk                 */
/*  Note:     Drive A = 00H                                        */
/*  ----      Drive B = 01H, and so on                             */
/*-----------------------------------------------------------------*/

select_dsk(DISK)
short DISK;
{
	toupper(DISK);	/* make this upper case */
	DISK = DISK - 66; /* convert to A = 0, B = 1, etc */

	bdos(LOGICAL,DISK);
}
/*-----------------------------------------------------------------*/
/*            OPEN_FILE                                            */
/*  Open a file  ==>  CP/M Function No. 15                         */
/*  To use this you must have previously formulated a File         */
/*  Control Block (FCB) using setfcb(fcbaddr, filename)            */
/*  Returns the Directory code in Register A, or 0FFH if it failed */
/*  Note particularly the nominated FCB must be declared for use   */
/*  GLOBALLY so that it can be known by other functions.           */
/*-----------------------------------------------------------------*/

open_file(FCB)
char FCB[36];
{
	bdos(OPEN_FILE,FCB);
}
/*-----------------------------------------------------------------*/
/*            CLOSE_FILE                                           */
/*  Close a file  ==>  CP/M Function No. 16                        */
/*  Will close the file named in the FCB                           */
/*  Same exit parameters as for "open_file"                        */

close_file(FCB)
char FCB[36];
{
	bdos(CLOSE_FILE,FCB);
}
/*-----------------------------------------------------------------*/
/*            SEARCH_FIRST                                         */
/*  Search for first match of file name  ==>  CP/M Function No. 17 */
/*  Returns 255 if file name not found in directory                */
/*  Else returns pointer to location of file name.                 */
/*-----------------------------------------------------------------*/

search_first(FCB)
char FCB[36];
{
	bdos(SEARCH_FIRST,FCB);
}
/*-----------------------------------------------------------------*/
/*            SEARCH_NEXT                                          */
/*  Search for next match  ==>  CP/M Function No. 18               */
/*  Can ONLY be used after a successful "search_first".            */
/*  Same return paramaters as for search_first.                    */
/*-----------------------------------------------------------------*/

search_next()
{
	bdos(SEARCH_NEXT,0);
}
/*-----------------------------------------------------------------*/
/*            ERASE_FILE                                           */
/*  Erase (delete) the named file from directory                   */
/*  Returns 255 if file not found.                                 */
/*-----------------------------------------------------------------*/

erase_file(FCB)
char FCB[36];
{
	bdos(ERASE,FCB);
}
/*-----------------------------------------------------------------*/
/*            READ_SEQ_SEC                                         */
/*  Reads a sector sequentially from opened file                   */
/*  Returns 00h if successful else returns non-zero number         */
/*-----------------------------------------------------------------*/

read_seq_sec(FCB)
char FCB[36];
{
	bdos(READ_SECT,FCB);
}
/*-----------------------------------------------------------------*/
/*            WRITE_SEQ_SEC                                        */
/*  Writes a sector sequentially to the opened file                */
/*  Returns 00h if successful else returns non-zero number         */
/*-----------------------------------------------------------------*/

write_seq_sec(FCB)
char FCB[36];
{
 	bdos(WRITE_SECT,FCB);
}
/*-----------------------------------------------------------------*/
/*            CREATE_FILE                                          */
/*  Creates the file named in the FCB  ==> CP/M Function No. 22    */
/*  Returns 255 if directory is full                               */
/*-----------------------------------------------------------------*/

create_file(FCB)
char FCB[36];
{
	bdos(CREATE,FCB);
}
/*-----------------------------------------------------------------*/
/*            GET_DEFAULT                                          */
/*  Get the current default disk No.  ==>  CP/M Function No. 25    */
/*  Returns the code, A = 0, B = 1,...and so on.                   */
/*-----------------------------------------------------------------*/

get_default()
{
	bdos(DEFAULT,0);
}
/*-----------------------------------------------------------------*/
/*            SET_DMA                                              */
/*  Sets the DMA (read/write buffer) address                       */
/*  Doesn't return anything                                        */
/*-----------------------------------------------------------------*/

set_dma(DMA_BUFF)
short DMA_BUFF[128];
{
	bdos(SETDMA,DMA_BUFF);
}
/*-----------------------------------------------------------------*/
/*            MAKE_RO                                             */
/*  Makes the logical disk selected by "select_dsk" R/O status     */
/*  Doesn't return anything?                                       */
/*-----------------------------------------------------------------*/

make_ro()
{
	bdos(READ_ONLY,0);
}
/*-----------------------------------------------------------------*/
/*            SET_ATTRIBUTES                                       */
/*  Sets the attributes in the nominated FCB.   Note these must    */
/*  have the high bit set prior to calling this function.          */
/*  Returns 255 if file not found.                                 */
/*-----------------------------------------------------------------*/

set_attributes(FCB)
char FCB[36];
{
	bdos(SET_ATTRIBUTES,FCB);
}
/*-----------------------------------------------------------------*/
/*            USER_ID                                              */
/*  Will set the User No., or get the User No., as instructed      */
/*  Code = 255 to get User No.                                     */
/*       = 0 to 15 to set User No.                                 */
/*  Returns current user number if Code = 255                      */
/*-----------------------------------------------------------------*/

user_id(CODE)
short CODE;
{
	bdos(USER_ID,CODE);
}
/*-----------------------------------------------------------------*/
/*  Functions from K & R  ==>  in BDS C                            */
/*-----------------------------------------------------------------*/
/*            BITCOUNT                                             */
/*  Counts the number of "1" bits in a byte (page 47)              */
/*-----------------------------------------------------------------*/

bitcount(n)
unsigned n;
{
	short b;

	for(b = 0; n != 0; n >>= 1)
		if(n & 01)
			b++;
	return(b);
}
/*-----------------------------------------------------------------*/
/*            BINARY                                               */
/*  Finds whether a binary number occurs in a sorted array (p 54)  */
/*-----------------------------------------------------------------*/

binary(x, v, n)
short x, v[], n;
{
	short low, high, mid;

	low = 0;
	high = n - 1;
	while(low <= high)
	{
		mid = (low + high)/2;
		if(x < v[mid])
			high = mid - 1;
		else if (x > v[mid])
			low = mid + 1;
		else return(mid);	/* found match */
	}
	return(-1);
}
/*-----------------------------------------------------------------*/
/*            SHELL_SORT                                           */
/*  Sort v[0].....v[n-1] into increasing order  (page 58)          */
/*-----------------------------------------------------------------*/

shell_sort(v, n)
short v[], n;
{
	short gap, i, j, temp;

	for(gap = n/2; gap > 0; gap = gap/2)
		for(i = gap; i < n; i++)
			for(j = i-gap; j >= 0 && v[j] > v[j+gap];j -= gap)
			{
				temp = v[j];
				v[j] = v[j+gap];
				v[j+gap] = temp;
			}
}
/*-----------------------------------------------------------------*/
/*            REVERSE                                              */
/*  Reverse a NULL-terminated string in place  (page 59)           */
/*-----------------------------------------------------------------*/

reverse(s)
char s[];
{
	int c, i, j;

	for(i = 0, j = strlen(s) - 1; i < j; i++, j--)
	{
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
/*-----------------------------------------------------------------*/
/*            ITOA                                                 */
/*  Convert integer into a printable ASCII string  (page 60)       */
/*-----------------------------------------------------------------*/

itoa(n,s)
char s[];
short n;
{
	short i, sign;

	if(( sign = n) < 0)
		n = -n;		/* make n positive */
	i = 0;

	do {
		s[i++] = n % 10 + '0';	/* put digits in reverse */
	   }
	while((n /= 10) > 0);
		if(sign < 0)
			s[i++] = '-';
	s[i] = '\0';	/* NULL terminator */
	reverse(s);	/* put into correct order */
}
/*-----------------------------------------------------------------*/
/*            INDEX                                                */
/*  Find index of string t in string s, return -1 if none (p 67)   */
/*-----------------------------------------------------------------*/

index(s, t)
char s[], t[];
{
	short i, j, k;

	for( i = 0; s[i] != '\0'; i++)
	{
		for(j = i, k = 0; t[k] != '\0' && s[j] == t[k];j++,k++)
		;
	if(t[k] == '\0')
		return(i);
	}
	return(-1);
}
/*-----------------------------------------------------------------*/
/*            PRINT_DEC                                            */
/*  Print a decimal number to the console  (page 85)               */
/*-----------------------------------------------------------------*/

print_dec(n)
short n;
{
	char s[10];
	short i;

	if( n < 0)
	{
		putchar('-');
		n = -n;		/* make number positive */
	}
	i = 0;
	if((i = n/10) != 0)
		print_dec(i);
	putchar(n % 10 + '0');
}
/*-----------------------------------------------------------------*/
/*            SWAP_POINTERS                                        */
/*  Interchange two pointers  (page 117)                           */
/*-----------------------------------------------------------------*/

swap_pointers(px, py)
char *px[], *py[];
{
	char *temp;

	temp = *px;
	*px = *py;
	*py = temp;
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==>  isalnum                                     */
/*  Tests whether a character is alpha-numeric                     */
/*  Returns   TRUE/FALSE                                           */
/*-----------------------------------------------------------------*/

isalnum(c)
char c;
{
	if(isalpha(c) || ( c >= '0' && c <= '9'))
		return(1);
	else return(0);
}
/*-----------------------------------------------------------------*/
hile((n /= 10) > 0);
		if(sign < 0)
			s[i++] = '-';
	s[i] = '\0';	