/*
** Basic scheme:
**
** The heap grows up from the top of static storage.
** It is controlled by three pointers, each declared "int *":
**   _heapbase -- first (lowest) byte usable by heap (relative to DS/SS/ES)
**   _heapfree -- address below which no free space exists in heap
**                searches start here
**   _heaptop  -- address of top (dummy) element in heap
**                don't let SP get too close to this!!
**
** Each heap entry -- allocated or free -- is on a 4-byte boundary & has an
** int[2] header.  header[0] is the size -- in halfwords -- of the entry.
** header[1] is the size -- in halfwords -- of the prior entry:  This is used
** to locate the prior entry for garbage collection.  The high-order bit of
** header[1] is set if entry is a free space entry, cleared if entry is
** allocated.
**
** The maximum legal request is 64K - 4.  Clearly, a request of this size
** will never actually be granted.
*/
 
#define PARANOID
 
malloc(size) int size; {
  int *SP, *ptr, *newblk, *nxtblk, oldsize, intcast;
#asm
       MOV [BP]-2,SP ;retrieve current stack ptr value (into "*SP")
#endasm
  if(_heaptop+100 < _heaptop || _heaptop+100 > SP) abort(ENOMEM);
  if(size == 0) return 0; /* return if was only a check for enough stg */
  size = (((size + 7) & (-4))/2) & 32767; /* get size in halfwords,
                                      plus header & rounded up */
  if(size == 0) {errno = EINVAL; return 0;} /* request too big */
  ptr = _heapfree; /* ptr to first place to look for free space */
#ifdef PARANOID
  if((intcast = _heapfree) & 3) abort(EFAULT);
#endif
  _heapfree = (-1); /* will set this during search */
  while(ptr[0]) { /* top (dummy) block is marked as zero length */
#ifdef PARANOID
    if((intcast = ptr) & 3) abort(EFAULT);
    if(ptr[0] & (-32768)) abort(EFAULT);
    if(ptr[0] & 1) abort(EFAULT);
    if(ptr[1] & 1) abort(EFAULT);
#endif
    if(ptr[1] & (-32768)) { /* if free space */
      if(ptr < _heapfree) _heapfree = ptr; /* update free space ptr */
      if(ptr[0] >= size) { /* if space is big enough */
        if(ptr[0] != size) { /* if space is too big */
          oldsize = ptr[0]; /* extract current size of block */
          newblk = ptr + size; /* address of new block to create */
          nxtblk = ptr + ptr[0]; /* address of next block beyond this one */
#ifdef PARANOID
          if(nxtblk[0] & (-32768)) abort(EFAULT);
          if(nxtblk[0] & 1) abort(EFAULT);
          if(nxtblk[1] & 1) abort(EFAULT);
#endif
          ptr[0] = size; /* set new block size */
          newblk[0] = nxtblk[1] = oldsize - size; /* size of remaining space */
          newblk[1] = size | (-32768); /* size of allocated block + flag */
          }
        ptr[1] &= 32767; /* clear free space flag */
        return &ptr[2]; /* address of 1st usable byte */
        }
      }
    ptr += ptr[0]; /* point at next block */
    }
  /* if we get this far, no free block was found that was large enough */
#ifdef PARANOID
  if(ptr != _heaptop) abort(EFAULT);
  if(_heaptop[0] & (-32768)) abort(EFAULT);
  if(_heaptop[0] & 1) abort(EFAULT);
  if(_heaptop[1] & 1) abort(EFAULT);
#endif
  oldsize = (SP - _heaptop) & 32767; /* available space (halfwords) */
  if(oldsize-size < 500) { /* return if no space */
    _heapfree = _heaptop; /* leave free ptr valid */
    errno = ENOMEM;
    return 0;
    }
  ptr[0] = size; /* set size of new block */
  ptr[1] &= 32767; /* clear free space flag */
  _heapfree = _heaptop = ptr + size; /* new top of heap */
  _heaptop[0] = 0; /* new dummy entry */
  _heaptop[1] = size | (-32768); /* size of entry just allocated */
  return &ptr[2]; /* return address of 1st usable byte */
  }
 
free(ptr) int *ptr; {
  int *ptr1, *ptr2, intcast;
  ptr -= 2; /* get address of header */
  if((intcast = ptr) & 3 || ptr < _heapbase || ptr >= _heaptop ||
      ptr[0] & (1-32768) || ptr[1] & (1-32768)) /* check for bad pointer */
    abort(EINVAL); /* pointer (or heap) is corrupted */
  ptr1 = ptr - ptr[1]; /* get address of prior block */
#ifdef PARANOID
  if(ptr1[0] & (-32768)) abort(EFAULT);
  if(ptr1[0] & 1) abort(EFAULT);
  if(ptr1[1] & 1) abort(EFAULT);
#endif
  if(ptr1[1] & (-32768)) { /* if prior block is free */
    ptr1[0] = ptr[0] + ptr[1]; /* combine sizes (careful of 1st block) */
    ptr2 = ptr1 + ptr1[0]; /* address of following block */
#ifdef PARANOID
    if(ptr2[0] & (-32768)) abort(EFAULT);
    if(ptr2[0] & 1) abort(EFAULT);
    if(ptr2[1] & 1) abort(EFAULT);
#endif
    ptr2[1] = ptr1[0] | (ptr2[1] & (-32768)); /* set back pointer */
    ptr = ptr1; /* new free space entry for following check */
    }
  ptr1 = ptr + ptr[0]; /* get address of next block */
#ifdef PARANOID
  if(ptr1[0] & (-32768)) abort(EFAULT);
  if(ptr1[0] & 1) abort(EFAULT);
  if(ptr1[1] & 1) abort(EFAULT);
#endif
  if(ptr1[1] & (-32768)) { /* if next block is free */
    ptr[0] += ptr1[0]; /* combine sizes */
    ptr1 = ptr + ptr[0]; /* address of new next block */
#ifdef PARANOID
    if(ptr1[0] & (-32768)) abort(EFAULT);
    if(ptr1[0] & 1) abort(EFAULT);
    if(ptr1[1] & 1) abort(EFAULT);
#endif
    ptr1[1] = ptr[0] | (ptr1[1] & (-32768)); /* set back ptr */
    }
  ptr[1] |= (-32768); /* mark the block free */
  if(ptr < _heapfree) _heapfree = ptr; /* set free space search ptr */
  if(!ptr1[0]) { /* if next block is top of heap */
#ifdef PARANOID
    if(ptr1 != _heaptop) abort(EFAULT);
#endif
    ptr[0] = 0; /* move it back to current block */
    _heaptop = ptr; /* remember new top */
    }
  }
