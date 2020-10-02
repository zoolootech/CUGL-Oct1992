// FEOW.C - function to ask user "File Exists, Over Write?"
//		PARAMS: char dir = directory name. 		MUST HAVE strlen(dir) < MAXDIR
//				char fn[8+1=3+1] = file name
//
//		User gets chance to change file name, so provide storage for 8+1+3+1
//		RETURNS: ENTER or ESCAPE
//
#include <stdlib.h>
#include <dir.h>
#include <sys\stat.h>
#include  <string.h>
#include "wtwg.h"
#include "dblib.h"
int fe_ow ( char *dir, char *fn )
	{
	int key, len;
	char  msg[200+MAXPATH], path[MAXPATH];
	char  old_fn[8+1+3+1];
	char  new_fn[8+1+3+1];
	
	
	struct stat statbuf;	
	memcpy ( old_fn, fn, sizeof(old_fn) );
	old_fn[ sizeof(old_fn)-1 ] = 0;
	
	key = 0;
	
		
	do	/* loop until old_fn has a  non-duplicate file or ESCAPE */
		{
		strcpy ( path, dir );
		len = strlen (dir);
		if ( len >0 && dir[len-1]!= '\\' )
			{
			/* make sure last char in dir is a \\
			 */
			strcat ( path, "\\" );
			} 	
		
		strcat ( path, old_fn );
	
		if ( -1 ==stat ( path, &statbuf ) )
			{
			errno =0;
			key = ENTER;	/* exits from loop */
			}
		else 
			{
			/* file was found - therefore must be a duplicate
			 */
			sprintf ( msg,  "File named %s already exists"
							"in directory %s\n"
							"Re-Enter the file name, or ESCAPE to quit", fn, dir );
			strcpy ( new_fn, old_fn );
			wsetlocation ( WLOC_ATCUR, 2, 2 );		
			key = wprompts ( " WARNING ", msg, new_fn, sizeof(new_fn) );
			if (key==ENTER)
				{
				strcpy ( old_fn, new_fn );
				key = 0;	/* force repeat of loop to test new name */
				}	/* end if ENTER */
			}	/* end else... file is duplicate */
		}
		while ( ! (key==ENTER || key == ESCAPE) );	/* until valid choice */

	if ( key == ENTER )
		{
		strcpy ( fn, old_fn );		/* give caller back unique filename */
		}
	
	return (key);	/* fe_ow */
	}
	
/*--------------------- end of FEOW.C -------------------------*/