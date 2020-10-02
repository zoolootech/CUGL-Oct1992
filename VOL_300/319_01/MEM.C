/*
	CPP V5 -- Memory management routines.

	source:  mem.c
	started: September 20, 1985
	version: October 15, 1987; March 24, 1988

	Written by Edward K. Ream.
	This software is in the public domain.

	See the read.me file for disclaimer and other information.
*/

#include "cpp.h"
#ifdef MICRO_SOFT
#include <malloc.h>
#else
#include <alloc.h>
#endif

/* Define local data structures. */
static long m_tot = 0;	/* Total number of bytes allocated.	*/

/*
	Allocate n bytes using calloc(), assumed to get memory from system

	The returned memory IS zeroed.
*/
void *
m_alloc(n)
int n;
{
	register char *p;

	TRACEP("m_alloc", printf("(%d): ", n));

	/* Align the request. */
	while (n & (sizeof(short int)-1) ) {
		n++;
	}

	p = calloc(1, (unsigned) n);
	if (p == NULL) {
		printf("sorry, out of memory\n");
		m_stat();
		exit(BAD_EXIT);
	}

	/* Update statistics. */
	m_tot += (long) n;

	TRACEN("m_alloc", printf("returns %p\n", p));
	return p;
}

/*
	Free memory allocated by m_alloc().
*/
void
m_free(p)
void *p;
{
	TRACEP("m_free", printf("(%p)\n", p));

	if (p == NULL) {
		syserr("m_free: trying to free a NULL pointer.");
	}
	else {
		free(p);
	}
}

/*
	Print statistics about memory manager.
*/
void
m_stat()
{
	TRACEP("m_stat", printf("m_tot: %ld (0x%lx)\n", m_tot, m_tot));
}
