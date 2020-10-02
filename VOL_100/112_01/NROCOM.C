/*
 *	external "common" for NRO word processor
 *
 *	Stephen L. Browning
 *	5723 North Parker Avenue
 *	Indianapolis, Indiana 46220
 */

struct docctl dc;
struct page pg;
struct _buf oub;
struct _buf *pout;
struct cout co;
struct _buf sofile[NFILES];	/* input file buffers	*/
struct macros mac;
