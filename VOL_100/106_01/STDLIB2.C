/*	---------------------------------------------------------
	Mods by H Moran:

	In case you wish to adopt some but not all of the mods:
	Lines modified by HRM marked with comment 'HRM mod'
	Lines added by HRM marked with comment 'HRM'


	1) added functions: for list reader and punch devices

		lprintf pprintf		-- list and punch devices
		rscanf			-- reader device
		rgets			-- reader device
		lputs   pputs		-- list and punch devices
		lputc   pputc		-- list and punch devices
		rgetc			-- reader device

	2) sprintf() modified to use '0' for left fill char
	   when radix is not decimal.

	3) binary field spec added to sprintf() and sscanf()
	   spec used is %b

	4) pad char for non-decimal numeric fields in sprintf()
	   changed to '0'

	5) printing of a..f changed to A..F in _uspr()

	6) fgets() fixed to remember reception of 0x1a
	   and handle it as EOF. See the one caveat in
	   the modified comments.
	-------------------------------------------------------
*/

/*
	This file contains the source for the following
	library functions:

	printf **	fprintf	**	sprintf *
	scanf		fscanf		sscanf
	gets		fgets
	puts		fputs
	swapin ***

	*) rewritten for 1.31
	**) work differently since they use the new "sprintf"
	***) brand new for 1.31

	An alternate version of "sprintf" is given in the file
	FLOAT.C for use with floating point numbers; see FLOAT.C
	for details. Note that "sprintf" is used by "printf", so
	this really amounts to an alternate version of "printf",
	too.

	Note that all the upper-level formatted I/O functions
	("printf", "fprintf", "scanf", and "fscanf") now use
	"sprintf" and "sscanf" for doing conversions. While
	this leads to very structured source code, it also
	means that calls to "scanf" and "fscanf" must process
	ALL the information on a line of text; if the format
	string runs out and there is still text left in the
	line being processed, the text will be lost (i.e., the
	NEXT scanf or fscanf call will NOT find it.)

	Also note that temporary work space is declared within
	each of the high-level functions as a one-dimensional
	character array. The length limit on this array is
	presently set to 132 by the #define MAXLINE statement;
	if you intend to create longer lines through printf,
	fprintf, scanf, or fscanf calls, be SURE to raise this
	limit by changing the #define statement.

	Some misc. comments on hacking text files with CP/M:
	The conventional CP/M text format calls for each
	line to be terminated by a CR-LF combination. In the
	world of C programming, though, we like to just use
	a single LF (also called a newline) to terminate
	lines. AND SO, the functions which deal with reading
	and writing text from disk files to memory and vice-
	versa ("fgets", "fputs") take special pains to convert
	CR-LF combinations into single '\n' characters when
	reading from disk ("fgets"), and convert '\n' char-
	acters to CR-LF combinations when writing TO disk
	("fputs"). This allows the C programmer to do things
	in style, dealing only with a single line terminator,
	while maintaining compatibility with the CP/M text
	format (so that, for example, a text file can be
	"type"d under the CCP.)
	To confuse matters further, the "gets" function
	(which simply buffers up a line of console input)
	terminates a line with '\0' (a zero byte) instead
	of CR or LF. Thus, if you wanted to read in lines
	of input from the console and write them to a file,
	you'd have to manually put out the CR and LF at the
	end of every line. Oh, and don't forget the 0x1a
	(control-Z) at the end of the file! Hey, kiddies,
	isn't CP/M fun to work with???
	Also, watch out when reading in text files using
	"getc". While a text file is USUALLY terminated
	with a control-Z, it MAY NOT BE if the file ends
	on an even sector boundary. This means that there
	are two possible return values from "getc" which
	signal and EOF: 0x1a (control-Z), and -1 (or 255
	if you assign it to a char variable.)
*/


#define RAM 0		/* start of CP/M RAM area */
#define MAXLINE 132	/* maximum length of text line */
#define EOF -1		/* End of file val returned by getc */
#define NULL 0		/* Returned by fgets on EOF */

char toupper(), isdigit();

struct buf {
	int fd;
	int nleft;
	char *nextp;
	char buff[128];
 };

/*
	printf

	usage:
		printf(format, arg1, arg2, ...);
	
	Note that since the sprintf function is used to
	form the output string and then puts is used to
	actually print it out, care must be taken to 
	avoid generating null (zero) bytes in the output,
	since such a byte will terminate printing of the
	string by puts. Thus, a statment such as:
		printf("%c foo",'\0');
	would print nothing at all.
	See the "sprintf" documentation, below, for more info.
*/

printf(format)
char *format;
{
	char line[MAXLINE];
	_mvl();
	sprintf(line,format);
	puts(line);
}


/*
	fprintf:
	Like printf, except that the first argument is
	a pointer to a buffered I/O buffer, and the text
	is written to the file described by the buffer:
	(-1 returned on error)
	usage:
		fprintf(iobuf, format, arg1, arg2, ...);
*/

fprintf(iobuf,format)
char *format;
struct buf *iobuf;
{
	char text[MAXLINE];
	sprintf(text);
	return fputs(text,iobuf);
}


/*
	fscanf:
	Like scanf, except that the first argument is
	a pointer to a buffered input file buffer, and
	the text is taken from the file instead of from
	the console.
	Usage:
		fscanf(iobuf, format, ptr1, ptr2, ...);
	Returns number of items matched (zero on EOF.)
	Note that any unprocessed text is lost forever. Each
	time scanf is called, a new line of input is gotten
	from the file, and any information left over from
	the last call is wiped out. Thus, the text in the
	file must be arranged such that a single call to
	fscanf will always get all the required data on a
	line.
*/

int fscanf(iobuf,format,arg1)
char *format;
struct buf *iobuf;
{
	char text[MAXLINE];
	if (!fgets(text,iobuf)) return 0;
	return sscanf(text,format,arg1);
}


/*
	scanf:
	This one accepts a line of input text from the
	console, and converts the text to the required
	binary or alphanumeric form (see Kernighan &
	Ritchie for a more thorough description):
	Usage:
		scanf(format, ptr1, ptr2, ...);
	Returns number of items matched.
	Since a new line of text must be entered from the
	console each time scanf is called, any unprocessed
	text left over from the last call is lost forever.
	This is a difference between BDS scanf and UNIX
	scanf. Another is that the field width specification
	is not supported here.
*/

scanf(format)
char *format;
{
	char line[MAXLINE];
	_mvl();
	gets(line);
	sscanf(line,format);
}


/*
	Internal function to move all function arguments
	over one place to the right, to make room for
	a new argument in the first position. This is
	necessary so that, for example, "sprintf" can
	be called from within "printf" without clobbering
	one of the arguments. This is NOT a portable
	feature of BDS C, and in fact represents one of
	the biggest kludges in the package. Oh well; live
	and learn.
*/

_mvl()
{
	int count, *ptr;
	ptr = (RAM + 0x3f7 + 0x2e);
	count = 23;
	while (count--) *ptr = *--ptr;
}


/*
	sprintf:
	Like fprintf, except a string pointer is specified
	instead of a buffer pointer. The text is written
	to where the string pointer points.
	Usage:
		sprintf(string,format,arg1, arg2, ...);

	This is my latest version of the "sprintf" standard library
	routine. This time, folks, it REALLY IS standard. I've
	tried to make it EXACTLY the same as the version presented
	in Kernighan & Ritchie: right-justification of fields is
	now the default instead of left-justification (you can GET
	left-justification by using a dash in the conversion, as
	specified in the book); the "%s" conversion can take a precision
	now as well as a field width; the "e" and "f" conversions, for
	floating point numbers, are supported in a special version of
	"sprintf" given in source form in the FLOAT.C file. If you do
	a lot of number crunching and wish to have that version be the
	default (it eats up a K or two more than this version), just
	replace this version of sprintf in DEFF.CRL with the one in FLOAT.C,
	using the CLIB program, or else be stuck with always typing in
	"float" on the clink command line...
*/


sprintf(line,format)
char *line, *format;
{
	char _uspr(), c, base, *sptr;
	char wbuf[80], *wptr, pf, ljflag;
	int width, precision,  *args;
	char lpadchr;	/* HRM */

	args = (RAM + 0x3fb);
	while (c = *format++)
	  if (c == '%') {
	    wptr = wbuf;
	    precision = 6;
	    ljflag = pf = 0;

	    if (*format == '-') {
		    format++;
		    ljflag++;
	     }

	    if ( !(width = _gv2(&format))) width++;

	    if ((c = *format++) == '.') {
		    precision = _gv2(&format);
		    pf++;
		    c = *format++;
	     }

	    lpadchr = '0';/* HRM pad char for non-decimal numeric fields */
	    switch(toupper(c)) {

		case 'D':	lpadchr = ' ';
				if (*args < 0) {
				*wptr++ = '-';
				*args = -*args;
				width--;
			    }

		case 'U':  base = 10; goto val;

		case 'B':  base = 2;  goto val;	/* HRM */

		case 'X':  base = 16; goto val;

		case 'O':  base = 8;

		     val:  width -= _uspr(&wptr,*args++,base);
			   goto pad;

		case 'C':  lpadchr = ' ';	/* HRM */
			   *wptr++ = *args++;
			   width--;
			   goto pad;

		case 'S': lpadchr = ' ';	/* HRM */
			  if (!pf) precision = 200;
			   sptr = *args++;
			   while (*sptr && precision) {
				*wptr++ = *sptr++;
				precision--;
				width--;
			    }

		     pad:  *wptr = '\0';
		     pad2: wptr = wbuf;
			   if (!ljflag)
				while (width-- > 0)
					*line++ = lpadchr;	/* HRM mod */

			   while (*line = *wptr++)
				line++;

			   if (ljflag)
				while (width-- > 0)
					*line++ = ' ';
			   break;

		 default:  *line++ = c;

	     }
	  }
	  else *line++ = c;

	*line = '\0';
}

/*
	Internal routine used by "sprintf" to perform ascii-
	to-decimal conversion and update an associated pointer:
*/

int _gv2(sptr)
char **sptr;
{
	int n;
	n = 0;
	while (isdigit(**sptr)) n = 10 * n + *(*sptr)++ - '0';
	return n;
}


/*
	Internal function which converts n into an ASCII
	base `base' representation and places the text
	at the location pointed to by the pointer pointed
	to by `string'. Yes, you read that correctly.
*/

char _uspr(string, n, base)
char **string;
unsigned n;
{
	char length;
	if (n<base) {
		*(*string)++ = (n < 10) ? n + '0' : n+('A'-10);/* HRM mod */
		return 1;
	}
	length = _uspr(string, n/base, base);
	_uspr(string, n%base, base);
	return length + 1;
}


/*
	gets:
	Accept an input line from the console, and
	store it at the location pointed to by the
	character pointer argument.
	The newline is NOT included as part of the
	returned text; i.e., typing "abc" and hitting
	carriage-return will result in the string:
	"abc\0"
*/

gets(s)
char *s;
{
	char c, *temp;
	int count, i;
	temp = s;
start:	count = 0;
	s = temp;
	while ((c=getchar()) != '\n') 
	switch(c) {
	 case 0x15:
		putchar('\n');
		goto start;
	 case 0x7f:
		if (!count) break;
		--count;
		--s;
		putchar(0x08); putchar(' '); putchar(0x08);
		break;
	 case 0x12:
		putchar('\n');
		for (i=0; i<count; ++i) putchar(temp[i]);
		break;
	 default:
		*s++ = c;
		++count;
	}
	*s = 0;
	return temp;
}


/*
	puts:
	Write out the given string to the console:
*/

puts(s)
char *s;
{
	while (*s) putchar(*s++);
}


/*
	fgets:
	This next function is like "gets", except that
	the line is taken from a buffered input file instead
	of from the console. This one is a little tricky
	due to the CP/M convention of having a carriage-
	return AND a linefeed character at the end of every
	text line. In order to make text easier to deal with
	under C programs, this function (fgets) automatically
	strips off the CR from any CR-LF combinations that
	come in from the file. Any CR characters not im-
	mediately followed by LF are left intact. The LF
	is included as part of the string, and is followed
	by a null byte.
	Note that there is no limit to how long a line
	can be here; care should be taken to make sure the
	string pointer passed to fgets points to an area
	large enough to accept any possible line length
	(a line must be terminated by a newline (LF, or '\n')
	character before it is considered complete.)

	comment below modified by HRM

	The value NULL (defined to be 0 here) is returned
	on EOF. If 0x1a is encountered, the string is considered
	terminated and the 0x1a is pushed bak using unget().
	Note that although unget() will return an error of NULL
	this routine does not bother to check because that can
	happen only with multiple pushbacks and this routine
	will never push back more than a single 0x1a. The next
	invocation of getc() will retrieve the 0x1a with as the
	first char of the string (which is recognized as EOF)
	This means you must not interleave calls to getc() and
	fgets() unless you explicitly handle the case of a 0x1a
	received from getc().

*/

char *fgets(s,iobuf)
char *s;
struct buf *iobuf;
{
	int count, c;
	char *cptr;
	count = (MAXLINE + 1);
	cptr = s;
	while (count-- && (c = getc(iobuf)) != EOF && (c != 0x1a))
		if ((*cptr++ = c) == '\n') {
		  if (cptr>s+1 && *(cptr-2) == '\r') {
			*(cptr-2) = '\n';
			cptr--;
		   }
		  break;
		}
	*cptr = '\0';
	if ( (c == 0x1a) ) {	/* HRM */
	  if ( cptr == s )	/* HRM	 left over from	last call */
	    return NULL;	/* HRM */
	  ungetc(iobuf);	/* HRM */ 
	  return s;		/* HRM */
	}			/* HRM */
	return ( (( c==EOF) && (cptr == s)) ? NULL : s );
}



/*
	fputs:
	This function writes a string out to a buffered
	output file. The '\n' character is expanded into
	a CR-LF combination, in keeping with the CP/M
	convention.
	If a null ('\0') byte is encountered before a
	newline is encountered, then there will be NO
	automatic termination character appended to the
	line.
	-1 returned on error.
*/

fputs(s,iobuf)
char *s;
struct buf *iobuf;
{
	char c;
	while (c = *s++) {
		if (c == '\n') putc('\r',iobuf);
		if (putc(c,iobuf) == -1) return -1;
	}
	return 0;
}

/*
	sscanf:
	This is the formatted input scanner, taking a
	text string, a format string and a list of pointer
	arguments, appropriate data is picked up from the
	text string and stored where the pointer arguments
	point according to the format string. See K&R.
	Note that the field width specification is not
	supported by this version.
*/

sscanf(line,format)
char *line, *format;
{
	char sf, c, base, n, *sptr;
	int sign, val, **args;
	args = (RAM + 0x3fb);
	n = 0;
	while (c = *format++) {
	   if (isspace(c)) continue;
	   if (c != '%') {
		if (c != _igs(&line)) return n;
		else line++;
	    }
	   else {
		sign = 1;
		base = 10;
		sf = 0;
		if ((c = *format++) == '*') {
			sf++;
			c = *format++;
		 }
		switch (toupper(c)) {
		   case 'X': base = 16;
			     goto doval;

		   case 'O': base = 8;
			     goto doval;

		   case 'B': base = 2;		/* HRM */
			     goto doval;	/* HRM */

		   case 'D': if (_igs(&line) == '-') {
				sign = -1;
				line++;
			      }
	   doval:  case 'U': val = 0;
			     if (_bc(_igs(&line),base)== -1)
				return n;
			     while ((c=_bc(*line++,base))!=255)
				val = val * base + c;
			     line--;
			     break;
		   case 'S': _igs(&line);
			     sptr = *args;
			     while ((c=*line++)&& !isspace(c))
				if (!sf) *sptr++ = c;
			     if (!sf) {
				n++;
				*sptr = '\0';
				args++;
				continue;
			      }
		   case 'C': if (!sf) {
				poke(*args++, *line);
				n++;
			     }
			     line++;
			     continue;
		   default:  return n;
		 }
		if (!sf) {
			**args++ = val * sign;
			n++;
		 }
	}}
	return n;
}

/*
	Internal function to position the character
	pointer argument to the next non white-space
	character in the string:
*/

_igs(sptr)
char **sptr;
{
	char c;
	while (isspace(c = **sptr)) ++*sptr;
	return (c);
}


/*
	Internal function to convert character c to value
	in base b , or return -1 if illegal character for that
	base:
*/

_bc(c,b)
char c,b;
{
	if ((c = toupper(c)) > '9') c -= 55;
			     else   c -= 0x30;
	return (c > b-1) ? -1 : c;
}


/*
	swapin:
	This is the swapping routine, to be used by the root
	segment to swap in a code segment in the area of memory
	between the end of the root segment and the start of the
	external data area. See the document "SWAPPING.DOC" for
	detailed info on the swapping scheme.

	Returns -1 on error, 0 if segment swapped in OK.

	This version does not check to make sure that the code
	yanked in doesn't overlap into the extenal data area (in
	the interests of keeping the function short.) But, if you'd
	like swapin to check for such problems, note that memory 
	locations ram+115h and ram+116h contain the 16-bit address
	of the base of the external data area (low order byte first,
	as usual.) By rewriting swapin to read in one sector at a time
	and check the addresses, accidental overlap into the data area
	can be avoided.
*/

swapin(name,addr)
char *name;		/* the file to swap in */
{
	int fd;
	if (( fd = open(name,0)) == -1) {
		printf("Swapin: cannot open %s\n",name);
		return -1;
	}
	if ((read(fd,addr,9999)) < 0) {
		printf("Swapin: read error on %s\n",name);
		return -1;
	}
	return 0;
}

/* HRM -------- all lines below added by H Moran -------- */




/*
	formatted output to list device

	usage:
		lprintf(format, arg1, arg2, ...);
*/

lprintf(format)		/* formatted print to list device */
	char *format;
	{
	char line[MAXLINE];

	_mvl();
	sprintf(line,format);
	lputs(line);
	}





/*
	formatted print to punch device

	usage:
		fprintf(iobuf, format, arg1, arg2, ...);
*/

pprintf(format)
	char *format;
	{
	char text[MAXLINE];
	sprintf(text);
	pputs(text);
	}


/*
	formatted scan of reader device

	Usage:
		rscanf(format, ptr1, ptr2, ...);
*/

rscanf(format,arg1)
	char *format;
	{
	char text[MAXLINE];
	if ( ! rgets(text) )
	  return 0;
	return sscanf(text,format,arg1);
	}



/*
	Accept an input line from the reader device, and
	store it at the location pointed to by the
	character pointer argument.
	The newline is NOT included as part of the
	returned text; i.e., receiving "abc" and carriage-return
	will result in the string:	"abc\0"
*/

rgets(s)
	char *s;
	{
	char c,*temp;
	int count, i;
	char rgetc();

	temp = s;
	for( count=0; (c=rgetc()) != '\n'; count++ )
	  *s++ = c;
	*s='\0';
	return temp;
	}


/*
	Write out the given string to the list device
*/

lputs(s)
	char *s;
	{
	while ( *s )
	  lputc(*s++);
	}




/*
	Write out the given string to the punch device
*/

pputs(s)
	char *s;
	{
	while ( *s )
	  pputc(*s++);
	}




/*
	read a char from the reader device
*/

rgetc()
	{
	return bios(6,0) & 255;
	}



/*
	send a char to the punch device
*/

pputc(c)
	char c;
	{
	return bios(5,c) & 255;
	}



/*
	send a char to the list device
*/

lputc(c)
	char c;
	{
	return bios(4,c) & 255;
	}


