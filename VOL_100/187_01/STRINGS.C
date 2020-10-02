/*@*****************************************************/
/*@                                                    */
/*@ strings is a package of string functions used      */
/*@        to process command tails.  I have not       */
/*@        used them and they do not compile on        */
/*@        DeSmet C.  They looked useful, so I have    */
/*@        saved them.                                 */
/*@                                                    */
/*@*****************************************************/

#include <stdio.h>
#include <ctype.h>
/*
;;      file:   _STRING.C
;;
;; --------------- Command Tail Processing Functions ---------------
;;
;;      This next set of functions are meant to be used together. Examples
;;on their use follows.
;;
;;
;;int num_args(string)  Returns the number of args in the string, seperated
;;                      by delims (see delim(), below). Leading delimiters
;;                      are ignored.
;;
;;char *next_arg(string)        Returns a pointer to the next arg, delimited
;;                      by a delim, skipping over the current arg. Use via
;;                      ptr= nextarg(ptr) to skip to each argument. All
;;                      switches at the end of the current arg are skipped.
;;
;;
;;char *skip_delim(string) Skips leading delims in a string. returns a pointer.
;;
;;cpyarg(to,from)               Copies a string, up to the next delim or switch.
;;                      Leading and trailing delimiters are stripped (from
;;                      the output string) and a null terminator is added.
;;
;;                      after cpyarg()          FROM: foo/b
;;                                              TO: foo
;;
;;delim(c)              Returns true if the character is a delimiter.
;;                      Nulls are not considered a delimiter. The list of
;;                      delimiters is contained in the array '_dlmlst', and
;;                      can be changed via newdelim().
;;
;;newdelim(s)           Replace the list of delimiters. The string 's' must
;;                      be less than 20 chars.
;;
;;isswitch(c)           Returns true if the character is the current DOS
;;                      switch character.
;;
;;char filtchar(c)      Convert a character to one legal for an MSDOS filename.
;;                      Illegal characters such as switch or path seperators,
;;                      control characters, etc are changed to '$'. Bit 7 is
;;                      masked off. (Disallows foreign language support??)
;;
;;wild(string)          Returns true if the string contains a star or question.
;;
;;char *strip_path(out,in) Copies the disk specifier or pathname to the output
;;                      array, and returns a pointer to the name in the input
;;                      string. Drive specs are considered a path, and are
;;                      treated as such by DOS. Stripping "a:foo" and
;;                      "bin/foo/framis.asm" result in:
;;
;;                              IN:     "A:"
;;                              IN:     "bin\foo"
;;
;;                              OUT:    "A:"
;;                              OUT:    "bin\"
;;
;;strip_switch(out,in)  Copy the switches from the in string, remove the switch
;;                      character and put all the characters in the out array.
;;                      Each is converted to upper case, and the string is null
;;                      terminated.
;;
;;ispath_delim(c)       Returns true if the character is a legal pathname
;;char c;               component seperator. The only two characters legal
;;                      (here at least) are \ and-or /. For example:
;;
;;                      Switch character = /
;;                              ispath_delim('/') == 0
;;                              ispath_delim('\\') == 1
;;                      Switch character = -
;;                              ispath_delim('/') == 1
;;                              ispath_delim('\\') == 1
;;                      Switch character = \
;;                              ispath_delim('/') == 1
;;                              ispath_delim('\\') == 0
;;
;;
;; --------- Command Tail Processing Examples ----------
;;
;;      This is an example of one way to use the above functions to process
;;an MSDOS program command tail. You do not need to know what the system switch
;;character or legal path seperators are. If you can't handle paths, strip them
;;off. Assume there is a pointer, p, that points to a command tail string:
;;
;;      p= "  source.ext abc.asm/v+\bin\def.com /x/y/z"
;;
;;      p= skip_delim(p);               /* p= "source.exe abc...... */
;;
;;      p= next_arg(p);                 /* p= "abc.asm/v.... "  */
;;      cpyarg(work,p);                 /* work= "abc.asm"  */
;;      s= strip_path(work,p));         /* s= "abc.asm"   path= ""  */
;;      strip_switch(sw,p);             /* sw= "V"  */
;;
;;      p= next_arg(p);                 /* p= "\bin\def.com"  */
;;      cpyarg(work,p);                 /* work= "\bin\def.com"  */
;;      s= strip_path(path,p);          /* s= "def.com"  path= "\bin\"  */
;;      strip_switch(sw,p);             /* sw= ""  */
;;
;;      p= next_arg(p);                 /* p= "/x/y/z"  */
;;      cpyarg(work,p);                 /* work= ""  */
;;      s= strip_path(path,p);          /* s= ""  path= ""  */
;;      strip_switch(sw,p);             /* sw= "XYZ"  */
;;
;;      p= next_arg(p);                 /* p= ""  */
;;
;;      while (num_args(p)              /* while not end of string ... */
;;      while (*p)                      /* another way ... */
;;
;;
;;char *name,sw[4],path[40];
;;
;;      p= skip_delim(p);
;;      while (num_args(p) > 0) {       /* *p points to the current arg, */
;;              name= strip_path(path,p);
;;              strip_switch(sw,p);
;;              printf("Path= %s, Name= %s, Switches= %s\n",path,name,sw);
;;      }
;;
*/
/* Return the number of args left in the string. */

num_args(string)
char *string;
{
int count;

        count= 0;
        string= (char *)skip_delim(string);     /* skip leading blanks, */
        while (*string) {
                ++count;                        /* count one, */
                string= (char *)next_arg(string); /* find next, */
        }
        return(count);
}
/* Return a pointer to the next argument in the string. */

next_arg(string)
char *string;
{
        while ((!delim(*string)) && *string)            /* skip this one, */
                ++string;                               /* up to delim, */
        string= (char *)skip_delim(string);             /* then skip delims, */
        return(string);
}

/* Skip over the leading delimiters in a string. */

skip_delim(string)
char *string;
{
        while (delim(*string) && *string)
                ++string;
        return(string);
}
/* Copy the string to the destination array, stopping if we find one
of our delimiters or switches. */

cpyarg(to,from)
char *to;
char *from;
{
        while ( (!delim(*from)) && (!isswitch(*from)) && *from)
                *to++= *from++;
        *to= '\0';
        return;
}
/* Strip any switches from the input string, put into the output array. */

strip_switch(out,in)
char *out;
char *in;
{
        while (*in && (!isswitch(*in)))         /* skip to end of string */
                ++in;                           /* or first switch, */

        while (*in && isswitch(*in)) {          /* copy switch args while */
                ++in;
                *out++ = toupper(*in);          /* stripping switch chars, */
                ++in;
        }
        *out= '\0';                             /* terminate it, */
        return;
}
/* ----- List of legal delimiters. This is the default list ----- */

char _dlmlst[20] = { " \t,+" }; /* space, tab, comma, plus */

/* Change the list of delimiters. */

newdelim(s)
char *s;
{
        strcpy(_dlmlst,s);
        return;
}

/* Return true if the character is a delimiter from the list above. */

delim(c)
char c;
{
int i;
        for (i= 0; _dlmlst[i]; ++i) {
                if (c == _dlmlst[i]) return(1);
        }
        return(0);
}
/* return true if the character is the current switch character. */

isswitch(c)
char c;
{
        return(c == _charop(0,0));
}
/* Clean up the character for a legal MSDOS filename. Convert undesireable
characters to a dollar. */

char filtchar(c)
char c;
{
        c&= 0x7f;                       /* strip bit 7, */
        if (isswitch(c) || ispath_delim(c) || (c < ' ') || (c > '~') )
                c= '$';                 /* dont allow illegal chars */
        c= toupper(c);                  /* all uppercase, */
        return(c);
}
/* Return 1 if the string is a wild filespec. */

wild(string)
char *string;
{
char *p;

        p= string;
        while (*p) {
                if (*p == '?')
                        return(1);
                if (*p == '*')
                        return(1);
                ++p;
        }
        return(0);                              /* not wild. */
}
/* Strip the pathname or disk specifier from a filename, return it in a
seperate array. We do this by initially copying the entire name in, then
searching for the colon or slash. Right after the last one we find,
stuff a null, removing the name part.

Also return a pointer to the name part in the input name. */

strip_path(out,in)
char *out;
char *in;
{
char *name;
char *endpath;

        strcpy(out,in);                 /* duplicate, for working, */
        name= in;                       /* point to name, */
        endpath= out;                   /* and end of path part, */

        while (*in) {                   /* look for slashes or colons, */
                if (*in == ':') {       /* if a colon, */
                        endpath= ++out; /* point to name, */
                        name= ++in;
                } else if (ispath_delim(*in)) {
                        endpath= ++out; /* move the pointer up, */
                        name= ++in;
                } else {
                        ++in;
                        ++out;
                }
        }
        *endpath= '\0';                 /* delete the name part, */
        return(name);                   /* return ptr to name part. */
}

/* Return true if the character is a legal path name component seperator.
The legal ones here are \ or /, depending on what the switch character is. */

ispath_delim(c)
char c;
{
        if ((c == '\\') && (!isswitch('\\'))) return(1);
        if ((c == '/') && (!isswitch('/'))) return(1);
        return(0);
}

