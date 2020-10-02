
/**********************************************************
 ***							***
 ***	Copyright (c) 1981 by David M. Fogg		***
 ***							***
 ***		2632 N.E. Fremont			***
 ***		Portland, OR 97212			***
 ***							***
 ***	    (503) 288-3502{HM} || 223-8033{WK}		***
 ***							***
 ***	Permission is herewith granted for non- 	***
 ***	commercial distribution through the BDS C	***
 ***	User's Group; any and all forms of commercial   ***
 ***	redistribution are strenuously unwished-for.	***
 ***							***
 **********************************************************/

/* ---> WC.C - COUNT CHARS/WORDS/LINES (BDS C VERSION) <--- */

#include <dmfio.h>

main (ac, av)
int ac;
char *av[];
{
   char c;
   unsigned chars, words, lines;
   BOOL inword;
   char iobuf[BUFSIZ];

   inword = NO;
   chars = words = lines = 0;

   if (ac < 2) errxit("Usage: wc filename");

   if (fopen(*++av, iobuf) == ERROR) errxit("File I/O Error");

   while ((c = getc(iobuf)) != CEOF && c != CPMEOF) {
      ++chars;
      if (c == '\n') ++lines;
      if (isspace(c))
	 inword = NO;
      else
	 if (inword == NO) {
	    inword = YES;
	    ++words;
	 }
   }
   printf("\nChars: %u   Words: %u   Lines: %u\n", chars, words, lines);
   fclose(iobuf);
}
