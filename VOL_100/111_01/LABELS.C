/*
HEADER:		;
TITLE:		Labels;
VERSION:	1.2;

DESCRIPTION:	"Prints labels from an address file.
		File CUG.ADR is a sample address file.";

KEYWORDS:	Labels, address, mail;
SYSTEM:		CP/M-80;
FILENAME:	LABELS.C;
WARNINGS:	"Documentation is cryptic.";
SEE-ALSO:	CUG.ADR;
AUTHORS:	Unknown;
COMPILERS:	BDS C;
*/
/************************************************************************

	Version 1.2:
		Upgrade to BDS "C" v1.44

	Command syntax:

	A>LABELS <address file> outfile_name [-switches]

	Addresses may be selected individually, or by catagory.

	Switches:
	
	-p<key>	Select a Particular Address
	-c<cat>	Select a catagory for mass labels

	Address file syntax: (each entry)

	comments ...
	[individual_key] category] ... [phone]
	Any number of address lines <two spaces><zipcode>
	[greeting line of letter

************************************************************************/

#include <bdscio.h>

#define MAXSTR 10		/* Maximum string length for matching */
#define NFORMS 4		/* Number of Labels per page */
#define FORMLINES 7		/* Number of lines that will fit on a label */
#define FORMSPACE 3		/* Number of lines to skip between labels */

#define CR 0x0D
#define LF 0x0A
#define FF 0x0C

/* Global Data */

char
	labcnt, lablin,
	ans[5], stype, srchlen, i, chr, done,
	srchstr[MAXSTR+2], scanbuf[MAXSTR+2];
int
	nlabls, ichr;

struct biobuf {bbf[BUFSIZ];};

struct biobuf adrs_file, labl_file;
struct biobuf *adrs_fd, *labl_fd;

main(argc,argv)
char argc, *argv[];
{
 printf("LABELS - Mailing Label Processor, v1.2\n");

 /* Attempt to open address file */
 if ((adrs_fd = fopen(argv[1], &adrs_file)) == ERROR) {
	printf("Cannot Open: %s", argv[1]); exit(); }

 /* Make the output file */
 if ((labl_fd = fcreat(argv[2], &labl_file)) == ERROR) {
	printf("Cannot Create: %s", argv[2]); exit(); }

 if (argc <= 3) {

 /* Conversation with the human */

	again:
	 printf("\nDo you want a particular address\n or a class of addresses");
	 printf(" [P or C]? ");
	 gets(ans);
	 if ((ans[0] = toupper(ans[0])) == 'P') stype = 0;	/* Key on '[' */
	 else if (ans[0] == 'C') stype = 1;	/* Key on blank following '[' */
	 else goto again;

	 if (stype)
	  printf("\nEnter the class name: ");
	 else
	  printf("\nEnter the desired address key: ");
	gets(srchstr);
	}

	for (i = 3; argc-- > 3; ++i) {	/* Process Switches */
	/* delete a leading '-' */
	if (*argv[i] == '-') ++argv[i];

	switch (tolower(*argv[i])) {

		case 'p':	/* A Particular address */
		 stype = 0;
		 strcpy(srchstr, ++argv[i]);	/* get the key */
		 break;

		case 'c':	/* a Class of addresses */
		 stype = 1;
		 strcpy(srchstr, ++argv[i]);
		 break;

		default:
		 printf("\n??? Bad Switch");
		 goto finished;
		}
	}

 strcat(srchstr, "]");
  
 scanbuf[srchlen = strlen(srchstr)] = '\0';	/* Scan this much */

 /* Fill the scan buffer initially */
 done = 0;
 for (i = 0; i < srchlen; ++i) {
	scanbuf[i] = getadrs();
	if (done) goto finished;
 }

 /* Make labels out of the appropriate addresses */
 labcnt = nlabls = 0;
 while ((chr = getadc()) != 0xff) {
	switch (chr) {
	  case '[':		
		if (stype) { /* Class Catagory check */
			while ((chr = getadc()) != 0xff) {
				switch (chr) {
				 case ' ':
					if (keymatch()) {
						putlabel();
						goto nxtadr;
					}
					break;
				 case '[': /* Skip phone number */
					nxtline();
					goto nxtgreet;
				}
			}
			goto finished;  /* EOF encountered */
		}	
		else { /* Find individual address */
			if (keymatch()) {
				putlabel();
			}
			else {
				nxtline();
				goto nxtgreet;
			}
		}
		break;
	}
nxtadr:	continue;
nxtgreet: /* scan to next '[' (the greeting) */
	while ((chr = getadc()) != 0xff) {
		if (chr == '[') break; }
}

finished:
	fflush(&labl_file);
	close(labl_fd);
	printf("All Done, %u Labels Generated", nlabls);
	exit();
}

char getadrs()
{
 /* Get a character from the address file,
     checking for the end of file */

if ((ichr = getc(&adrs_file)) == ERROR) done = TRUE;
return (ichr);
}

char getadc()	
{
 char
	chr;

	chr = scanbuf[0];
	 /* Shift the scan buffer down a character */
	for (i = 0; i < srchlen; ++i) {
		scanbuf[i] = scanbuf[i+1]; }
	scanbuf[srchlen-1] = getadrs();
	return (chr);
}

char keymatch()
{
 /* Return TRUE if srchstr == scanbuf */
 return (strcmp(srchstr, scanbuf) == 0);
}

nxtline()
{
 /* Move to the next line */
 while ((chr = getadc()) != 0xff) {
	if (chr == LF) return; }
}

putlabel()
{
 /* Output a label to the labl_file */

 nxtline();

 /* Check for a form feed every NFORMS labels */
 if (labcnt++ >= NFORMS) {
	labcnt = 0;
	putlab(FF);
 }

 /* Output the label, counting lines */
 printf("\n Label %u:\n\n", ++nlabls);
 lablin = 0;
 while ((chr = getadc()) != 0xff) {
	if (chr == '[') goto labdone;
	if (chr == LF) {
		if (++lablin > FORMLINES) {
			printf("Label %u has too many lines", nlabls);
			goto labdone;
		}
	}
	putlab(chr);
	putchar(chr);	/* console echo */
 }
labdone:
	/* Space to the next label */
	for (; lablin < (FORMLINES + FORMSPACE); ++lablin) {
		putlab(CR);
		putlab(LF);
		if (!stype) break;	/* only one label */
	}
}

putlab(chr)
char chr;
{
	putc(chr, &labl_file);
	}
