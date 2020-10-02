/* 18 MAY 81    midnight */
#include "BDSCIO.H"

#define		DEBUG		(debug != 0)
int debug;

#define 	TREEDONE	0
#define		NOT_DONE	-1
#define		HUGE_NO		30000
#define		NUM_CH_TYPES	256
#define		NREAD		4
#define		NELEM		NUM_CH_TYPES*2
#define		NUMSECS		8
#define		ELEMSIZ		8
#define		READ		0
#define		WRITE		1
#define		BOTH		2
#define		NIL		-1	/* nil pointer */
#define		NUL		'\0'
#define		OUTCHAR		'>'

int STACK[50];
int *SP;
int OUTBYTE;
int COUNTER;
int BIT;
int C;   /* general purpose to speed things */
int BYTE_DONE;

struct ELEMENT 
      { int occur;
	int dad;
	int son_zero;
	int son_one;
      }	tree [NELEM+100];

main(argc, argv)
int argc;
char **argv;
{
char tempfile[20], infile [20], outfile [20], s[100];
char arg[30];
int big_daddy;
int i, j;
int nsectors;	/* no. sectors in input file */
int outsecs;	/* no. sectors after encoding file */
int z;

debug = FALSE;

if (argc == 1)
      { puts("\nThis program compresses a file into an encoded form,");
	puts("\nfor more efficient storage. To 'unpack' and get the");
	puts("\noriginal file back, use UNPACK.COM.");
	puts("\nFormat:		A>pack	infile");
	puts("\n    or		A>pack	infile>outfile");
	puts("\ndefault outfile name is infile.PAK");
	exit();
      }

strcpy (tempfile, "TEMPCODE.$$$");
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
		if (ERROR != (j = find_char (arg,'.')))
		      { for (i=0; i<j; i++)
				outfile[i] = arg[i];
			outfile[j] = NUL;
		      }
		else strcpy (outfile, arg);
		strcat (outfile, ".PAK");
	      }

	if (ERROR != (j = find_char (outfile,':')))
	      {	for (i=0; i<j+1; i++)
			tempfile[i]=outfile[i];
		tempfile[j+1] = NUL;
		strcat(tempfile,"TEMPCODE.$$$");
	      }
	if DEBUG printf("\ninfile = <%s>, outfile = <%s>",
			   infile,        outfile);
	setmem (tree, NELEM*ELEMSIZ, NIL); /* set all to NIL */
	for (i=0; i<NUM_CH_TYPES*2; i++)	tree[i].occur = 0;
	if (ERROR == (nsectors =  count_occur (infile)))
	      { printf("\ncan't construct char. occur. table for <%s>",
							      infile);
		continue;
	      }
	if DEBUG printf("\nnsectors = %d", nsectors);
	big_daddy = generate_tree ();
if DEBUG
      { printf("\nbig_daddy = %d",big_daddy);
	printf("\n\nAnd here's the tree:\n\n");
	puts ("\nHIT AN N TO KILL THIS\n");
	for (z=0; z<NELEM; z++)
		if (tree[z].dad!=NIL || tree[z].occur!=0)
		printf("\n%d	0x%x	%d	%d	%d	%d",z,z,tree[z].occur,tree[z].dad,tree[z].son_one,tree[z].son_zero);
      }
	if (OK != write_occur (tempfile))
	      { printf("\nerror writing occurence info to <%s>",tempfile);
		puts ("\nI can't pack this file");
		continue;
	      }
	if (OK != set_zero_one (tree))
	      { printf("\ncan't pack <%s>",infile);
		continue;
	      }
	if (ERROR == (outsecs = encode_file(infile, tempfile,nsectors)))
	      { printf("\nerror encoding <%s>", infile);
		continue;
	      }
	outsecs+=4;	/* this is for the occurrence info */
	printf("\n%d sectors in <%s>;	%d sectors in <%s>",
		nsectors, infile, outsecs, outfile);
	printf("\nPercent savings is %d",
			(100*(nsectors-outsecs))/nsectors);
	if ( outsecs >= nsectors)
	      { printf("\nno savings in packing <%s>",infile);
		unlink (tempfile);
		continue;
	      }
	else if (ERROR != open (outfile,READ))
	      { printf("\n<%s> already exists",outfile);
		printf("\nShould I erase? (Y/N)");
		if ('Y' != toupper (getchar()))		exit();
		unlink (outfile);
	      }
	rename (tempfile, outfile);
	printf("\nencoded form of <%s> is in <%s>", infile, outfile);
      }

}

/********************************************************************
count # of occurenceds of each character in infile, storing values in
tree structure; return total number sectors in infile, or ERROR
********************************************************************/
int count_occur (infile)
char *infile;
/*	struct ELEMENT tree[];	 is global now */
{
char charbuf [NREAD * SECSIZ];
char *string;
int i, fd, k;
/* int c;   c is global */
int nread, nsectors;

k=0;
string=
"zzzz ?snore zzzzzzbuzz click zot foo baz help! forp sniggledorp foobar fripnozzle blod yippeezoo this is a big file WOW\7";

if (ERROR == (fd = open (infile, READ)))
      { printf("\nerror opening <%s> in count_occur", infile);
	return (ERROR);
      }

printf("<%s>\nand now we pause for station identification\nw",infile);
nsectors = 0;
nread = NREAD;
while (nread == NREAD)
      { setmem (charbuf, NREAD*SECSIZ, NUL);
	nread = read(fd, charbuf, NREAD);
	if (nread==ERROR) {printf("error in count char"); return ERROR;}
	printf("%c",string[k++]);
	if (k>= strlen (string))	k=0;
	nsectors+=nread;
	if DEBUG printf("\n\tnsectors=%d, 	nread=%d",
			     nsectors,		nread);
	for (i=0; i < nread*SECSIZ; i++)
	      {	if (tree[charbuf[i]].occur++ >30000)
			printf("oi chi wah wah");
	      }

      }
return (nsectors);
}
/********************************************************************
Generate the tree (backwards) that will be used to encode the file
********************************************************************/
generate_tree ()
/* struct ELEMENT tree[];  is now global */
{
int daddy, zero, one;
int i, val, nextpapa;

daddy = NIL;
nextpapa = NUM_CH_TYPES;
puts("\nnow just be patient, this takes a while....");
while (1)
      { find_lowest ( &one, &zero);
	if (one == NIL && zero == NIL)
		return (daddy);		/* all done; even # char's */
	else if (one == NIL)
		return (zero);		/* all done; odd # chars   */

	if ( !(nextpapa%3))	putchar('z');
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
int find_lowest (low1, low0)
/* struct ELEMENT tree[];   now global */
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

/********************************************************************
 sets tree[].occur at each active element to 1 or 0 depending  on 
 whether its dad says it's a son_one or a son_zero
*********************************************************************/
int set_zero_one ()
/* struct ELEMENT tree[]; now global */
{
int i, daddy;

for (i=0; i<NELEM; i++)
      { if (tree[i].dad == NIL)
		continue;

	daddy = tree[i].dad;
	if (tree[daddy].son_zero == i)
		tree[i].occur = 0;
	else if (tree[daddy].son_one == i)
		tree[i].occur = -1;	/* -1 to be used when encoding */
	else
	      { printf("\nelement #%d = %c does not ackowledge #%d = %c to be its son",
			 daddy, daddy,				i, i);

		return (ERROR);
	      }
      }
return (OK);
}

/****************************************************************
 write out the character occurrence information to tempfile before
 packing the file
*****************************************************************/
int write_occur (tempfile)
char *tempfile;
/* struct ELEMENT tree[];  now global */
{
int fd, *ip;
int i;
char outbuf [4*SECSIZ]; /* info takes up 256*2 bytes = 4 sectors  */

if (ERROR == (fd = creat(tempfile)))
      { printf("\nerror can not create <%s>",tempfile);
	return (ERROR);
      }
ip = outbuf;
for (i=0; i<NUM_CH_TYPES; i++)
	*ip++ = tree[i].occur;
if (4 != write (fd, outbuf, 4))
      { printf("\nerror writing occur. info to <%s>", tempfile);
	return (ERROR);
      }
if (ERROR == close (fd))
      { printf("\nerror closing <%s> after writing occur. info",
                            tempfile);
	return (ERROR);
      }
return (OK);
}
/*******************************************************************
Encodes the file.  A bit code for each char is generated by starting 
at that char's element in the tree and following the pointers from it
up to the "big-daddy" of the tree, picking off the occur fields of
each element passed on the way. This is the bit code, backwards. 
encode_file    expects that the occur field of each element will have 
been cleared to  0  if it is a son_zero, or set to -1  if it is a
son_one.
Returns -1 on error, or the number of sectors in the ouput file
********************************************************************/
int encode_file (infile, tempfile, nsectors)
char *infile, *tempfile;
/* struct ELEMENT tree[]; now global */
int nsectors;
{
char inbuf [SECSIZ*NUMSECS], tbuf [SECSIZ*NUMSECS];
int i, j, k, l;
int pi, pt, fd, tfd;
/* int C;         note-- these two have been made global for speed
int BYTE_DONE;	 TRUE when have just finished another
			   byte of encoded output file */
int sec_finished;	/* TRUE when have just finished another
			   sector of encoded ouput file */
int outsecs;		/* number of sectors in output file */
int secsread;		/* number of sectors actually read */
int done;		/* says whether were almost done */

if (ERROR == (fd = open (infile, READ)))
      { printf("\nencode_file: can't open <%s> for reading", infile);
	return (ERROR);
      }
else if (ERROR == (tfd = open (tempfile, BOTH)))
      { printf("\nencode_file: can't open <%s> for writing",tempfile);
	close (fd);
	return (ERROR);
      }
pt = 0;	/* index into output buffer */
sec_finished = FALSE;
outsecs = 0;
OUTBYTE = 0;	/* "buffer" for each encoded byte to output buffer */
COUNTER = 128;
SP = STACK;
j=0;

seek(tfd,3,0);
read(tfd,tbuf,1);	/* this is so we are poised to write */
tbuf [pt++] = nsectors/256;	/* store length of un-encoded file */
tbuf [pt++] = nsectors%256;	/* store length of un-encoded file */
printf("\nSectors done on <%s>", infile);

done=FALSE;
while (!done && ERROR != (secsread= read (fd, inbuf, NUMSECS))) /* get it in */
      {	if (secsread!=NUMSECS) done=TRUE;
	sec_finished = FALSE;
        for (pi=0; pi<SECSIZ*secsread; pi++)	/* for (all chars in buffer) */
	      { C = inbuf[pi];
		if (!(pi%SECSIZ)) printf("."); /* another sector */
		if DEBUG printf("\n-------------------\n0x%x\n",C);

		/* get the bit pattern code into STACK */
		do
		      { *SP++ = tree[C].occur;
			C = tree[C].dad;
		      } while ( NIL != tree[C].dad );


	/* Stuff the bit code into the output buffer. Each element in
	   the array STACK rep's a bit; one-bits were set to neg. one's
	   previously. COUNTER starts at 128 and is divided by two each
	   time to shift its bit pattern right one place each time;
	   ANDing this with BIT will set or clear the appropriate bit
	   position */

		do
		      { BIT = *--SP;
		/*	if DEBUG printf("%d ",(BIT==-1)?1:0);	*/
			BYTE_DONE = FALSE;
			OUTBYTE |= (BIT & COUNTER);
			COUNTER /=2;
			if (COUNTER == 0)	/* eight bits done */
			      { COUNTER = 128;
				BYTE_DONE = TRUE;
				tbuf [pt++] = OUTBYTE;
				OUTBYTE = 0;
				if (pt == SECSIZ*NUMSECS)
				      { if (NUMSECS != write (tfd, tbuf, NUMSECS))
					      { printf("\nerror writing to <%s> in encode_file",tempfile);
					        close (tfd);
						close (fd);
						return (ERROR);
					      }
					 pt = 0;
					sec_finished = TRUE;
					outsecs+=NUMSECS;
				      }
			      }
		      } while (SP != STACK);
	      }	/* end for ( all chars in input buffer ) */
      }	/* end while ( there are more sectors....) */
if (BYTE_DONE != TRUE)
      { tbuf[pt++] = OUTBYTE;
	sec_finished = FALSE;
      }
if (sec_finished != TRUE)
      { if ((pt/SECSIZ)+1 != write(tfd, tbuf, (pt/SECSIZ)+1))
	printf("\nerr writing to <%s> at end of encode_file",tempfile);
	outsecs+=pt/SECSIZ+1;
      }

if (ERROR == close (tfd))
      { printf("\nencode_file: error closing <%s>",tempfile);
	close (fd);
	return (ERROR);
      }
close (fd);

return (outsecs);
}
("\nencode_file: error closin