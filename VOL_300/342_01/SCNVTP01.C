/*-
 *  ----------------------------------------------------------------------
 *  File        :   SCNVTP01.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00            February 1991
 *  Language    :   Microsoft Quick C   Version 2.0
 *  Purpose     :   Format String Function Test Program
 *              :   Convert Long, Int, or Char to a Binary string.
 *  ----------------------------------------------------------------------
 */

#define     SCNVTP01_C_DEFINED  1
#include    <stdio.h>
#include    <stdlib.h>
#undef      SCNVTP01_C_DEFINED

extern  void scv_uvtobs ( void *data, char *s,
						  int msbit, int lsbit,
						  int schar, int cchar );
void main (void );

/*-
 *  ----------------------------------------------------------------------
 *  Function Definition
 *  ----------------------------------------------------------------------
 */

void main ( void )
	{
	char sbuf[48];
	char tchr = 0xFF;
	int  tint = 0x0A0A;
	long tlng = 0xF0F0F0F0;

	printf ( "\nDemonstrate <Data> -> <Binary String>\n");

	printf ( "Convert 0xFF       into binary string : ");
	scv_uvtobs (&tchr, sbuf, 7, 0, '1', '0' );
	printf ( "%s\n", sbuf );

	printf ( "Convert 0x0A0A     into binary string : ");
	scv_uvtobs (&tint, sbuf, 15, 0, '1', '0' );
	printf ( "%s\n", sbuf );

	printf ( "Convert 0xF0F0F0F0 into binary string : ");
	scv_uvtobs (&tlng, sbuf, 31, 0, '1', '0' );
	printf ( "%s\n", sbuf );

	printf ( "Convert 0xAA       into binary string : ");
	tchr = 0xAA;
	scv_uvtobs (&tchr, sbuf, 7, 0, 'S', 'C' );
	printf ( "%s\n", sbuf );

	printf ( "Convert 0xA0       into binary string : ");
	tchr = 0xA0;
	scv_uvtobs (&tchr, sbuf, 7, 0, 'Y', 'N' );
	printf ( "%s\n", sbuf );

	printf ( "Convert 0xAA       into binary string : ");
	tchr = 0xAA;
	scv_uvtobs (&tchr, sbuf, 7, 0, 'T', 'F' );
	printf ( "%s\n", sbuf );

	printf ( "Convert 0xAA       into binary string : ");
	tchr = 0xAA;
	scv_uvtobs (&tchr, sbuf, 7, 0, 'H', 'L' );
	printf ( "%s\n", sbuf );

	exit ( 0 );
	}

/*-
 *  ----------------------------------------------------------------------
 *  END SCNVTP01.C Test Program
 *  ----------------------------------------------------------------------
 */
