/*
** get a string from "stdin"
** terminate with \0
*/
extern int *stdin, fgetc();
 
gets(s) char *s; {
  int n, ch;
  char *str;
  str=s; /* save original value */
  n = 256;
  while(--n) {
    ch=fgetc(stdin);
    if(ch<0) {
      *s='\0';
      return 0;
      }
    if(ch=='\n') break;
    *s++=ch;
    }
  *s='\0';
  return str;
  }
