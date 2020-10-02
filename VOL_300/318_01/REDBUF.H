/*
	Header for global buffer constants, structures.
	Source:  redbuf.h
	Version: November 15, 1983; January 18, 1990.

	Written by
	
		Edward K. Ream
		166 N. Prospect
		Madison WI 53705
		(608) 257-0802


	PUBLIC DOMAIN SOFTWARE

	This software is in the public domain.

	See red.h for a disclaimer of warranties and other information.
*/

/*
	You may tune these constants for better disk performance.

DATA_SIZE:  The size of that part of struct BLOCK which is
	    written to the disk.  Make sure that DATA_SIZE
	    is a multiple of the size of your disk sectors.

MAX_RES:    The maximum number of BLOCKS resident in memory.
	    The code assumes this number is at least MIN_RES.
	    The actual number of resident blocks is DATA_RES.
	    DATA_RES is set by calling alloc(DATA_SIZE) until
	    MAX_RES blocks are allocated or until alloc() fails.

	    I recommend MAX_RES = 40  so that the small data model
	    can be used.
*/

#define DATA_SIZE 1024
#define MAX_RES	  40	/* must have MIN_RES <= MAX_RES */

/*
	!! Do NOT change the following constant !!

	The MIN_RES constant determines the minimum number of blocks that
	must be allocated for RED to work properly.  MIN_RES must be large
	enough so that the combine routine will work properly.  Determining
	the true lower bound is tricky, and 10 is sure to work.
*/
#define MIN_RES 10

#if MAX_RES < MIN_RES
#error too few buffers specified.
#endif

/*
	Partially define the format of a block.  The end of
	the data area contains an "index table" of indices
	into the front of the data area.  Each index is the
	index of the last byte of a line.

	Thus, it is possible to calculate the starting address
	and length of each line in the block WITHOUT searching
	through either the data field or the index table.

	The d_back and d_next fields in the header are used
	to doubly-link the disk blocks so that stepping
	through the blocks either forward or backwards is
	efficient.  -1 denotes the end of each list.

	When blocks become totally empty they are entered
	on a list of free blocks.  The links of this list
	are kept in the blocks themselves in the d_next field.
	The b_free variable is the head of this list.
*/

/*
	Warning!!  RED will crash if the following two
		   defines are not right.
*/

#define HEADER_SIZE 3*sizeof(int)	/* size of the block header	*/
#define STATUS_SIZE 3*sizeof(int)	/* size of the status table	*/

#define BUFF_SIZE  (DATA_SIZE - HEADER_SIZE)
#define BLOCK_SIZE (DATA_SIZE + STATUS_SIZE)

struct BLOCK {

	/*	The block header.				*/

	int	d_back;		/* # of previous block		*/
	int	d_next;		/* # of next block		*/
	int	d_lines;	/* # of lines on block		*/

	/*	The data area and index table.			*/

	char	d_data [BUFF_SIZE];

	/*	The status table -- not written to disk.	*/

	int	d_lru;		/* lru count		*/
	int	d_status;	/* FULL, FREE or DIRTY	*/
	int	d_diskp;	/* disk pointer		*/
};

/*
	Define the entries in the d_status field.
*/

#define FREE	1	/* status:  block is available	*/
#define FULL	2	/* status:  block is allocated	*/
#define DIRTY	3	/* status:  must swap out	*/

#define bufatbot() (b_line > b_max_line)
#define bufattop() (b_line == 1)
#define bufchng()  b_cflag
#define bufln()	   b_line
#define bufmax()   b_max_line
#define bufnrbot() (b_line >= b_max_line)

/*
	The following are all defined as macros for much greater speed.

	These macros replace corresponding functions in redbuf2.c.
	See those functions for an explanation of what these macros do.
*/
#define b1_avail()              b_avail(b_bp)
#define b1_length(line)         b_length(b_bp, (line))
#define b1_nlines()             (b_bp -> d_lines)
#define b1_prefix(line)         b_prefix(b_bp, (line))
#define b1_ptr(line)            b_ptr(b_bp, (line))
#define b1_settab(index, value)	b_settab(b_bp, (index), (value))
#define b1_tab(index)	        b_tab(b_bp, (index))

#define b_getnum(p)             (* ((int *) (p)))
#define b_putnum(p, num)        (* ((int *) (p)) = (num))

#define b_avail(bp) \
	( ((bp)-> d_lines==0) ?	BUFF_SIZE : \
	  (BUFF_SIZE-b_tab((bp),(bp)->d_lines-1)-(sizeof(int)*bp->d_lines)) )

#define b_length(bp,line) \
	( (bufatbot()) ? 0 : \
	( ((line)==0) ? (b_tab((bp), line)) : \
	  (b_tab((bp),(line)) - b_tab((bp),(line)-1)) ))

#define b_nlines(bp) ((bp) -> d_lines)

#define b_prefix(bp, line) \
	( ((line)==0) ? 0 : (b_tab((bp),(line)-1)) )

#define b_ptr(bp, line) \
	( ((line)==0) ? ((bp)->d_data) : \
	  ((bp)->d_data + b_tab((bp),(line)-1)) )

#define b_settab(bp,index,value) \
	b_putnum((bp)->d_data+BUFF_SIZE-(sizeof(int)*((index)+1)),(value))

#define b_tab(bp, index) \
	b_getnum((bp)->d_data+BUFF_SIZE-(sizeof(int)*((index)+1)))

#define is_dirty(bp) ((bp) -> d_status = DIRTY)
