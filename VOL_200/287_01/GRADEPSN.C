struct prtcmd {
    int length,factor,numh,numl;
    char code[8];
};

extern struct prtcmd skp, skd, setp, endg;

extern struct prtcmd header[];
extern int MAXPSIZ[], NHEADER, sixlpi;

struct prtcmd skp = { 4, 3, -1, 2, { 0x1b, 0x33, 0, 0x0a, 0,0,0,0} };
struct prtcmd skd = { 4, 1, -1, 2, { 0x1b, 0x33, 0, 0x0a, 0,0,0,0} };
struct prtcmd setp = { 5, 1, -1, 2, { 0x1b, 0x41, 0, 0x1b, 0x32, 0,0,0} };
struct prtcmd endg = { 1, 1, -1, -1, { 0x0d, 0,0,0,0,0,0,0 } };

struct prtcmd header[] =
    {  { 4, 1, 3, 2, { 0x1b, 0x4b, 0, 0, 0,0,0,0 } } ,
       { 5, 1, 4, 3, { 0x1b, 0x2a, 5, 0, 0, 0,0,0} } ,
       { 5, 1, 4, 3, { 0x1b, 0x2a, 4, 0, 0, 0,0,0} } ,
       { 5, 1, 4, 3, { 0x1b, 0x2a, 6, 0, 0, 0,0,0} } ,
       { 4, 1, 3, 2, { 0x1b, 0x4c, 0, 0, 0,0,0,0} } ,
       { 4, 1, 3, 2, { 0x1b, 0x5a, 0, 0, 0,0,0,0} } };
int MAXPSIZ[] = { 480, 576, 640, 720, 960, 1920 };

int NHEADER=6;

int sixlpi=12;
