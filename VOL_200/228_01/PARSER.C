/*
HEADER:         CUGXXX;
TITLE:          Generalized, finite-state parser;
DATE:           3-20-86;
DESCRIPTION:    Powerful parser allowing extraction of single tokens from
                character strings.  User can specify delimiters/escape
                character.
KEYWORDS:       Generalized finite-state parser, Parser;
FILENAME:       PARSER.C;
WARNINGS:       None;
AUTHORS:        Lloyd Zusman;
COMPILER:       DeSmet C;
REFERENCES:     US-DISK 1308;
ENDREF
*/
#ifdef IGNORE_THIS

/*

NOTE:	lower down in this program description section, i have examples
	of C code with embedded comments.  this "#ifdef" is here because
	without it, the C compiler treats the end-of-comment delimiters
	(asterisk-slash) as significant, and my examples start generating
	code and errors.  the "#ifdef" causes this whole section to be
	ignored, including the end-of-comment delimiters.


	PARSER.C	Lloyd Zusman, Master Byte Software, Trump User Group
			(408) 395-5693 (voice only)

	This program is a generalized, finite state token parser.  It's
	it is the most powerful parser I've seen on any BBS (if I do say
	so myself).  It allows you extract tokens one at a time from a
	string of characters.  The characters used for white space, for
	break characters, and for quotes can be specified.  Also,
	characters in the string can be preceded by a specifiable escape
	character which removes any special meaning the character may have.

	There are a lot of formal parameters in this subroutine call, but
	once you get familiar with them, this routine is fairly easy to use.
	"#define" macros can be used to generate simpler looking calls for
	commonly used applications of this routine.

	First, some terminology:

	token			used here, a single unit of information in
				the form of a group of characters.

	white space		space that gets ignored (except within quotes
				or when escaped), like blanks and tabs.  in
				addition, white space terminates a non-quoted
				token.

	break character 	a character that separates non-quoted tokens.
				commas are a common break character.  the
				usage of break characters to signal the end
				of a token is the same as that of white space,
				except multiple break characters with nothing
				or only white space between generate a null
				token for each two break characters together.

				for example, if blank is set to be the white
				space and comma is set to be the break
				character, the line ...

				A, B, C ,  , DEF

				... consists of 5 tokens:

				1)	"A"
				2)	"B"
				3)	"C"
				4)	""      (the null string)
				5)	"DEF"

	quote character 	a character that, when surrounding a group
				of other characters, causes the group of
				characters to be treated as a single token,
				no matter how many white spaces or break
				characters exist in the group.	also, a
				token always terminates after the closing
				quote.	for example, if ' is the quote
				character, blank is white space, and comma
				is the break character, the following
				string ...

				A, ' B, CD'EF GHI

				... consists of 4 tokens:

				1)	"A"
				2)	" B, CD" (note the blanks & comma)
				3)	"EF"
				4)	"GHI"

				the quote characters themselves do
				not appear in the resultant tokens.  the
				double quotes are delimiters i use here for
				documentation purposes only.

	escape character	a character which itself is ignored but
				which causes the next character to be
				used as is.  ^ and \ are often used as
				escape characters.  an escape in the last
				position of the string gets treated as a
				"normal" (i.e., non-quote, non-white,
				non-break, and non-escape) character.
				for example, assume white space, break
				character, and quote are the same as in the
				above examples, and further, assume that
				^ is the escape character.  then, in the
				string ...

				ABC, ' DEF ^' GH' I ^ J K^ L ^

				... there are 7 tokens:

				1)	"ABC"
				2)	" DEF ' GH"
				3)	"I"
				4)	" "     (a lone blank)
				5)	"J"
				6)	"K L"
				7)	"^"     (passed as is at end of line)


	OK, now that you have this background, here's how to call "parser":

	result=parser(flag,token,maxtok,string,white,break,quote,escape,
		      brkused,next,quoted)

	result: 	0 if we haven't reached EOS (end of string), and
			1 if we have (this is an "int").

	flag:		right now, only the low order 3 bits are used.
			1 => convert non-quoted tokens to upper case
			2 => convert non-quoted tokens to lower case
			0 => do not convert non-quoted tokens
			(this is a "char").

	token:		a character string containing the returned next token
			(this is a "char[]").

	maxtok: 	the maximum size of "token".  characters beyond
			"maxtok" are truncated (this is an "int").

	string: 	the string to be parsed (this is a "char[]").

	white:		a string of the valid white spaces.  example:

			char whitesp[]={" \t"};

			blank and tab will be valid white space (this is
			a "char[]").

	break:		a string of the valid break characters.  example:

			char breakch[]={";,"};

			semicolon and comma will be valid break characters
			(this is a "char[]").

			IMPORTANT:  do not use the name "break" as a C
			variable, as this is a reserved word in C.

	quote:		a string of the valid quote characters.  an example
			would be

			char whitesp[]={"'\"");

			(this causes single and double quotes to be valid)
			note that a token starting with one of these characters
			needs the same quote character to terminate it.

			for example,

			"ABC '

			is unterminated, but

			"DEF" and 'GHI'

			are properly terminated.  note that different quote
			characters can appear on the same line; only for
			a given token do the quote characters have to be
			the same (this is a "char[]").

	escape: 	the escape character (NOT a string ... only one
			allowed).  use zero if none is desired (this is
			a "char").

	brkused:	the break character used to terminate the current
			token.	if the token was quoted, this will be the
			quote used.  if the token is the last one on the
			line, this will be zero (this is a pointer to a
			"char").

	next:		this variable points to the first character of the
			next token.  it gets reset by "parser" as it steps
			through the string.  set it to 0 upon initialization,
			and leave it alone after that.	you can change it
			if you want to jump around in the string or re-parse
			from the beginning, but be careful (this is a
			pointer to an "int").

	quoted: 	set to 1 (true) if the token was quoted and 0 (false)
			if not.  you may need this information (for example:
			in C, a string with quotes around it is a character
			string, while one without is an identifier).

			(this is a pointer to a "char").

	Example 1:

	char *whitesp[]={" \t");        /* blank and tab */
	char *breakch[]={",\r");        /* comma and carriage return */
	char *quotech[]={"'\""};        /* single and double quote */
	char escape='^';                /* "uparrow" is escape */

	main()
	{
	  char *fgets(),line[81],brkused,quoted,token[81];
	  int i,next;

	  while(fgets(line,80,stdin)!=NULL)	/* get line */
	  {

	    printf("Line: %s",line);            /* already has <CR> */
	    i=0;

	    next=0;				/* make sure you do this */

	    while(parser(2,token,80,line,whitesp,breakch,quotech,escape,
			 &brkused,&next,&quoted)==0)
	    {
	      printf(" Token %d = (%s)\n",++i,token);

	      if(brkchar=='\r') /* <CR> is a break so it won't be included  */
		break;		/* in the token.  treat as end-of-line here */
	    }
	  }
	}



	In the above example, lines are read from stdin and broken up into
	tokens.  All non-quoted tokens are converted to lower case.  Since
	fgets() returns the final carriage return, we treat it as a break
	character to keep it out of the returned token.  Also, since the only
	way "parser" will return a non-zero error code is at end of line,
	we test "brkchar" to see if we've gotten to the final carriage
	return, and we explicitly break out of the inner loop if we've
	hit it.  Note that since fgets() puts the final <CR> right before
	the end-of-string, if we left out the "if(brkchar='\r')" test,
	we'd get one extra null token (just as if the line ended with a
	single comma).	Run this example to see how it all works.

	Example 2:

		.
		.
		.

	next=0;
	result=parser(1, newstr, 80, str, "", "", "", 0, &brkused, &next,
		      &quoted);
		.
		.
		.

	this call takes whatever is in "str" and converts it to upper case,
	putting the result in "newstr".

	*** end of examples ***

	in case you're interested, "parser.c" was inspired by a system
	subroutine that comes as part of the PRIMOS operating system for
	the Prime Computer:  "gt$par.plp".  i loosely patterned this routine
	after the Prime routine.

	Revisions:

	09/30/84	Lloyd Zusman	Initial coding


NOTE:	This program was developed to run on the "Trump Card" (a Z8000
	CPU on a board specially adapted for plugging into an IBM PC).
	The C compiler I used is fairly standard, and I assume this
	program will compile pretty much "as is" on either the Lattice
	C compiler or the Computer Innovations C compiler.  Any
	incompatibilities between the Trump C compiler and these other
	compilers should be minimal.

	Since the C compiler I used generates 32-bit pointers, you may
	feel the urge to use the large model for Lattice or Computer
	Innovations.  However, I don't think I do anything in this
	program that depends on the pointers being any given size, so
	you should be able to compile it using any "size" model you see
	fit.

	Incidentally, this "Trump Card" I mentioned is something I'm
	really happy with.  It has a 10 MHz Z8000 and up to 512K of
	memory.  The memory is standard "4164"-type dynamic RAM, and
	therefore the computer doesn't quite run at 10 MHz all the
	time.  The fastest "4164" RAM I've been able to find is 120
	nanosecond memory, but I'm pushing my "Trump Card" up to
	9 MHz and it works just fine with these chips.

	There is a simple operating system that comes with this "Trump
	Card", and you can go back and forth between it and your PC.
	As of now (mid October, 1984), the Trump Card's operating system
	doesn't quite support concurrent processes going simultaneously
	on buth CPU's, but that capability should come out fairly soon.

	Other nice things that come with this card are:

		A Z80 emulator that lets you run CPM programs.

		A C compiler (mentioned above).  A 600+ line C program
		that took 6.5 minutes to compile and link on the PC
		(Computer Innovations C compiler, IBM PC standard
		linker) took exactly 22 seconds to compile and link
		on the Trump Card.  You could test and develop a C program
		on the Trump Card, taking advantage of the speed, and then
		move it over to the PC for one last re-compile when
		you're ready to put it into production.  So far, the
		only differences I could find between the Trump's C
		compiler and Computer Innovations are (1) slight
		differences in the format string in "printf" (these
		normally won't even show up); (2) "register" data
		types really get assigned to registers on the Trump
		C compiler; (3) in-line machine language is allowed
		in the Trump C compiler.

		A BASIC system that will run IBM's Basic programs
		with very few changes, but quite a bit faster.

		A compiler for a language called "Y".  This is
		a "multi-level language", which at it's lowest
		level is just a Z8000 assembler, at the next highest
		level lets you use C-like and Pascal-like constructs
		in your assembly code, and at it's highest level is
		a Meta-Compiler (like YACC, for example) which lets
		you use Backus-Naur grammer constructs to define your
		own high level languages.

		Software to make all or part of Trump memory look
		like a ramdisk and/or a print spooler for your PC.

		Function calls (analogous to interrupt 021h in MS-DOS)
		which let you do lots of things from applications
		programs on the Trump card, including the ability to
		initiate interrupts on the 8088.

		Other various and sundry things like an editor, a linker,
		etc.

	Using this Trump Card has really brought home to me how far away
	from "state of the art" these IBM PC's (and XT's and even AT's)
	are.  IBM used slow, substandard technology in it's PC's, XT's,
	and even AT's.  The 6.5-minute C compile and link that took 22
	seconds on the Trump Card illustrates this point.  If you assume
	that the Computer Innovations C compiler I used on the PC is twice
	as slow as it could be (a fairly safe assumption:  Lattice runs
	quite a bit faster), factoring that in still leaves an approximately
	8-to-1 time differential that can only be accounted for by hardware
	inefficiencies in the PC (the badly designed Intel 8088 chip, the
	slow 4.7 MHz clock rate, etc.).  We can't even attribute this slowness
	to the hard disk IBM uses (there are cheap, reliable, commercially
	available internal 10 MB hard disks that are 3 times faster than
	the ones IBM uses) nor to its bus, since the Trump Card uses the
	same disk and the same bus.  Nor can we attribute it to the fact
	that the Trump Card has 512 K of memory, since I have an AST
	"Six Pack Plus" on my PC which has 384 K on it, so my total memory
	on the PC side of things is 640 K.  If you have one of those new
	AT's, you're still only running 3 times faster than the PC (this is
	IBM's claim ... I'll bet you that in most real applications, it's not
	even that fast), so the Trump card is still more than twice as fast
	as the AT can ever hope to be.	What this all boils down to is that
	it's necessary to soup up these PC's in order to make them capable
	of doing anything useful in a reasonable period of time.  The Trump
	Card is one of probably many good ways to turn your IBM PC into
	a real computer.

	There was a writeup on the Trump Card in two consecutive recent
	"Byte" issues (May and June, 1984, I believe).  If you're interested
	in more information, you can contact me at the phone number above,
	or you can dial into the official Trump BBS at (408) 923-5565 and
	look around or leave a message for the Sysop.

	-Lloyd Zusman, Master Byte Software


#endif		/* goes with initial "#ifdef" */

/* states */

#define IN_WHITE 0
#define IN_TOKEN 1
#define IN_QUOTE 2
#define IN_OZONE 3

int _p_state;	   /* current state	 */
unsigned _p_flag;  /* option flag	 */
char _p_curquote;  /* current quote char */
int _p_tokpos;	   /* current token pos  */

/* routine to find character in string ... used only by "parser" */

sindex(ch,string)
char ch,*string;

{
  char *cp;
  for(cp=string;*cp;++cp)
    if(ch==*cp)
      return (int)(cp-string);	/* return postion of character */
  return -1;			/* eol ... no match found */
}

/* routine to store a character in a string ... used only by "parser" */

chstore(string,max,ch)
char *string,ch;
int max;

{
  char c;
  if(_p_tokpos>=0&&_p_tokpos<max-1)
  {
    if(_p_state==IN_QUOTE)
      c=ch;
    else
      switch(_p_flag&3)
      {
	case 1: 	    /* convert to upper */
	  c=toupper(ch);
	  break;

	case 2: 	    /* convert to lower */
	  c=tolower(ch);
	  break;

	default:	    /* use as is */
	  c=ch;
	  break;
      }
    string[_p_tokpos++]=c;
  }
  return;
}

/* here it is! */

parser(inflag,token,tokmax,line,white,brkchar,quote,eschar,brkused,next,quoted)
unsigned inflag;
char *token,*line,*white,*brkchar,*quote,*brkused,*quoted,eschar;
int tokmax,*next;

{
  int qp;
  char c,nc;

  *brkused=0;		/* initialize to null */
  *quoted=0;		/* assume not quoted  */

  if(!line[*next])	/* if we're at end of line, indicate such */
    return 1;

  _p_state=IN_WHITE;	   /* initialize state */
  _p_curquote=0;	   /* initialize previous quote char */
  _p_flag=inflag;	   /* set option flag */

  for(_p_tokpos=0;c=line[*next];++(*next))	/* main loop */
  {
    if((qp=sindex(c,brkchar))>=0)  /* break */
    {
      switch(_p_state)
      {
	case IN_WHITE:		/* these are the same here ...	*/
	case IN_TOKEN:		/* ... just get out		*/
	case IN_OZONE:		/* ditto			*/
	  ++(*next);
	  *brkused=brkchar[qp];
	  goto byebye;

	case IN_QUOTE:		 /* just keep going */
	  chstore(token,tokmax,c);
	  break;
      }
    }
    else if((qp=sindex(c,quote))>=0)  /* quote */
    {
      switch(_p_state)
      {
	case IN_WHITE:	 /* these are identical, */
	  _p_state=IN_QUOTE;	    /* change states   */
	  _p_curquote=quote[qp];	 /* save quote char */
	  *quoted=1;	/* set to true as long as something is in quotes */
	  break;

	case IN_QUOTE:
	  if(quote[qp]==_p_curquote)	/* same as the beginning quote? */
	  {
	    _p_state=IN_OZONE;
	    _p_curquote=0;
	  }
	  else
	    chstore(token,tokmax,c);	/* treat as regular char */
	  break;

	case IN_TOKEN:
	case IN_OZONE:
	  *brkused=c;			/* uses quote as break char */
	  goto byebye;
      }
    }
    else if((qp=sindex(c,white))>=0)	   /* white */
    {
      switch(_p_state)
      {
	case IN_WHITE:
	case IN_OZONE:
	  break;		/* keep going */

	case IN_TOKEN:
	  _p_state=IN_OZONE;
	  break;

	case IN_QUOTE:
	  chstore(token,tokmax,c);     /* it's valid here */
	  break;
      }
    }
    else if(c==eschar)			/* escape */
    {
      nc=line[(*next)+1];
      if(nc==0) 		/* end of line */
      {
	*brkused=0;
	chstore(token,tokmax,c);
	++(*next);
	goto byebye;
      }
      switch(_p_state)
      {
	case IN_WHITE:
	  --(*next);
	  _p_state=IN_TOKEN;
	  break;

	case IN_TOKEN:
	case IN_QUOTE:
	  ++(*next);
	  chstore(token,tokmax,nc);
	  break;

	case IN_OZONE:
	  goto byebye;
      }
    }
    else	/* anything else is just a real character */
    {
      switch(_p_state)
      {
	case IN_WHITE:
	  _p_state=IN_TOKEN;	    /* switch states */

	case IN_TOKEN:		 /* these 2 are     */
	case IN_QUOTE:		 /*  identical here */
	  chstore(token,tokmax,c);
	  break;

	case IN_OZONE:
	  goto byebye;
      }
    }
  }		/* end of main loop */

byebye:
  token[_p_tokpos]=0;	/* make sure token ends with EOS */

  return 0;

}

