/* obstack.c - subroutines used implicitly by object stack macros
   Copyright (c) 1986 Free Software Foundation, Inc.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "awk.h"
#include "obstack.h"


STATIC VOID NEAR PASCAL i_obstack_begin(struct obstack *h,
					VOID *(*chunkfun)());
STATIC VOID NEAR PASCAL i_obstack_newchunk(struct obstack *h,
					   VOID *(*chunkfun)(),
					   int length);
STATIC VOID NEAR PASCAL i_obstack_free(struct obstack *h,
				       VOID (*freechunkfun)(),
				       char *obj);


VOID NEAR PASCAL i_obstack_begin(struct obstack *h, VOID *(*chunkfun)())
{
    register OBCHUNK	      *chunk;	   /* points to new chunk */

    chunk	   = h->chunk = (OBCHUNK *) (*chunkfun)(h->chunk_size);
    h->next_free   = h->object_base = chunk->obstack_l_0;
    h->chunk_limit = chunk->obstack_l_limit = (char *) chunk + h->chunk_size;
    chunk->obstack_l_prev = 0;
    return;
}


/* Allocate a new current chunk for the obstack *H
   on the assumption that LENGTH bytes need to be added
   to the current object, or a new object of length LENGTH allocated.
   Copies any partial object from the end of the old chunk
   to the beginning of the new one.  */

VOID NEAR PASCAL i_obstack_newchunk(struct obstack *h, VOID *(*chunkfun)(),
				    int length)
{
    register OBCHUNK   *old_chunk = h->chunk;
    register OBCHUNK   *new_chunk;
    register long	new_size;
    register int	obj_size = h->next_free - h->object_base;

    /* Compute size for new chunk.  */
    new_size = (obj_size + length) << 1;
    if (new_size < h->chunk_size)
	new_size = h->chunk_size;

    /* Allocate and initialize the new chunk.  */
    new_chunk = h->chunk = (OBCHUNK *) (*chunkfun)(new_size);
    if (NULL == new_chunk)
	panic("Out of memory in function i_obstack_newchunk");
    new_chunk->obstack_l_prev = old_chunk;
    new_chunk->obstack_l_limit = h->chunk_limit =
				  (char *) new_chunk + new_size;

    /* Move the existing object to the new chunk.  */
    memcpy(new_chunk->obstack_l_0, h->object_base, obj_size);
    h->object_base = new_chunk->obstack_l_0;
    h->next_free   = h->object_base + obj_size;
    return;
}


VOID NEAR PASCAL i_obstack_free(struct obstack *h, VOID (*freechunkfun)(),
				char *obj)
{
    register OBCHUNK	*lp;	 /* below addr of any objects in this chunk */
    register OBCHUNK	*plp;	 /* point to previous chunk if any */

    lp = (h)->chunk;
    while (lp != 0 && ((char *) lp > obj || (h)->chunk_limit < obj))
    {
	plp = lp->obstack_l_prev;
	(*freechunkfun)(lp);
	if (lp == plp)
	    plp = 0;
	lp = plp;
    }
    if (lp)
    {
	(h)->object_base = (h)->next_free = (char *) (obj);
	(h)->chunk_limit = lp->obstack_l_limit;
	(h)->chunk	 = lp;
    }
    else
	if (obj != 0)	   /* obj is not in any of the chunks! */
	    abort();
    return;
}


VOID PASCAL obstack_begin(OBSTACK *h, int try_length)
{
    h->chunk_size = try_length + sizeof(OBCHUNK);
    h->alignment_mask = (1 << 2) - 1;
    i_obstack_begin(h, obstack_chunk_alloc);
    return;
}


VOID PASCAL obstack_grow(OBSTACK *h, char *where, int length)
{
    if (h->next_free + length > h->chunk_limit)
	i_obstack_newchunk(h, obstack_chunk_alloc, length);

    memcpy(h->next_free, where, length);
    h->next_free += length;
    return;
}


VOID PASCAL obstack_grow0(OBSTACK *h, char *where, int length)
{
    if (h->next_free + length + 1 > h->chunk_limit)
	i_obstack_newchunk(h, obstack_chunk_alloc, length + 1);

    memcpy(h->next_free, where, length);
    h->next_free += length;
    *(h->next_free)++ = 0;
    return;
}


VOID PASCAL obstack_1grow(OBSTACK *h, int datum)
{
    if (h->next_free + 1 > h->chunk_limit)
	i_obstack_newchunk(h, obstack_chunk_alloc, 1);

    *(h->next_free)++ = datum;
    return;
}


VOID PASCAL obstack_blank(OBSTACK *h, int length)
{
    if (h->next_free + length > h->chunk_limit)
	i_obstack_newchunk(h, obstack_chunk_alloc, length);

    h->next_free += length;
    return;
}


VOID * PASCAL obstack_finish(OBSTACK *h)
{
    auto     char	*p, *object_base;

    h->next_free = (char *) ((unsigned long) (h->next_free + h->alignment_mask)
			  & (~((unsigned long) h->alignment_mask)));

    p = (char *) h->chunk;
    if (h->next_free - p > h->chunk_limit - p)
	h->next_free = h->chunk_limit;

    object_base    = h->object_base;
    h->object_base = h->next_free;
    return(object_base);
}


VOID PASCAL obstack_free(OBSTACK *h, char *obj)
{
    auto     int	 len;
    auto     char	*p;

    p	= (char *) h->chunk;
    len = obj - p;
    if (len >= 0 && len < h->chunk_limit - p)
	h->next_free = h->object_base = p + len;
    else
	i_obstack_free(h, obstack_chunk_free, p + len);
    return;
}
