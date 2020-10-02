/*
 * For BDS C, a quicker version of prime number prog in Sep 81 BYTE p. 186
 * Timings (Bell to Bell)
 *	ZPU (4mHz)	13.7 Seconds
 *	Z89 (2mHz)	28.0 Seconds
 */
#include "A:BDSCIO.H"
#define TRUE 1
#define FALSE 0
#define SIZE 8190
#define SIZEP1 8191

char flags[SIZEP1], *p,*s;
int i,prime,count,iter;

main()
{
	printf("10 Iterations\007\n");
	for(iter=10; --iter>=0;) {
		count=0;
		setmem(flags, SIZE, TRUE);

		for(p=flags+(i= -1); ++i<=SIZE;) {
			if(*++p) {
				prime= i+i+3;
				for(s=p; (s+=prime)<=(flags+SIZE);)
					*s = FALSE;
				++count;
				/*printf("%d\n",prime);*/
			}
		}
	}
	printf("\n%d primes\007", count);
}

/* submit file for above
xsub
cc1 b:bytebnch.c -e 1000 -o
clink b:bytebnch -d
50.9 seconds RETURN to first bell with Z89 and dd 8" drives (Magnolia)

Observations:
	C compilers are notoriously inefficient at optimizing subscript
calculations.  In many cases, such as the above, array indexing can be
replaced by pointer arithmetic, a more idiomatic usage of C.
	A few of the "tricks" I used for BDS C:
	0.  All array refrences were recoded as pointer arithmetic.  Pointers
are one of C's great strength, so use them when performance is critical.
	1.  All variables were made external. This places them in fixed
memory locations which can be accesses by direct addressing on the 8080.
	2.  The array is cleared by "setmem", a function provided in the
BDS C library, which corresponds to "aryset" in ZSPL.  This would be faster
than a loop coded in BDS C.
	3.  Combining operations may produce better code.  On machines such as
the 8080 or even 8086, arithmetic operations are carried out in an accumulator,
so try to avoid unnecessary loads and stores.
	4.  When the value is needed, preincrement and predecrement wins
over postincrement.  When postincrement is used, a copy of the old value must
be saved, then the increment takes place, and finally the saved copy of the
old value is used.  Much quicker to modify the value, then use it.
	5.  Note that some of the choices would not be optimal for a machine
such as the PDP-11 or 68000, whose hardware supports address autoincrement.
However, the program will still be correct!
8-4-81
Chuck Forsberg
Computer Development Inc
Beaverton OR
RCP/M 503/621-3193 (300,450,1200baud: 24hrs intermittent)

*/es by direct addressing on the 8080.
	2.  The array is cleared by "setmem", tion 1A to 4F, 1B to 20, and 1C to 11.
e0a 00w		set buffer location 0a to 0 and write buffer
			Note no space after last data                     YAM (Yet Another Modem program)

                           by Chuck Forsberg

                         Manual Revised 8-29-81

 YAM  performs  a  number  of  "Super-Terminal"  functions.  It may be
 regarded  as  a  (not  proper)  superset  of  MODEM7.   Commands  and
 subcommands are listed below.

 XYAM is a version of yam which corresponds to XMODEM.



 a:               Change to a: disk (or b, etc.)

 a1:              Change to a: disk user 1

 bm               Set baudrate to m.  Example: "b19200"

 bye              Drop any call in progress and prepare to make/answer
                  another.

 call name        Enable Data  Terminal  Ready (DTR), and set baudrate
                  to the value (if present) corresponding to name.  If
                  autodial is supported,  dial  the telephone number.
                  If autodial is not  supported,  the telephone number
                  and  preferred baudrate are displayed.   It  is  not
                  necessary to type the entire  name  as it appears in
                  the file.  Example: "call tcbbs" or "call tc"

 close            Dump cpature buffer if a recrive file is open, close
                  files.

 dir [pattern ...] Display  pathnames  matching  pattern  alphabetized
                  across the page.  The usual  ambiguous filenames are
                  allowed under CP/M  provided  a  correct  version of
                  setfcb() is used.   Since  dir or any other commands
                  which  accept  a pattern use the circular buffer for
                  filename  expansion,  be  sure  to  write  out   any
                  captured data first!  Example: "dir" "dir  *.c" "dir
                  *.c *.h"

 dirr [pattern ...]
                  Displays the directory with the number of sectors in
                  each file.  The  number  of files matched, number of
                  blocks,  number  of kb allocated to those files, and
                  estimated  transmission time at the current baudrate
                  for  all  files  listed are displayed.  Transmission
                  time estimate is  based on batch transmission from a
                  Z89 with 700kb Tandon drives to a Cromenco 4mHz 4fdc
                  system  with  Persci  277 drive.  Time includes file
                  i/o but not error correction.

 d{mode}           Disable  mode,  where  mode is 1  or  more  of  the
                  following.        (Modes       affecting        file
                  transmission/reception affect term mode; the "s" and
                  "r" commands always send data transparently.

                  e              Exit  from  term  mode  when  EOF  is
                               encountered on transmitted file.

                  f            Full duplex.

                  h            Half Duplex.

                  g            Resumes (GO) sending  file once in term
                               mode, equivalent to XON.

                  i            Image transparent  data  capture, all 8
                               bits   of  all   characters   received,
                               including NULLS. This  overrides  the t
                               and/or z modes.

                  l             List  unit  (Printer)  on.   Since the
                               printer   is  separately  buffered,  it
                               needn't be as fast as the incoming data
                               as   long  as  the  difference  doesn't
                               exceed  the  buffer  size.   The rewind
                               command may be used to get extra copies
                               of the received data.

                  n               Send   NEWLINE   (lf)   only    when
                               transmitting file (no CR).

  