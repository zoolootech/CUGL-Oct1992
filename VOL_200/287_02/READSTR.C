#include <stdio.h>
#include <gds.h>

ReadStr(buffer, length, startx, starty, mode, cursor1, cursor2)
char *buffer;
int length, startx, starty, mode, cursor1, cursor2;
{
    int ch,count,tmptype,w,h,loop,kbret;

    count=0;
    tmptype=CUR_PLOT;
    for (;;) {
        writec(startx,starty,cursor1,&w,&h,0x38);
        switch (mode & 0x03) {
        case 1:
            writec(startx,starty,cursor1,&w,&h,0);
            break;
        case 3:
            PlotType(2);
            writec(startx,starty,cursor1,&w,&h,0);
        case 2:
            PlotType(2);
            for(;;) {
                writec(startx,starty,cursor1,&w,&h,mode & 0x01?0x38:0);
                for(loop=TEN_MS; loop>0; loop--) {
                    if ((kbret=kbhit()) != 0) break;
                }
                writec(startx,starty,cursor1,&w,&h,mode & 0x01?0x38:0);
                if (kbret) break;
                for(loop=TEN_MS; loop>0; loop--) {
                    if ((kbret=kbhit())!=0) break;
                }
                if (kbret) break;
            }
            PlotType(tmptype);
        case 0:
            break;
        }
        ch=getch();
        switch (mode & 0x03) {
        case 1:
            writec(startx,starty,cursor1,&w,&h,0);
            break;
        case 3:
            PlotType(2);
            writec(startx,starty,cursor1,&w,&h,0);
            PlotType(tmptype);
        case 2:
        case 0:
            break;
        }
        writec(startx,starty,cursor1,&w,&h,0x38);
        if (ch==0x0d) break;
        if (ch==0) {
            ch=getch();
        } else if (ch==0x08) {
            if (count) {
                count--;
                PlotType(2);
                writec(startx,starty,*(--buffer),&w,&h,1);
                PlotType(tmptype);
                startx-=w;
            }
        } else {
            count++;
            if (count >= length) {
                count--;
            } else {
                *buffer++ = ch;
                writec(startx,starty,ch,&w,&h,0);
                startx+=w;
            }
        }
    }
    *buffer='\0';
    PlotType(tmptype);
    if (mode & 0x10) {
        writec(startx,starty,cursor2,&w,&h,0);
        startx+=w;
    }
LASTX=startx;
LASTY=starty;
}

