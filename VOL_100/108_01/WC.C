
/*  This program counts words in an ASCII file. Les Hancock
is the author.   */

#define NO 0
#define YES 1
#define TAB 9
#define LF 10
#define CR 13
#define EOF 26
main (argc, argv)
int argc;
char *argv[];
{
     int fid;
     unsigned linect, wordct, cct;
     char buf[134], c, inword;
     if ((fid = fopen (argv[1], buf)) == -1)  {
        printf ("File missing.\nUsage: WC <filename.ext>\n");
        exit ();
    }
    for (linect = wordct = cct = 0, inword = NO;
         (c = getc(buf)) != EOF; ++cct) {
      if (c > 7 && (c < 14 || c > 31) && c < 128)  {
         if (c == CR)
           ++linect;
      if (c == CR || c == LF || c == ' ' || c == TAB)
          inword = NO;
      else if (!inword)  {
         inword = YES;
         ++wordct;
         }
     } else {
        printf ("Not ASCII file\n");
        exit ();
     }
  }
  printf ("Lines = %u\nWords = %u\nCharacters = %u\n", linect, wordct, cct);
  close(fid);
}
