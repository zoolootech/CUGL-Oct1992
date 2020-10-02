/* wdtparse.c
 *	a simple parser used by date and time validation funcs.
 *
 *this routine helps parse strings in the form n1/n2/n3/n4/n5.....
 *	into successive numeric values n1, n2, n3, n4,
 *	with each call, the ptr is moved past the next slash.
 * when done , returns 0 for n.
 *
 */
#include "wsys.h"

/*this routine helps parse strings in the form n1/n2/n3/n4/n5.....
 *	into successive numeric values n1, n2, n3, n4,
 *	with each call, the ptr is moved past the next slash.
 * when done , returns 0 for n.
 */
void wdtparse (unsigned int *n_ptr, char **ptr_ptr, char parse_char)
	{
	char *ptr = *ptr_ptr;
	if ( ptr == NULL )
		{
		*n_ptr =0;	
		}
	else
		{
		*n_ptr = atoi ( ptr );
		ptr = strchr ( ptr, parse_char );	/* advance to next slash */	
		if ( ptr != NULL )  ++ptr;	/* adv. beyond slash, if it was present */
		*ptr_ptr = ptr;
		}
	return;
	}

/*----------------- end of WDTPARSE.C ----------------*/
