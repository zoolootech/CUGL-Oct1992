#include <stdio.h>
#include <gds.h>
#include <prtfont.h>
#include <fcntl.h>
#include <sys\types.h>
#include <io.h>
#include <sys\stat.h>

#define ERROR (-1)
#define OK 0
#define VOIDBLOCK 1

int far *phyaddr();

/*==============================================================*
 *  This file contains the following routines:                  *
 *                                                              *
 *==============================================================*/

BlockSave(sx,sy,filename,length,height)
int sx,sy,length,height;
char *filename;
{
    int ret, far *sptr, byte_len, outfile, cfrmsave;

    if ((length<=0) || (height <= 0)) return; /* height must be positive */
    sx += ORGX;              /* get absolute position */
    sy += ORGY;

    if (blockclip(&sx,&sy,&length,&height) == VOIDBLOCK) {
        return;
    }

    if ((outfile=open(filename,O_CREAT | O_BINARY| O_TRUNC | O_WRONLY,
                        S_IREAD | S_IWRITE)) == ERROR) {
        graderror(2,4);
        return;
    }

    byte_len=(((sx & 0x000f) + length + 15) >> 4) << 1; 
    ret=((0x20 + (sx & 0x0f)) << 8) + 101; 
    write(outfile, (char *) &ret, 2);
    write(outfile, (char *) &length, 2);
    write(outfile, (char *) &height, 2);
    write(outfile, (char *) &byte_len, 2);
    ret=CUR_PLOT; 
    cfrmsave=CUR_FRAME;
    writetype();
    do { 
        sptr=phyaddr(sx,sy++);
        CUR_FRAME=0;    /* force it to use physical address */
        hlcopy(sptr,(int far *) CFBUF,sx,length);
        CUR_FRAME=cfrmsave;
        if (write(outfile,CFBUF,byte_len) < byte_len) {
            close(outfile);
            PlotType(ret);
            graderror(2,4);
            return;
        }
    } while (--height);
    PlotType(ret); 
    if (close(outfile)==ERROR) { 
        graderror(2,4);
        return;
    }
} 

BlockLoad(dx,dy, filename)
int dx,dy;
char *filename;
{
    int ret, far *dptr, byte_len, infile, sx, length, height, cfrmsave;
    int right, down;

    dx += ORGX;              /* get absolute position */
    dy += ORGY;

    if ((infile=open(filename,O_RDONLY | O_BINARY)) == ERROR) {
        graderror(2,6,filename);
        return(ERROR);
    }

    read(infile, (char *) &ret, 2);
    if ((ret & 0xff) != 101) {
        close(infile);
        graderror(2,5,filename);
        return(ERROR);
    }
    read(infile, (char *) &length, 2);
    read(infile, (char *) &height, 2);
    ret >>= 8;
    sx=ret & 0x0f;
    ret >>= 4;
    if ((ret=(ret<<2)-6) < 0) {
        close(infile);
        graderror(2,5,filename);
        return(ERROR);
    }
    if (ret) {
        read(infile, CFBUF, ret);
    }
    byte_len=((sx + length + 15) >> 4) << 1;
    dx = (dx & 0xfff0) | sx;
    right=dx;
    down=dy;
    if (blockclip(&dx,&dy,&length,&height) == VOIDBLOCK) {
        close(infile);
        return(OK);
    }

    right=dx-right;
    down=dy-down;
    sx+=right;
    right=(sx >> 3) & 0xfffe;
    sx &= 0x0f;
    cfrmsave=CUR_FRAME;
    do { 
        if (read(infile, CFBUF, byte_len) < byte_len) {
            close(infile);
            graderror(2,5,filename);
        }
        if (down-- > 0) continue;
        dptr=phyaddr(dx,dy++);
        CUR_FRAME=0;
        hlcopy((int far *) (CFBUF+right), dptr, sx, length);
        CUR_FRAME=cfrmsave;
        height--;
    } while (height);
    close(infile);
    return(OK);
}

