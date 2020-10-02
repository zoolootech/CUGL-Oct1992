/* grep.c: attempt to get UNIX pattern-matching on a micro */

/* From DECUS C Tools package on DEC systems - for non-commercial
	use only.  Modifications by Chuck Allison, April 1985:

		- handles quoted args (for embedded spaces)
		- expands wildcards against indicated directory
		- distinguishes case (ignore with -i)
*/

#include <makearg.c>	/* ..includes <stdio.h>, <ctype.h>, <dos.h>.. */

#define LMAX    512
#define PMAX    256
#define MAXFILES 150
#define CHAR    1
#define BOL     2
#define EOL     3
#define ANY     4
#define CLASS   5
#define NCLASS  6
#define STAR    7
#define PLUS    8
#define MINUS   9
#define ALPHA   10
#define DIGIT   11
#define NALPHA  12
#define WHITE   13
#define RANGE   14
#define ENDPAT  15
#define TRUE    1
#define FALSE   0

int  cflag = FALSE,
   	 fflag = FALSE,
     nflag = FALSE,
     vflag = FALSE,
	 iflag = FALSE;

char    *pp,
        lbuf[LMAX],
        pbuf[PMAX];


main()
{
   register char   *p;
   register int    i;
   FILE            *f;
   char *xargv[MAXFILES];
   int xargc, maxarg = MAXFILES;

   /* ..this must be first; processes args.. */
   makeargv();

   if (argc <= 1)
      usage("No arguments");

   for (; *(p = *(argv+1)) == '-'; ++argv, --argc)
   		while (*++p)
            switch(tolower(*p))
			{
            	case 'c': 
					cflag = !cflag;
               		break;
            	case 'f':
					fflag = !fflag;
               		break;
            	case 'n':
					nflag = !nflag;
               		break;
            	case 'v':
					vflag = !vflag;
               		break;
            	case 'i':
					iflag = !iflag;
               		break;
            	default:	usage("Unknown flag");
            }

   compile(*++argv);
   --argc;

   if (argc == 1)
      grep(stdin, 0);
   else
   {
	  /* ..expand file arguments.. (Mark Williams C only!).. */
	  xargc = exargs("",argc,argv,xargv,maxarg);

	  /* ..check file switch.. */
	  if (xargc > 1)
	  	fflag = !fflag;

      for (i = 0; i < xargc; ++i)
			if ((f = fopen(xargv[i],"r")) != NULL)
			{
				grep(f,xargv[i]);
				fclose(f);
			}
			else
				fprintf(stderr,"can't open %s\n",xargv[i]);
   }
}


usage(s)
char *s;
{
   fprintf(stderr, "?GREP-E-%s\n", s);
   fprintf(stderr,
      "Usage: grep [-cfnv] pattern [file ...] \n");
   exit(1);
}


compile(source)
char *source;
/* ..Compile the pattern into global pbuf[].. */
{
   register char  *s;         /* Source string pointer     */
   register char  *lp;        /* Last pattern pointer      */
   register int   c;          /* Current character         */
   int            o;          /* Temp                      */
   char           *spp;       /* Save beginning of pattern */
   char           *cclass();  /* Compile class routine     */

   s = source;
   pp = pbuf;

   while (c = *s++)
   {
      /* ..STAR, PLUS and MINUS are special.. */
      if (c == '*' || c == '+' || c == '-')
	  {
         if ( pp == pbuf ||
              (o=pp[-1]) == BOL ||
              o == EOL ||
              o == STAR ||
              o == PLUS ||
              o == MINUS
		    )
        	badpat("Illegal occurrance op.", source, s);
         store(ENDPAT);
         store(ENDPAT);
         spp = pp;               /* Save pattern end     */
         while (--pp > lp)       /* Move pattern down    */
            *pp = pp[-1];        /* one byte             */
         *pp =   (c == '*') ? STAR :
            (c == '-') ? MINUS : PLUS;
         pp = spp;               /* Restore pattern end  */
         continue;
      }

      /* ..All the rest.. */
      lp = pp;         /* ..Remember start.. */
      switch(c)
	  {
	      case '^':
	         store(BOL);
	         break;
	      case '$':
	         store(EOL);
	         break;
	      case '.':
	         store(ANY);
	         break;
	      case '[':
	         s = cclass(source, s);
	         break;
	      case ':':
	         if (*s)
			 {
	            c = *s++;
	            switch(tolower(c))
				{
		            case 'a':
		               store(ALPHA);
		               break;
		            case 'd':
		               store(DIGIT);
		               break;
		            case 'n':
		               store(NALPHA);
		               break;
		            case ' ':
		               store(WHITE);
		               break;
		            default:
		               badpat("Unknown : type", source, s);
	            }
	            break;
         	}
         	else
				badpat("No : type", source, s);
	     case '\\':
	        if (*s)
            c = *s++;
	     default:
	        store(CHAR);
	        store(iflag ? tolower(c) : c);
      }
   }
   store(ENDPAT);
   store('\0');
}


char *cclass(source, src)
char *source;	/* ..Pattern start.. */
char *src;      /* ..Class start.. */
/* ..Compile a class (within []).. */
{
   register char   *s;        /* Source pointer    */
   register char   *cp;       /* Pattern start     */
   register int    c;         /* Current character */
   int             o;         		/* ..Temp.. */

   s = src;
   o = CLASS;
   if (*s == '^')
   {
      ++s;
      o = NCLASS;
   }
   store(o);
   cp = pp;
   store(0);                         /* ..Byte count.. */

   while ((c = *s++) && c!=']')
   {
      if (c == '\\')
	  {  /* ..Store quoted char.. */
         if ((c = *s++) == '\0')     /* ..Gotta get something.. */
            badpat("Class terminates badly", source, s);
         else
		 	store(iflag ? tolower(c) : c);
      }
      else if (c == '-' && (pp - cp) > 1 && *s != ']' && *s != '\0')
	  {
         c = pp[-1];             /* Range start     */
         pp[-1] = RANGE;         /* Range signal    */
         store(c);               /* Re-store start  */
         c = *s++;               /* Get end char and*/
		 store(iflag ? tolower(c) : c);
      }
      else
		 store(iflag ? tolower(c) : c);
   }

   if (c != ']')
      badpat("Unterminated class", source, s);
   if ((c = (pp - cp)) >= 256)
      badpat("Class too large", source, s);
   if (c == 0)
      badpat("Empty class", source, s);
   *cp = c;
   return(s);
}


store(op)
{
   if (pp >= &pbuf[PMAX])
      error("Pattern too complex\n");
   *pp++ = op;
}


badpat(message, source)
char  *message,       /* ..Error message.. */
      *source;        /* ..Pattern start.. */
{
   fprintf(stderr, "-GREP-E-%s, pattern is\"%s\"\n", message, source);
   exit(1);
}



grep(fp, fn)
FILE       *fp;       /* File to process            */
char       *fn;       /* File name (for -f option)  */
/* ..Scan the file for the pattern in pbuf[].. */
{
   register int lno, count, m;

   lno = 0;
   count = 0;
   while (fgets(lbuf, LMAX, fp))
   {
      ++lno;
      m = match();
      if ((m && !vflag) || (!m && vflag))
	  {
         ++count;
         if (!cflag)
		 {
            if (fflag && fn) 
			{
			   fprintf(stdout,"\n\nFile: %s\n\n",fn);
               fn = 0;
            }
            if (nflag)
               printf("%d\t", lno);
			fputs(lbuf,stdout);
         }
      }
   }
   if (cflag)
   {
      if (fflag && fn)
			fprintf(stdout,"\n\nFile: %s\n\n",fn);
      printf("%d\n", count);
   }
}


match()
/* ..Match the current line (in lbuf[]), return 1 if it does.. */
{
   register char *l;        /* ..Line pointer.. */
   char *pmatch();

   for (l = lbuf; *l; l++)
      if (pmatch(l, pbuf))
         return(1);

   return(0);
}


char *pmatch(line, pattern)
char *line,     /* ..(partial) line to match.. */
     *pattern;  /* ..(partial) pattern to match.. */
{
   register char   *l;        /* ..Current line pointer.. */
   register char   *p;        /* ..Current pattern pointer.. */
   register char   c;         /* ..Current character.. */
   register char   d;         /* ..Temporary character.. */
   char            *e;        /* ..End for STAR and PLUS match.. */
   int             op;        /* ..Pattern operation.. */
   int             n;         /* ..Class counter.. */
   char            *are;      /* ..Start of STAR match.. */

   l = line;
   p = pattern;
   while ((op = *p++) != ENDPAT)
   {
      switch(op)
	  {
	      case CHAR:
			 c = iflag ? tolower(*l++) : *l++;
			 d = iflag ? tolower(*p++) : *p++;
	         if (c != d)
	            return(0);
	         break;
	      case BOL:
	         if (l != lbuf)
	            return(0);
	         break;
	      case EOL:
	         if (*l != '\0' && *l != '\n')
	            return(0);
	         break;
	      case ANY:
	         if (*l++ == '\0')
	            return(0);
	         break;
	      case DIGIT:
	         if (!isdigit(c = *l++))
	            return(0);
	         break;
	      case ALPHA:
			 if (!isalpha(c = *l++))
	            return(0);
	         break;
	      case NALPHA:
			 if (!isalnum(c = *l++))
			 	return(0);
	         break;
	      case WHITE:
	         if (!isspace(c = *l++))
	            return(0);
	         break;
	      case CLASS:
	      case NCLASS:
	         c = iflag ? tolower(*l++) : *l++;
	         n = *p++ & 0x00ff;
	         do
			 {
	            if (*p == RANGE)
				{
	               p += 3;
	               n -= 2;
	               if (c >= p[-2] && c <= p[-1])
	                  break;
	            }
	            else if (c == *p++)
	               break;
	         } while (--n > 1);
	         if ((op == CLASS) == (n <= 1))
	            return(0);
	         if (op == CLASS)
	            p += n - 2;
	         break;
	      case MINUS:
	         e = pmatch(l, p);       /* ..Look for a match.. */
	         while (*p++ != ENDPAT); /* ..Skip over pattern.. */
	         if (e)                  /* ..Got a match?.. */
	            l = e;               /* ..Yes, update string.. */
	         break;                  /* ..Always succeeds.. */
	      case PLUS:                 /* ..One or more.. */
	         if ((l = pmatch(l, p)) == 0)
	            return(0);           /* ..Gotta have a match.. */
	      case STAR:                 /* ..Zero or more.. */
	         are = l;                /* ..Remember line start.. */
	         while (*l && (e = pmatch(l, p)))
	            l = e;               /* ..Get longest match.. */
	         while (*p++ != ENDPAT); /* ..Skip over pattern.. */
	         while (l >= are)
			 {  /* ..Try to match rest.. */
	            if (e = pmatch(l, p))
	               return(e);
	            --l;                 /* ..Nope, try earlier.. */
	         }
	         return(0);              /* ..Nothing else worked.. */
	      default:
	         printf("Bad op code %d\n", op);
	         error("Cannot happen -- match\n");
	  }
   }
   return(l);
}


error(s)
char *s;
{
   fprintf(stderr, "%s", s);
   exit(1);
}

