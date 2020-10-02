/*	Program to generate a relocation directory in a .CRL	*/

/*	This code is in the public domain	*/

/*	The following program is intended for applications which need to
	move code around at runtime in a `C' program.  It is called by:

	genrel input-crl-file function-name output-crl-file reloc-fn-name

	It reads input-crl-file, looking for a function called function-name.
	This function may not reference any external functions.  If it finds
	the function, it copies the relocation information associated with
	it on the .CRL file into memory.  It then creates a new .CRL file
	called output-crl-file.  This file will have the single function,
	reloc-fn-name, which returns a pointer to the relocation data.

	An example of how the resulting function might be used is:

union ptf {
	int (*f) ();
	char *b;
	unsigned *w;
	};		/* Define 'pointer-to-function' */

move_fn (to, from, size, reloc) 
    union ptf to;		/* Place in memory to put the new function */
    union ptf from;		/* Place in memory where function is now */
    unsigned size;		/* Size of the function in bytes */
    union ptf reloc;		/* Relocation function built by GENREL */
    {
	unsigned diff;		/* Difference in the loading origins */
	unsigned * reltab;	/* Relocation table */
	unsigned nrelocs;	/* Number of relocation words */
	union ptf relptr;	/* Pointer to word being relocated */

	movmem (from, to, size);	/* Move the function */

	diff = to - from;		/* Find out the relocation offset */
	reltab = (*reloc.f) ();		/* Get the relocation data */
	nrelocs = *reltab++;		/* Get the relocation count */

	while (nrelocs--) {		/* Count relocation entries */
		relptr = to.b + *reltab++; /* Get a relocation entry */
		*relptr.w += diff;	/* Add offset to relocated word */
		}
	}

******************************************************************************/

#include "bdscio.h"

/*	Externals for the package	*/

char crlfile [134];		/* CRL file being read or written via CHARIO */
char crldir [512];		/* Function directory from .CRL file */
union {
	unsigned *w;
	char *b;
	} crlptr;		/* Pointer to current byte in crldir */
char fname [9];			/* Current function name */
char * namptr;			/* Pointer to current byte of fname */
unsigned * reloctab;		/* Relocation table for the new .CRL file */
int nrelocs;			/* Number of relocation bytes */
int seekad;			/* Seek address in current file */
unsigned ending;		/* Length of current file */
unsigned i;			/* Working word */
char c;				/* Working byte */

main (argc, argv)
    int argc;
    char ** argv;
    {
	if (argc != 5) {
		printf ("Syntax: genrel infile function outfile relfunct\n");
		exit ();
		}

	if (copen (crlfile, argv[1], 0) == ERROR) {
		printf ("Can't open %s: %s.\n", argv[1], errmsg (errno ()));
		exit ();
		}

	if (cread (crlfile, crldir, 512) < 512) {
		printf ("Can't read CRL directory from %s: %s\n",
				argv [1], errmsg (errno ()));
		exit ();
		}

	crlptr.b = &crldir;		/* Initialize scan pointer */

	for (;;) {
		if (*crlptr.b == 0x80) {	/* Didn't find the function */
			printf ("Can't locate %s on %s\n", argv[2], argv[1]);
			exit ();
			}
		namptr = &fname;
		while ((*crlptr.b & 0x80) == 0) *namptr++ = *crlptr.b++;
		*namptr++ = *crlptr.b++ & 0x7F;
		*namptr++ = 0;			/* Scan function name */

		if (!strcmp (fname, argv[2])) break;  /* Found the function */
		++crlptr.w;		/* No match; skip seek address */
		}

	seekad = *crlptr.w;		/* Find the function's external dir */

	cseek (crlfile, seekad, 0);	/* Seek to it */
	cread (crlfile, &c, 1);		/* Read the first byte of ext dir */
	if (c) {
		printf ("Function %s on %s has externals and can't be moved\n",
			fname, argv [1]);
		exit ();
		}
	
	cread (crlfile, &seekad, 2);	/* Now read the size of function body*/
	cseek (crlfile, seekad, 1);	/* Seek around it */

	cread (crlfile, &nrelocs, 2);	/* Read relocation item count */
	if ((reloctab = alloc (2*nrelocs)) == NULL) {
		printf ("Can't find enough memory for reloctab.\n");
		exit ();
		}
	cread (crlfile, reloctab, 2*nrelocs);	/* Read relocation data */
	cclose (crlfile);		/* Done with input file */

	if (ccreat (crlfile, argv [3]) == ERROR) {
		printf ("Can't create output file %s: %s.", argv[3],
					errmsg (errno ()));
		exit ();
		}

	crlptr.b = &crldir;
	i = 0;
	namptr = argv [4];
	while (*crlptr.b++ = *namptr++) ++i;
	crlptr.b [-2] |= 0x80;		/* Build output CRL directory */
	cwrite (crlfile, crldir, i);
	seekad = 0x0205;
	cwrite (crlfile, &seekad, 2);  i += 2;
	c=0x80;
	cwrite (crlfile, &c, 1); ++i;
	ending = seekad + 2*nrelocs + 13;
	cwrite (crlfile, &ending, 2); i+=2;
	c=0;
	while (i < seekad) {
		cwrite (crlfile, &c, 1);
		++i;
		}
	cwrite (crlfile, &c, 1);	/* Write external directory terminate*/
	seekad = 2*nrelocs+6;
	cwrite (crlfile, &seekad, 2);	/* Write function body size */
	c=0x21;
	cwrite (crlfile, &c, 1);	/* Write $$$LXI H, table */
	i=4;
	cwrite (crlfile, &i, 2);
	c=0xC9;				/* Write $$$RET */
	cwrite (crlfile, &c, 1);
	cwrite (crlfile, &nrelocs, 2);	/* Write item count */
	cwrite (crlfile, reloctab, 2*nrelocs);	/* Write relocation items */
	i=1;
	cwrite (crlfile, &i, 2);	/* Write real relocation count of 1 */
	cwrite (crlfile, &i, 2);	/* Write real relocation item */
	cflush (crlfile);
	cclose (crlfile);
	}

	i = 0;
	namptr = argv [4];
	while (*crlptr.b++ = *namptr++) ++i;
	crlptr.b [-2] |= 0x80;		/* Build output