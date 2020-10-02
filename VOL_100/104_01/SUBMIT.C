/********************************************************/
/*							*/
/*	copyright(c) 1981 By Mike Bernson		*/
/*							*/
/*	submit command for cpm 2.x			*/
/*	read input file and convert to $$$.sub with	*/
/*	$1-$9 to pram from comand line and convert	*/
/*	^a to control character				*/
/*							*/
/********************************************************/

#define ERROR	-1		/* error return value */
#define LF	0x0a		/* value used for line feed */
#define CR	0x0d		/* value used for return */
#define ENDFILE	0x1a		/* value used for end of file */

char infcb[36];			/* pointer to input fcb */
char outfcb[36];		/* output fcb */

char *prt;			/* just a spare pointer */
int  inpoff;			/* input buffer offset */
int  outoff;			/* output buffer offset */

int  line;			/* current line number */

char inbuff[128];		/* input buffer */
char outbuff[17000];		/* output buffer */

/********************************************************/
/*							*/
/*	main						*/
/*							*/
/*	function:	open input and output data file */
/*			also process data and output	*/
/*			data to $$$.sub and set loc 8 ff*/
/*							*/
/*	Date written:	Jan 28, 1981 by Mike Bernson	*/
/*							*/
/********************************************************/
main(argc,argv)
int argc;		/* number of arg on command line +1 */
int argv[];		/* pointer to each arg on command line */
{
	char filename[132];	/* hold input file name */
	
	strcpy(filename,argv[1]);
	strcat(filename,".sub");

	if (open(infcb,filename) == ERROR) {
		infcb[0]=1;		/* try drive a */
		if (bdos(15,infcb) == 0xff) {
			puts("Input file not found");
			exit();
			}
		}
	for(line=argc; line<12; argv[line++]="");
	inpoff=128;
	line=1;

	/* process input file and write output */
	while(1) {
		doinput(argv+1);
		dooutput();
		}
	}


/********************************************************/
/*							*/
/*	doinput						*/
/*							*/
/*	function:	to proccess input buffer and	*/
/*			change $1-$9 to text needed and	*/
/*			convert '^'a to control char	*/
/*							*/
/*	date written	Jan 28, 1981 By Mike Bernson	*/
/*							*/
/********************************************************/
doinput(parm)
int parm[];
{
	int current;   /* current character working on */

	outoff=1;	/* set output offset to zero */

	/* pick up 1 line of input */
	while(outoff<120) {

		/* get charcter and see what need to be done */
		switch(current=getbuff()) {

		/* end of file  */
		case ENDFILE :
			done();
	
		/* '$' can be $1-$9 or $$ */
		case '$' :
			if ((current=getbuff())=='$') {
				putbuff('$');
				break;
				}
			if (current>='0' && current<='9') {
				prt=parm[current-'0'];
				while(*prt) putbuff(*prt++);
				break;
				}
			error("Invalid parmeter");
			break;

		/* check for control chraracter */
		case '^' :
			current=getbuff();
			if (current>='A' && current<='Z')
				putbuff(current-'@');
				else error("Invalid control character ");
			break;
		
		/* check for end of line */
		case CR :
			return;
	
		/* line	feed ingore */
		case LF :
			break;
		
		/* must be a viald cjaracter put in output buffer */
		default:
			putbuff(current);
		}}
	}

/********************************************************/
/*							*/
/*	dooutput					*/
/*							*/
/*	function:	to write output record to disk	*/
/*			put character count at offset 0 */
/*							*/
/*	date written:	dec 16, 1980 By Mike Bernson	*/
/*							*/
/********************************************************/
dooutput()
{
	outbuff[(line-1)*128]=outoff-1; /* character count */

	/* zero buffer till end */
	while(outoff<128) putbuff(0);
	if (++line>148)	error("Too many	lines");
	}

/********************************************************/
/*							*/
/*	done						*/
/*							*/
/*	function:	to write output buffer out	*/
/*			in backward record format	*/
/*							*/
/*	Date written:	Dec 16, 1980 By Mike Bernson	*/
/*							*/
/********************************************************/
done()
{
	char temp;

	/* create output file on disk a	*/
	if (open(outfcb,"A:$$$.SUB") != ERROR) 
		outfcb[32]=outfcb[15];
	else if (create(outfcb,"A:$$$.SUB") == ERROR) {
		puts("Output File not created");
		exit();
		}
	while(--line) {
		if (write(outfcb,outbuff+128*(line-1),1) == ERROR) {
			error("Disk is Full");
			exit();
			}
		}
	close(outfcb);
	prt=8;
	*prt=255;
	exit();
	}


/********************************************************/
/*							*/
/*	getbuff						*/
/*							*/
/*	function:	to getn next character from 	*/
/*			from input buffer and convert	*/
/*			to upper case			*/
/*							*/
/*	date written:	Dec 16, 1980 by Mike Bernson	*/
/*							*/
/********************************************************/
getbuff()
{
	if (inpoff == 128) {
		if (read(infcb,inbuff,1) == ERROR) return ENDFILE;
		inpoff=0;
		}
	return toupper(inbuff[inpoff++]);
	}

/********************************************************/
/*							*/
/*	putbuff						*/
/*							*/
/*	function:	to write chracter to output 	*/
/*			buff and inc offset		*/
/*							*/
/*	input:		character to write		*/
/*							*/
/*	date written:	Dec 16, 1980 By Mike Bernson	*/
/*							*/
/********************************************************/
putbuff(data)
char data;
{
	outbuff[(line-1)*128+outoff++]=data;
	}


/********************************************************/
/*							*/
/*	error						*/
/*							*/
/*	function:	to print error message follow	*/
/*			by at line (line number)	*/
/*							*/
/*	date written:	dec 16, 1980 By Mike Bernson	*/
/*							*/
/********************************************************/
error(msg)
char *msg;
{
	puts(msg);		/* print error message */
	puts(" at line ");	/* print "at line" */
	outdec(line);		/* print line number */
	exit() ;		/* end back to cpm */
	}

/********************************************************/
/*							*/
/*	outdec						*/
/*							*/
/*	function:	to output number in dec from	*/
/*			to console			*/
/*							*/
/*	date written:	Dec 16, 1980 By Mike Bernson	*/
/*							*/
/********************************************************/
outdec(number)
int number;
{
	char zero,num;
	int  place;

	place=10000;
	zero=0;

	while(place>0) {
		num=number/place+'0'; /* get current digit */
		if (num != '0' || place ==1 || zero) {
			zero=1; /* set zero suppress */
			putch(num); /* print character */
			}
		number=number % place;
		place=place/10;
		}
	}
place=place/10;
		}
	}
10;
		}
	}
;
		place=place/10;