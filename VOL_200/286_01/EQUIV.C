#include <stdio.h>
#include <dos.h>
#include <gds.h>

/*
int far *calcaddr(x,y)
int x,y;
{
  long tloc;
  int temp;

  tloc = (y & 0x01) ? 0xba000000L : 0xb8000000L;
  temp = y >> 1;
  tloc += (temp << 4) + (temp << 6) + ((x >> 3) & 0xfffe);
  return ( (int far *) tloc );
}  

set_text()
{
  union REGS inregs;
  union REGS outregs;

  inregs.h.al = 2;
  inregs.h.ah = 0;
  int86(0x10,&inregs,&outregs);
}

set_graph()
{
  union REGS inregs;
  union REGS outregs;

  inregs.h.al = 6;
  inregs.h.ah = 0;
  int86(0x10,&inregs,&outregs);
}
*/

dot(x,y)
int x,y;
{
  int far *addr;

  addr=calcaddr(x,y);
  fr_write(addr,DOTVALUE[x & 0x0f]);
}

vertline(x,y,length)
int x,y,length;
{
  int far *addr, far *downline();
  unsigned int pword;
  int loop;

  addr=calcaddr(x,y);
  LASTY=y;
  pword=DOTVALUE[x & 0x0f];
  while (length-- > 0) {
    fr_write(addr,pword);
    addr=downline(addr);
    }
}

horzline(x,y,length)
int x,y,length;
{
  int far *addr;
  unsigned int pword;
  int len,loop;

  addr=calcaddr(x,y);
  len = (x & 0x0f) + length;
  if (len <= 16) {
    pword=LEFTWORD[x & 0x0f] ^ LEFTWORD[len];
    fr_write(addr,pword);
    return;
    }
  fr_write(addr,LEFTWORD[x & 0x0f]);
  addr++;
  for (loop=len >> 4; loop > 1; loop--,addr++)
    fr_write(addr,0xffff);
  fr_write(addr,RIGHTWORD[len & 0x0f]);
}

circle_pt(ctrx,ctry,x,y)
int ctrx,ctry,x,y;
{
  dot(x+ctrx,y+ctry);
  dot(y+ctrx,x+ctry);
  dot(y+ctrx,-x+ctry);
  dot(x+ctrx,-y+ctry);
  dot(-x+ctrx,-y+ctry);
  dot(-y+ctrx,-x+ctry);
  dot(-y+ctrx,x+ctry);
  dot(-x+ctrx,y+ctry);
}

circle(centerx,centery,radius)
int centerx,centery,radius;
{ 
  int x,y,d, incr1, incr2;

  x=0;
  y=radius;
  incr1=6;
  incr2=10-(radius<<2);
  d=3-(radius<<1);
  while (x < y) {
    circle_pt(centerx,centery,x++,y);
    if (d<0) {
        d+=incr1;
        incr2+=4;
    } else {
        d+=incr2;
        incr2+=8;
        y--;
    }
    incr1+=4;
  }
  if (x==y) circle_pt(centerx,centery,x,y);
}

ellipse_pt(ctrx,ctry,x,y)
int ctrx,ctry,x,y;
{
  dot(x+ctrx,y+ctry);
  dot(x+ctrx,-y+ctry);
  dot(-x+ctrx,-y+ctry);
  dot(-x+ctrx,y+ctry);
}

ellipse(ctrx,ctry,a,b)
int ctrx,ctry,a,b;
{
  long incr1,incr2,a2,b2,d,step1,step2,ptx,pty;
  int x,y;

  x=0;
  y=b;
  a2= (long) a * a;
  b2= (long) b * b;
  step1 = a2 << 2;
  step2 = b2 << 2;
  d= ((b2 - (a2 * b)) << 1) + a2;
  incr1= step2 + (b2 << 1);
  incr2= step1 + step2 + (b2 << 1) - ((a2 * b) << 2);
  ptx=0;
  pty=a2 * y;
  while (ptx < pty) {
    ellipse_pt(ctrx,ctry,x++,y);
    if (d<0)
      d+=incr1;
    else {
      d+=incr2;
      incr2+=step1;
      y--;
      pty-=a2;
      }
    ptx+=b2;
    incr1+=step2;
    incr2+=step2;
    }
  if (ptx == pty) ellipse_pt(ctrx,ctry,x,y);
  y=0;
  x=a;
  d= ((a2 - (b2 * a)) << 1) + b2;
  incr1= step1 + (a2 << 1);
  incr2= step2 + step1 + (a2 << 1) - ((b2 * a) << 2);
  pty=0;
  ptx=b2 * x;
  while (pty < ptx) {
    ellipse_pt(ctrx,ctry,x,y++);
    if (d<0)
      d+=incr1;
    else {
      d+=incr2;
      incr2+=step2;
      x--;
      ptx-=b2;
      }
    pty+=a2;
    incr1+=step1;
    incr2+=step1;
    }
  if (ptx==pty) ellipse_pt(ctrx,ctry,x,y);
}

line(x1,y1,x2,y2)
int x1,y1,x2,y2;
{
  int dx,dy,d,x,y,xend,yend,len,incr1,incr2,xdir,ydir,dir;

  xdir=ydir=0;
  if ((dx=x2-x1) < 0) { dx = -dx; xdir=(-1); }
  if ((dy=y2-y1) < 0) { dy = -dy; ydir=(-1); }
  if (dx == 0) {
    if (xdir)
      vertline(x1,y2,++dy);
    else
      vertline(x1,y1,++dy);
    }
  if (dy == 0) {
    if (ydir)
      horzline(x1,y2,++dx);
    else
      horzline(x1,y1,++dx);
    }
  if ((xdir==(-1)) && (ydir==(-1)))
    dir=1;
  else if (xdir || ydir)
    dir=xdir+ydir;
  else
    dir=1;
  if (dy <= dx) {
    d = (dy << 1) - dx;
    incr1 = dy << 1;
    incr2 = (dy - dx) << 1;
    if (xdir) {
      x=x2; y=y2; xend=x1;
      }
    else {
      x=x1; y=y1; xend=x2;
      }
    len=1;
    while (x++<xend) {
      if (d<0) {
        d+=incr1;
        len++;
        }
      else {
        horzline(x-len,y,len);
        len=1;
        d+=incr2;
        y+=dir;
        }
      }
    horzline(x-len,y,len);
    }
  else {
    d = (dx << 1) - dy;
    incr1 = dx << 1;
    incr2 = (dx - dy) << 1;
    if (ydir) {
      x=x2; y=y2; yend=y1;
      }
    else {
      x=x1; y=y1; yend=y2;
      }
    len=1;
    while (y++<yend) {
      if (d<0) {
        d+=incr1;
        len++;
        }
      else {
        vertline(x,y-len,len);
        len=1;
        d+=incr2;
        x+=dir;
        }
      }
    vertline(x,y-len,len);
    }
}

main()
{
    setgraph();
    horzline(0,0,640);
    vertline(0,0,200);
    line(0,0,639,199);
    circle(320,100,90);
    ellipse(320,100,300,95);
    getch();
    settext();
}

