/*	fileio2.h	library i/o function header file for dos 2.00
			NEW VERSION - by LPA 1/4/83
*/

struct bufstr{
  unsigned bufflag;		/* control flags etc */
  unsigned char buferet;	/* error return code */
  unsigned char bufuget;	/* unget character storage */
  int bufsize;			/* length of the buffer in bytes */
  int bufnext;			/* next character to be read/written */
  int buffill;			/* the number of characters in buffer */
  int bufsync;			/* for buffer synchronization */
  int buffull;			/* for buffer synchronization */
  unsigned bufhand;		/* the handle for 2.00 files */
  unsigned buffild;     /* the file descriptor for 2.00 files */
  unsigned char bufdata[1];	/* the data buffer */
};

#define BF_READ 1
#define BF_WRITE 2
#define BF_ASCII 4
#define BF_UNGET 8
#define BF_CDEV 0x10
#define BF_DIRTY 0x20
#define BF_ERROR 0x40
#define BF_CRETFLG 0x80

#define MAXFILES 32		/* maximum file number */

extern unsigned char *_opentab[MAXFILES];	/* file table */

struct regval{unsigned ax,bx,cx,dx,si,di,ds,es;};

/*	end of fileio2.h
*/
