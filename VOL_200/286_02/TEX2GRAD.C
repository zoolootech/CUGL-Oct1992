#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys\types.h>
#include <io.h>
#include <sys\stat.h>

/*==============================================================*
 *                                                              *
 *    This is a program to convert a TEX pixel file (1002 PXL   *
 *    file) to a GRAD font file. Input and output file name     *
 *    can be the same but not recommended.                      *
 *                                                              *
 *    Character position 32 (0x20, ' ') in GRAD is forced to    *
 *    be a space and width equal to 10/16 of the design size.   *
 *    If it is a TeleType (TT) font, you may have to change     *
 *    the width of the space character to become the same as    *
 *    other characters. The magnification field (the last but   *
 *    7 to last but 4 bytes) must contain the number of dot per *
 *    inch the pixel file is for times 5. For 300dpi pixel      *
 *    the field should contain 1500.                            *
 *                                                              *
 *    If you know about TEX pixel file, you should not have     *
 *    any problem understanding the above paragraph.            *
 *    If you don't know about TEX, it is no use looking at      *
 *    this program.                                             *
 *                                                              *
 *==============================================================*/

#define TRUE 1
#define FALSE 0
#define ERROR (-1)
 
#define TOTAL_PATSIZE 32768     /* Maximum pattern size for TEX font file */
 
long get4();
int get2();
 
int infile, outfile;
 
char infilename[64];    /* store input file name */
char outfilename[64];   /* store output file name */

unsigned char pattern[TOTAL_PATSIZE];
 
long pxlid,dirloc,designsize,mag,checksum,designwidth;
unsigned char designheight;
 
int dir_pixwid[128], dir_pixhgt[128];
int dir_leftmargin[128], dir_topline[128];
long dir_ptr[128];
long dir_tfm[128];
 
int dir_bpr[128];
 
unsigned char buffer[32];
 
main(argc, argv)
 
int argc;
char *argv[];
 
{
        int loop1, total, dpi;
        long pos;


        if (argc!=3) {
            printf("Usage: tex2grad <input-PXL> [<output-FON>]\n");
            exit(1);
        }
 
        strcpy(infilename,argv[1]);
        strcpy(outfilename,argv[2]);

        if ((infile=open(infilename,O_RDONLY+O_BINARY)) == ERROR) {
             printf("Can't open input PXL file '%s' for reading.\n",infilename);
             exit(1);
          }

        printf("Reading packed PXL file '%s'.\n",infilename);

/* Begin by checking the pxlIDs. */
        pxlid = get4(infile, 0L, SEEK_SET );
 
        if (pxlid!=1002) {
            printf("Invalid PXL-ID in PXL file, should be 1002: %ld\n",pxlid);
            exit(1);
        }

        pxlid = get4(infile, -4L, SEEK_END );
 
        if (pxlid!=1002) {
            printf("Invalid PXL-ID at end of PXL file, should be 1002: %ld\n"
                      ,pxlid);
            exit(1);
        }

/* Get the rest of the "trailer" information */
        dirloc = get4(infile, -8L,  SEEK_END); 
        if (dirloc>TOTAL_PATSIZE) {
            printf("Requires %ld bytes for pattern information\n",dirloc);
            exit(1);
        }
        designsize = get4(infile, -12L, SEEK_END);
        mag = get4(infile, -16L, SEEK_END); 
        dpi = mag / 5;
        designwidth=((designsize>>4) * dpi / 0x48452L);
        designheight=(unsigned char) ((designwidth + 0x08) >> 4);
        checksum = get4(infile, -20L, SEEK_END); 
        printf("dirloc=%ld, designsize=%ld, mag=%ld, checksum=%ld\n"
                    ,dirloc, designsize, mag, checksum);
        pos = dirloc;
        for (loop1=0; loop1<=127; loop1++) {
            dir_pixwid[loop1] = get2(infile, pos+0, SEEK_SET);
            dir_pixhgt[loop1] = get2(infile, pos+2, SEEK_SET);
            dir_leftmargin[loop1] = -get2(infile, pos+4, SEEK_SET);
            if ((dir_topline[loop1] = get2(infile, pos+6, SEEK_SET)) < 0) {
                dir_topline[loop1] = 0;
            }
            dir_ptr[loop1] = get4(infile, pos+8, SEEK_SET);
            dir_tfm[loop1] = get4(infile, pos+12, SEEK_SET);

            if (dir_ptr[loop1]==0) {
                dir_bpr[loop1]=0;
            } else {
                dir_bpr[loop1]=(dir_pixwid[loop1] + 7) >> 3;
            }
/*            printf("loop1=%d, wid=%d, hgt=%d, ptr=%ld\n"
                 ,loop1,dir_pixwid[loop1],dir_pixhgt[loop1],dir_ptr[loop1]); */
            pos+=16;
        }

/* Next read in the pattern information */

        get(infile, 0L, SEEK_SET, pattern, dirloc);
        close(infile);
 
/* OK.  Now write the unpacked output file */
 
        if ((outfile=open(outfilename,O_WRONLY+O_CREAT+O_TRUNC+O_BINARY,
                            S_IREAD | S_IWRITE))
                == ERROR) {
            printf("Can't open FON file '%s' for writing.\n",outfilename);
            exit(1);
            }
                                      
        printf("Writing data to FON file '%s'.\n",outfilename);

        buffer[0]=51;
        for (loop1=1; loop1<15; loop1++) buffer[loop1]=' ';
        for (loop1=15; loop1<32; loop1++) buffer[loop1]=0;
        buffer[16]=1;
        buffer[17]=128;
        buffer[19]=0;
        buffer[24]=0x7f;
        write(outfile, buffer, 32);

        for (loop1=0; loop1<=127; loop1++) {
            if (loop1==' ') {   /* force character 0x20 as a space */
                                /* width is 9/16 of design size    */
                buffer[0]=(unsigned char ) ((designwidth*9+128) >> 8);
                buffer[1]=buffer[2]=buffer[3]=buffer[4]=0;
                dir_ptr[loop1]=0;
            } else {
                buffer[0]=(unsigned char)
                        ((designwidth * (dir_tfm[loop1] >> 4) + 0x80000) >> 20);
                buffer[1]=dir_pixwid[loop1];
                buffer[2]=dir_pixhgt[loop1];
                buffer[3]=dir_leftmargin[loop1];
                buffer[4]=dir_topline[loop1];
            }
            buffer[5]=designheight;
            write(outfile, buffer, 6);
        }
        for (loop1=0; loop1<=127; loop1++) {
            pos = dir_ptr[loop1];
            if (pos==0) continue;

            total = dir_bpr[loop1] * dir_pixhgt[loop1];
            if (write(outfile,&pattern[pos],total) != total) {
                printf("Write error\n");
                exit(1);
            }
        }

        close(outfile);
 
}

 
/* 
  Here is a function to read data from arbitrary position in a PXL file.
*/
get(fd, pos, origin, buf, n)
int fd, n, origin;
long pos; 
char *buf;
{
        int rc;
 
        lseek(fd, pos, origin);  /* get to proper position */
        if ( (rc = read(fd,buf,n)) < n ) {
            printf("Bad formatted PXL file at %d. Program aborted.\n",rc);
            exit(1);
        }
}

long getn( fd, n )
int fd;
int n;
{
        long num;
        char c;

        num = 0;
        while( n-- ) {
            if( read( fd, &c, 1 ) < 1 ) {
                printf( "Bad PXL file.  Program aborted!\n" );
                exit(1);
            }
            num <<= 8;
            num |= c;
        }


        return( num );
}

long get4( fd, pos, origin )
int fd, origin;
long pos;
{
        lseek(fd, pos, origin);  /* get to proper position */
        return( getn( fd, 4 ) );
}

get2( fd, pos, origin )
int fd, origin;
long pos;
{
        lseek(fd, pos, origin);  /* get to proper position */
        return( (int) getn( fd, 2 ) );
}


