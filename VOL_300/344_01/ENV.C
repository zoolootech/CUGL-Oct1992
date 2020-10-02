/* 	wmcenv.c (c) 1989 Bryan R Leipper

SYNOPSIS:	print envelopes in HP LJ II from address list file
DESCRIPTION:
	reads a file of addresses separated by a line of @@@ and
	prints and envelope for each.
RETURNS: not implemented.
CAVEATS: MSC 5.1 and MSDOS: watch for file permissions
	needs eof check for last entry to avoid need for @@@ line
	on last address.
FILES: input, output, stdprn
NOTES:
SEE ALSO:
HISTORY:	adapted from env.c 2/24/90.
	19 April 1990 - cleanup
REGISTRATION:
	Bryan R Leipper, 714 Terra Ct, Reno NV 89506

*/
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
#include <brl/standard.h>			/* local machine and app specific */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>

/* -----------------------------------------------------------------------
;       EQUATES  for Envelope Margins - Note Bytes reversed
;-----------------------------------------------------------------------
LMARG_ADD_S     EQU     "06"            ; Left Address Margin - Small
LMARG_RET_S     EQU     "54"            ; Left Return Margin - Small
LMARG_ADD_L     EQU     "05"            ; Left Address Margin - Large
LMARG_RET_L     EQU     "71"            ; Left Return Margin - Large
TMARG_RET_S     EQU     "03"            ; Top Return Margin - Small
TMARG_RET_L     EQU     "92"            ; Top Return Margin - Large
TMARG_RET_2     EQU     "61"            ; Top Return Margin - Series 2
;-----------------------------------------------------------------------
; Laser Jet Command Sequences
;-----------------------------------------------------------------------
*/
#define ESCAPE	27
static BYTE RESET_LJ[] = {ESCAPE,"E"};		/* reset printer */
static BYTE ENVSIZE[] = {ESCAPE,"&l81A"};	/*  select #10 envelope size */
static BYTE LNDSCP[] = {ESCAPE,"&l1O"};		/* landscape */
static BYTE ROMAN8[] =	{ESCAPE,"(8U"};		/* Roman-8 symbol set */
static BYTE COURIER[] = {ESCAPE,"(sp10h12vsb3T"};	
			/* 10-pitch  12-point upright  med-weight  Courier */
static BYTE ENVFEED[] =	{ESCAPE,"&l2H"};	/* envelope tray feed */
static BYTE EJECT[] =	{ESCAPE,"&l0H"};	/* eject sheet */
static BYTE TRAFEED[] =	{ESCAPE,"&l1H"};	/* tray feed */
static BYTE MANFEED[] =	{ESCAPE,"&l2H"};	/* manual feed */
static BYTE TOP_MARGIN[] = {ESCAPE,"&l1E"};
static BYTE RTRN_MARGIN[] = {ESCAPE,"&a2l1R\r"};
			/* rtrn addrs at row 1 col 2  */
static BYTE ADRS_MARGIN[] = {ESCAPE,"&a55l11R\r"};
			/* address at col 55 row 11 (\r is <CR> to assure proper pos) */
static BYTE CRLF[] = {13, 10, 0};
static BYTE delimstr[] = {"@@"};	/* signifies end of an address */


/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
static BYTE *rtn_adrs[] = { "Bryan R Leipper"
			,	"714 Terra Court"
			,	"Reno, NV 89506"
			,	""};

/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
main (int argc, char *argv[]) { /*
*/ 

	FILE *ifd;
	BYTE adrs[6][80];	/* max address size 6 lines by 80 char! */
	SHORT flag, cnt, i, j;

	if (argc < 2) {
		printf("\nusage: env infile [outfile] ");
		exit(-1);
	}

	if ((ifd = fopen(argv[1],"rt")) == NULL) {
		printf("\ncannot open input file %s",argv[1]);
		exit(-1);
	}
	if (argc > 2) {
		if (freopen(argv[2],"wt", stdprn) == NULL) {
			printf("\ncannot open output file %s",argv[2]);
			exit(-1);
		}
	} else
		setmode(fileno(stdprn), O_TEXT);

	fprintf(stdprn, "%s", RESET_LJ);
	fprintf(stdprn, "%s", ENVSIZE);
	fprintf(stdprn, "%s", LNDSCP);
	fprintf(stdprn, "%s", COURIER);
	fprintf(stdprn, "%s", ENVFEED);
	fprintf(stdprn, "%s", TOP_MARGIN);
	flag = TRUE;
	cnt = 0;
	j = 0;		/* address line counter */
	while (fgets(adrs[j], 80, ifd)) {
		if ((strlen(adrs[j]) > 1) && (adrs[j][0] > 32)) {
			if (strstr(adrs[j], delimstr) == NULL) {
				if (j < 6) ++j;
			} else {
				printf("%d. %s\n", ++cnt, adrs[0]);
				fprintf(stdprn, "%s", RTRN_MARGIN);
	for (i=0; rtn_adrs[i][0]; fprintf(stdprn, "%s\n", rtn_adrs[i++]));
				fprintf(stdprn, "%s", ADRS_MARGIN);
	for (i=0; i<j; fprintf(stdprn, "%s", adrs[i++]));
				fprintf(stdprn, "%s", EJECT);
				j = 0;
			}
		}
	} ;

	fprintf(stdprn, "%s", RESET_LJ);

	printf("printed %d envelopes\n", cnt);

}
