/*
**  reverse str in place
*/
reverse(str) char *str; {
int c, i, j;
  for(i=0, j=strlen(str)-1; i<j; ++i, --j) {
    c=str[i];
    str[i]=str[j];
    str[j]=c;
    }
  }
