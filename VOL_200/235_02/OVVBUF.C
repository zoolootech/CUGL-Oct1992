/* 006  13-Dec-86  ovvbuf.c

        Buffer management routines for ovview.c

        Copyright (c) 1986 by Blue Sky Software.  All rights reserved.
*/

#include <stdio.h>
#include <fcntl.h>
#include "ov.h"

#define MAX_VBUF 8                     /* allow up to 8 in memory buffers */
#define VBUF_LEN 16*1024               /* a buffer is 16K - make a power of 2 */

typedef struct {                       /* define the View BUFfer DEScriptior */
   int buflen;
   long bufoff;
   char far *bufp;
} VBUF_DES;

static int vidx;                       /* current VBUF descriptor in use     */
static int vlast;                      /* last VBUF descriptor in use        */
static int num_vbuf;                   /* highest buffer that can be alloc'd */
static VBUF_DES vbufd[MAX_VBUF];       /* the arrary of VBUF descriptors     */
static long bufoff;                    /* offset in file of current buffer   */
static unsigned int buflen;            /* length of data in current buffer   */
unsigned char far *bufp;               /* begining of data in buffer         */
unsigned char far *curp;               /* current char location in buffer    */
unsigned char far *endp;               /* end of current buffer              */

/*global*/  int ALTCALL vbuf_init(int );
/*global*/  int ALTCALL vbuf_free(void);
/*global*/  unsigned long ALTCALL vtell(void);
/*global*/  int ALTCALL vseek(int ,long );
/*global*/  int ALTCALL vnextch(int );
/*global*/  int ALTCALL vprevch(int );
static  int ALTCALL pagefwd(int ,long );
static  int ALTCALL pagebck(int ,long );
static  int ALTCALL vswitch(int );


/*****************************************************************************
                          V B U F _ I N I T
 *****************************************************************************/

int ALTCALL
vbuf_init(fh)          /* initialize the buffer system */
int fh;
{
   vlast = -1;                         /* set indexes, etc to force an  */
   num_vbuf = MAX_VBUF;                /* so the initial read loads the */
   pagefwd(fh,0L);                     /* first buffer from the file    */
   vswitch(0);                         /* make the 1st buffer active    */
   curp = bufp;                        /* at the 1st char in 1st buffer */
}


/*****************************************************************************
                          V B U F _ F R E E
 *****************************************************************************/

int ALTCALL
vbuf_free() {          /* release all memory buffers */

   register int i;
   register VBUF_DES *vp;

   for (i = 0, vp = vbufd; i <= vlast; i++, vp++) {  /* release all buffers */
      free_f(vp->bufp);
      vp->bufp = NULL;
   }
}


/****************************************************************************
                              V T E L L
 ****************************************************************************/

unsigned long ALTCALL
vtell() {              /* return file offset of current character */

   return(bufoff + (curp - bufp));
}


/*****************************************************************************
                               V S E E K
 *****************************************************************************/

int ALTCALL
vseek(fh,offset)       /* seek to specified offset in file/buffers */
int fh;
long offset;
{
   register int i;
   long readoff, labs();
   register VBUF_DES *vp;

   /* "seek" is easy if offset is in current buffer - normal case I hope */

   if (offset >= bufoff && offset < bufoff + VBUF_LEN) {

      curp = bufp + (offset - bufoff);
      return;

   } else {    /* not in current buffer, is it in any buffer? */

      for (i = 0, vp = vbufd; i <= vlast; i++, vp++)
         if (offset >= vp->bufoff && offset < vp->bufoff + VBUF_LEN) {
            vswitch(i);                        /* switch to buffer with */
            curp = bufp + (offset - bufoff);   /* wanted offset */
            return;
         }
      }

   /* offset location isn't in memory, do it the old fashion way, read it */

   readoff = offset & ~((long)(VBUF_LEN-1));   /* where to read from  */

   /* special case if seeking what would be the next or prev buffer, we
      keep as much buffered in memory as possible */

   if (labs(readoff - bufoff) == VBUF_LEN) {   /* reading next or prev block? */

      if (readoff > bufoff) {                  /* next block? */
         pagefwd(fh,readoff);                  /* swap in next buffer */
         vswitch(vidx+1);                      /* make it active */
      } else {                                 /* previous block */
         pagebck(fh,readoff);                  /* swap in prev buffer */
         vswitch(vidx-1);                      /* make it active */
      }

   } else {            /* not reading next or prev buffer */

      /* seeking to some random position in file, discard all current buffers
         and start over from new position */

      vidx = vlast = 0;                        /* discard all buffers */
      vbufd[0].bufoff = bufoff = readoff;      /* where to read from  */
      bufp = vbufd[0].bufp;                    /* where to read to    */
      l_seek(fh,readoff);
      vbufd[0].buflen = buflen = readbuf(fh,bufp,VBUF_LEN);
      endp = bufp + buflen;
   }

  curp = bufp + (offset - bufoff);             /* got buffer, goto offset */
  return;
}


/****************************************************************************
                              V N E X T C H
 ****************************************************************************/

int ALTCALL
vnextch(fh)            /* return next character from file/buffers */
int fh;
{
   /************** Following code is duplicated by some callers **************/

   if (curp < endp)            /* the normal case is just to return */
      return(*curp++);         /*  the next character in the buffer */

   /**************************************************************************/

   if (buflen < VBUF_LEN)      /* only last buffer can be < VBUF_LEN */
      return(EOF);

   if (vidx == vlast)                          /* read more from file? */
      if (pagefwd(fh,bufoff+VBUF_LEN) == EOF)  /* try paging into the file */
         return(EOF);                          /* watch for EOF */

   vswitch(vidx+1);                    /* switch to next buffer  */
   curp = bufp;                        /* at start of buffer */

   return(*curp++);                    /* return 1st char in it */
}


/****************************************************************************
                              V P R E V C H
 ****************************************************************************/

int ALTCALL
vprevch(fh)            /* return previous character from file/buffers */
int fh;
{
   /************** Following code is duplicated by some callers **************/

   if (curp > bufp)            /* the normal case is just to return */
      return(*--curp);         /*  the prev character in the buffer */

   /**************************************************************************/

   if (bufoff == 0)            /* at top of file? (offset == 0) */
      return(EOF);

   if (vidx == 0)                      /* is this the oldest buffer? */
      pagebck(fh,bufoff-VBUF_LEN);     /*   if so, page backward a buffer */

   vswitch(vidx-1);                    /* switch to prev buffer  */
   curp = endp;                        /* at end of buffer */

   return(*--curp);                    /* return last char in it */
}


/*****************************************************************************
                             P A G E F W D
 *****************************************************************************/

static int ALTCALL
pagefwd(fh,offset)     /* page forward into the file being read */
int fh;
long offset;
{
   int i;
   register VBUF_DES *vp;
   char far *bp, far *malloc_f();

   /* bump the index of the last buffer used, check if all buffer
      descriptors are used, if so reuse the oldest, otherwise try
      to allocate another buffer and use that */

getbuf:                /* look ma, a label for a goto! */

   if (vlast == num_vbuf-1) {          /* all buffer descriptors used? */

      bp = vbufd[0].bufp;        /* remember where buffer is */
      for (i = 0; i < num_vbuf-1; i++) /* throw away oldest buffer by */
         vbufd[i] = vbufd[i+1];        /*  moving others down over it */
      vbufd[vlast].bufp = bp;    /* reuse oldest buffer */
      vidx--;                          /* descriptors were shifted 1 */

   } else {            /* try to allocate another buffer */

      vlast++;                         /* move to the next descriptor */

      /* if we haven't already done so, allocate another buffer for the
         descriptor, failing that use a reduced number of buffers.  We
         need to be able to alloc at least 1 */

      if (vbufd[vlast].bufp == NULL)
         if ((vbufd[vlast].bufp = malloc_f(VBUF_LEN)) == NULL) {
            if (vlast == 0)
               show_error(0,15,1,"No memory for view buffer");
            num_vbuf = vlast--;   /* can't alloc another buffer, operate with */
            goto getbuf;          /*  a reduced number (minimum 1) of buffers */
         }
   }

   /* a descriptor/buffer is ready, fill it with data from the file */

   vp = &vbufd[vlast];                         /* speed things up */
   l_seek(fh,vp->bufoff = offset);             /* where to read from */
   vp->buflen = readbuf(fh,vp->bufp,VBUF_LEN);

   if (vp->buflen == 0)                   /* very unlikley, but maybe EOF */
      return(EOF);
   else
      return(1);
}


/*****************************************************************************
                             P A G E B C K
 *****************************************************************************/

static int ALTCALL
pagebck(fh,offset)     /* page backward into the file being read */
int fh;
long offset;
{
   char far *bp;
   register int i;

   /* reuse the "newest" buffer (last one in descriptor array) */

   bp = vbufd[vlast].bufp;             /* remember where buffer is */
   for (i = vlast; i > 0; i--)         /* throw away newest buffer by */
      vbufd[i] = vbufd[i-1];           /*  moving others over it */

   vbufd[0].bufp = bp;                 /* reuse newest buffer */
   vidx++;                             /* descriptors were shifted 1 */

   /* a descriptor/buffer is ready, fill it with data from the file */

   l_seek(fh,vbufd[0].bufoff = offset);        /* where to read from */
   vbufd[0].buflen = readbuf(fh,bp,VBUF_LEN);
}


/*****************************************************************************
                             V S W I T C H
 *****************************************************************************/

static int ALTCALL
vswitch(idx)           /* make buffer idx the current buffer */
int idx;
{
   register VBUF_DES *vp;

   vp = &vbufd[vidx = idx];    /* assign vidx, address of vidx descriptor */

   bufp = vp->bufp;            /* buffer location */
   bufoff = vp->bufoff;        /* file offset of buffer */
   buflen = vp->buflen;        /* len of data in buffer */
   endp = bufp + buflen;       /* 1 past the last valid data byte in buffer */
}
