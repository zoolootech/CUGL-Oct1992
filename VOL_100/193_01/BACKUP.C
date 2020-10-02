/*-----------------------------------------------------------------*/
/*  PROGRAMME  ==>  backup.c                                       */
/*  Will back up any files which don't have the archive bit        */
/*  ( = t3) already set.                                           */
/*  VERSION:  1.3       Written:  13th July, 1986                  */
/*  -------             -------   Phil. E. Cogar                   */
/*                                                                 */
/*  Note: It uses an 8K buffer to do the file transfers            */
/*  ----                                                           */
/*  Usage is -                                                     */
/*                 backup [d1] [d2]                                */
/*                                                                 */
/*  where, "d1" and "d2" are drive names.                          */
/*                                                                 */
/*  This programme uses the CP/M functions -                       */
/*                                                                 */
/*                 15   open file                                  */
/*                 16   close file                                 */
/*                 17   search first                               */
/*                 18   search next                                */
/*                 19   erase file                                 */
/*                 26   set DMA address                            */
/*                 30   set file attributes                        */
/*  which will be found in the file DEFF3 and which will be added  */
/*  automatically to the "COM" file when you use CLINK.            */
/*                                                                 */
/*  to do the required work.                                       */
/*  Written by:         Phil Cogar - July 12, 1986                 */
/*  ----------                                                     */
/*  Copyright 1986 - Cogar Computer Services Pty.Ltd.              */
/*-----------------------------------------------------------------*/
#include <bdscio.h>	/* NOTE: This programme is specific to     */
			/* ----  BDS 'C'.   If you use a different */
			/*       C then you must change this line. */
#include <pec.h>	/* Required for this file                  */
/*-----------------------------------------------------------------*/
#define SECTORS 64	/* To be used in 8K buffer, transfer_buf   */
#define VERSION "1.3"	/* Current version number                  */
char	transfer_buf[SECTORS*128]; /* which is for the copy part   */
				   /* of the programme.            */
/*-----------------------------------------------------------------*/

main(argc,argv)
short argc;
char *argv[];
{
/*  The defined values used in the programme                       */
/*-----------------------------------------------------------------*/
	char fcb[36];	/* Use for the ambiguous file mask         */
	short drive1, drive2, d1, d2;	/* The nominated drives.   */
	char name_buf[3200];	/* Use to hold the file names      */
			        /* Note present limit of 100       */
	char fcb1[36];	/* The file control blocks                 */
	char fcb2[36];	/* used in the file_copy routine           */
	char dma[128];	/* The DMA buffer for CP/M to use  */
	char mode[4];	/* The file mode                           */
		mode[0] = 'A';  /* I know it can be done more      */
		mode[1] = 'R';  /* elegantly, but this makes it    */
		mode[2] = 'C';	/* clear that mode = "ARC"         */
		mode[3] = '\0';	/* with NULL terminator.           */
	short count, file_count, i, j, k, n; /* counters             */
	short value;		/* General purpose function return */
	short offset;	/* Used in recovering the names from the   */
			/* name_buf, where we store them.          */
	short pointer;	/* Used in transferring data to-and-from   */
			/* the transfer buffer.                    */
/*-----------------------------------------------------------------*/
/*  The programme proper starts here.                              */
/*  First construct the ambiguous search mask  for use in the      */
/*  CP/M "search first" and "search next" routines                 */
/*-----------------------------------------------------------------*/

	fcb[0] = '\0';	/* Note: Drive not allocated yet           */
	for(i = 1; i < 12; i++)
		fcb[i] = '?';
	for(i = 12; i < 36; i++)
		fcb[i] = '\0';
		
	
	offset = file_count = 0;	/* starting values         */

	pec_clear();	/* clear the screen to start */
	sign_on();	/* and put sign-on message                 */

/*-----------------------------------------------------------------*/
/*  Now set the DMA buffer for transfer of information to and from */
/*  the nominated drives.   This buffer will be used throughout    */
/*  the programme whenever a file is accessed.                     */
/*-----------------------------------------------------------------*/

	set_dma(dma);
	
/*-----------------------------------------------------------------*/
/*  The programme proper starts here.   First see whether the two  */
/*  drive names have been  entered.                                */
/*-----------------------------------------------------------------*/

	if(argc < 3)	/* check to see if both drive names given  */
	{
		drive1 = mess1(drive1); /* and if not then we have */
		drive2 = mess2(drive2); /* to ask for them         */
	}
	else if(argc == 3)
	{
		drive1 = toupper(*argv[1]);
		drive2 = toupper(*argv[2]);
	}
	else mess3(); /* Otherwise you've screwed up somehow.      */

/*-----------------------------------------------------------------*/
/*  Now check to see whether valid drive names were entered.       */
/*  This programme only recognises drives A, B, C and D.   If you  */
/*  have other drives change "check_drive" to suit.                */
/*-----------------------------------------------------------------*/

	check_drive(drive1);
	check_drive(drive2);

/*-----------------------------------------------------------------*/
/*  For the programme to work the drive names have to be different */
/*  so now check that they are, else quit.                         */
/*-----------------------------------------------------------------*/

	if(drive1 == drive2)
		mess4();

/*-----------------------------------------------------------------*/
/*  Now convert the drive names to CP/M format and use the search  */
/*  first and search next functions to collect those names where   */
/*  the t3 character doesn't have bit 7 set.   Save these names    */
/*  into the name buffer                                           */
/*-----------------------------------------------------------------*/

	d1 = drive1 - 'A' + 1;	/* Convert drives to CP/M format   */
	d2 = drive2 - 'A' + 1;

	fcb[0] = d1;	/*  Put FROM drive into search mask then   */
			/*  call the search first routine          */

	if((i = search_first(fcb)) != 255)
	{
		count = save_name(i, dma, name_buf, offset);
		file_count = file_count + count;
		if(count)
			offset = offset + 32;
	}
	else mess5();


	while((i = search_next()) != 255) /* Now do search next    */
	{
		count = save_name(i, dma, name_buf, offset);
		file_count = file_count + count;
		if(count)
			offset = offset + 32;
	}

/*-----------------------------------------------------------------*/
/*  Now check the name buffer and see if any files were found to   */
/*  copy.   If not then quit.                                      */
/*-----------------------------------------------------------------*/

	if(file_count == 0)
		mess6();

/*-----------------------------------------------------------------*/
/*  If we get to here then there are some files to be archived so  */
/*  we can now start to copy these across to the back-up drive.    */
/*-----------------------------------------------------------------*/

	printf("\n\nStart of copying\n");
	printf("----------------\n\n");

	offset = 0;	/*  SŽ at beginning of name buffer      */
	for(n = 0; n < file_count; n++)
	{
/*-----------------------------------------------------------------*/
/*  Now put the name into the two file control blocks so that it   */
/*  can be used in the READ/WRITE functions which follow.          */
/*-----------------------------------------------------------------*/
	fcb_zero(fcb1);	/* clear the file control blocks           */
	fcb_zero(fcb2);
	
	for(k = 0; k < 12; k++)
		fcb1[k] = name_buf[offset + k];
	fcŒÆ RAHAM -2 LINCOLN -3

then the decryption runstring would be 

          cypher file.new file.doc -3 LINCOLN -2 ABRAHAM


FV.C - LISTING #5

As I mentioned earlier,  this is my replacement for the CP/M dump 
utility.  It  allows  the user to pass one or two  files  in  the 
runstring  for  display.  If  one  file name  is  passed  in  the 
runstring,  the output appears much like the CP/M dump.com output 
with  the  addition of the ascii display.  If two file names  are 
passed,  the output consists of a line from file one, a line from 
file  two and a third line containing the exclusive oring of  the 
two files (labeled "dif"). In all cases, non printable characters 
are  replaced with a carrot ( ^ ) in the ascii portion and  nulls 
are  replaced  with an  equal sign ( =  ),  to  readily  identify 
comparisons between two files. The comparative output is purely a 
byte  for  byte operation and no attempt is made to  realign  the 
file  to comparing characters as in a compare utility.  The first 
file length controls display length.  Figures 1 shows an  example 
of  screen output from the runstring <fv cypher1.c>  and Figure 2 
from the runstring <fv cypher1.c cypher2.c>


FSTAT.C - LISTING #6

Descriptive  statistics is the name of the game here and as  with 
any statistical evaluation, one must be brutally honest (at least 
with oneself) to draw an objective conclusion. The entire file is 
read,  16Kbytes at a time. As we read, the occurrences of each of 
the 256 tokens is accumulated and we obtain the sum of all  bytes 
as well as the min and max token occurrences.  The sum is divided 
by  the total characters to obtain the mean,  the max becomes the 
mode  and the range is the difference between min and  max.  Next 
the  256  byte array is copied to a second array  and  sorted  to 
obtain the median.

With   all  calculations  completed,   the  numerical  values  of 
occurrences  are displayed in a 16 X 16 display  for  evaluation. 
The  statistical  characteristics are displayed and  the  program 
pauses  to  await some keyboard entry to display  the  histogram. 
Depressing  the space bar prints a scaled horizontal histogram of 
16 groups (0-15, 16-31, . . ., 241-256).

The ideal random file (which is what we'd like to see) would have 
the following characteristics:

     mean      127.5
     mode      not critical
     range     < 20% of the total bytes divided by 256
     median    at midpoint of range

     histogram reasonably flat

Remember I said ideal !  This is where judgement and self honesty 
come  into  play.  A  sequential file will  display  these  ideal 
characteristics as well as a true random file.  Also,  files that 
look  too  good statistically should be just as suspect as  those 
that don't. Figure 3 is the output produced by this article as is 
and Figure 4 when its encrypted with the runstring

    cypher crypt-tb.art new frederick -1 angelo -2 scacchitti -3

In   all  fairness, I  must  state  that  other  possibly  better 
statistical  methods  are better for  determining  randomness.  I 
opted  to use descriptive statistic because they are more  easily 
understood and implemented.


MAKEF.C - LISTING #7

A simple enough utility but an absolutely necessity if we are  to 
evaluate  encryption schemes.  A filename of n 256 byte blocks is 
created and if a value between 0 and 255 is passed the file  will 
be  filled  with  this value.  If no value (or one that  is  non-
numeric) is passed, each block contains a sequential count from 0 
to 255.  An added benefit to this program is its ability to clean 
a disk.  The user simply creates a file the size of the remaining 
disk  space  and then erases it.  This results in all  free  disk 
space being set as the user defined.


SP.C - LISTING #8

Along  with the fstat utility this one confirms or  denys  (maybe 
even  questions) the statistical data we received.  A brute force 
search  method is used to find matching character strings in  the 
file.  By default,  the search starts at the first character  and 
searches the first 128 bytes for a match of 4 or more characters. 
If  the  match depth exceeds the block size it is  searching  the 
program  will return to CP/M after displaying the match data.  If 
not it will continue in its search. Block size to search, minimum 
depth  of  comparison  and  starting  point  in  the  buffer  may 
optionally be changed in the runstring. 

Also  if  an  additional argument is passed ( one more  than  the 
three mentioned) the software converts the data in the buffer  to 
delta-form.  That is,  element[n] = element[n] - element[n+1] for 
all data in the buffer.  After the conversion is made the  search 
scheme  continues as before.  This feature allows us to  evaluate 
the file for some mathematical sequence.

One drawback to this program as it stands is the limiting  factor 
of the buffer size.  No attempt is made to search beyond it. This 
shouldn't matter for most text files.


SMALL C, CP/M, AND MISCELLANEOUS

chkkbd() is a function which enables us to stop display (program) 
activity  if Control-S is depressed.  Following with a  Control-C 
causes  a  return to CP/M and any other character will allow  the 
program to continue. This function calls a bdos() function so the 
user  may have to modify this for other  operating  systems.  The 
getchx()  function  is a non-echoing version of  getchar()  which 
uses BDOS function 6. getchar() may be substituted for getchx().

calloc(), malloc() and cfree() functions are used for the dynamic 
allocation and deallocation of memory. My allocation/deallocation 
scheme  is  of the simple variety where the programmer  must  pay 
heed to order or pay the consequences.  The source code contained 
here should work with most implementations of these functions.

Printer  output may be obtained from any of the programs by using 
the  CP/M  Control  P function.  It was the  simplest  method  to 
implement.

Math  functions  (especially  floating point) are  difficult  for 
Small-C.  There  are several routines in the fstat.c source  that 
perform the necessary long and fractional calculations.  It's not 
necessary  to  change these however,  if your  compiler  supports 
floats and longs, have at it.

Each program will display the usage if entered without the proper 
number of arguments in the run string.  Also, since most software 
users  begin  to feel uncomfortable when their  computer  is  off 
somewhere  performing exotic calculations,  each program displays 
status to the screen thus putting these fears at rest.  


CYPHER BENCHMARKS

My version, written in Small C, is generic enough to adapt to any 
C  compiler.  Running   on  a  4 Mhz Z80 based  CP/M  system,  it 
benchmarks  at less than 1K/sec for file I/O,  16K/sec  for  file 
transposition  and  approximately 4K/sec per key for  encryption. 
Key  encryption is difficult to benchmark since it  includes  the 
time  to generate the prime length key which varies from 1009  to 
1999 characters in length.


AND FINALLY

These tools should be employed with a measure of common sense.  A 
strong  cypher  is  only indicated when  both  statistically  and 
pattern-whise  indicated.  (And it doesn't hurt to view the  file 
either) My intent in developing these utilities was to enable the 
cryptographer-programmer  a means to evaluate the strength of  an  
encryption  scheme  as  well as the  resistance  of   schemes  to 
"cracking".  However  (like  most  tools) these can be  used  for UP - Version  %s\n", VERSION);
	printf("Copies all files which haven't been previously copied.\n");
	printf("-----------------------------------------------------\n");
	printf("Copyright 1986 - Cogar Computer Services Pty. Ltd.\n");
	printf("All rights reserved.\n\n");
}
/*------------FINISH OF BACKUP-------------------------------------*/("\nThere are no files on this disk.