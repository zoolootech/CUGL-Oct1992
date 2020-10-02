/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* ADDITIONS TO AND REDEFINITIONS FOR THE STANDARD BDSCIO.H FOR BDSC 1.5

	FOR PROGRAMS BY ERIC MARTZ */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* CHARACTER CONSTANTS */

#define BELL '\7'
#define CTRL_Z '\32'
#define ESC	'\033'	/* Standard ASCII 'escape' character	*/
#define META 128
#define NEWLINE '\n'
#define SPACE ' '
#define XON '\21'
#define XOFF '\23'

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* MISCELLANEOUS */

#define YES 1
#define NO 0

#define FOREVER while(1)

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* I/O DIRECTIONS */

#define CON_IN 0 /* SEE PAGE 45 OF MANUAL */
#define CON_OUT 1
#define stderr 4
#define STDERR 4
#define LST 2
#define RDR 3
#define PUN 3

/* LEOR'S NAMES FROM STDLIB1.C */

#define STD_IN	0
#define STD_OUT 1
#define STD_ERR 4

#define DEV_LST 2
#define DEV_RDR 3
#define DEV_PUN 3

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* FILES AND RANDOM FILE I/O */

#define FILE struct _buf
#define FILENAME 16 /* length of char buffer for a filename A:12345678.123 */

struct foffset {
	unsigned record;
	char byte;
};

#define RECSIZ 128 /* SECSIZ is a misnomer except for single density */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* OPERATOR REPLACEMEMTS */

#define EQ ==
#define NE !=
#define GE >=
#define LE <=
#define AND &&
#define OR ||

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* END OF BDSCIO+.H */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
PAGE 45 OF MANUAL */
#define CON_OUT 1
#define stderr 4
#define STDERR 4
#define LST 2
#defin