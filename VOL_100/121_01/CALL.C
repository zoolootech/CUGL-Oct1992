/*
HEADER: CUG 121.??;

    TITLE:	Call - dial phone numbers;
    VERSION:	1.0;
    DATE:	12/01/85;
    DESCRIPTION: "Call is a program which uses the auto-dial features of the
		Hayes Smartmodem to dial phone numbers by name."
    KEYWORDS:	modem, dial;
    SYSTEM:	CP/M;
    FILENAME:	CALL.C;
    WARNINGS:	"Copyright (c) 1981, Mike W. Meyer.
		Requires local.c for link.
		Uses Hayes Smartmodem or compatible.  Recommended switch
		settings for the Smartmodem are: switches 2, 3, 4 down,
		others as required by system."
    SEE-ALSO:	CALL.DAT (phone number file), CALL.NRO (unformatted doc
		file), CALL.DOC (formatted doc file);
    AUTHORS:	Mike W. Meyer;
    COMPILERS:	BDS-C 1.50;
*/

/*	This toy takes two arguments, the second of which is optional.
 *	The first argument is the name of the system that you want to talk
 *	to. See the next paragraph for more on this.  The second argument
 *	is the name of a com file that you want the thing to run when it
 *	gets the dial tone.  If this argument is missing, it runs the name
 *	given in the DEFFCOM define.  Alternately, this argument could be
 *	a '-', which causes it to return to the ccp.  Note that the call
 *	usually gets you into this program BEFORE the system on the other end
 *	answers the phone.  For telnet, this is nice, as you get to see the
 *	response from the smartmodem.  For other things, this may be unwished
 *	for.  If so, delete the V1 from the Dial command string, and then wait
 *	for the modem port to have a character for you.  It should be a '4'.
 *
 *	Now, as for the system name you type at it.  This is compared with a
 *	list of names in the file DATAFILE.  The names appear as the first
 *	field in each entry, one entry per line in the file.  The format of
 *	the entries is "name:number:flags:comments".  Name is the previously
 *	mentioned system name.  Number is the number that the smartmodem needs
 *	to dial.  Flags are the flags to be passed to the modem after dialing.
 *	The problem here is that there are three places to put flags on
 *	the dial command.  These are:
 *
 *		1) before the number
 *		2) after the number
 *		3) after the number/semicolon pair, terminated by an 'O'
 *
 *	In practice, the only thing that appears in position #2 is an 'R',
 *	which is used when you want to call an originate-only modem. Thus,
 *	there are several things you can do here.  You could ignore the flag
 *	field entirely, and just put the whole mess in the number field. You
 *	can also mix and match, with 1 always in the number field, & a ';'
 *	between positions #2 and #3 at all times.  If you start changing the
 *	code, things get truly wild.  I think that the best way to handle this
 *	is to put all the flags that can go after the number in the flags
 *	field, with a ';' terminating the number and the 'R' if it's there,
 *	with a final flag of 'O', which forces the modem to go online.
 *	Anything that has to go in front of the number can be put in the
 *	number field.
 *
 *	As to actual usage, the code likes to have the Smartmodem with E0 set
 *	before the code starts running.  Since this is settable via the front
 *	panel (?) switches, this is not a problem, though it can be an
 *	inconvenience.  I don't know that the code will die if the modem
 *	echoes, but you can't test the results from modem commands very well.
 *
 *	Recommended switch settings for the Smartmodem are:
 *		switches 2, 3, 4 down
 *		others as required by system
 */
#include <bdscio.h>
#include <hardware.h>

#define DATAFILE	"call.dat"
#define DEFFCOM		"telnet"

main(argc, argv)
char **argv;
{
char *flags, *number, file[BUFSIZ], buf[MAXLINE], *command;

	if (argc < 2 || argc > 3)
		barf("usage: call <system> [-|<command>]\n");
	if (argc == 2)
		command = DEFFCOM;
	else if (argv[2][0] == '-')
		command = NULL;
	else command = argv[2];

	/* make the modem look like I like it */
	setup() ;
	if (fopen(DATAFILE, file) == ERROR)
		barf("Can't open data file!\n");
	if (!findline(file, buf, strlower(argv[1])))
		barf("I don't know the number for that system!\n");
	if (sscanf(buf, "%*s:%s:%s:", number, flags) < 2)
		*flags = NULL;
	sprintf(buf, "AT V1 D%s %s\r", number, flags);
	hayesput(buf);
	if (command)
		exec(command);
}

/*
 * setup - make the modem look nice for me...
 */
setup()
{
	/*
	 * would you believe quiet mode (V0), with a pulse phone (P), and
	 *	a '.' for the escape character?
	 */
	hayesput("AT V0 P S2=46\r");
}
                                                              