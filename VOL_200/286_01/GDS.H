#ifdef COLOR
#  ifdef HERC
#    undef COLOR        /* both are defined, use Hercules */
#   endif
#else
#  ifndef HERC
#    define HERC        /* neither is deinfed, default to Hercules */ 
#  endif
#endif


#ifdef HERC
#  define MAX_XSCN 719
#  define MAX_YSCN 347
#endif

#ifdef COLOR
#  define MAX_XSCN 639
#  define MAX_YSCN 199
#endif

#define NOT_USED 0
#define USED 1
#define PERMANENT 2

#define FALSE 0
#define TRUE 1

#define inside(x,y) x >= WINX1 && x <= WINX2 && y >= WINY1 && y <= WINY2
#define outside(x,y) x < WINX1 || x > WINX2 || y < WINY1 || y > WINY2

struct fdesc {
  int status;
  int far *faddr;
  int ln_byte;
  int horz, vert;
  int forgx,forgy;
  int fwinx1,fwinx2,fwiny1,fwiny2;
};

struct fontspec {
    unsigned char width,inkwidth,height;
      signed char leftmargin;
    unsigned char topline,cellheight;
};

struct FONTABLE {
    int status,type, start, lastchar, defch, direction;
    struct fontspec huge *chinfo;
    char huge *fontaddr;
    long huge *szinfo;
};

struct prtcmd {
    int length,factor,numh,numl;
    char code[8];
};

extern unsigned int DOTVALUE[], LEFTWORD[], RIGHTWORD[];
#ifdef LINT_ARGS
  int far *calcaddr(int,int), far *nor_ptr(int far *), far *phyaddr(int,int);
  int fr_read(int far *), fr_write(int far *, int);
  long dosread(int, char huge *, long);
#else
  int far *calcaddr(), far *nor_ptr(), far *phyaddr();
  int fr_read(),fr_write();
  long dosread();
#endif
extern int far *FRM_ST;
extern int exchange();
extern int CUR_FRAME, CUR_PLOT;
extern unsigned int STYLE;
extern int WINX1, WINY1, WINX2, WINY2, XLIMIT, YLIMIT;
extern int ORGX, ORGY;
extern struct fdesc FTABLE[];
extern int LNBYTE, LADDRO, LADDRS, LASTX, LASTY, LASTX2, LASTY2;
extern int NULLPRT, ARCSPEC;
extern struct FONTABLE FONTTAB[];
extern char CFBUF[];
extern int CURFONT;
extern int TEN_MS;
extern void (*PRE_ERROR_FUNC)(), (*POST_ERROR_FUNC)();
extern int (*SPACING_FUNC)();
extern int PRE_ERROR_LEVEL, POST_ERROR_LEVEL;

