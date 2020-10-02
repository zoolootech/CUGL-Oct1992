/*      sample.c

Sample ITEX 100 program 

*/
#include "itex100.h"
#include "stdtyp.h"
#include <stdio.h>
unsigned char buf[256];
main ()
{FILE *fn;
 int i,line;
 char fname[40];
sethdw(0x300,0xa0000L,PSEUDO_COLOR,1);
setdim(512,512,12);
initialize();
sclear(0,WAIT);
printf("Enter file name:");
scanf("%s",fname);
printf("Enter number of line:");
scanf("%d",&line);
fn=fopen(fname,"rb");
for(i=line;i>0;i--){
    fread(buf,1,256,fn);
    bwhline(0,i,256,buf);
}
fclose(fn);
}
