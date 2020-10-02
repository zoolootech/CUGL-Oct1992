/*  WREALLOC.C  - shell for realloc() function.
 *
 */
#include "wsys.h"

static char OUTOF[] = "OUT OF MEMORY IN ";

void *wrealloc ( void *ptr, size_t size,  char *errmsg )
	{
	char both_messages[80];

	ptr = realloc ( ptr, size );

	_NORMALIZE (ptr);	/* model-dependent */

	if ( !ptr  && errmsg )
		{
		strcpy (both_messages, OUTOF);
		strcat (both_messages, errmsg);
		werror ( 'W', both_messages );
		}

	return (ptr);

	}
/*------------------- end WREALLOC.C --------------------------*/