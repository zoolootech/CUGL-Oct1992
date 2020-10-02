/* rxb.c--removes most recently loaded xbios module.
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


main()
{
	struct xbhdr *xbintro;
	char *vers,*descr;

	printf(VERSION);
	/* find start of suspected module */
	cpmintro=0;
	xbintro=(struct xbhdr *)cpmintro->bdosloc;
	/* now see if the signature is where it should be */
	if (compare(idstring,xbintro->signature,SIGLNTH)) {
		printf("removing xbios from %x\n",(int)xbintro);
		vers = &xbintro->vstart;
		descr = &vers[strlen(vers)+1];
		printf("%s\n%s\n\n",vers,descr);
		interrupt(FALSE); /* disable interrupts */
		remove(xbintro);
		interrupt(TRUE); /* reinstate interrupts */
	}
	else printf("no xbios to remove\n");
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


interrupt(choice)
/* if choice TRUE, enables interrupts
   else disables them */
boolean choice;
{
	/* not implemented in this version, 
	   since my bios doesn't use interrupts.
	   --use inline code if needed */
}


remove(xbmod)
struct xbhdr *xbmod;
{
	int i;
	struct jumpinstr {
		char jump,	/* the 0xc3 itself */
		     *dest;	/* the target address */
	} *xbjmptbl,*biosjmptbl;

	/* pick up the start of the next xbios module
	   from the one about to be removed */
	printf("new bdos entry will be %x\n",(int)xbmod->oldbdos);
	cpmintro->bdosloc=(char *)xbmod->oldbdos;	/* fix jmp bdos at 5 */
	/* next copy the "old jump table" from the module into the bios */
	xbjmptbl= (struct jumpinstr *)(&xbmod->vstart + 
		(0x00FF & xbmod->otbloffset));	/* mask out hi bits of offset */
	biosjmptbl=(struct jumpinstr *)(cpmintro->wboot);
	for (i=0; i<xbmod->jmpnum; i++)
		(biosjmptbl++)->dest=(xbjmptbl++)->dest;
}
