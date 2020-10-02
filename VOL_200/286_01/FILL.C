#include <stdio.h>
#include <gds.h>

struct rec1 {
    int leftx, rightx, y, direction;
};

typedef struct rec1 fillrec;

static int MAXQ;
static int head, tail, qlen, empty, pattern;
static fillrec *FQUE;

char *alloca();

SolidFill(x,y)
int x,y;
{
    int leftx, rightx, dir;
    unsigned int state;

    x+=ORGX;
    y+=ORGY;
    if (outside(x,y)) return;

    if ((state=stackavail()) < 64+16) {
        graderror(4,9);
    }
    MAXQ=(state-64) >> 3;
    FQUE=(fillrec *)alloca(MAXQ << 3);
    head=tail=qlen=0;
    switch (CUR_PLOT) {
    case 2:
        state=fr_read(calcaddr(x,y)) & DOTVALUE[x & 0x0f];
        if (state) goto and_type;
    case 0:
        empty=0; pattern=0xffff; break;
    case 1:
    case 3:
and_type:
        empty=0xffff; pattern=0; break;
    default:
        graderror(10,10,__FILE__,__LINE__);
    }
    if (hlnlmt(x,y,empty,pattern,&leftx,&rightx)) return;
    queue(leftx, rightx, y, 3);
    while (qlen) {
        dequeue(&leftx,&rightx,&y,&dir);
        if (dir & 0x01) {
            x=leftx;
            do {
                x=filline(x,leftx,rightx,y-1,1,3);
            } while (x<=rightx);
        }
        if (dir & 0x02) {
            x=leftx;
            do {
                x=filline(x,leftx,rightx,y+1,2,3);
            } while (x<=rightx) ;
        }
    }
}

queue(leftx,rightx,y,direction)
int leftx,rightx,y,direction;
{
    if (y==WINY2) {
        direction &= 0x01;
    }
    if (y==WINY1) {
        direction &= 0x02;
    }
    if (direction==0) return;
    if (qlen++ >= MAXQ) {
        qlen--;
        return;
    }
    FQUE[tail].leftx=leftx;
    FQUE[tail].rightx=rightx;
    FQUE[tail].y=y;
    FQUE[tail].direction=direction;
    if (++tail >= MAXQ)
        tail=0;
}

dequeue(leftx,rightx,y,direction)
int *leftx, *rightx, *y, *direction;
{
/*    if (qlen <= 0) {
        graderror(4,11,"Stack Underflow");
    } */
    qlen--;
    *leftx=FQUE[head].leftx;
    *rightx=FQUE[head].rightx;
    *y=FQUE[head].y;
    *direction=FQUE[head].direction;
    if (++head >= MAXQ)
        head=0;
}

filline(x, leftx,rightx, y, cur_dir, allow_dir)
int x, leftx, rightx, y, cur_dir, allow_dir;
{
    int newleftx, newrightx, newdir;

    x=skipright(x,y,~empty, rightx);
    if (x <= rightx) {
        hlnlmt(x,y,empty, pattern, &newleftx, &newrightx);
        if ((cur_dir == allow_dir) ||
               (newleftx < leftx) || (newrightx > rightx)) {
            newdir=allow_dir;
        } else {
            newdir=cur_dir;
        }
        queue(newleftx, newrightx, y, newdir);
        x=newrightx+2;
    }
    return(x);
}

PatternFill(x,y,patptr,flag)
int x,y, flag;
int *patptr;
{
    int leftx, rightx, dir;
    unsigned int state;

    x+=ORGX;
    y+=ORGY;
    if (outside(x,y)) return;

    if ((state=stackavail()) < 64+16) {
        graderror(4,9);
    }
    MAXQ=(state-64) >> 3;
    FQUE=(fillrec *)alloca(MAXQ << 3);
    head=tail=qlen=0;
    switch (CUR_PLOT) {
    case 2:
        state=fr_read(calcaddr(x,y)) & DOTVALUE[x & 0x0f];
        if (state) goto and_type;
    case 0:
        empty=0; break;
    case 1:
    case 3:
and_type:
        pattern=0; break;
    default:
        graderror(10,10,__FILE__,__LINE__);
    }
    if (hlnlmt(x,y,empty,patptr[y & 0x0f],&leftx,&rightx)) return;
    queue(leftx, rightx, y, 3);
    while (qlen) {
        dequeue(&leftx,&rightx,&y,&dir);
        if (dir & 0x01) {
            x=leftx;
            do {
                pattern=patptr[(y-1) & 0x0f];
                x=filline(x,leftx,rightx,y-1,1,1);
            } while (x<=rightx);
        }
        if (dir & 0x02) {
            x=leftx;
            do {
                pattern=patptr[(y+1) & 0x0f];
                x=filline(x,leftx,rightx,y+1,2,2);
            } while (x<=rightx);
        }
    }
}

