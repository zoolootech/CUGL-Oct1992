/* FILESIZE.C - get and return filesize.
 */
#include <stdlib.h>
#include <sys\stat.h>

long filesize ( char *spec )
	{
	long l;
	struct stat st;
	
	l = stat ( spec, &st );
	if ( l != -1 )
		{
		l = st.st_size; 
		} 
	return (l);		/* filesize () */
	}