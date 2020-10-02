/* whelp.h  definitions for indexed help files
 *
 * method:  help text is in an ASCII file. New topics are identified by
 *	a '@' in column 1 followed immediately by the topic name.
 *	The text for that topic follows.
 *
 *	The help index file .HX contains the 2-byte count of the number of
 *	entries and then an array of struct HX giving the topics, locations
 *	and number of bytes for the text.
 */


/* size of topics, not counting terminal NULL
 */
#define TOPIC_SZ	45
#define TOPIC_SIZE	45



typedef struct	{
	char 	hxtopic [TOPIC_SZ +1];
	long 	hxpos;
	size_t 	hxsize;
		}
	HX;

#define	MAX_TOPICS	2 * (32000 / sizeof(HX))

/*------------------------ end of whelp.h -----------------------*/