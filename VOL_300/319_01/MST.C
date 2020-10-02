/*
	CPP V5 -- macro symbol table routines.

	source:  mst.c
	started: September 22, 1985
	version: May 26, 1988

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

#include "cpp.h"

/* Declare local routines. */
static int mst_hash	(unsigned char *);

/* Define the hash table used to access macro table nodes. */

#define MAC_PRIME 101
typedef struct mst_node mactab;
static struct mst_node {
	mactab * mst_next; 	/* Pointer to next bucket.	*/
	int	 mst_nargs;	/* Number of args in text.	*/
	char *	 mst_name;    	/* Pointer to name.		*/
	char *	 mst_text;    	/* Pointer to replacement text. */
};
static mactab mst_ht [MAC_PRIME];

/*
	Define the "initial undef list" for
	arguments undefined on the command line.
*/
typedef struct u_node unode;
static struct u_node {
	unode *	u_next;
	char *		u_name;
};
unode undef_list;	/* list header. */

/*
	Remove ALL instances of a symbol from the symbol table.
	This ensures that an #undef will expose any non-macro version
	of a routine.
*/
void
mst_delete (symbol)
register char * symbol;
{
	register mactab *bp1, *bp2;
	int hash;

	TRACEP("mst_delete",printf("entry: symbol=<%s>\n", symbol));
	
	hash = mst_hash(symbol);
	bp1 = & mst_ht [hash];
	bp2 = bp1 -> mst_next;

	while(bp2 != NULL) {

		if(str_eq(symbol, bp2 -> mst_name)) {

			TRACEP("mst_delete", printf("deletes\n"));
			bp1 -> mst_next = bp2 -> mst_next;
			m_free(bp2);
			bp2 = bp1 -> mst_next;
		}
		else {
			bp1 = bp2;
			bp2 = bp2 -> mst_next;
		}
	}
}

/*
	Place a macro in the symbol table along with the number of arguments
	to the macro and a pointer to the macro's replacement text.

	Multiple entries for a symbol can exist in which case the last entry
	defined is active.

	This code conforms to the new standard.  It rejects duplicate 
	definitions of a macro unless the new definition is exactly the
	same as the old.  

	This code also checks the "initial undef list" to see if the first
	definition of a macro should be ignored.  If the macro name is
	found on the list, it is removed from the list and no entry is made
	in the macro symbol table.
*/
void
mst_enter (symbol, txt, nargs)
register char *symbol;
register char *txt;
register int nargs;
{
	register mactab *bp, *bp2;
	int hash;
	unode *up1, *up2;

	TRACEP("mst_enter", printf("(%s, <%s>, %d)\n",
		symbol, pr_str(txt), nargs));

	/*
		Search the global "initial undef list" (created by command line
		-u arguments) to see if the initial definition of name should
		be rejected.  If the name appears on the list, remove it from
		the list and do not enter the name in the macro table.
	*/

	if (undef_list . u_next != NULL) {
		up1 = &undef_list;
		up2 = up1 -> u_next;

		while (up2 != NULL) {
			if (str_eq(symbol, up2 -> u_name)) {
				/* Remove the entry from the list. */
				up1 -> u_next = up2 -> u_next;
				return;
			}
			else {
				up1 = up2;
				up2 = up2 -> u_next;
			}
		}
	}

	/* Search down the list of mst_nodes. */
	hash = mst_hash(symbol);
	bp   = & mst_ht [hash];
	for (bp = bp -> mst_next; bp; bp = bp -> mst_next) {

		/* Reject redefinitions of a macro unless identical. */
		if (str_eq (symbol, bp -> mst_name)) {
			if (nargs == bp -> mst_nargs) {
				if (str_eq(txt, bp -> mst_text)) {
					return;
				}
			}
			err3(	"Non-identical redefinition of ", symbol,
				" ignored.");
			return;
		}
	}

	/* Dynamically allocate space for node. */
	bp2 = m_alloc(sizeof(mactab));
	
	/* Hang node from hash table. */
	bp = & mst_ht [hash];
	bp2 -> mst_next = bp -> mst_next;
	bp  -> mst_next = bp2;

	/* Fill in the name and text fields. */
	bp2 -> mst_name  = str_alloc(symbol);
	bp2 -> mst_text  = str_alloc(txt);
	bp2 -> mst_nargs = nargs;
}

/*
	Return the hash value for symbol.
*/
static int
mst_hash (symbol)
register unsigned char * symbol;
{
	register int hash;

	STAT("mst_hash");

	for (hash = 0; *symbol; ) {
		hash *= 3;
		hash += (int) *symbol++;
		hash %= MAC_PRIME;
	}
	return hash;
}

/*
	Initialize the macro module.
	This must be done BEFORE command line arguments are processed.
*/
void
mst_init()
{
	register int i;

	TICK("mst_init");

	/* Clear the hash table. */
	for (i = 0; i < MAC_PRIME; i++) {
		mst_ht [i] . mst_next = NULL;
		mst_ht [i] . mst_name = "<NAME>";
		mst_ht [i] . mst_text = "<TEXT>";
		mst_ht [i] . mst_nargs = 0;
	}

	/* Clear the "initial undef list." */
	undef_list . u_next = NULL;
}

/*
	Enter the predefined macros into the table.
	Do nothing if the initial definitions have been
	suppressed with -u options.

	This must be done AFTER command line options have been processed.
*/
void
mst2_init()
{
	mst_enter("__LINE__", "", -3);
	mst_enter("__FILE__", "", -3);
	mst_enter("__STDC__", "", -3);
	mst_enter("__TIME__", "", -3);
	mst_enter("__DATE__", "", -3);
}

/*
	Look up a symbol in the macro table.
	Return TRUE if found, and set text and nargs.
*/
bool
mst_lookup(symbol, text, nargs)
register char * symbol;
char ** text;
int *nargs;
{
	register mactab *bp;
	char buffer [100];
	int hash;

	TRACEP("mst_lookup", printf("(%s)\n", symbol));

	/* Calculate the hash value of the symbol. */
	hash = mst_hash(symbol);
	bp = & mst_ht [hash];

	/* Search down the list of mst_nodes. */
	for (bp = bp -> mst_next; bp; bp = bp -> mst_next) {

		if (str_eq(symbol, bp -> mst_name)) {

			/* Special case for __line__ and __file__. */
			if (bp -> mst_nargs == -3) {
				if (str_eq(bp -> mst_name, "__LINE__")) {
					/* Set current line number. */
					conv2s(t_line, buffer);
					bp -> mst_text = str_alloc(buffer);
				}
				else if (str_eq(bp -> mst_name, "__FILE__")) {

					/* Set current file name. */
					strcpy(buffer, "\"");
					strcat(buffer, t_file);
					strcat(buffer, "\"");
					bp -> mst_text = str_alloc(buffer);
				}
				else if (str_eq(bp -> mst_name, "__TIME__")) {
					/* Set current file name. */
					bp -> mst_text = systime();
				}
				else if (str_eq(bp -> mst_name, "__DATE__")) {
					bp -> mst_text = sysdate();
				}
				else if (str_eq(bp -> mst_name, "__STDC__")) {
					bp -> mst_text = "1";
				}
			}

			/* Return success. */
			*nargs = max(-1, bp -> mst_nargs);
			*text  = bp -> mst_text;

			TRACEP("mst_lookup", printf("found:\n"));
			return TRUE;
		}
	}

	TRACEP("mst_lookup", printf("not found\n"));

	/* Return failure. */
	return FALSE;
}

/*
	Add an argument to the "initial undef list."
*/
void
mst_unarg(arg)
char * arg;
{
	unode * p;

	TICK("mst_unarg");

	p = m_alloc(sizeof(unode));
	p -> u_name		= arg;
	p -> u_next		= undef_list . u_next;
	undef_list . u_next	= p;
}
