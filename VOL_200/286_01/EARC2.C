#include <stdio.h>
#include <gds.h>

extern int ARCSTTX, ARCSTTY, ARCENDX, ARCENDY, ARCSTTR, ARCENDR;

struct cpoint {
    int point,state,region;
};

extern int arcreg[], regbit[];

extern unsigned int TANVAL[];
 
#define on 1
#define off 0

Earc2(ctrx,ctry,a,b,st_deg,deg)
int ctrx,ctry,a,b;
register int st_deg,deg;
{
    int arcp;
    int region2, rotreg;
    int count1,count2,curcnt;
    unsigned int temp;
    struct cpoint pnt1, pnt2, pnt3, pnt4;

    if (deg<=0) {
setnull:
        LASTX=ctrx;
        LASTY=ctry;
        ARCSTTX=ARCSTTY=ARCENDX=ARCENDY=0;
        return;
    }
    if (deg>=360) Earc2(ctrx,ctry,a,b,0xff);
    ctrx+=ORGX; 
    ctry+=ORGY;

    if ((ctrx+a < WINX1) || (ctrx-a > WINX2) ||
        (ctry+b < WINY1) || (ctry-b > WINY2)) goto setnull;

    seteptr(1);
    arcp=st_deg/360;
    st_deg-=arcp*360;
    if (st_deg<0) st_deg+=360;
    if (a<b)
        temp=(int) (( ((long) a << 16)/b+1)>>1);
    else
        temp=(int) (( ((long) b <<16)/a+1)>>1);
    for(count1=0, count2=46; count1+1 != count2; ) {
        curcnt=(count1+count2)>>1;
        if (temp>=TANVAL[curcnt])
            count1=curcnt;
        else
            count2=curcnt;
    }
    if (a>b) curcnt=90-count1; else curcnt=count1;
    rotreg=(st_deg/90)<<1;
    st_deg %= 90;
    if (st_deg>=curcnt) {
        rotreg++;
        deg-=curcnt;
        count1=90-curcnt;
        count2=curcnt;
    } else {
        count1=curcnt;
        count2=0;
    }
    deg += st_deg;
    region2=(deg/90)<<1;
    deg %= 90;
    if (deg>=count1) {
        region2++;
    }
    deg+=count2;
    if (deg>=90) deg-=90;
    arcp= 0xff ^ arcreg[region2];
    arcp=arcp<<rotreg;
    arcp=(arcp & 0xff) | ((arcp & 0xff00) >> 8);
    region2=(region2+rotreg) & 0x07;
    ARCSTTR=rotreg;
    ARCENDR=region2;
    setpnt(&pnt1,&pnt3,rotreg,on,off,a,b,90-st_deg,st_deg);
    setpnt(&pnt2,&pnt4,region2,off,on,a,b,90-deg,deg);
    if (pnt1.point==0) {
        ARCSTTX=0;
        ARCSTTY=b;
    }
    if (pnt2.point==0) {
        ARCENDX=0;
        ARCENDY=b;
    }
    adjpnt(&arcp,&pnt1,&pnt2,(st_deg<deg),rotreg);
    if (pnt3.point==0) {
        ARCSTTX=0;
        ARCSTTY=a;
    }
    if (pnt4.point==0) {
        ARCENDX=0;
        ARCENDY=a;
    }
    adjpnt(&arcp,&pnt3,&pnt4,(st_deg<deg),rotreg);
    Ell(ctrx,ctry,a,b,arcp & 0xff,STYLE,&pnt1,&pnt2,&pnt3,&pnt4);
    if ((pnt1.region & 0xff) && (pnt1.point >= 0)) {
        ARCSTTX=LASTX2;
        ARCSTTY=LASTY2;
    }
    if ((pnt2.region & 0xff) && (pnt2.point >= 0)) {
        ARCENDX=LASTX2;
        ARCENDY=LASTY2;
    }
    if ((pnt3.region & 0xff) && (pnt3.point >= 0)) {
        ARCSTTX=LASTX;
        ARCSTTY=LASTY;
    }
    if ((pnt4.region & 0xff) && (pnt4.point >= 0)) {
        ARCENDX=LASTX;
        ARCENDY=LASTY;
    }
    LASTX=ctrx;
    LASTY=ctry;
}

