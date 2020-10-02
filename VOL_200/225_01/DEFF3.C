/*-----------------------------------------------------------------*/
/*  FILE:          DEFF3.C
    ----
    COMPILER:      BDS C  V 1.50
    --------
    WRITTEN:       12th October, 1985
    -------
    REVISED:       10th November, 1986
    -------
    VERSION:       1.4
    -------
    ADDED FCTs:    SCOPY_N, GET_ALV
    ----------                                                     */

/*  This file contains the following functions -

         BINARY         GET_DEFAULT         RESET_DSK
         BITCOUNT       GET_IOBYTE          REVERSE
         CGET           INDEX               SEARCH_FIRST
         CLOSE_FILE     ISALNUM             SEARCH_NEXT
         CLRSCREEN      ITOA                SELECT_DSK
         CPUT           LISTC               SET_ATTRIBUTES
         CON_STAT       MAKE_RO             SETDMA
         CREATE_FILE    OPEN_FILE           SET_IOBYTE
         DIRECTC        PRINT_DEC           SHELL_SORT
         DPB_ADR        PRT_STR             SWAP_POINTERS
         ERASE_FILE     READ_SEQ_SEC        USER_ID
         GET_CPM        READ_STR            WRITE_SEQ_SEC

              ADDED FUNCTIONS
              ---------------

         L_SHIFT        R_SHIFT             PRINT_BIN
         BIT_SET        BIT_RESET           ISPRINT
         SCOPY_N        GET_ALV
                                                                   */
/*  all of which are described in DEFF3.TXT and are                */
/*  contained in the linking file  ==>  DEFF3.CRL                  */
/*  Copyright 1986 - Cogar Computer Services Pty. Ltd.             */
/*-----------------------------------------------------------------*/
#include "pec.h"	/* definitions required                    */
/*=================================================================*/
/*            CGET                                                 */
/*  Read Console Byte  ==>  CP/M Function No. 1                    */
/*  Note will read any BYTE value from the console                 */
/*-----------------------------------------------------------------*/

char cget()
{
	return(bdos(READ,0));
}
/*-----------------------------------------------------------------*/
/*            CPUT                                                 */
/*  Write Console Byte  ==> CP/M Function No. 2                    */
/*  Note will send any BYTE value to the console                   */
/*-----------------------------------------------------------------*/

void cput(c)
int c;
{
	bdos(WRITE,c);
}
/*-----------------------------------------------------------------*/
/*            CLRSCREEN                                            */
/*  Clear screen function for Hazeltine Esprit terminal            */
/*-----------------------------------------------------------------*/

void clrscreen()
{
	cput(126);
	cput(28);
}
/*-----------------------------------------------------------------*/
/*            LISTC                                                */
/*  List byte to line printer                                      */
/*-----------------------------------------------------------------*/

void listc(c)
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


char directc(DUTY)
int DUTY;
{
	return(bdos(DCIO,DUTY));
}
/*-----------------------------------------------------------------*/
/*            GET_IOBYTE                                           */
/*  Get IOBYTE setting                                             */
/*  Returns the current IOBYTE setting                             */
/*-----------------------------------------------------------------*/

char get_iobyte()
{
	return(bdos(IOBYTE,0));
}
/*-----------------------------------------------------------------*/
/*            SET_IOBYTE                                           */
/*  Sets the IOBYTE                                                */
/*  Doesn't return anything                                        */
/*-----------------------------------------------------------------*/

void set_iobyte(IOB)
short IOB;	/* new IOBYTE setting */
{
	bdos(SET_IOBYTE,IOB);
}
/*-----------------------------------------------------------------*/
/*            PRT_STR                                              */
/*  Print a "$" terminated string to the console                   */
/*  Doesn't return anything                                        */
/*-----------------------------------------------------------------*/

void prt_str(BUFF)
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

void read_str(BUFF)
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

char con_stat()
{
	return(bdos(CON_STATUS,0));
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

int get_cpm()
{
	return(bdos(CPM_NUM,0));
}
/*-----------------------------------------------------------------*/
/*            RESET_DSK                                            */
/*  Reset the disk system                                          */
/*  Typically used after a disk is changed                         */
/*  Does not return anything                                       */
/*-----------------------------------------------------------------*/

void reset_dsk()
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

void select_dsk(DISK)
char DISK;
{
	char DRIVE;
	toupper(DISK);	/* make this upper case */
	DRIVE = DISK - 65; /* convert to A = 0, B = 1, etc */

	bdos(LOGICAL,DRIVE);
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

char open_file(FCB)
char FCB[36];
{
	return(bdos(OPEN_FILE,FCB));
}
/*-----------------------------------------------------------------*/
/*            CLOSE_FILE                                           */
/*  Close a file  ==>  CP/M Function No. 16                        */
/*  Will close the file named in the FCB                           */
/*  Same exit parameters as for "open_file"                        */
/*-----------------------------------------------------------------*/

char close_file(FCB)
char FCB[36];
{
	return(bdos(CLOSE_FILE,FCB));
}
/*-----------------------------------------------------------------*/
/*            SEARCH_FIRST                                         */
/*  Search for first match of file name  ==>  CP/M Function No. 17 */
/*  Returns 255 if file name not found in directory                */
/*  Else returns pointer to location of file name.                 */
/*-----------------------------------------------------------------*/

char search_first(FCB)
char FCB[36];
{
	return(bdos(SEARCH_FIRST,FCB));
}
/*-----------------------------------------------------------------*/
/*            SEARCH_NEXT                                          */
/*  Search for next match  ==>  CP/M Function No. 18               */
/*  Can ONLY be used after a successful "search_first".            */
/*  Same return paramaters as for search_first.                    */
/*-----------------------------------------------------------------*/

char search_next()
{
	return(bdos(SEARCH_NEXT,0));
}
/*-----------------------------------------------------------------*/
/*            ERASE_FILE                                           */
/*  Erase (delete) the named file from directory                   */
/*  Returns 255 if file not found.                                 */
/*-----------------------------------------------------------------*/

char erase_file(FCB)
char FCB[36];
{
	return(bdos(ERASE,FCB));
}
/*-----------------------------------------------------------------*/
/*            READ_SEQ_SEC                                         */
/*  Reads a sector sequentially from opened file                   */
/*  Returns 00h if successful else returns non-zero number         */
/*-----------------------------------------------------------------*/

char read_seq_sec(FCB)
char FCB[36];
{
	return(bdos(READ_SECT,FCB));
}
/*-----------------------------------------------------------------*/
/*            WRITE_SEQ_SEC                                        */
/*  Writes a sector sequentially to the opened file                */
/*  Returns 00h if successful else returns non-zero number         */
/*-----------------------------------------------------------------*/

char write_seq_sec(FCB)
char FCB[36];
{
 	return(bdos(WRITE_SECT,FCB));
}
/*-----------------------------------------------------------------*/
/*            CREATE_FILE                                          */
/*  Creates the file named in the FCB  ==> CP/M Function No. 22    */
/*  Returns 255 if directory is full                               */
/*-----------------------------------------------------------------*/

char create_file(FCB)
char FCB[36];
{
	return(bdos(CREATE,FCB));
}
/*-----------------------------------------------------------------*/
/*            GET_DEFAULT                                          */
/*  Get the current default disk No.  ==>  CP/M Function No. 25    */
/*  Returns the code, A = 0, B = 1,...and so on.                   */
/*-----------------------------------------------------------------*/

char get_default()
{
	return(bdos(DEFAULT,0));
}
/*-----------------------------------------------------------------*/
/*            SETDMA                                               */
/*  Sets the DMA (read/write buffer) address                       */
/*  Doesn't return anything                                        */
/*-----------------------------------------------------------------*/

void setdma(DMA_BUFF)
char DMA_BUFF[128];
{
	bdos(26, &DMA_BUFF[0]);
}
/*-----------------------------------------------------------------*/
/*            GET_ALV
    Gets the address (pointer to) the allocation vector for the
    allocation blocks.   Function 27.                              */
/*-----------------------------------------------------------------*/
unsigned get_alv()
{
	return(bdos(ALLOCATION, 0));
}
/*-----------------------------------------------------------------*/
/*            MAKE_RO                                              */
/*  Makes the logical disk selected by "select_dsk" R/O status     */
/*  Doesn't return anything?                                       */
/*-----------------------------------------------------------------*/

int make_ro()
{
	bdos(READ_ONLY,0);
}
/*-----------------------------------------------------------------*/
/*            DPB_ADR                                              */
/*  This is Function 31 - get Disk Parameter Block Address.   It   */
/*  returns the address of the 15 byte block where the disk format */
/*  information is stored by the BIOS.                             */
/*-----------------------------------------------------------------*/

unsigned dpb_adr()
{
	return(bdos(DPB, 0));
}
/*-----------------------------------------------------------------*/
/*            SET_ATTRIBUTES                                       */
/*  Sets the attributes in the nominated FCB.   Note these must    */
/*  have the high bit set prior to calling this function.          */
/*  Returns 255 if file not found.                                 */
/*-----------------------------------------------------------------*/

char set_attributes(FCB)
char FCB[36];
{
	return(bdos(SET_ATTRIBUTES,FCB));
}
/*-----------------------------------------------------------------*/
/*            USER_ID                                              */
/*  Will set the User No., or get the User No., as instructed      */
/*  Code = 255 to get User No.                                     */
/*       = 0 to 15 to set User No.                                 */
/*  Returns current user number if Code = 255                      */
/*-----------------------------------------------------------------*/

char user_id(CODE)
char CODE;
{
	return(bdos(USER_ID,CODE));
}
/*-----------------------------------------------------------------*/
/*  Functions from K & R  ==>  in BDS C                            */
/*-----------------------------------------------------------------*/
/*            BITCOUNT                                             */
/*  Counts the number of "1" bits in a byte (page 47)              */
/*-----------------------------------------------------------------*/

short bitcount(n)
unsigned n;
{
	short b;

	for(b = 0; n != 0; n >> 1)
		if(n & 01)
			b++;
	return(b);
}
/*-----------------------------------------------------------------*/
/*            BINARY                                               */
/*  Finds whether a binary number occurs in a sorted array (p 54)  */
/*-----------------------------------------------------------------*/

short binary(x, v, n)
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

void shell_sort(v, n)
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

void reverse(s)
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

void itoa(n,s)
char s[];
int n;
{
	int i, sign;

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

int index(s, t)
char s[], t[];
{
	int i, j, k;

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

void print_dec(n)
int n;
{
	char s[10];
	int i;

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

void swap_pointers(px, py)
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

char isalnum(c)
char c;
{
	if(isalpha(c) || ( c >= '0' && c <= '9'))
		return(1);
	else return(0);
}
/*-----------------------------------------------------------------*/
/*            L_SHIFT                                             */
/*       Multiply a value by 2 raised to the power given.         */
/******************************************************************/
unsigned l_shift(value, number)
unsigned value;
int number;
{
	int i, factor;
	factor = 1;	/* Starting value */

	for(i = 0; i < number; i++)
	 	factor = factor*2;
	value = value*factor;
	return(value);
}
/*-----------------------------------------------------------------*/
/*            R_SHIFT                                              */
/*       Divide a value by 2 raised to the power given.            */
/*******************************************************************/
unsigned r_shift(value, number)
unsigned value;
int number;
{
	int i, factor;
	factor = 1;	/* Starting value */

	for(i = 0; i < number; i++)
	 	factor = factor*2;
	value = value/factor;
	return(value);
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==>  PRT_BIN

    Will convert an unsigned integer to a printable ASCII string, in
    binary notation.  e.g. the number 0xff will be converted to

              11111111

    Copyright 1986 - Cogar Computer Services Pty. Ltd.             */
/*-----------------------------------------------------------------*/

void print_bin(i)
unsigned i;
{
	char str[17];	/* The string to hold the result           */
	int n;		/* Bit counter                             */

	str[16] = '\0';	/* String must be null-terminated          */
	if(i & 0x01)
		str[15] = '1';
	else str[15] = '0';	/* Check first bit                 */
	for(n = 0; n < 15; n++)
	{
		i = i >> 1;	/* Shift right by one bit          */
		if(i & 0x01)
			str[14 - n] = '1';
		else str[14 - n] = '0';
	}
	printf("%s", str);	/* Print the binary string         */
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==>  BIT_SET

    This function will set the nominated bit in an unsigned
    integer.   Note the bits are numbered from 0 to 15 = 16
    bits altogether.   It returns the unsigned integer with
    the nominated bit set.

    Usage:    bit_set(u, n);
    -----     Where u = unsigned integer and n = bit number.

    Copyright 1986 - Cogar Computer Services Pty. Ltd.             */
/*-----------------------------------------------------------------*/

unsigned bit_set(u, n)
unsigned u;
int n;
{
	unsigned modifier;
	int i;

	if(n == 0)
		modifier = 1;
	else if( n > 0)
	{
		modifier = 1;
		for(i = 1; i <= n; i++)
		{
			modifier = modifier*2;
		}
	}
	return(u | modifier);
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==>  BIT_RESET

    This function will reset the nominated bit in an unsigned
    integer.   Note the bits are numbered from 0 to 15 = 16
    bits altogether.   It returns the unsigned integer with
    the nominated bit reset to zero.

    Usage:    bit_reset(u, n);
    -----     Where u = unsigned integer and n = bit number.

    Copyright 1986 - Cogar Computer Services Pty. Ltd.             */
/*-----------------------------------------------------------------*/

unsigned bit_reset(u, n)
unsigned u;
int n;
{
	unsigned modifier;
	int i;

	if(n == 0)
		modifier = 1;
	else if( n > 0)
	{
		modifier = 1;
		for(i = 1; i <= n; i++)
		{
			modifier = modifier*2;
		}
	}
	modifier = modifier | 0xffff;
	return(u & modifier);
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==>  ISPRINT

    Will return TRUE (+1) if the character being tested is able to
    be printed on the console, else returns FALSE (0).

    Copyright 1986 - Cogar Computer Services Pty. Ltd.             */
/*-----------------------------------------------------------------*/

char isprint(c)
char c;
{
	if(c > 0x1f && c < 0x7f)
		return(1);	/* A printable character           */
	else return(0);
}
/*-----------------------------------------------------------------*/
/*  LIBRARY FILE  ==>  SCOPY_N

    This will copy string2 to string1 for "n" characters.    If "n"
    is greater than strlen(string2) then it will only copy until
    all the characters in string2 have been done.

    NOTE:     It doesn't terminate string1 with the '\0' character
    ----      when it has copied across the nominated number of
              characters from string2.   This is so you can use
              the routine to overwrite/insert a string within a
              buffer.

    Copyright 1986 - Cogar Computer Services Pty.Ltd.              */
/*-----------------------------------------------------------------*/

void scopy_n(s1, s2, n)
char *s1, *s2;
int n;
{
	int i, j;
	if(n > strlen(s2))
		i = strlen(s2);
	else i = n;

	for(j = 0; j < i; j++)
		s1[j] = s2[j];
}
/*-----------------------------------------------------------------*/
unsigned integer with
    the nominated bit reset to zero.

    Usage:    bit_reset(u, n);
    -