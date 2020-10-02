#include <stdio.h>
#include <a.out.h>
#include <ldfcn.h>
#include "unc.h"

unsigned convtosun(sym)
struct syment *sym;
{
    unsigned suntype;

    /*	everything in shlib is an external constant	*/
    if (sym->n_value >= 0x300000) {
	sym->n_scnum = N_ABS;
     	sym->n_sclass = C_EXT;
    }
    switch(sym->n_scnum) {
    case N_UNDEF:
    case N_DEBUG:	/* call things that don't map well "undefined" */
    case N_TV:
    case P_TV:
	suntype = S_UNDF;
	break;
    case N_ABS:
	suntype = S_ABS;
	break;
    case 1:
	suntype = S_TEXT;
	break;
    case 2:
	suntype = S_DATA;
	break;
    case 3:
	suntype = S_BSS;
	break;
    }

    if (sym->n_sclass == C_EXT)
	suntype = suntype | S_EXT;

    return(suntype);
}
