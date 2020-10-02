/* wprintf
 *	window-limitted printf function
 *
 *	supports all ANSI codes except total # bytes output must be < 500
 */
#include "wsys.h"
#include <stdarg.h>





int wprintf( char *format, ...) {


	char *buffer;
	va_list arg_ptr;
	int num;

	#if __TINY__
		werror("WPRINTF - not supported in tiny model");
	#endif 	/* model is other than TINY, so stack exists  */



	buffer = wmalloc (WPRINTF_LIMIT, "wprintf");


	va_start(arg_ptr, format);


	num = vsprintf(buffer, format, arg_ptr);

	if (WPRINTF_LIMIT < num )
		{
		werror('W', "WPRINTF string too long.");
		}

	va_end(arg_ptr);

	wputs(buffer);


	free (buffer);

	return(num);

	}/* end of wprintf */
