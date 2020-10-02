/*
	'mouse' interperter

	Adapted from a Pascal program in Byte (July 79)

	*** Macro expansions do not work properly ***

	This was done less as something useful in its
	own right than to get a clue how easily Pascal
	is translatable into C.		H.R.M.

*/

#define MACRO 0
#define PARAM 1
#define LOOP 2

#define TAGTYPE char	/* set of (MACRO,PARAM,LOOP) */


/*		--- Global variables ---		*/

	int trace;
	char prog[500];			/* program buffer */
	int definitions[26];		/* macro definitions */
	int calstack[20];		/* macro nesting stack */
	struct frametype {
		TAGTYPE tag;
		int pos,off;
		}  stack[20];
	int data[260];
	char ch;
	int cal,chpos,level,offset,parnum,parbal,temp;

	char file[134];	/* input file buffer structure */
	int fd;		/* file descriptor */



main(argc,argv)
	int argc;
	char *argv[];
	{

	if( (fd = fopen(argv[1],file)) < 0 )
	  exit(printf("No such file: %s\n",argv[1]));
	if( *argv[2] == 'T' )
	  trace = 1;
	else
	  trace = 0;
	load();
	chpos = level = offset = cal = 0;
	do {
	  getchr();
	  if( ch == ']' || ch == '$' || isspace(ch) )
	    ;
	  else if( isdigit(ch) ) {
	    temp = 0;
	    while( isdigit( ch ) ) {
	      temp = temp*10 + val(ch);
	      getchr();
	      }
	    pushcal(temp);
	    ungetchr();
	    }
	  else if( isalpha( ch ) )
	    pushcal( offset + num(ch) );
	  else
	    switch(ch) {

	      case '?':	scanf("%d",&temp);
			pushcal(temp);
			break;

	      case '!':	printf("%d",popcal());
			break;

	      case '+':	pushcal(popcal() + popcal());
			break;

	      case '-':	pushcal( - popcal() + popcal() );
			break;

	      case '*':	pushcal( popcal() * popcal() );
			break;

	      case '/':	temp = popcal();
			pushcal(popcal() / temp);
			break;

	      case '.':	pushcal(data[popcal()]);
			break;

	      case '=':	temp = popcal();
			data[popcal()] = temp;
			break;

	      case '"':	while(getchr() != '"' )
			  if( ch == '!' )
			    putchar('\n');
			  else
			    putchar(ch);
			break;

	      case '[':	if( popcal() <= 0 )
			  skip('[',']');
			break;

	      case '(':	push(LOOP);
			break;

	      case '^':	if( popcal() <= 0 ) {
			  pop();
			  skip('(',')');
			  }
			break;

	      case ')':	chpos = stack[level].pos;
			break;

	      case '#':	getchr();
			if( definitions[num(ch)] > 0 ) {
			  push(MACRO);
			  chpos = definitions[num(ch)];
			  offset += 26;
			  }
			else
			  skip('#',';');
			break;

	      case '@':	pop();
			skip('#',';');
			break;

	      case '%':	getchr();
			parnum = num(ch);
			push(PARAM);
			parbal = 1;
			temp = level;
			do {
			  switch( stack[--temp].tag ) {

			    case PARAM:
			    case MACRO:	parbal--;
					break;

			    case LOOP:	break;
			    }
			  } while( parbal != 0 );
			chpos = stack[temp].pos;
			offset = stack[temp].off;
			do {
			  getchr();
			  if( ch == '#' ) {
			    skip('#',';');
			    getchr();
			    }
			  if( ch == ',' )
			    parnum--;
			  } while( parnum != 0 && ch != ';' );
			if( ch == ';' )
			  pop();
			break;

	      case ',':
	      case ';':	pop();
			break;

	      }
	  } while( ch != '$');
	}


num(chr)
	char chr;
	{
	return tolower(chr) - 'a' + 1;
	}



val(chr)
	char chr;
	{
	return tolower(chr) - '0';
	}



getchr()
	{
	if(trace) {	/* --- debug tracing selected --- */
	  putchar('\n');
	  putchar('\\');
	  putchar(prog[++chpos]);
	  putchar('\\');
	  return (ch = prog[chpos]);
	  }
	else
	  return (ch = prog[++chpos]);
	}



pushcal(datum)
	int datum;
	{
	return (calstack[++cal] = datum);
	}



popcal()
	{
	return (calstack[cal--]);
	}



push(tagval)
	TAGTYPE tagval;
	{

	level++;
	stack[level].tag = tagval;
	stack[level].pos = chpos;
	stack[level].off = offset;
	}



pop()
	{

	chpos = stack[level].pos;
	offset = stack[level].off;
	return --level;
	}




skip(lch,rch)
	char lch,rch;
	{
	int cnt;

	cnt = 1;
	while( cnt ) {
	  getchr();
	  if( ch == lch )
	    cnt++;
	  else if( ch == rch )
	    cnt--;
	  else
	    ;
	  }
	}




load()
	{
	char this,last;
	int charnum;

	for(charnum = 1; charnum <= 26; charnum++)
	  definitions[charnum] = 0;
	charnum =0;
	this = ' ';
	while( ! ( last == '$' && this == '$' ) ) {
	  last = this;
	  this = getc(file);
	  prog[++charnum] = this;
	  if( last == '$' && isalpha(this) )
	    definitions[num(this)] = charnum;
	  }
	}



ungetchr()
	{
	chpos--;
	}



