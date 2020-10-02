/*
   --- Version 2.0 90-10-12 10:33 ---

   TSKALLOC.C - CTask - Dynamic memory allocation interface

   Public Domain Software written by
      Thomas Wagner
      Ferrari electronic Gmbh
      Beusselstrasse 27
      D-1000 Berlin 21
      Germany

   This file is new with Version 1.1

   This module contains the memory allocation functions that are needed
   if TSK_DYNAMIC is defined.

   Version 2.1 adds the tsk_calloc and tsk_realloc functions, and
   provides an option to compile direct malloc replacements. This
   option may be used if the C-runtime library functions have been
   renamed, so that all external functions using the allocation
   routines arrive here. If you have the runtime library source code,
   it is relatively painless to change the names. Without the source,
   you will have to resort to a binary editor to patch the entry names
   directly.
*/

#include "tsk.h"

#if (TSK_TURBO)
#include <alloc.h>
#else
#include <malloc.h>
#endif

/*
   Define MALLOC_REPLACED TRUE if you replaced the C run-time library
   entries for malloc/free/realloc with UPPERCASE names, as suggested 
   in the manual.
*/

#define MALLOC_REPLACED    FALSE

/*
   You can replace the following definitions to use different
   allocation routines if desired.
*/

#if (!MALLOC_REPLACED)

#define xalloc    malloc
#define xcalloc   calloc
#define xrealloc  realloc
#define xfree     free

#else

extern void *MALLOC (size_t size);
extern void *REALLOC (void *buffer, size_t size);
extern void *FREE (void *buffer);

#define xalloc    MALLOC
#define xcalloc   calloc
#define xrealloc  REALLOC
#define xfree     FREE

#endif

resource Neardata alloc_resource;


#if (!MALLOC_REPLACED)


farptr Globalfunc tsk_alloc (word size)
{
   farptr ptr;

   request_resource (&alloc_resource, 0L);
   ptr = (farptr)xalloc (size);
   release_resource (&alloc_resource);

   return ptr;
}


farptr Globalfunc tsk_calloc (word item, word size)
{
   farptr ptr;

   request_resource (&alloc_resource, 0L);
   ptr = (farptr)xcalloc (item, size);
   release_resource (&alloc_resource);

   return ptr;
}


farptr Globalfunc tsk_free (farptr item)
{
   request_resource (&alloc_resource, 0L);
   xfree ((void *)item);   /* Ignore warning in small model */
   release_resource (&alloc_resource);
   return LNULL;
}


farptr Globalfunc tsk_realloc (farptr item, word size)
{
   farptr ptr;

   request_resource (&alloc_resource, 0L);
   ptr = (farptr)xrealloc ((void *)item, size);   /* Ignore warning in small model */
   release_resource (&alloc_resource);
   return ptr;
}


#else

void *malloc (size_t size)
{
   void *area;

   if (ctask_active)
      request_cresource (&alloc_resource, 0L);

   area = xmalloc (size);
                                 
   if (ctask_active)
      release_resource (&alloc_resource);

   return area;
}


void *realloc (void *item, size_t size)
{
   void *area;

   if (ctask_active)
      request_cresource (&alloc_resource, 0L);

   area = xrealloc (item,size);
                                 
   if (ctask_active)
      release_resource (&alloc_resource);

   return area;
}


void free (void *area)
{
   if (ctask_active)
      request_cresource (&alloc_resource, 0L);

   xfree (area);

   if (ctask_active)
      release_resource (&alloc_resource);
}


farptr Globalfunc tsk_alloc (word size)
{
   return (farptr)malloc (size);
}


farptr Globalfunc tsk_calloc (farptr item, word size)
{
   return (farptr)calloc (item, size);
}


farptr Globalfunc tsk_free (farptr item)
{
   free (item);
   return LNULL;
}


farptr Globalfunc tsk_realloc (farptr item, word size)
{
   return (farptr)realloc (item, size);
}

#endif

