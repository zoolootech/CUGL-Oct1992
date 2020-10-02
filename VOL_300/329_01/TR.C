/*-
 * tr - transliterate characters
 *
 * Usage: tr [-bcds] [inset [outset]]
 *
 * Use as a filter. Outset is padded to the length of
 * inset by repeating its last character, if necessary.
 * Inset and outset may contain ranges of the form a-b, where a and/or b
 * may be omitted, and octal numbers of the form \ooo, where ooo is 1-3
 * octal digits.  Combining the two (\1-\5) is allowed.
 * Nulls are acceptable both in the input stream and in the arguments
 * (in the form of an octal escape).
 *
 * Options:
 * -b   operate in binary mode (default is text)
 * -c   complement inset with respect to 1-0377 octal (in ASCII order)
 * -d   delete inset
 * -s   squeeze repeated characters in outset into one on output
 *
 * David MacKenzie
 * Latest revision: 05/19/88
**
**	Roberto Artigas Jr
**	P.O. Box 281415
**	Memphis, TN 38168-1415
**	home: 901-762-6092
**	work: 901-373-4738
**
**	1988.12.09 - Get to run under OS/2.
**		Used C/2 version 1.10 under OS/2 E 1.1
**		cl -c -AL tr.c
**		link tr,/noi,tr,llibce+os2, ;
**+
*/
#define AZTEC	0

#define	CMP	0
#define	DOS	0
#define	OS2	1
#include	<stdio.h>

#if	DOS || OS2
#include	<stdlib.h>
#include	<string.h>
#include	<io.h>
#define	agetc	getc
#define	aputc	putc
#endif

/*
 * Function prototypes 
 */
void parse();
void simpoct();
void simprange();
int indexo();
void complement();
void pad();
void tr();
void outchar();
void usage();


#define NCHARS 256


#define isoct(d) ((d) >= '0' && (d) <= '7')

/* Counted strings, to allow nulls in the arguments (via octal escapes). */
typedef struct
{
    int len;
    char buf[NCHARS];
} SET;
#if AZTEC
#undef getchar
#define getchar() (bflag ? getc(stdin) : agetc(stdin))
int bflag = 0;
#endif

int cflag = 0,
 dflag = 0,
 sflag = 0;

int 
main(argc, argv)
int argc;
char **argv;
{
    int optind;
    SET inset,
     outset,
     inset2,
    *insetp;

    for (optind = 1; optind < argc && argv[optind][0] == '-'; ++optind)
    {
	while (*++argv[optind])
	    switch (*argv[optind])
	    {
#if AZTEC
	    case 'b':
		bflag = 1;
		break;
#endif
	    case 'c':
		cflag = 1;
		break;
	    case 'd':
		dflag = 1;
		break;
	    case 's':
		sflag = 1;
		break;
	    default:
		usage();
		break;
	    }
    }
    if (optind == argc - 2)
    {
	parse(argv[optind], &inset);
	parse(argv[optind + 1], &outset);
    } else
    if (optind == argc - 1)
	parse(argv[optind], &inset);
    else
    if (optind != argc)
	usage();

    if (cflag)
    {
	complement(&inset, &inset2);
	insetp = &inset2;
    } else
	insetp = &inset;

    pad(insetp, &outset);

    tr(insetp, &outset);

    return (0);
    exit(0);
}

void 
parse(in, out)
char *in;			       /* Null-terminated string. */
SET *out;
{
    SET tempset;

    simpoct(in, &tempset);
    simprange(&tempset, out);
}
/*
 * Copy string in to string out, with octal escapes simplified to their
 * actual value. 
 */

void 
simpoct(in, out)
char *in;			       /* Null-terminated string. */
SET *out;
{
    int i;			       /* Digit counter for octal escapes. */
    char *outp;

    outp = out->buf;
    while (*in)
	if (*in == '\\')
	{
	    *outp = 0;
	    for (i = 0, ++in; i < 3 && isoct(*in); ++i, ++in)
		*outp = *outp * 8 + *in - '0';
	    if (i == 0)
		*outp = *in++;	       /* \d, where !isoct(d), = d */
	    ++outp;
	} else
	    *outp++ = *in++;

    out->len = outp - out->buf;
}
/*
 * Copy string in to string out, with character ranges simplified to the
 * actual range of values. 
 */

void 
simprange(in, out)
SET *in,
*out;
{
    char first,
     last;			       /* First, last chars in range. */
    char *inp,
    *outp;

    inp = in->buf;
    outp = out->buf;
    while (inp < in->buf + in->len)
	if (*inp == '-')
	{
	    if (outp == out->buf)
		/* "-..." = "\1-..." */
		*outp++ = 1;
	    first = outp[-1];
	    ++inp;
	    if (inp == in->buf + in->len)
		/* "...-" = "...-\377" */
		last = 0377;
	    else
		last = *inp++;
	    for (++first; first <= last; ++first)
		*outp++ = first;
	} else
	    *outp++ = *inp++;
    out->len = outp - out->buf;
}
/*
 * Put the complement of in with respect to 1-0377 octal into out, in ASCII
 * order. 
 */

void 
complement(in, out)
SET *in,
*out;
{
    char *outp;
    int i;

    outp = out->buf;
    for (i = 1; i <= 0377; ++i)
	if (indexo(in, i) == -1)
	    *outp++ = i;
    out->len = outp - out->buf;
}
/*
 * If necessary, pad outset to the length of inset with outset's last
 * character. 
 */

void 
pad(inset, outset)
SET *inset,
*outset;
{
    char last;

    last = outset->buf[outset->len - 1];
    while (outset->len < inset->len)
	outset->buf[outset->len++] = last;
}
/*
 * Copy standard input to standard output; if a character is a member of
 * inset, transliterate it to the corresponding member of outset. 
 */

void 
tr(inset, outset)
SET *inset,
*outset;
{
    int c;			       /* One character of input. */
    int i;			       /* Index into in and out bufs. */

    while ((c = getchar()) != EOF)
	if ((i = indexo(inset, c)) != -1)
	{
	    if (!dflag)
		outchar(outset->buf[i], outset);
	} else
	    outchar(c, outset);
}
/*
 * Send c to standard output, removing duplicate consecutive characters that
 * are members of outset if the -s flag was given. 
 */
void 
outchar(c, outset)
int c;
SET *outset;
{
    static int prevc = -1;

    if (!sflag || c != prevc || indexo(outset, c) == -1)
    {
#if AZTEC
	if (!bflag)
	    aputc(c, stdout);
	else
#endif
	    putc(c, stdout);
    }
    prevc = c;
}
/*
 * Return the offset (0 through s->len - 1) of the first occurrence of
 * character c in s, or -1 if not found. 
 */

int 
indexo(s, c)
SET *s;
char c;
{
    int i;

    for (i = 0; i < s->len && s->buf[i] != c; ++i)
	 /* Do nothing. */ ;
    return i < s->len ? i : -1;
}

void 
usage()
{
#if AZTEC
    fprintf(stderr, "Usage: tr [-bcds] [inset [outset]]\n");
#else
    fprintf(stderr, "Usage: tr [-cds] [inset [outset]]\n");
#endif
    exit(1);
}
