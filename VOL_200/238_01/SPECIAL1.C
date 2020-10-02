#include <stdio.h>
#include <gds.h>
#include <GRADio.h>

int increasing();

int spacing;

main()
{  int x,y;

   setgraph();
   spacing=0;
   SPACING_FUNC=increasing;
   WriteStr(VAR_MOVE,0,10,100,"This is a special test 123456",0,0);
   getch();
   settext();
}

increasing(curx, cury, w, h, remaining)
int *curx, *cury, w, h;
char *remaining;
{
    *curx+=w+spacing;
    spacing+=2;
    return(0);
}

