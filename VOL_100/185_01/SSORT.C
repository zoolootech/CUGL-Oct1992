/*
 * SSORT.C - Selecta-SORT
 * version 1.0
 * This is a modified version of LEXSORT which permits a command line
 * option to select the collating sequence from a "magic" file.
 * The name was changed only due to the fact that LEXSORT was no longer
 * (if it ever was) appropriate.  LEXSORT.LBR should be retired.
 *
 * Differences from LEXSORT.C:
 *
 * The only changed functions were options() and usage(). Two functions
 * were added: get_collating_sequence(), collate_file().
 * Four #defines were added:  INPUT, ABSOLUTE, FUDGE, COLLATE_NAME.
 * The signon message was changed.  Globals were not modified.
 *
 *	Harvey Moran	2/26/84
 *
 * To build with BDS C vers 1.5:
 *	cc ssort -o -e 3800
 *	casm lexlate
 *	asm lexlate.DDz		;choose drives (D) to suit
 *	ddt
 *	f100,1000,0  (not necessary, makes file compares of '.crl' meaningful)
 *	iLEXLATE.HEX
 *	g0
 *	save 4 lexlate.crl
 *	l2 ssort lexlate
 *
 * Usage:
 *
 *	SSORT <infile> <outfile> [-c<index 0 to n>] [-k<key field selections>]
 *
 *	-c<n> seletcs the n'th sort order precedence table from the
 *	"magic" file SSORT.OVL to be overlayed into lexlate().  See
 *	SORTORDR.ASM for the definition of these tables. ASseMble and LOAD
 *	SORTORDR.ASM, then rename to SSORT.OVL. 
 *	If -c is not used, the default remains the same lexicographical
 *	increasing order sort from LEXSORT (or whatever you "wire" into
 *	LEXLATE.CRL).
 *
 *	All other command line syntax remains the same as LEXSORT.
 *
 *		Harvey Moran 2/26/84
 *
 * =====================================================================
 * LEXSORT.C (modified SORT3.C for lexicographical ordering) HRM 11/6/83
 * version 1.0
 * To build with BDS C vers 1.5:
 *	cc lexsort -o -e 3400
 *	casm lexlate
 *	asm lexlate.DDz		;choose drives (D) to suit
 *	ddt
 *	f100,1000,0  (not necessary, makes file compares of '.crl' meaningful)
 *	iLEXLATE.HEX
 *	g0
 *	save 4 lexlate.crl
 *	l2 lexsort lexlate
 *
 * Usage: lexsort <infile> <outfile> [-k<sort key list>]
 *
 * where <sort key list> is:
 *
 * A comma separated list of column numbers or ranges
 * specifing the sort key positions.
 *	e.g.
 * lexsort messy.dat neat.dat -k3-5,7-9,1-2,12
 *
 * specifies that:
 * the  input file is MESSY.DAT
 * the output file is  NEAT.DAT
 *
 *         The primary sort key is columns  3 thru 5
 * The first secondary sort key is columns  7 thru 9
 * The next  secondary sort key is columns  1 thru 2
 * The last  secondary sort key is columns 12 thru end of line
 *
 * A sort key of 1 column may be specified as 3-3 for example.
 * A sort key which goes to end of line need NOT be the last one.
 *
 * The leftmost column is numbered 1.
 * The default sort key is the entire line.
 *
 * Implementation note:
 *
 *	LEXLATE.CSM contains function lexlate() which determines the
 *	character ordering for the character set.  This concept includes
 *	the notion of totally INGOREing some characters.  The LEXLATE.CSM
 *	provided IGNOREs all characters but space, A-Z, a-z, 0-9.  It also
 *	treats 'A' as less than but adjacent to 'a', etc.  If a specified
 *	key field consists entirely of IGNORE characters, it is considered
 *	the lowest order for that key.  A line which has no entry for the
 *	specified key field (because it is too short) will be the next lowest
 *	order in the sort for that field.
 *
 * Derived from: SORT3.C by H.R. Moran, Jr. 11/5/83
 * changes made:
 *	1) Re-format, indent, and comment to suit me, including deletion
 *	   of some extraneous declarations and code lines.
 *	2) Re-write compar(), and include use of lexlate()
 *	3) Add options() to allow key field selections.
 *
 * SORT3.C comments follow:
 * --------------------------------------------------------------------------- 
 *	Sort/Merge from Software Tools
 *
 *	Last Modified : 21 September 1982
 *
 *	Converted from Software Tools RATFOR to BDS C by Leor Zolman
 *	Sep 2, 1982
 *
 *	Usage: sort <infile> <outfile>
 *
 *	Main variables have been made external; this is pretty much in
 *	line with the RATFOR call-by-name convention anyway.
 *
 *	Requires lots of disk space, up to around three times the length
 *	of the file file being sorted. This program is intended for files
 *	bigger than memory; simpler, faster sorts can be implemented for
 *	really short files (like ALPH.C)
 *		-leor
 *
 *	Compile & Link:
 *		A>cc sort.c -e2800 -o
 *		A>l2 sort
 *	(or...)
 *		A>cc sort.c -e2900 -o
 *		A>clink sort
 *
 */


#include <bdscio.h>

#define SIGNON "ssort version 1.0 - 2/26/84 - hrm\n"

#define FUDGE 0xe /* offset into the lexlate function for table address */
#define COLLATE_NAME collate_file()

/* #define DEBUG */	/* enables debug printout when defined */

#define BOOL  int		/* BOOlean */
#define PROC  int		/* PROCedure */
#define TRIAD int		/* -1, 0, or +1 */

#define IGNORE 0xff		/* lexlate() token to ignore the character */

#define VERBOSE 1		/* give running account of file activity */

#define MAXLINE 200		/* longest line we want to deal with */
#define NBUFS 7			/* Max number of open buffered files */
#define MAXPTR  3000		/* Max number of lines (set for dict) */
#define MERGEORDER (NBUFS-1)	/* Max # of intermediate files to merge */
#define NAMESIZE 20		/* Max Filename size */
#define LOGPTR 13		/* smallest value >= log (base 2) of  MAXPTR */
#define EOS '\0'		/* string termination character */
#define FILE struct _buf

#define stderr 4
#define fputc putc

char name[NAMESIZE], name2[NAMESIZE + 10];
FILE buffers[NBUFS + 1];	/* up to NBUFS general-purp. buffered files */
FILE *infil[MERGEORDER + 1];	/* tmp file ptrs for sort operation */
unsigned linptr[MAXPTR + 1], nlines;
int temp;
unsigned maxtext;		/* max # of chars in main text buffer */

/*
 * KEY FIELD selection support variables - hrm
 */
#define MAXFIELDS 20
#define FIELDS struct _fields

FIELDS {
  int _numfields;
  int _strtcol[MAXFIELDS];
  int _stopcol[MAXFIELDS];
  } sortfields;

#define FASTLOCAL

#ifdef FASTLOCAL

/*
 * FAST locals for compar()
 */
struct {
  unsigned i, j;
  unsigned k, l;
  unsigned x1, x2;
  unsigned len_i, len_j;
  char c1, c2;
  } z;
#endif

#ifdef DEBUG
char spacebuffer[200];
#endif

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*!!!!!!!!!!!!!!!!              WARNING  (H.Moran)             !!!!!!!!! */
/*! The algorithm INSISTS that THIS name be the LAST global	       ! */
/*!                                                                    ! */
char *linbuf;		/* text area starts after this variable */
/*                                                                       */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

main(argc, argv)
  int argc;
  char *argv[];
{

	int min(), gtext();

	FILE *infile, *outfile;		/* main input and output streams */
	FILE *tmpfile;
	unsigned high, lim, low;
	BOOL more_text;

	puts(SIGNON);
	linbuf = endext();		/* start of text buffer area */
	maxtext = topofmem() - endext() - 500;
	tmpfile = buffers[0];
	options(&argc, argv);	/* process command line options */
	if ( argc != 3 ) {
	  usage("");
	  }
	infile = buffers[1];
	if ( fopen(argv[1], infile) == ERROR ) {
	  puts("Can't open ");
	  puts(argv[1]);
	  exit(-1);
	  }
#if VERBOSE
	fputs("Beginning initial formation run\n", stderr);
#endif
	high = 0;		/* Initial formation of runs:	*/
	do {
	  more_text = gtext(infile);
	  quick(nlines);		
	  high++;
	  makfil(high, tmpfile);
	  ptext(tmpfile);
	  fclout(tmpfile);
	  } while ( more_text );
	fclose(infile);		/* free up the input file buffer */
#if VERBOSE
	fputs("Beginning merge operation\n", stderr);
#endif
	for ( low = 1; low < high; low += MERGEORDER ) { /* merge */
	  lim = min(low + MERGEORDER - 1, high);
	  gopen(low, lim);		/* open files */
	  high++;
	  makfil(high, tmpfile);
	  merge(lim - low + 1, tmpfile);
	  fclout(tmpfile);	/* terminate, flush and close file */
	  gremov(low, lim);
	  }
	/*
	 * Now write the sorted output file:
	 */
#if VERBOSE
	fputs("Merge complete. Writing output file.\n", stderr);
#endif
	gname(high, name);	/* create name of result file */
	infile = buffers[0];
	if ( fopen(name, infile) == ERROR ) {
	  puts("Something's screwy; I can't open ");
	  puts(name);
	  exit(-1);
	  }
	outfile = buffers[1];
	while ( fcreat(argv[2], outfile) == ERROR ) {
	  puts("Can't create ");
	  puts(argv[2]);
	  puts("\nEnter another name to call the output: ");
	  gets(name2);
	  argv[2] = &name2;
	  }
	while ( fgets(linbuf, infile) ) {
	  fputs(linbuf, outfile);
	  fputs("\r\n", outfile);
	  }
	fclout(outfile);
	fabort(infile->_fd);
	unlink(name);
}


PROC
options(pac, av)
  int *pac;
  char *av[];
{
	int i, j;
	char *s;

	sortfields._numfields  = 1;
	sortfields._strtcol[0] = 0;
	sortfields._stopcol[0] = 32767;
	for ( i = 1; i < *pac; ++i ) {
	  if ( *av[i] == '-' && tolower(av[i][1]) == 'c' ) {
	    get_collating_sequence(atoi(&av[i][2]));
	    for ( j = i; j < *pac; ++j ) /* remove options from av[] */
	      av[j] = av[j+1];
	    *pac -= 1;
	    }
	  if ( *av[i] == '-' && tolower(av[i][1]) == 'k' ) {
	    s = av[i] + 2;
	    for ( j = 0; isdigit(*s) && j < MAXFIELDS; ++j ) {
	      sortfields._strtcol[j] = 0;
	      while ( isdigit(*s) ) {
	        sortfields._strtcol[j] *= 10;
	        sortfields._strtcol[j] += (*s++ - '0');
	        }
	      if ( (sortfields._strtcol[j] -= 1) < 0 )
	        sortfields._strtcol[j] = 0;
	      sortfields._stopcol[j] = 32767;
	      if ( *s == '-' ) {
	        ++s;
	        sortfields._stopcol[j] = 0;
	        while ( isdigit(*s) ) {
	          sortfields._stopcol[j] *= 10;
	          sortfields._stopcol[j] += (*s++ - '0');
	          }
	        if ( (sortfields._stopcol[j] -= 1) < 0 )
	          sortfields._stopcol[j] = 32767;
	        if ( *s != ',' )
	          break;
	        ++s;
	        }
	      else if ( *s != ',' )
	        break;
	      else
	        ++s;
	      sortfields._numfields++;
	      }
	    for ( j = i; j < *pac; ++j ) /* remove options from av[] */
	      av[j] = av[j+1];
	    *pac -= 1;
	    }
	  }
	for ( i = 0; i < sortfields._numfields; ++i )
	  if ( sortfields._strtcol[i] > sortfields._stopcol[i] )
	    usage("ILLEGAL SORT KEY SPECIFICATION");
}

PROC
fclout(obuf)
  FILE *obuf;
{
	putc(CPMEOF, obuf);
	fflush(obuf);
	fclose(obuf);
}


/*
 * Quick: Quicksort for character lines
 */

PROC
quick(nlines)
  unsigned nlines;
{
	unsigned i, j, lv[LOGPTR + 1], p, pivlin, uv[LOGPTR + 1];
	int compar();

	lv[1] = 1;
	uv[1] = nlines;
	p = 1;
	while ( p > 0 )
	  if ( lv[p] >= uv[p] )	/* only 1 element in this subset */
	    p--;		/* pop stack			*/
	  else {
	    i = lv[p] - 1;
	    j = uv[p];
	    pivlin = linptr[j];	/* pivot line		*/
	    while ( i < j ) {
	      for ( i++; compar(linptr[i], pivlin) < 0; i++ )
	        ;
	      for ( j--; j > i; j-- )
	        if ( compar(linptr[j], pivlin) <= 0 )
	          break;
	      if ( i < j ) {	/* out of order pair		*/
#ifdef DEBUG
	        printf("SWAPPING:\n%s\n%s\n\n",
			&linbuf[linptr[i]], &linbuf[linptr[j]]);
#endif
	        temp = linptr[i];
	        linptr[i] = linptr[j];
	        linptr[j] = temp;
	        }
	      }
	  j = uv[p];		/* move pivot to position 1 	*/
	  temp = linptr[i];
	  linptr[i] = linptr[j];
	  linptr[j] = temp;
	  if ( (i - lv[p]) < (uv[p] - i) ) {
	    lv[p + 1] = lv[p];
	    uv[p + 1] = i - 1;
	    lv[p] = i + 1;
	    }
	  else {
	    lv[p + 1] = i + 1;
	    uv[p + 1] = uv[p];
	    uv[p] = i - 1;
	    }
	  p++;
	  }			
}			


/*
 * Ptext: output text lines from linbuf onto the buffered output file given
 */

PROC
ptext(outfil)
  FILE *outfil;
{
	int i;

	for ( i = 1; i <= nlines; i++ ) {
	  fputs(&linbuf[linptr[i]], outfil);
	  fputc('\0', outfil);	/* terminate the line */
	  }
}


/*
 * Gtext: Get text lines from the buffered input file provided, and
 * 	  place them into linbuf
 */

int
gtext(infile)
  FILE *infile;
{
	unsigned lbp, len;

	nlines = 0;
	lbp = 1;
	do {
	  if ( (len = fgets(&linbuf[lbp], infile)) == NULL )
	    break;
	  len = strlen(&linbuf[lbp]);
	  linbuf[lbp + len - 1] = '\0';
	  nlines++;
	  linptr[nlines] = lbp;
	  lbp += len;	/* drop '\n', but keep NULL at end of string */
	  } while ( lbp < (maxtext - MAXLINE) && nlines < MAXPTR );
	return (len);		/* return 0 if done with file */
}


/*
 * Makfil: Make a temporary file having suffix 'n' and open it for
 * 	   output via the supplied buffer
 */

PROC
makfil(n, obuf)	/* make temp file having suffix 'n' */
  int n;
  FILE *obuf;
{
	FILE *fp;
	char name[20];
	gname(n, name);
	if ( fcreat(name, obuf) == ERROR ) {
	  puts("Can't create ");
	  puts(name);
	  exit(-1);
	  }
}


/*
 * Gname: Make temporary filename having suffix 'n'
 */

char *
gname(n, name)
  char *name;
  int n;
{
	char tmptext[10];

	strcpy(name, "TEMP");		/* create "TEMPn.$$$"	*/
	strcat(name, itoa(tmptext, n));
	strcat(name, ".$$$");
	return (name);		/* return a pointer to it	*/
}


/*
 * Itoa: convert integer value n into ASCII representation at strptr,
 * 	 and return a pointer to it
 */

char *
itoa(strptr, n)
  char *strptr;
  int n;
{
	int length;

	if ( n < 0 ) {
	  *strptr++ = '-';
	  strptr++;
	  n = -n;
	  }
	if ( n < 10 ) {
	  *strptr++ = (n + '0');
	  *strptr = '\0';
	  return (strptr - 1);
	  }
	else {
	  length = strlen(itoa(strptr, n/10));
	  itoa(&strptr[length], n % 10);
	  return (strptr);
	  }
}


/*
 * Gopen: open group of files low...lim
 */

PROC
gopen(low, lim)
  int lim, low;
{
	int i;

#if VERBOSE
	fprintf(stderr, "Opening temp files %d-%d\n", low, lim);
#endif

	for ( i = 1; i <= (lim - low + 1); i++ ) {
	  gname(low + i - 1, name);
	  if ( fopen(name, buffers[i]) == ERROR ) {
	    puts("Can't open: ");
	    puts(name);
	    exit(-1);
	    }
	  infil[i] = &buffers[i];
	  }
}


/*
 * Remove group of files low...lim
 *		(should use "close" instead of "fabort" for MP/M II)
 */

PROC
gremov(low, lim)
  int lim, low;
{
	int i;

#if VERBOSE
	fprintf(stderr, "Removing temp files %d-%d\n", low, lim);
#endif
	for ( i = 1; i <= (lim - low + 1); i++ ) {
	  fabort(infil[i]->_fd);		/* forget about the file */
	  gname(low + i - 1, name);
	  unlink(name);			/* and physically remove it */
	  }
}

/*
 * Fputs: special version that doesn't epand LF's and aborts on error
 */
 
PROC
fputs(s, iobuf)
  char *s;
  FILE *iobuf;
{
	char c;

	while ( c = *s++ )
	  if ( putc(c, iobuf) == ERROR ) {
	    fputs("Error on file output\n", stderr);
	    exit(-1);
	    }
}


/*
 * Merge: merge infil[1]...infil[nfiles] onto outfil
 */

PROC
merge(nfiles, outfil)
  FILE *outfil;
{
	char *fgets();

	int i, inf, lbp, lp1, nf;

	lbp = 1;
	nf = 0;
	for ( i = 1; i <= nfiles; i++)  /* get one line from each file */
	  if ( fgets(&linbuf[lbp], infil[i]) != NULL ) {
	    nf++;
	    linptr[nf] = lbp;
	    lbp += MAXLINE;	/* leave room for largest line */
	    }
	quick(nf);		/* make initial heap */
	while ( nf > 0 ) {
	  lp1 = linptr[1];
	  fputs(&linbuf[lp1], outfil);				
	  fputc('\0', outfil);
	  inf = lp1 / MAXLINE + 1;	/* compute file index */
	  if ( fgets(&linbuf[lp1], infil[inf]) == NULL ) {
	    linptr[1] = linptr[nf];
	    nf--;
	    }
	  reheap(nf);
	  }
}

		
/*
 * Reheap: propogate linbuf[linptr[1]] to proper place in heap
 */

PROC
reheap(nf)
  unsigned nf;
{
	unsigned i, j;

	for ( i = 1; (i + i) <= nf; i = j ) {
	  j = i + i;
	  if ( j < nf && compar(linptr[j], linptr[j + 1]) > 0 )
	    j++;
	  if ( compar(linptr[i], linptr[j]) <= 0 )
	    break;
	  temp = linptr[i];
	  linptr[i] = linptr[j];
	  linptr[j] = temp;
	  }
}

/*
 * Just like regular library version, except that NULL is also
 * taken as a string terminator.
 */

char *
fgets(s, iobuf)
  char *s;
  FILE *iobuf;
{
	int count, c;
	char *cptr;
	count = MAXLINE;
	cptr = s;
	if ( (c = getc(iobuf)) == CPMEOF || c == EOF )
	  return (NULL);
	do {
	  if ( (*cptr++ = c) == '\n' ) {
	    if ( cptr>s+1 && *(cptr-2) == '\r' )
	      *(--cptr - 1) = '\n';
	    break;
	    }
	  if ( ! c )
	    break;
	   } while ( count-- && (c=getc(iobuf)) != EOF && c != CPMEOF );
	if ( c == CPMEOF )
	  ungetc(c, iobuf);	/* push back control-Z */
	*cptr = '\0';
	return (s);
}


PROC
usage(s)
  char *s;
{
	fprintf(stderr, "\n%s\nUsage:\n", s);
	fputs("\tssort <infile> <outfile> [-c<collating sequence>] [-k<sort key list>]\n",
		stderr);
	fputs("where <sort key list> is:\n", stderr);
	fputs(" a comma separated list of column numbers or ranges\n", stderr);
	fputs("specifing the sort key columns\n\te.g.\n", stderr);
	fputs("-k3-5,7-9,1-2,12\n\tspecifies that\n", stderr);
	fputs("       the primary sort key is columns  3 thru 5\n", stderr);
	fputs("the next secondary sort key is columns  7 thru 9\n", stderr);
	fputs("the next secondary sort key is columns  1 thru 2\n", stderr);
	fputs("the last secondary sort key is columns 12 thru end of line.\n",
		stderr);
	fputs("\nThe leftmost column is numbered 1.\n", stderr);
	fputs("The default sort key is the entire line.\n", stderr);
	fputs("\n-c<n> selects the n'th collating sequence from the \"magic\"\n", stderr);
	fprintf(stderr, "file %s. Where sequences are numbered from zero.\n",
		COLLATE_NAME); 
	exit();
}


/*
 * Compar: Compare two strings; return 0 if equal, -1 if first is
 *	   lexically smaller, or 1 if first is bigger
 */



TRIAD
compar(lp1, lp2)
  unsigned lp1, lp2;
{
	char lexlate();

#ifndef FASTLOCAL
	struct {
	  unsigned i, j; /* string relative char positions being considered */
	  unsigned k, l;		/* scratch for # keys, key columns */
	  unsigned x1, x2;	/* string relative end of key field columns */
	  unsigned len_i, len_j; /* lengths of the two incoming strings */
	  char c1, c2;		/* characters currently being considered */
	  } z;
#endif

	z.len_i = strlen(&linbuf[lp1]);
	z.len_j = strlen(&linbuf[lp2]);
	for ( z.k = 0; z.k < sortfields._numfields; ++z.k ) {
	  z.l  = sortfields._strtcol[z.k];	/* for each sort key */
	  z.i  = lp1 + min(z.l, z.len_i);
	  z.j  = lp2 + min(z.l, z.len_j);
	  z.l  = sortfields._stopcol[z.k];
	  z.x1 = lp1 + min(z.l, z.len_i);
	  z.x2 = lp2 + min(z.l, z.len_j);
	  do {				/* for each char position in a key */
	    while ( (z.c1=lexlate(linbuf[z.i])) == IGNORE )
	      if ( ++z.i > z.x1 )
	        break;
	    while ( (z.c2=lexlate(linbuf[z.j])) == IGNORE )
	      if ( ++z.j > z.x2 )
	        break;
	    if ( (z.i > z.x1) && (z.j <= z.x2) ) {
#ifdef DEBUG
	      printf("END FIELD LESS THAN:\n%s\n%s\n",
			&linbuf[lp1], spaces(z.i-lp1));
	      printf("%s\n%s\n\n", &linbuf[lp2], spaces(z.j-lp2));
#endif
	      return (-1);
	      }
	    if ( (z.j > z.x2) && (z.i <= z.x1) ) {
#ifdef DEBUG
	      printf("END FIELD GREATER THAN:\n%s\n%s\n",
			&linbuf[lp1], spaces(z.i-lp1));
	      printf("%s\n%s\n\n", &linbuf[lp2], spaces(z.j-lp2));
#endif
	      return (1);
	      }
	    if ( (z.i > z.x1) && (z.j > z.x2) )
	      break;
	    if ( z.c1 != z.c2 ) {
	      if ( z.c1 == IGNORE ) /* terminated on IGNORE class of char */
	        z.c1 = 0;
	      if ( z.c2 == IGNORE ) /* terminated on IGNORE class of char */
	        z.c2 = 0;
	      if ( z.c1 == z.c2 ) {
#ifdef DEBUG
	        printf("MAPPED MISMATCH EQUAL TO:\n%s\n%s\n",
			&linbuf[lp1], spaces(z.i-lp1));
	        printf("%s\n%s\n\n", &linbuf[lp2], spaces(z.j-lp2));
#endif
	        return (0);
	        }
#ifdef DEBUG
	      if ( z.c1 < z.c2 ) {
	        printf("MAPPED MISMATCH LESS THAN:\n%s\n%s\n",
			&linbuf[lp1], spaces(z.i-lp1));
	        printf("%s\n%s\n\n", &linbuf[lp2], spaces(z.j-lp2));
	        }
	      else {
	        printf("MAPPED MISMATCH GREATER THAN:\n%s\n%s\n",
			&linbuf[lp1], spaces(z.i-lp1));
	        printf("%s\n%s\n\n", &linbuf[lp2], spaces(z.j-lp2));
	        }
#endif
	      return ((z.c1 < z.c2) ? -1 : 1);
	      }
	    ++z.i;
	    ++z.j;
	    } while ( 1 );
	  /*
	   * end of sort key field with equal keys, use next key field
	   */
	  }
	/*
	 * Final result at end of ALL sort key fields
	 */
	if ( z.c1 == IGNORE ) /* terminated on the IGNORE class of char */
	  z.c1 = 0;
	if ( z.c2 == IGNORE ) /* terminated on the IGNORE class of char */
	  z.c2 = 0;
	if ( z.c1 == z.c2 ) {
#ifdef DEBUG
	  printf("FINAL EQUAL TO:\n%s\n%s\n",
		&linbuf[lp1], spaces(z.i-lp1));
	  printf("%s\n%s\n\n", &linbuf[lp2], spaces(z.j-lp2));
#endif
	  return (0);
	  }
#ifdef DEBUG
	if ( z.c1 < z.c2 ) {
	  printf("FINAL LESS THAN:\n%s\n%s\n",
		&linbuf[lp1], spaces(z.i-lp1));
	  printf("%s\n%s\n\n", &linbuf[lp2], spaces(z.j-lp2));
	  }
	else {
	  printf("FINAL GREATER THAN:\n%s\n%s\n",
		&linbuf[lp1], spaces(z.i-lp1));
	  printf("%s\n%s\n\n", &linbuf[lp2], spaces(z.j-lp2));
	  }
#endif
	return ((z.c1 < z.c2) ? -1 : 1);
}


#ifdef DEBUG
spaces(n)
{
	int i;

	spacebuffer[0] = '\0';
	for ( i = 0; i < n; ++i )
	  spacebuffer[i] = ' ';
	spacebuffer[i++] = '^';
	spacebuffer[i] = '\0';
	return (spacebuffer);
}
#endif



PROC
get_collating_sequence(n)
  int n;
{
	int lexlate();
	char *p;
	int f;

#define INPUT 0
#define ABSOLUTE 0
	if ( (f=open(COLLATE_NAME, INPUT)) < 0 ) {
	  fprintf(stderr, "Can't find file %s\n", COLLATE_NAME);
	  exit();
	  }
	if ( seek(f, 2*n, ABSOLUTE) < 0 ) {
	  fprintf(stderr, "Error in seeking on file %s\n", COLLATE_NAME);
	  exit();
	  }
	p = lexlate;
	p += FUDGE;
	if ( read(f, p, 2) != 2 ) {
	  fprintf(stderr, "Error in reading file %s\n", COLLATE_NAME);
	  exit();
	  }
	fabort(f);
}


char *
collate_file()
{
	return ("SSORT.OVL\0xxxxxxxxx");
}
.j-lp2));
	  }
	else {
	  printf("FINAL GREATER THAN:\n%s\