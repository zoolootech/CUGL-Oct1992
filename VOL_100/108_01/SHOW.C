
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

/* ---> SHOW <--- : Show all chars in a file

   21 Nov 80: creation day

   Displays all characters in a file; control chars are upcase
   inverse video. Parity bits are stripped.

   -P : prefix chars w/ parity bit status: '_' lo; '^' hi
   -C : new line @ <CR> (default is <LF>
   -E : quit at physical EOF (default is ctrl-Z)

*/

#include <std.h>

#define MASK   0x7F  /* parity strip mask */

main (ac, av)
int ac;
char *av[];
{
   int argn;
   char *arg;
   char pc, c;
   BOOL paron, cron, eofon;
   char ibuf[BUFSIZ];

   paron = cron = eofon = NO;

   if (ac < 2) {
      puts("usage: show FILE [-C -E -P]\n");
      puts(" FILE: name of file to show\n");
      puts("   -C: new line @ <CR> (default is <LF>\n");
      puts("   -E: stop at physical EOF (default is ^Z)\n");
      puts("   -P: prefix parity status chars: '_' lo; '^' hi\n");
      exit ();
   }

   for (argn = 2; argn < ac; ++argn) {	   /* get/set options */
      arg = av[argn];
      if (*arg == '-')
	 switch (arg[1]) {
	    case 'C': cron = YES;
		      break;
	    case 'E': eofon = YES;
		      break;
	    case 'P': paron = YES;
		      break;
	    default :
	       errxit("Bad option value");
	 }
   }

   if (fopen(av[1], ibuf) == ERROR)
      errxit("INPUT FILE ERROR");

/*
   ---> MAIN LOOP <---
*/
   while ((pc = getc(ibuf)) != CEOF) {
      if (!eofon && pc == CPMEOF) exit ();
      c = pc & MASK;
      if (paron)
	 if (pc > MASK)
	    putchar('^');
	 else
	    putchar('_');
      if ((!cron && pc == '\n') ORR (cron && pc == '\r'))
	 putchar('\n');
      else if (c >= ' ')
	 putc(c, OCON);
      else
	 printf("%s%c%s", VIDINV, c + '@', VIDNOR);
   }
}
