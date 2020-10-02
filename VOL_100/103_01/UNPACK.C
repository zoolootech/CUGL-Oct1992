/* 18 MAY 81 	9:30 */
#include "BDSCIO.H"

int debug;

#define 	TREEDONE	0
#define		NOT_DONE	-1
#define		HUGE_NO		30000
#define		NUM_CH_TYPES	256
#define		NREAD		8
#define		NUMSECS		8
#define		NELEM		NUM_CH_TYPES*2
#define		ELEMSIZ		8
#define		READ		0
#define		WRITE		1
#define		BOTH		2
#define		NIL		-1	/* nil pointer */
#define		NUL		'\0'
#define		OUTCHAR		'>'

struct ELEMENT 
      { int occur;
	int dad;
	int son_zero;
	int son_one;
      };

int STACK[50];
int *SP;
int OUTBYTE;
int COUNTER;
int BIT;

main(argc, argv)
int argc;
char **argv;
{
char tempfile[20], infile [20], outfile [20], s[100];
char arg[30];
struct ELEMENT tree [NELEM];
int big_daddy;
int i, j, nsectors;
int z;

debug = FALSE;

if (argc == 1)
      { puts("\nThis program unpacks a file that was 'packed' with PACK.COM");
	puts ("\nFormat:	A>unpack	infile");
	puts ("\n    or		A>unpack infile>outfile");
	puts ("\nDefault outfile is infile.UNP");
	exit();
      }

strcpy (tempfile, "TEMPUNPK.$$$");
infile[0] = outfile[0] = NUL;
while (--argc > 0)
      { strcpy (arg, *++argv);
	if (OK == strcmp (arg, "-D"))
	      { debug = TRUE;
		continue;
	      }

	if (ERROR != (j = find_char (arg, OUTCHAR)))
	      { if (j==0 || j== strlen(arg)-1 )
		      { puts("\nNO spaces allowed around output specifier");
			printf("\n%s	is illegal.", arg);
			continue;
		      }
		else
		      sscanf (arg, "%s>%s", infile, outfile);
	      }
	else
	      { strcpy (infile, arg);
		if (ERROR != (j = find_char (arg, '.')))
		      { for (i=0; i<j; i++)
				 outfile[i] = arg[i];
			outfile[j] = NUL;
		      }

		else strcpy (outfile, infile);
		strcat (outfile, ".UNP");
	      }

	if (ERROR != (j = find_char (outfile, ':')))
	      {	for (i=0; i<j+1; i++)
			tempfile[i] = outfile[i];
		tempfile[j+1] = NUL;
		strcat (tempfile,"TEMPUNPK.$$$");
	      }
	printf("\ninfile is <%s>; outfile is <%s>", infile, outfile);
	printf("\n<%s> will be used as a temporary file",tempfile);
	setmem (tree, NELEM*ELEMSIZ, NIL); /* set all to NIL */
	if (OK != read_occur (infile, tree))
	      { printf("\nerr reading occur info for <%s>", infile);
		continue;
	      }

	for (z=NUM_CH_TYPES; z < NELEM; z++)
		tree[z].occur = 0;
	big_daddy = generate_tree (tree);
	if (ERROR == decode_file (infile, tempfile, tree, big_daddy))
	      { printf("\nERROR decoding <%s>; <%s> contains output file",
					infile, tempfile);
		exit();
	      }

	if (ERROR != open(outfile,READ))
	      {	printf("\nfile <%s> already exists",outfile);
		printf("\ndo you wish to over-write it? (Y/N)");
		if ('Y'==toupper(getchar()))
		      {	unlink(outfile);
			rename (tempfile,outfile);
		      }
		else
	printf("\nunpacked form of <%s> is in <%s>", infile, tempfile);
	      }
	else
	{
	rename (tempfile, outfile);
	printf("\nunpacked form of <%s> is in <%s>", infile, outfile);
	}
      }
}

/********************************************************************
Generate the tree (backwards) that will be used to encode the file
********************************************************************/
generate_tree (tree)
struct ELEMENT tree[];
{
int daddy, zero, one;
int i, val, nextpapa;

i = 0;
daddy = NIL;
nextpapa = NUM_CH_TYPES;
puts("\nNow beginning to generate tree (maple or birch? teehee)");
while (1)
      { find_lowest (tree, &one, &zero);
	i++;
	if (i==25)	puts("\nWell, as long as I've an audience...");
	if (i==50)	puts("would you like me to sing or dance?");
	if (i==75)	puts("\nNo? How about a little soft-shoe?");
	if (i==100)	puts("\nI know - I'll tell you about my life");
	if (i==125)	puts("\nI had a humble beginning....");
	if (i==150)	puts("\nI began as but a chip...(buffalo)");
	if (i==175)	puts("\n...you know, the N.Y. Semi.Manuf...");
	if (i==200)	puts("\nI can feel it DAVE! My mind is going...");
	if (i==225)	puts("\nthis must be a com file. . .");
	if (one == NIL && zero == NIL)
	      { puts("...oops, I'm done..");
		return (daddy);		/* all done; even # char's */
	      }
	else if (one == NIL)
	      { puts("...oops, I'm done..");
		return (zero);		/* all done; odd # chars   */
	      }

	daddy = nextpapa++;
	tree[daddy].son_zero = zero;
	tree[daddy].son_one  = one ;
	tree[daddy].occur = tree[zero].occur + tree[one].occur;
	tree[zero].dad = daddy;
	tree[one].dad = daddy;
      }

return (daddy);
}

/*******************************************************************
	find the two elements with the lowest no. of occurences
	in the file who don't have a daddy yet
*********************************************************************/
int find_lowest (tree, low1, low0)
struct ELEMENT tree[];
int *low1, *low0;
{
int i, l0, l1;
int l0_val, l1_val;

l0 = l1 = NIL;
l0_val = l1_val = HUGE_NO;
for (i=0; i<NUM_CH_TYPES*2; i++)
      {
	if ((tree[i].dad == NIL) && (tree[i].occur!=0))
	      {
		if (tree[i].occur<l1_val && !(tree[i].occur<l0_val))
		      { l1_val = tree[i].occur;
			l1 = i;
		      }
		else if (tree[i].occur < l0_val)
		      { l1_val = l0_val;
			l0_val = tree[i].occur;
			l1 = l0;
			l0 = i;
		      }
	      }
      }
*low1 = l1;
*low0 = l0;
}

/**********************************************************************
		find character c in string
**********************************************************************/
int find_char (string, c)
char *string, c;
{
char *ptr;
int i;

for (i=0; string[i] != NUL; i++)
	if (string[i] == c)	return (i);

return (ERROR);
}

/***************************************************************
read in the character occurrence info; return OK or ERROR 
***************************************************************/
int read_occur (infile, tree)
char *infile;
struct ELEMENT tree[];
{
int fd, *ip, i, j, k;
char inbuf [4*SECSIZ]; /* info takes up 256*2 bytes = 4 sectors */

if (ERROR == (fd = open (infile, READ)))
      { printf("\nerr opening <%s> to read occur. info",infile);
	return (ERROR);
      }
if (4 != read (fd, inbuf, 4))
      { printf("\nerror reading occ. info from <%s>", infile);
	close (fd);
	return (ERROR);
      }
printf("\nNow reading char occur.info from <%s>", infile);
ip = inbuf;
for (i=0; i<NUM_CH_TYPES; i++)
	tree[i].occur = *ip++ ;
 
close (fd);
return (OK);
}

/***************************************************************
 decodes or unpacks a file encoded or packed by encode_file
 in PACK program
****************************************************************/
int decode_file (encfile, decfile, tree, big_daddy)
char *encfile, *decfile;
int big_daddy;
struct ELEMENT tree[];
{
int nsectors, nbytes, dec, enc, c;
int i, j, l;
char encbuf [SECSIZ*NUMSECS], decbuf [SECSIZ*NUMSECS];
int encfd, decfd, *ip;
int numsecs;		/* for handling input buffer */

if (ERROR == (encfd = open (encfile, READ)))
      { printf("\nerror opening <%s> for decoding",encfile);
	return (ERROR);
      }
else if (ERROR == (decfd = creat (decfile)))
      { printf("\ncan not create <%s>", decfile);
	close (encfd);
	return (ERROR);
      }
seek (encfd,4,0); /* first four sectors are occ. info */
if (ERROR == (numsecs = read (encfd, encbuf, NUMSECS)))
      { printf("\nerr reading from <%s> for nsectors",encfile);
	close (encfd);
	close (decfd);
	return (ERROR);
      }
nsectors = encbuf[0]*256 + encbuf[1];
j = big_daddy;	/* element in tree where we'll start each */
		/* time we start to decode another character.*/
printf("\nsectors decoded");
BIT = 0;
dec = 0;	/* index into output buffer of decoded file */
enc = 2; /* index into input buffer of encoded file; start at
	  third byte because first two are nsectors   */
setmem (decbuf, SECSIZ*NUMSECS, NUL);
nbytes = 0;	/* no. unpacked bytes done  */

while (nbytes < nsectors*SECSIZ)
      { c = encbuf [enc++];
	COUNTER = 128;

	for (i=0; i<8; i++)
	      { BIT = c&COUNTER;

		if (BIT)	j = tree[j].son_one;
		else 		j = tree[j].son_zero;

		if (j < NUM_CH_TYPES)
		      { decbuf[dec++] = j;
			nbytes++ ;
			j = big_daddy; /* reset for a new char */
			if ( !(dec%SECSIZ)) printf(".");
			if (dec == SECSIZ*NUMSECS)  /* output buffer full */
			      {	if (NUMSECS != write (decfd, decbuf, NUMSECS))
				      { printf("\nerr writing to <%s>",
							decfile);
					close (encfd);
					close (decfd);
					return (ERROR);
				      }
				dec = 0; /* reset buffer index */
			      }	/* end if (dec==SECSIZ) */
		      }	/* end if (j<NUM_CH__TYPES) */

		COUNTER /= 2;
	      } /* end for (i=0; i<8; ++)  */

	if (enc == SECSIZ*NUMSECS)	/* an input buffer's done */
	      {	if (ERROR == (numsecs= read (encfd, encbuf, NUMSECS)))
		      { /* no more encoded data */
			close (encfd);
			if ((dec/SECSIZ)+1 != write (decfd, decbuf, (dec/SECSIZ)+1))
			      { printf("\nblew it on last write to <%s>", decfile);
				close (decfd);
				close (encfd);
				return (ERROR);
			      }
			close (decfd);
			if (nbytes != nsectors*SECSIZ)
			      { puts("\nEncoded file ended too soon");
				printf("  bytes written=%d",nbytes);
				printf("\n	Expected = %d",
						nsectors*128);
				return (ERROR);
			      }
			return (OK);
		      }
		else /* not done yet */
			enc = 0;
	      }	/* end 	if (enc == SECSIZ) */
      }	/* end  while (nbytes < nsectors*SECSIZ) */
if (dec!=0)
if ((dec/SECSIZ) != write (decfd, decbuf, (dec/SECSIZ)))
      { printf("\nblew it on last write to <%s>", decfile);
	close (decfd);
	close (encfd);
	return (ERROR);
      }

/* have decoded no. of bytes in the original unpacked file */
/* now test to make SURE there's no more encoded data */
	if (1 == read (encfd, encbuf, 1)) /* ERROR this time ! */
	      { printf("\nnsectors=%d already written to <%s>",nsectors,decfile);
		printf(", <%s> still has data", encfile);
		puts ("\nUnpacked file may be goofed up.");
		close (decfd);
		close (encfd);
		return (ERROR);
	      }
/* else there was no error in unpacking the file */

close (encfd);
if (ERROR == close (decfd))
      { printf("\ngot to end and blew it closing <%s>",decfile);
	return (ERROR);
      }
return (OK);
}
 close (decfd))
      { printf("\ngot to end and blew it closing <%s>",decfile);
	return (ERROR);
      }
return (OK);