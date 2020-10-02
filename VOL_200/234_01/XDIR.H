/*
   xdir.h
   Superwash
   to process the DOS directories
*/
struct tdir {
   char d_name[DIRSIZ];
   char d_attrib;
   unsigned int d_time;
   unsigned int d_date;
   long  d_size;
   char  d_path[PATHSIZ];
   };

#define  MAXDIR  150    /* max of 40 directories to process */
#define  MAXFILE 850    /* max data fiules */
int  bildir(void);
int  dpath(int pcount);
int  bilfil(struct tdir *pnam, char *srmask, int cdate, int att);
void dfnam(int fcount, char *cdir);
int  pcomp(int i, int j);
void pswap(int i, int j);
int  fcomp(int i, int j);
void fswap(int i, int j);

void dirform(struct tdir *cp, char *buffer);
int  cvdate(char *buffer);
void typ(char *filename);
void lis(char *filename);
int  get1char(int *cntl);
int  cvatt(char *buffer);
void prin(int mo, char *temp);  /* format function */

void xrename(char *old, char *new);
void xdump(char *fnam);
void xcopy(char *from,  char *to);
void dline(char *ibuff, char *xbuff, long offset);
void disx(struct tdir *text[], int pcount, char *answer, int typ);
int  isval(int val, int typ);
void display(struct tdir *text[], char *answer, int fcount, int typ);
void cvn(char *ibuff, char *obuff);
void cvp(char *ibuff, char *obuff);
void linex(int n, char c);
void boxx(int wide, int deep, int row, int col);
void action(char *answer, int pcount);
int  criteria(char *srmask, int *att, int *cf);
void cypher(char *buffer, char *code, int num);
void dscr(void);
int  fcompd(int i, int j);
int  fcomps(int i, int j);
int  getpath(char *fnam, char *pnam);
void lis(char *filename);
int  secure(char *ifile, char *ofile, char *key, int count);
void upmod(char *fnam, char *atstr);


