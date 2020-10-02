/*

	The following is a most extroadinary program.


	Why??

	It's a software analogue to David and Goliath,  where an
inexpensive S-100 home computer programmed in a high level language
slays a giant** IBM mainframe written that most giant of all
languages, Assembler.

	My purpose in disseminating these results is to prod people
into considering the advantages of programming in a modern, rich,
portabale, maintainable high level language such as C.

	You can't dismiss a high level language out of hand just
because of slow execution speed!

---------------------------------------------------------------------

TIMES:

TRS-80 BASIC		Z80	23470 SEC

ASSEMBLER		Z80	 1370 SEC

FIFTH			Z80	  365 SEC

OPTIMIZING		370/148	   79 SEC
PL/I

BAL			370/148	   56 SEC

VAX-11 BASIC		11/780	   58.2 CPU SEC
	(I/O TIME NOT INCLUDED)

BDS C 1.42 + CP/M	CROMENCO ZPU 4 MHZ 56 SEC

BDS C 1.42 + CP/M		Z-89	  127 sec

BDS C 1.43a	Godbout dual	8085	   45.84 sec
	(8085 running at 5 Mhz)

*/

/*
 * C version of Prime Nuber Prog #3 of 3
 * From Page 7-21 of 1980 DEC VAX Technical Summary
 * Originally from March 80 Byte Magazine
 * Incredible execution times are at end of this file.
 *
 * Sorry to say, my translation is no more readable than the
 * Basic version.
 *
 */

/* Translated to C by Chuck Forsberg WA7KGX 7/3/81 */
#include "a:bdscio.h"
int c,m,k,p;
struct _buf out;

main()
{
	fcreat("B:primes3.tmp", out);
	printf("PRIME3\n\007");
	fprintf(out, "	%d	%d	%d\n", 1,2,3);
	c=0; m=3;
p80:
	m += 2;
	p = m>>1;
	for(k=3; k<=p; k += k-1) {
		if(m%k == 0)
			goto p190;
	}
	if(++c & 07)
		fprintf(out, "	%d", m);
	else
		fprintf(out, "	%d\n", m);
p190:
	if(m<10000)
		goto p80;
	fprintf(out, "\nC= %d\n", c);
	printf("C= %d\n", c);
	/*
	 * I'm cheating just like VAX Basic by excluding file
	 * open and closing times.
	 */
	printf("\007Time's Up\n");
	putc(CPMEOF, out);
	fflush(out);
	fclose(out);
}
/*	The VAX Basic program.
15 DECLARE INTEGER M,K;
20 OPEN "PRIMES3.TMP" FOR OUTPUT AS FILE #1
30 PRINT "PRIME3"+TIME$(0)
40 PRINT
50 PRINT #1, 1;2;3;
55 C=0
70 M=3
80 M=M+2
90 FOR K=3 TO M/2 STEP K-1
100  IF(INT(M/K)*K-M = 0 THEN 190
110 NEXT K
121 PRINT #1%, M;
122 C=C+1
190 IF M < 10000 THEN 80
195 PRINT #1%, "C=";C
196 PRINT "C=";C
199 T2=TIME(1%)
    P$="DONE"+NUM1$(0.1*(T2-T1))+"CPU SEC"
200 PRINT P$
    PRINT #1, P$
201 END

	Version modified for Microsoft BASIC-80 Dialect

15 DEFINT C,I,J,K,M
30 PRINT "PRIMES3"
40 PRINT
50 PRINT 1;2;3
55 C=0
70 M=3
80 M=M+2
90 FOR K=3 TO M/2 STEP K-1
100 IF ( M MOD K ) = 0 THEN 190
110 NEXT K
121 PRINT M;
122 C=C+1
190 IF M < 10000 THEN 80
196 PRINT "C=";C
200 PRINT "DONE"

	The above program was stopped at 7 minutes, it had only
reached about 3000, running on a 4Mhz ZPU with no wait states.

	FIFTH version of the above.  365 Seconds on 64k Z89
(2 mHz Z80).

( prime numbers benchmark ) PRIMARY DECIMAL
: PBENCH ." Prime numbers benchmark" CR CR ." 1 2 3 "
        0 ( C)   3 ( m )
        BEGIN 2+ ( m=m+2)
            TRUE ( flag) OVER 2/ 3 DO
              OVER ( m) I MOD  ( m mod k)
              IF ELSE NOT LEAVE ENDIF I 1- +LOOP ( step k-1)
              IF DUP . SWAP 1+ SWAP ENDIF ( print prime, c=c+1)
        DUP 10000 >= UNTIL
        CR ." C=" DROP . CR ." Done" 7 0 PUT ;

;S

*/

/*
 * BAL means Basic Assembly Language, used on IBM mainframes
 * BDS C was timed on a S-100 system with Cromenco ZPU 4 Mhz,
 * 4FDC, Persci 277 (Single Dens), Microbyte static RAM, and
 * Tek 4012 at 9600 baud.  Timed by Microma Digital Chronograph
 * and bell-activated right hand index finger.
 * The Zenith Z-89 had 64k and twin 5 inch drives.
 * Timing for the BDS C systems includes most of the i/o, except
 * for the final buffer flush and file closing.  The VAX Basic
 * (and, presumably, PDP-10 basic as well) do NOT include i/o
 * time!
 *
 *
 * Other benchmarks suggest that C on 16 bit CPU's is many
 * times faster than BDS C (on 8080's or Z80's).
 *
 * DEC IBM BDS 11 TRS et al trademarks
 */
cuchleft!=CURLEFT) {
		poke(cuaddr,cuchleft);
		poke(cuaddr+1,cuchrght);
		}
	switch (x)
		{
	case 'h': left();	/* move cursor left */
		break;
	case 'k': right();	/* and right */
		break;
	case 'j': down();	/* or down */
		break;
	case 'u': up();		/* or up */
		break;
	case 'b': bright();	/* place dot at cursor position */
		break;
	case 'd': dim();	/* remove dot f/*
	Binary to HEX-ASCII utility

	Makes a binary file into a HEX file, 32 bytes/block with
	proper sumcheck byte at the end. The starting address for the
	HEX file is settable; the input is assumed to be contiguous
	locations. Some garbage may be included at the end of the
	HEX file since the program does not recognize 0x1A as an
	end-of-file code in the binary input.

	Called with:
		bin2hex [<binary file 1> [<bin. file 2>...]]

	Nothing special required, but if you have NSECTS
	set larger than 1 you may want to include the
	modified "getc" for better end-of-file indication.

written by:
	Robert Pasky
	36 Wiswall Rd
	Newton Centre, MA 01259
*/

#include "bdscio.h"

#define CTRLQ 0x11	/* quit key: control-q */

int i, j, fd;
int addr,sad;
char fnbuf[30], *fname;		/* filename buffer & ptr */
char onbuf[30], *oname;		/* output filename buf & ptr */
char ibuf[BUFSIZ];	/* buffered input buffer */
char obuf[BUFSIZ];	/* buffered output */
char hextab[16];
char kybdbuf[20];
char sumcheck;

main(argc,argv)
char **argv;
{
	strcpy(hextab,"0123456789ABCDEF");

	while (1)
	{
		oname = onbuf;
		if (argc