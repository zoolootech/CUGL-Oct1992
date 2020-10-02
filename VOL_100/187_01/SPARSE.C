/*@*****************************************************/
/*@                                                    */
/*@ sparse.c is a set of routines to maintain a        */
/*@        sparse matrix in free storage.  The         */
/*@        user must define KEYSIZE for his data.      */
/*@                                                    */
/*@*****************************************************/

#include	"misc.mon"

#define		TNAMEMX		13		/* max size of name field */
int dummy;
#define KEYSIZE TNAMEMX+sizeof(dummy)

#include	"blkdef.mon"

/*@********************************************************************** */
/*@                                                                       */
/*@ get_blk retrieves the block with the given key.                       */
/*@                                                                       */
/*@   It returns a pointer to the block, if found or zero, if not found   */
/*@                                                                       */
/*@********************************************************************** */

int get_blk(list, key, keylen, to_blk, blklen)
struct ndx *list;			/* start of list */
char *key;					/* ptr to key for which to search */
int keylen;					/* length of key */
char *to_blk;				/* ptr to area to receive block */
int blklen;					/* length of block (for move)  */
{
	while(key_cmp(key, keylen, list->nkey))
		if ((list = list->next) == 0)
			break;				/* not found */

	if (list)
		cpyblk(list->blk_ptr, to_blk, blklen);
	
	return list;

}


/*@********************************************************************** */
/*@                                                                       */
/*@ key_cmp compares a key in the ndx entry with a given key.             */
/*@                                                                       */
/*@   It returns zero for a match, non-zero otherwise.                    */
/*@                                                                       */
/*@********************************************************************** */

int key_cmp(key, keylen, keyptr)
char *key;				/* ptr to key user's match key */
int keylen;				/* length of key */
char *keyptr;			/* ptr to ndx area key */
{
	while (keylen--)
		if (*key++ != *keyptr++)
			break;
				
	return ++keylen;

}


/*@********************************************************************** */
/*@                                                                       */
/*@ put_blk writes a given block from the specified area.                 */
/*@                                                                       */
/*@   It returns zero for a failure, non-zero otherwise.                  */
/*@                                                                       */
/*@********************************************************************** */

int put_blk(list, key, keylen, from_blk, blklen)
struct ndx *list;			/* place to start search */
char *key;					/* key value use */
int keylen;					/* length of key */
char *from_blk;				/* ptr to area from which to copy */
int blklen;					/* length of block */
{
	struct ndx *prev_ptr;

	while (key_cmp(key, keylen, list->nkey)) {
		prev_ptr = list;			/* save for insert */
		if ((list = list->next) == 0)
			break;					/* not found */
	}

	if (list == 0) {
		if (!add_ndx(prev_ptr, key, keylen, from_blk, blklen))
			return FALSE;
	}
	else
		cpyblk(from_blk, list->blk_ptr, blklen);

	return TRUE;

}



/*@********************************************************************** */
/*@                                                                       */
/*@ add_ndx inserts an index block at the end of the list.                */
/*@      It will initialize it to point to the given block.               */
/*@                                                                       */
/*@   It returns zero for a failure, non-zero otherwise.                  */
/*@                                                                       */
/*@********************************************************************** */

int add_ndx(prev_ptr, key, keylen, from_blk, blklen)
struct ndx *prev_ptr;			/* insert after here */
char *key;						/* key string to use */
int keylen;						/* length of key */
char *from_blk;					/* area pointed to by ndx block */
int blklen;						/* length of block */
{
	char *d;
	struct ndx *p, *b;

	if (!(p = (struct ndx *)malloc (sizeof(queue_hd))))
		return FALSE;			/* error */

	if (!(b = (char *)malloc (blklen)))
		return FALSE;			/* error */

	prev_ptr->next = p;			/* save ptr to ndx entry */
	p->next = 0;				/* new end of list */
	d = p->nkey;				/* copy key info */
	while(keylen--)
		*d++ = *key++;

	p->blk_ptr = b;
	cpyblk(from_blk, b, blklen);

	return TRUE;

}

/*@********************************************************************** */
/*@                                                                       */
/*@ cpyblk copys from the user's area to the stored area or vice-versa.   */
/*@                                                                       */
/*@********************************************************************** */

int cpyblk(area1, area2, length)
char *area1, *area2;
int length;
{
	while(length--)
		*area2++ = *area1++;

}
