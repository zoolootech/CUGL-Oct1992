/*
HEADER: CUG 121.??;

    TITLE:	If - conditionally execute coommands;
    VERSION:	1.0;
    DATE:	09/01/85;
    DESCRIPTION: "This program executes any other program (passing command
		line arguments if specified) when a conditional test is met.
		The following conditions may be tested:
			file exists/does not exist
			file is read-only/read-write
			string arguments are equal/unequal.";
    KEYWORDS:	conditional, command;
    SYSTEM:	CP/M;
    FILENAME:	IF.C;
    WARNINGS:	"Copyright (c) 1982, Steve Blasingame.
		Requires local.c for link.
		You can't run any builtin (CCP) commands from IF.";
    SEE-ALSO:	SH.C;
    AUTHORS:	Steve Blasingame;
    COMPILERS:	BDS-C 1.50;
*/
/********************************************************
 *	IF - Execute Commands Conditionally
 ********************************************************

 Description
	This thing is based very roughly on the V6 Unix
	'IF' Command.  It accepts the following types
	of conditional expressions:

	if -r file command args		(if readable)
	if -w file command args		(if writeable)
	if !(-r) file command args	(if not found)
	if !(-w) file command args	(if readonly)
	if string1 == string2 command args
	if string1 != string2 command args


	Obviously there is much room for improvement,
	but consider what is underneath this stuff.
	If you have suggestions or FIXUPS please
	contact me at one of the following places.


	Usenet addresses;
				duke!uok!bsteve (ECN Unix)
				duke!uok!ishtar!bsteve (cnet)

	Cnode Ishtar;		(405)364-1373 Weekdays
				mail to bsteve or unix,bsteve

*/

#include <bdscio.h>
#include "fcb.h"	/* structure of a CP/M fcb */

#define	NOTFOUND	-1
#define	READWRITE	0
#define	READONLY	1

int	doshell();
int	gripe();
int	fstat();

main(argc,argv)
int argc;
char *argv[];
{

	if (argc < 4)
		gripe("if: bad syntax\n");
	argv[argc] = NULL;		/* null terminate for execv */

	if (strcmp(strlower(argv[1]),"-r") == 0) {
		if (fstat(argv[2]) != NOTFOUND)	/* it exists, it's readable */
			execv(argv[3],&argv[4]);
		else doshell();
	}
	else if (strcmp(strlower(argv[1]),"!(-r)") == 0) {
		if (fstat(argv[2]) == NOTFOUND)	/* not found */
			execv(argv[3],&argv[4]);
		else doshell();
	}
	else if (strcmp(strlower(argv[1]),"-w") == 0) {
		if (fstat(argv[2]) == READWRITE)	/* rw */
			execv(argv[3],&argv[4]);
		else doshell();
	}
	else if (strcmp(strlower(argv[1]),"!(-w)") == 0) { 
		if (fstat(argv[2]) != READWRITE)	/* readonly */
			execv(argv[3],&argv[4]);
		else doshell();
	}
	else if (strcmp(argv[2],"==") == 0) {
		if (strcmp(strlower(argv[1]),strlower(argv[3])) == 0)
			execv(argv[4],&argv[5]);
		else doshell();
	}
	else if (strcmp(argv[2],"!=") == 0) {
		if (strcmp(strlower(argv[1]),strlower(argv[3])) != 0)
			execv(argv[4],&argv[5]);
		else doshell();
	}
	else gripe("if: unknown condition\n");
}

fstat(filnam)
char *filnam;
{
char *byte, c;
fcb address;

	setfcb(address,filnam);
	if ((c=bdos(17,&address)) == 255)
		return NOTFOUND;	/* file not found */

	byte = (0x80 + (c*32) + _MBYTE); /* permission byte */

	if ((*byte & '\200') == 0)
		return READWRITE;	/* readwrite */
	else return READONLY;		/* readonly */
}

doshell()
{
	/*	execl("a:sh",0);	*/
	exit(0);
}

gripe(string)
char *string;
{
	puts(string);		/* give message */
	doshell();		/* & maybe reinvoke shell */
}
                               