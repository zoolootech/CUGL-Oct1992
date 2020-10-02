/* [UNIQUE.C of JUGPDS Vol.16]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* unique - strip adjacent duplicate lines */

#include <stdio.h>
#include <dio.h>
#include <def.h>

#define LINES 10000

char	opt_d;		/* dictionary order */
char	opt_f;		/* fold order	    */
char	opt_n;		/* counter option   */

main(argc, argv)
char	**argv;
{
	char	buf1[MAXLINE], buf2[MAXLINE];
	int	i, len;
	unsigned wcount;
	char *ap, sub[8][16];

	dioinit(&argc,argv);
	opt_d = opt_f = opt_n = OFF;
	i = 0;
	while(--argc > 0)
		if ( (*++argv)[0] == '-')
			for (ap = argv[0]+1; *ap != '\0'; ap++)
				switch( toupper(*ap) ) {
				case 'D':
					opt_d = ON;
					break;
				case 'F':
					opt_f = ON;
					break;
				case 'N':
					opt_n = ON;
					break;
				}
		else
			strcpy(sub[i++], *argv);
	len = getlin(buf2, MAXLINE);
	while( len > 0 ) {
		strcpy(buf1, buf2);
		wcount = 1;
		while( (len = getlin(buf2, MAXLINE)) > 0 ) {
			if( opt_d == ON && opt_f == ON ) {
				if (strdfcmp( buf1, buf2) != 0)
					break;
				}
			else if( opt_d == ON ) {
				if (strdcmp( buf1, buf2) != 0)
					break;
				}
			else if( opt_f == ON ) {
				if (strfcmp( buf1, buf2) != 0)
					break;
				}
			else if (strcmp( buf1, buf2) != 0)
					break;
			wcount++;
			}
		if( opt_n == ON )
			printf( "%6d: %s", wcount, buf1);
		else
			printf( "%s", buf1 );
		}
	dioflush();
}
