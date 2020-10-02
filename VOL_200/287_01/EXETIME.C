#include <stdio.h>
#include <dos.h>

/* #define SCREEN */

struct TFMT1 {
    unsigned char min, hr, hsec, sec;
};

struct TFMT2 {
    unsigned int hrmin, sec;
};

union HHMMSShh {
    struct TFMT2 l;
    struct TFMT1 s;
};

main()
{
    union HHMMSShh time1, time2;
    register int loop1, loop2;
    int frame_nu;

    GRADinit();
#ifdef SCREEN
    setgraph();
#else
    frame_nu=CreateFrame(720,348);
    SelectFrame(frame_nu);
#endif
    PlotType(2); 
    printf("Time is in HH:MM:SS.hh (hh is one-hundredth second portion)\n\n");
    printf("Blank loops     = ");
    dostime(&time1);
    for (loop1=1000; loop1 > 0; loop1--) {
        for (loop2=1000; loop2 > 0; loop2--) ;
    }
    dostime(&time2);
    elapsetime(time1,time2);
    printf("\n");
    printf("HorzLine (1)    = ");
    dostime(&time1);
    for (loop1=1000; loop1 > 0; loop1--) {
        HorzLine(0,0,300,1);
    }
    dostime(&time2);
    elapsetime(time1,time2);
    printf("\n");
    printf("HorzLine (20)   = ");
    dostime(&time1);
    for (loop1=1000; loop1 > 0; loop1--) {
        HorzLine(0,0,300,20);
    }
    dostime(&time2);
    elapsetime(time1,time2);
    printf("\n");
    printf("VertLine (1)    = ");
    dostime(&time1);
    for (loop1=1000; loop1 > 0; loop1--) {
        VertLine(0,0,300,1);
    }
    dostime(&time2);
    elapsetime(time1,time2);
    printf("\n");
    printf("VertLine (2)    = ");
    dostime(&time1);
    for (loop1=1000; loop1 > 0; loop1--) {
        VertLine(0,0,300,2);
    }
    dostime(&time2);
    elapsetime(time1,time2);
    printf("\n");
    printf("VertLine (32)   = ");
    dostime(&time1);
    for (loop1=1000; loop1 > 0; loop1--) {
        VertLine(0,0,300,32);
    }
    dostime(&time2);
    elapsetime(time1,time2);
    printf("\n");
    printf("Circle  (100)   = ");
    dostime(&time1);
    for (loop1=1000; loop1 > 0; loop1--) {
        Circle(360,174,100);
    }
    dostime(&time2);
    elapsetime(time1,time2);
    printf("\n");
    printf("Ellipse(100,100)= ");
    dostime(&time1);
    for (loop1=1000; loop1 > 0; loop1--) {
        Ellipse(360,174,100,100);
    }
    dostime(&time2);
    elapsetime(time1,time2);
    printf("\n");
#ifdef SCREEN
    settext();
#endif
}

dostime(timerec)
union HHMMSShh *timerec;
{
    union REGS inregs, outregs;

    inregs.h.ah=0x2c;
    intdos(&inregs, &outregs);
    timerec->l.hrmin = outregs.x.cx;
    timerec->l.sec = outregs.x.dx;
}

elapsetime(time1,time2)
union HHMMSShh time1, time2;
{
    union HHMMSShh etime;
    int temp, carry;
    
    temp=time2.s.hsec-time1.s.hsec;
    if (temp >= 0) {
         etime.s.hsec=temp;
         carry=0;
    } else {
        etime.s.hsec=100+temp;
        carry=1;
    }
    temp=time2.s.sec-time1.s.sec-carry;
    if (temp >= 0) {
         etime.s.sec=temp;
         carry=0;
    } else {
        etime.s.sec=60+temp;
        carry=1;
    }
    temp=time2.s.min-time1.s.min-carry;
    if (temp >= 0) {
         etime.s.min=temp;
         carry=0;
    } else {
        etime.s.min=60+temp;
        carry=1;
    }
    temp=time2.s.hr-time1.s.hr-carry;
    etime.s.hr=(temp>=0) ? temp : 24+temp;

    printf("%02d:%02d:%02d.%02d",
             etime.s.hr,etime.s.min,etime.s.sec,etime.s.hsec);
}

