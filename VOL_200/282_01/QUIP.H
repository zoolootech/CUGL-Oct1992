/*
HEADER:        ;
TITLE:         QUIP;
VERSION:       2.0;
DESCRIPTION:   "Part of a fortune cookie system
             
               This header file defines the files for use by the quip 
               rograms";
KEYWORDS:      quip, fortune, utility, startup;
SYSTEM:        MS-DOS, UNIX;
FILENAME:      quip.h;
WARNINGS:      "";
SEE-ALSO:      QUIP.C, QUIP.H, QUPUPDT.C QUIPADD.C, QUIP.DAT, QUIP.KEY;
AUTHORS:       ??;
MODIFIED BY:   David Bryant;
COMPILERS:     Microsoft Quick C;
*/

#ifdef UNIX

/** UNIX version **/
/* Use a public directory accesable to all */
char  seekname[50] = ":udd:chas.sys:quip:quip.key",
      quipname[50] = ":udd:chas.sys:quip:quip.dat";

#else

/** MS-DOS version **/
/* Use either the current directory or a general data directory */
char  seekname[65] = "quip.key",
      quipname[65] = "quip.dat";

#endif

#define  SEPERATOR '~'                    /* Char that marks end of a quip */
#define  MAX_SIZE  2048                   /* Max characters in a quip */


