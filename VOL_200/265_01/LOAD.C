#include <stdio.h>
#include <dos.h>
#include <ctype.h>

#define TESTING 0
#define MAX_LINE 128
#define DISP_WID 64

/*
   customize these defines to make high-level functions
   conform to the disk format
*/

#define DR_SEL 1
#define SEC_PER_BLOCK 1
#define BLKS_PER_TRACK 18
#define CHARS_PER_BLOCK 512
#define CHARS_PER_SEC CHARS_PER_BLOCK/SEC_PER_BLOCK
#define SEC_MAP 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17
#define ID_STRING "cpio loader for PC-clone reading PC-clone disks"

/*   functions defined in this file */

void printblk( char *);
void readblk(int blk, char *buf);  /* rewrite these three functions */
void initdisk(void);              /* to adapt loader to your */
void restore_disk(void);          /* operating system. */

int map_tab[ BLKS_PER_TRACK ] = { SEC_MAP };

main( argc, argv)
int argc;
char *argv[];
{
char buf[CHARS_PER_BLOCK + 1];
char key_buf[MAX_LINE];
int i, blk, num_blks ;

fprintf(stderr,"\n%s",ID_STRING);
fprintf(stderr,"\nNumber of blocks to load ");
num_blks = atoi(gets(key_buf));

/* clear buffer */
for (i=0; i < CHARS_PER_BLOCK+1; i++, buf[i] = 0);

/* perform load */
initdisk();
for (blk = 0 ; blk < num_blks; blk += 1) {
   readblk(blk,buf);
   if (argc < 2)
      fprintf(stderr,"\n\nBlock %d\n", blk);
   printblk(buf);
   }
restore_disk();
}

void printblk(buf)
char * buf;
{
int i, k;

#if TESTING
for (i=0, k=0; i < CHARS_PER_BLOCK; i++, k++){
   if (isprint(buf[i])) putchar(buf[i]);
   else putchar('.');
   if (k == (DISP_WID - 1)) {
      putchar('\n');
      k = -1;
      }
   }
putchar('\n');

#else
for (i=0; i < CHARS_PER_BLOCK; i++) putchar(buf[i]);

#endif
}

void readblk(blk_num,buf)
int blk_num;
char * buf;

{
union REGS inreg, outreg;
int block_offset, track_offset, mapped_blk;

block_offset = blk_num % BLKS_PER_TRACK;
track_offset = blk_num / BLKS_PER_TRACK;
mapped_blk = map_tab[block_offset] + track_offset * BLKS_PER_TRACK;
#if TESTING
fprintf(stderr,"\nmapped block = %d  ",mapped_blk);
#endif

inreg.h.al = (char) (DR_SEL - 1);
inreg.x.cx = 1;
inreg.x.dx = mapped_blk;
inreg.x.bx = (int) buf;
int86(0x25, &inreg, &outreg);
}

void initdisk()
{
char key_buf[MAX_LINE];

fprintf(stderr,"\nWhen drive light goes out, replace program disk");
fprintf(stderr,"\nwith cpio disk. Press <return> to continue.");
gets(key_buf);
}

void restore_disk()
{
char key_buf[MAX_LINE];

fprintf(stderr,"\nWhen drive light goes out, replace cpio disk");
fprintf(stderr,"\nwith a proper MS-DOS disk. Press <return> to continue.");
gets(key_buf);
}