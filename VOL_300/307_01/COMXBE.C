/*
HEADER: 	;
TITLE:		PC com port driver back end;
VERSION:	1.0;

DESCRIPTION:	"Dummy BSS definition for use by COMX.C.";

WARNINGS:	"Microsoft specific.";
SYSTEM: 	MS-DOS v2 or later;
FILENAME:	COMXBE.C;

SEE-ALSO:	COMX, COMX.C, COMXFE.ASM;
AUTHORS:	Hugh Daschbach;
COMPILERS:	Microsoft v5.0,v5.1;
*/
/*----------------------------------------------------------------------*/
/* comxbe.c: Back end to comx.c.  Allows the end of the BSS area to
 *   be located at run time.
 */
char bss_end;
