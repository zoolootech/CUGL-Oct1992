#include <stdio.h>


		/*
		 *	Global structures and definitions
		 */


#define TRUE	1
#define FALSE	0

		/*
		 *	Declare the type of the KEY
		 */

typedef char * KEY;	/* Key = addr returned from malloc */


		/*
		 *	... ditto for the INFO field
		 */

typedef struct {
	int MalCallNum;	/* malloc call number */
	int MalSize;	/* malloc'd size */
	char * MalAddr;	/* malloc'd address */
	struct list *lp;
} INFO;

typedef struct Datum {
	KEY	key;
	INFO	inf;
} DATUM;

		/*
		 *	This is the definition of
		 *	the nodes of the B-Tree
		 */

#define	M	2
typedef struct btnode {
	int			t_active;		/* # of active keys */
	DATUM			t_dat  [2 * M];		/* Keys  + Data */
	struct btnode		*t_ptr [2 * M + 1];	/* Subtree ptrs */
} NODE, *BTREE;

BTREE Insert();

BTREE Delete();

DATUM *Search();

int Apply();
