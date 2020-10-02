struct prtcmd {
    int length,factor,numh,numl;
    char code[8];
};

extern struct prtcmd skp, skd, setp, endg;

extern struct prtcmd header[];
extern int MAXPSIZ[], NHEADER, sixlpi;

struct prtcmd skp = { 4, 2, -1, 3, { 0x1b, 0x25, 0x35, 0, 0,0,0,0} };
struct prtcmd skd = { 4, 1, -1, 3, { 0x1b, 0x25, 0x35, 0, 0,0,0,0} };
struct prtcmd setp = { 0, 1, -1, -1, { 0,0,0,0,0,0,0,0} };
struct prtcmd endg = { 3, 1, -1, -1, { 0x03, 0x02, 0x0d, 0,0,0,0,0} };

struct prtcmd header[] =
    {  { 6, 1, -1, -1, { 0x1b, '*', 0x65, 0x50, 58, 3,0,0 } } ,
       { 6, 1, -1, -1, { 0x1b, '*', 0x67, 0x50, 58, 3,0,0 } } ,
       { 6, 1, -1, -1, { 0x1b, '*', 0x69, 0x50, 58, 3,0,0 } } ,
       { 6, 1, -1, -1, { 0x1b, '*', 0x6a, 0x50, 58, 3,0,0 } } };

int MAXPSIZ[] = { 480, 576, 960, 1152 };

int NHEADER=4;

int sixlpi=12;
