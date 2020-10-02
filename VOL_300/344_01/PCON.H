/* HEADER:       (cat #);
   TITLE:        Header file for PCON.C;
   DATE:         06/08/1989;
   SYSTEM:       MS-DOS;
   FILENAME:     PCON.H;
   SEE-ALSO:     pcon.c;
   AUTHOR:       Eric Horner;
   COMPILERS:    Turbo C 2.0;
*/

	/***** Function prototypes/definitions *****/

void print_instructions(void);	/* prints operating information		*/
void strtoupper(char *strptr);  /* converts a string to all upper case  */

#define MAXCODES 26		/* change if code[] table changes!	*/
#define TEXTLEN  20		/* change if explain table changes!	*/
#define MAXPARMS 10	        /* change as needed (arbitrary)		*/

/***** Command line strings and printer control strings *****/

/* The first column (such as "LM20") is the string which appears on the
   command line. The second column (such as "\x1b\x6c\x14") is the string
   of hex characters to be sent to the printer. Change either or both
   columns to suit your own printer!
*/

char *codes[] =
{
    "LM20",	"\x1b\x6c\x14",		/* left margin 20	*/
    "LM5",	"\x1b\x6c\x05",		/* left margin 5	*/
    "LM10",	"\x1b\x6c\x0a",		/* left margin 10	*/
    "LM15",	"\x1b\x6c\x0f",		/* left margin 15	*/
    "SK",	"\x1b\x4e\x06",		/* skip 6 lines		*/
    "NSK",	"\x1b\x4f",		/* no skip zone		*/
    "DW",	"\x1b\x0e",		/* double width chars   */
    "NDW",	"\x14",			/* double width off	*/
    "EXP",	"\x1b\x0e",		/* double width chars   */
    "NEXP",	"\x14",			/* double width off	*/
    "COM",	"\x1b\x0f",		/* compressed mode	*/
    "NCOM",	"\x12",			/* no compressed mode	*/
    "PICA",	"\x1b\x50",		/* pica size chars	*/
    "PICEM",    "\x1b\x50\x1b\x45",	/* emphasized pica	*/
    "NEM",	"\x1b\x46",		/* emphasized off	*/
    "EL",	"\x1b\x4d",		/* elite size chars	*/
    "DS",	"\x1b\x47",		/* double strike mode	*/
    "NDS",	"\x1b\x48",		/* double strike off	*/
    "UND",	"\x1b\x2d\x01",		/* underline mode	*/
    "NUND",	"\x1b\x2d\0",		/* underline off	*/
    "RES",	"\x1b\x40",		/* printer reset	*/
    "ITAL",	"\x1b\x34",		/* italic mode on	*/
    "NITAL",    "\x1b\x35",		/* italic mode off	*/
    "NLQ",	"\x1b\x78\x01",		/* near letter quality  */
    "DOT",	"\x1b\x78\0",		/* dot mode (NLQ off)	*/
    "TEST",	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n"
};
/*    ^			^					*/
/* Command        Sent to printer				*/


/***** error messages *****/

char *ers[] =
{
    "\7 invalid command line parameter(s) - enter PCON alone for info.\n",
    "\7\nToo many parameters (10 max)\n",
    "\7\nUnable to open file:"
};


/***** operator instruction *****/

/* This provides on-screen help when no command line parameters are present.
   Change the command line strings and descriptions here to agree with any
   changes which you make to the *codes[] strings above. */

char *explain[] =
{
    "PRINTER CONTROL UTILITY  V1.0 (EPSON FX80)    ERIC HORNER, 2/11/89\n",
    "All small, unmarked bills to 5559 Lambeth Lane, Rockford, Il 61107\n\n",

    "PCON.COM     - VALID COMMANDS:\n\n",

    "LM5, LM10    - set left margin to 5 or 10.\n",
    "LM15, LM20   - set left margin to 15 or 20.\n",
    "SK, NSK      - skip or no skip.\n",
    "DW, NDW      - double width or no double width.\n",
    "EXP, NEXP    - expanded or not expanded.\n",
    "COM, NCOM    - compressed or no compressed.\n",
    "PICA,        - pica character size.\n",
    "PICEM        - pica size, emphasized.\n",
    "NEM          - emphasis off.\n",
    "EL           - elite size characters.\n",
    "DS, NDS      - double strike or no double strike.\n",
    "UND, NUND    - underline or no underline.\n",
    "RES          - reset printer.\n",
    "ITAL, NITAL  - italics or no italics.\n",
    "NLQ, DOT     - near letter or normal quality.\n",
    "TEST         - prints out the alphabet, to test settings.\n\n",

    "SYNTAX       - pcon <option1> <option2>......<option10>\n"
};
