/*
** _format(format, argc, argv, call, parm)
**        formatter for printf, fprintf, and sprintf
**        operates as described by Kernighan & Ritchie
**        only d, o, x, c, s, and u specs are supported.
**
** format -- pointer to a standard printf format string
** argc   -- number of arguments in argument list
** argv   -- pointer to argument list (last argument first)
** call   -- address of function to call (eg, fputc)
** parm   -- second argument to call (eg, stdout);
*/
 
#include <errno.h>
extern int errno, abort(), isdigit();
 
_format(format, argc, argv, call, parm)
    char *format; int argc, *argv, call, parm; {
  int i, width, prec, len;
  char c, left, str[7], *sptr, pad;
  argv += (argc - 1); /* address "first" argument */
  while(c=*format++) {
    if(c!='%') {
      if(call(c, parm) == -1) abort(errno);
      continue;
      }
    if(*format=='%') {
      if(call(*format++, parm) == -1) abort(errno);
      continue;
      }
    if(left=(*format=='-')) ++format;
    if(*format=='0') {pad='0'; ++format;} else pad=' ';
    if(*format == '*') {
      width = *argv--;
      if(!argc--) abort(E2BIG); /* argument list error */
      format++;
      }
    else {
      width = 0;
      if(isdigit(*format)) while(isdigit(c = *format)) {
        width = (width * 10) + (c - '0');
        format++;
        }
      }
    prec = 0;
    if(*format=='.') {
      format++;
      if(*format == '*') {
        prec = *argv--;
        if(!argc--) abort(E2BIG); /* argument list error */
        format++;
        }
      else {
        if(isdigit(*format)) while(isdigit(c = *format)) {
          prec = (prec * 10) + (c - '0');
          format++;
          }
        }
      }
    c=*format++;
    i=*(argv--);
    if(!argc--) abort(E2BIG); /* argument list error */
    sptr=str;
    if(c=='d') itod(i, str);
    else if(c=='u') itou(i, str);
    else if(c=='o') itoo(i, str);
    else if(c=='x') itox(i, str);
    else if(c=='c') {str[0]=i; str[1]=0;}
    else if(c=='s') sptr=i;
    else continue;
    if(c != 's' && c != 'c') while(*sptr==' ') ++sptr;
    len = -1; while(sptr[++len]); /* get length */
    if((c == 's') & (len > prec) & (prec > 0)) len = prec;
    if(!left) while(((width--) - len) > 0)
      if(call(pad, parm) == -1) abort(errno);
    while(len) {
      if(call(*sptr++, parm) == -1) abort(errno);
      --len;
      --width;
      }
    while(((width--) - len) > 0)
      if(call(pad, parm) == -1) abort(errno);
    }
  }
 
/*
** itod -- convert nbr to signed decimal string of width 7
**         (including null) right adjusted, blank filled
*/
static itod(nbr, str) int nbr; char str[]; {
  char sgn;
  int count;
  if(nbr<0) {nbr = -nbr; sgn='-';}
  else sgn=' ';
  str = str+6; /* find end of string */
  *str-- = 0; /* store ending null */
  count = 6; /* remaining characters */
  while(1) {
    *str-- = (nbr % 10 + '0');
    count--;
    if(!(nbr /= 10)) break;
    }
  *str-- = sgn;
  while(--count) *str-- = ' ';
  }
 
/*
** itou -- convert nbr to unsigned decimal string of width 7
**         (including null) right adjusted, blank filled
*/
static itou(nbr, str) int nbr; char str[]; {
  int count, lowbit;
  str = str+6; /* find end of string */
  *str-- = 0; /* store ending null */
  count = 6; /* remaining characters */
  while(1) {
    lowbit = nbr & 1;
    nbr = (nbr >> 1) & 32767;  /* divide by 2 */
    *str-- = ((nbr % 5) << 1) + lowbit + '0';
    count--;
    if(!(nbr /= 5)) break;
    }
  while(count--) *str-- = ' ';
  }
 
/*
** itoo -- convert nbr to unsigned octal string of width 7
**         (including null) right adjusted, blank filled
*/
static itoo(nbr, str) int nbr; char str[]; {
  int count;
  str = str+6; /* find end of string */
  *str-- = 0; /* store ending null */
  count = 6; /* remaining characters */
  while(1) {
    *str-- = (nbr & 7) + '0';
    count--;
    if(!(nbr = ((nbr >> 3) &  8191))) break;
    }
  while(count--) *str-- = ' ';
  }
 
/*
** itox -- convert nbr to unsigned hexadecimal string of width 7
**         (including null) right adjusted, blank filled
*/
static itox(nbr, str) int nbr; char str[]; {
  int count, digit;
  str = str+6; /* find end of string */
  *str-- = 0; /* store ending null */
  count = 6; /* remaining characters */
  while(1) {
    *str-- = ((digit = (nbr & 15)) < 10) ? (digit + '0') : (digit + ('A' - 10));
    count--;
    if(!(nbr = ((nbr >> 4) &  4095))) break;
    }
  while(count--) *str-- = ' ';
  }
