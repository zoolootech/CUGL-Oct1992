/** m20.c  --  defines _main for DOS 2.0   6.15.83
*
* This module defines a version of _main which processes the
* command line for arguments and sets up _iob so that the first
* three files defined are stdin, stdout, stderr which are opened
* by DOS 2.0.  Re-direction is supported via DOS.
* Stack size override is not supported.
*
*			Ted Reuss     c/o South Texas Software, Inc.
*		  Home: 713/961-3926	  4544 Post Oak Place, Suite 176
*		  Offi: 713/877-8205	  Houston, Tx 77027
*
**/
#include <stdio.h>
#define STDIN  0
#define STDOUT 1
#define STDERR 2
#define MAXARG 32		/* maximum command line arguments */

_main(line)
char *line;
{
static int argc = 0;
static char *argv[MAXARG];

while (isspace(*line)) line++;	/* find program name */
while (*line != '\0' && argc < MAXARG)
   {			/* get command line parameters */
   argv[argc++] = line;
   while (*line != '\0' && isspace(*line) == 0) line++;
   if (*line == '\0') break;
   *line++ = '\0';
   while (isspace(*line)) line++;
   }

stdin->_flag |= _IONBF+_IOREAD;
stdin->_file = STDIN;
stdin->_base = stdin->_ptr = getmem(_BUFSIZ);
stdin->_cnt  = 0;

stdout->_flag |= _IONBF+_IOWRT;
stdout->_file = STDOUT;
stdout->_base = stdout->_ptr = getmem(_BUFSIZ);
stdout->_cnt  = 0;

stderr->_flag |= _IONBF+_IOWRT;
stderr->_file = STDERR;
stderr->_base = stderr->_ptr = getmem(_BUFSIZ);
stderr->_cnt  = 0;

main(argc, argv);	/* call main function */
}
/** END M20 **/
_base = stderr->_ptr = getmem(_BUFSIZ);
stderr->_cnt  = 0