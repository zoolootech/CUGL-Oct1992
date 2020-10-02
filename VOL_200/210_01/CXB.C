/* cxb.c--checks for xbios modules.
   Copyright 7/85 N.T.Carnevale.
   Permission granted for nonprofit use only.
   See my article in the July/August 1986 issue of 
   Micro/Systems Journal pp.72-85.
*/

#include "printf.h"

/* vital definitions */
#include "xb.h"

#define VERSION "v.1.2 7/16/85\n\n"

char *idstring=SIG;
boolean found=FALSE;


main()
{
	struct xbhdr *xbintro;
	struct cpmhdr *cpmintro;
	char *vers,*descr;
	boolean xbexists;

	printf(VERSION);
	xbexists=TRUE;	/* assume there is one */
	/* find start of suspected module */
	cpmintro=0;
	xbintro=(struct xbhdr *)cpmintro->bdosloc;
	while (xbexists) {
		/* now see if the signature is where it should be */
		if (compare(idstring,xbintro->signature,SIGLNTH)) {
			found=TRUE;
			printf("xbios present at %x\n",(int)xbintro);
			vers = &xbintro->vstart;
			descr = &vers[strlen(vers)+1];
			printf("%s\n%s\n\n",vers,descr);
			/* try for the next one */
			xbintro=(struct xbhdr *)xbintro->oldbdos;
		}
		else xbexists=FALSE;
	}
	if (!found) printf("no xbios installed\n");
}


boolean compare(s1,s2,num)
/* compare num bytes pointed to by s1 and s2
   return TRUE if identical, otherwise return FALSE */
char s1[],s2[];
int num;
{
	int i;
	boolean equal;

	for (i=0,equal=TRUE; (i<num) && equal; i++) 
		equal = s1[i]==s2[i];
	return (equal);
}
