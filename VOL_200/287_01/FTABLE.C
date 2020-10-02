#ifdef COLOR
#  ifdef HERC
#    undef COLOR        /* both are defined, use Hercules */
#   endif
#else
#  ifndef HERC
#    define HERC        /* neither is defined, default to Hercules */ 
#  endif
#endif

#ifdef HERC
#  define MAX_XSCN 719
#  define MAX_YSCN 347
#  ifdef HERCHALF
#    define SCREEN_ADR 0xb0000000L
#  else
#    define SCREEN_ADR 0xb8000000L
#  endif
#endif

#ifdef COLOR
#  define MAX_XSCN 639
#  define MAX_YSCN 199
#  define SCREEN_ADR 0xb8000000L
#endif

#define NOT_USED 0
#define USED 1
#define PERMANENT 2

struct fdesc {
  int status;
  int far *faddr;
  int ln_byte;
  int horz, vert;
  int forgx,forgy;
  int fwinx1,fwinx2,fwiny1,fwiny2;
};

extern int far *FRM_ST;
extern int WINX1, WINY1, WINX2, WINY2, XLIMIT, YLIMIT;
extern int ORGX, ORGY;
extern struct fdesc FTABLE[];
extern int LNBYTE, LADDRO, LADDRS, LASTX, LASTY, LASTX2, LASTY2;


extern int SCREEN[];
int far *FRM_ST = (int far *) SCREEN_ADR;


#define NFRAME 10       /* Don't change this number */

struct fdesc FTABLE[NFRAME] =
    { { PERMANENT, (int far *) SCREEN_ADR,
        (MAX_XSCN+8)/8, MAX_XSCN , MAX_YSCN ,
        0, 0, 0, MAX_XSCN, 0, MAX_YSCN }
#ifdef JLASER
    , { PERMANENT, (int far *) 0,
        (2559+8)/8, 2559, 3161, 0, 0, 0, 2559, 0, 3161 }
#endif
     };

int WINX1 = 0, WINY1 = 0, WINX2 = MAX_XSCN, WINY2 = MAX_YSCN;
int XLIMIT = MAX_XSCN, YLIMIT = MAX_YSCN;
int ORGX = 0, ORGY = 0;
int LNBYTE = (MAX_XSCN+8)/8, LADDRO = (MAX_XSCN+8)/8, LADDRS = 0;

