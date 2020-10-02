#define NFRAME 10
#define MAXNFONT 10

#define ROMFONT_ADR 0xffa6000eL

extern char CFBUF[];

extern int NULLPRT,MAXPH,PBCOUNT;
extern char *PBPTR;

extern struct prtcmd skp, skd, setp, endg;

extern struct prtcmd header[];
extern int MAXPSIZ[], NHEADER;

extern struct fontspec ROMchinfo;
extern struct FONTABLE FONTTAB[];
extern int CURFONT, sixlpi;

