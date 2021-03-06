/*
HEADER: CUG 121.??;

    TITLE:	Setup - inits teleray 10 function keys;
    VERSION:	1.0;
    DATE:	09/01/85;
    DESCRIPTION: "This program assigns strings to function keys 1-4
		of a teleray 10 terminal.";
    KEYWORDS:	teleray, function keys;
    SYSTEM:	CP/M;
    FILENAME:	SETUP.C;
    WARNINGS:	"Requires a Teleray 10 terminal.  Sets the function keys
		to values hard-coded in the program.  Not too general-purpose,
		huh ?";
    AUTHORS:	Mike W. Meyer;
    COMPILERS:	BDS-C 1.50;
*/

#include <bdscio.h>

main(argc, argv) char **argv; {
	char *p, buffer[MAXLINE] ;

	if (argc != 2) {
		puts("setup WHAT!?!\n") ;
		exit(1) ;
		}
	p = argv[1] ;
	sprintf(buffer, "mince b:%s.c\r", p) ;
	keyset(1, buffer) ;
	sprintf(buffer, "cc b:%s.c\r", p) ;
	keyset(2, buffer) ;
	sprintf(buffer, "clink b:%s\r", p) ;
	keyset(3, buffer) ;
	sprintf(buffer, "b:%s\r", p) ;
	keyset(4, buffer) ;
	}
/*
 * keyset - takes a number and a string, and puts that string in that function
 *	key on a teleray 10.
 */
keyset(key, string) char *string; {

	printf("Key F%d is now set to:", key) ;
	printf("\033U%02d%s\033V\n", key, string) ;
	}
                                                                                                             