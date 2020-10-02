/* wmalloc.c()
 *
 *	general purpose malloc() and farmalloc()
 *    with validation and ptr normalization
 *	also note that in large data models, wmalloc is not generated.
 */

#include "wsys.h"

#undef WANT_NEAR

#ifdef __TINY__
	#define WANT_NEAR
#endif
#ifdef __SMALL__
	#define WANT_NEAR
#endif
#ifdef __MEDIUM__
	#define WANT_NEAR
#endif



#ifdef WANT_NEAR
#define MSG_SIZE  17
static char OUTOF_NEAR[MSG_SIZE+1] = "OUT OF MEMORY in ";

void *wmalloc ( size_t size,  char *errmsg )
	{
	void *ptr;
	#define  MAX_BOTH_MSG  200
	char both_msg[MAX_BOTH_MSG];

	ptr = malloc ( size );

	_NORMALIZE (ptr);	/* model-dependent */

	if (  (ptr==NULL)  &&  (errmsg!=NULL) )
		{
		memcpy (both_msg, OUTOF_NEAR, MSG_SIZE);
		memcpy (both_msg+MSG_SIZE, errmsg, MAX_BOTH_MSG - MSG_SIZE -1);
		werror ( 'W', both_msg );
		}

	return (ptr);

	}
#endif		/* WANT_NEAR */



#define OUTOF_FAR_SIZE 21 
static char OUTOF_FAR[OUTOF_FAR_SIZE+1] = "OUT OF FAR MEMORY in ";

void far *wfarmalloc ( unsigned long nbytes, char *errmsg )
	{
	#define MAX_BOTHFAR_MSG	200
	char bothfar_msg[MAX_BOTHFAR_MSG];
	void far *ptr;

	ptr = farmalloc ( nbytes );

	if (  (ptr==NULL)  &&  (errmsg!=NULL) )
		{
		memcpy (bothfar_msg, OUTOF_FAR, OUTOF_FAR_SIZE);
		memcpy (bothfar_msg+OUTOF_FAR_SIZE, errmsg, 
						MAX_BOTHFAR_MSG - OUTOF_FAR_SIZE -1);
		werror ('W', bothfar_msg );
		}
	NORMALIZE (ptr);
	return (ptr);		/* wfarmalloc */

	}



/*----------------------- end of wmalloc ----------------------*/