/*	Externals for CTOA: CRL-to-ASM postprocessor.	*/

/*	Configuration parameters	*/

#define NAMLEN	33		/* Maximum length of a symbol */
#define MAXEXT	150		/* Maximum number of externals per function */
#define MAXLAB	200		/* Maximum number of relocatable labels per
				   function */
#define MAXEXT	200		/* Maximum number of external variables */
#define MAXAUT	50		/* Maximum number of auto variables per fn */
#define MAXPAR	10		/* Maximum number of formal parameters */

#define EXTRNS	0x0115		/* Address of the "extrns" word in C.CCC */

/*	Define Scott Layson's CHARIO interface	*/

#define	CFILE	struct chfile
#define CABS	0		/* Absolute seek */
#define CREL	1		/* Relative seek */
struct chfile {			/* For Scott's CHARIO package */
	int fd;
	int isect;
	int nextc;
	char buff [SECSIZ];
	};

/*	Define the opcode table		*/

struct opc_entry * opctabl ();
struct opc_entry * opc_table;

/*	Particular opcodes of interest	*/

#define	DADB	0x09
#define	DADD	0x19
#define	DADSP	0x39
#define	RET	0xC9

/*	Define the CCC symbol table	*/

struct ccc_entry * ccctabl ();
struct ccc_entry * ccc_table;

/*	Define the source file	*/

char srcfnam [20];		/* Name of the source file */
FILE srcfile;			/* I/O buffer for the source file */
char slopen;			/* Flag = TRUE if part way thru source line */
int sline;			/* Line number relative to function start */
char seof;			/* Flag = TRUE if at EOF in source */
int slevel;			/* Depth of curly-brace nesting in source */
char sname [NAMLEN];		/* Most recent (non-braced) symbol found */
char infunct;			/* Flag = TRUE iff reading a function body */

/*	Define the CRL file, and the structure of the CRL directory.	*/

char crlfnam [20];		/* Name of the .CRL file */
CFILE crlfile;			/* I/O buffer for the .CRL file */
struct {
	char crldtext [512];	/* Function directory in the .CRL file */
	char crleflag;		/* Flag for '-e' option on compile */
	int crleloc;		/* Location specified on '-e' */
	int crlelen;		/* Sixe of external data segment */
	} crldir;

/*	Define the CDB file, and the structure of a CDB entry 	*/

char cdbfnam [20];		/* Name of the .CDB file */
CFILE cdbfile;			/* CHARIO buffer for the .CDB file */
struct {
	char cdbname [8];		/* Name of a symbol */
	char cdbflag1, cdbflag2;	/* Symbol attributes */
	unsigned cdbaddr;		/* Address of the symbol */
	unsigned cdbsize;		/* Size of the item */
	unsigned cdb2dim;		/* Second array dimension */
	} cdbentry;
char havecdb;			/* Flag = TRUE iff .CDB file present */
char cdbopen;			/* Flag = TRUE iff .CDB file is now open */
int ncdbents;			/* Number of entries on the CDB file */

/*	Define the variable tables: external, auto, parameter 	*/

struct vtabent {
	char vtname [9];	/* Name of a variable */
	unsigned vtaddr;	/* Address of the variable */
	};
struct vtabent extvtab [MAXEXT];	/* External variable table */
int nextvs;				/* Number of external vars */
struct vtabent autvtab [MAXAUT];	/* Auto variable table */
int nautvs;				/* Number of auto vars */
struct vtabent parvtab [MAXPAR];	/* Parameter table */
int nparvs;				/* Number of parameters */

/*	Define the characteristics of the function under analysis	*/

int fnno;			/* Ordinal number of function being processed*/
char fname [NAMLEN];		/* Name of the function being processed. */
unsigned faddr;			/* Seek address of the function extern list */
unsigned ftaddr;		/* Seek address of the function body */
unsigned ftlen;			/* Length of the function body */
char * ftext;			/* The function body, stored in heap storage */
unsigned frlen;			/* Length of the relocation information */
unsigned * freloc;		/* The relocation information, in heap */
unsigned framesize;		/* Stack frame size */
int nexts;			/* Number of external function refs */
char xname [MAXEXT] [NAMLEN];	/* Names of the externals */

/*	Working storage for analyzing the function	*/

char pass1;			/* Flag =TRUE for label pass, FALSE for 
					content pass */
unsigned locctr;		/* Current location counter */
unsigned highcode;		/* Function's highest jump address */
unsigned retaddr;		/* Function's highest return address */
char opcode;			/* Current opcode being examined */

/*	Symbol table for the function under analysis	*/

struct {
	unsigned lab_val;	/* Relocatable offset for a label */
	char lab_name [8];	/* Name of the label */
	} lab_tab [MAXLAB];
int nlabs;			/* Number of labels in the table */

/*	Textword must be declared unsigned in advance of using it.	*/

unsigned textword ();
t;			/* The function body, store