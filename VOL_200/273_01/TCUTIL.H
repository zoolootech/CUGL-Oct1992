#if __STDC__
#define _Cdecl
#else
#define _Cdecl  cdecl
#endif

#ifndef va_start
#include <stdarg.h>
#endif

#define BLACK   0x00                    /* foreground */
#define RED     0x04                    /* background */
#define GREEN   0x02                    /* colors */
#define YELLOW  0x06                    /* bg << 4 | fg */
#define BLUE    0x01
#define MAGENTA 0x05
#define CYAN    0x03
#define WHITE   0x07
#define BLINK   0x80
#define BOLD    0x08
#define NDISPB  0x00                    /* non display black */
#define NDISPW  0x77                    /* non display white */
#define RVIDEO  0x70                    /* reverse video */
#define UNLINE  0x01                    /* under line (BLUE) */

/* Values for special keys */

/*function keys*/
#define K_F1  315
#define K_F2  316
#define K_F3  317
#define K_F4  318
#define K_F5  319
#define K_F6  320
#define K_F7  321
#define K_F8  322
#define K_F9  323
#define K_F10 324

/*shifted function keys*/
#define K_SF1  340
#define K_SF2  341
#define K_SF3  342
#define K_SF4  343
#define K_SF5  344
#define K_SF6  345
#define K_SF7  346
#define K_SF8  347
#define K_SF9  348
#define K_SF10 349

/*CONTROL function keys*/
#define K_CF1  350
#define K_CF2  351
#define K_CF3  352
#define K_CF4  353
#define K_CF5  354
#define K_CF6  355
#define K_CF7  356
#define K_CF8  357
#define K_CF9  358
#define K_CF10 359

/*ALT + function keys*/
#define K_AF1  360
#define K_AF2  361
#define K_AF3  362
#define K_AF4  363
#define K_AF5  364
#define K_AF6  365
#define K_AF7  366
#define K_AF8  367
#define K_AF9  368
#define K_AF10 369

/*alternate keys*/
#define K_ALTA 286
#define K_ALTB 304
#define K_ALTC 302
#define K_ALTD 288
#define K_ALTE 274
#define K_ALTF 289
#define K_ALTG 290
#define K_ALTH 291
#define K_ALTI 279
#define K_ALTJ 292
#define K_ALTK 293
#define K_ALTL 294
#define K_ALTM 306
#define K_ALTN 305
#define K_ALTO 280
#define K_ALTP 281
#define K_ALTQ 272
#define K_ALTR 275
#define K_ALTS 287
#define K_ALTT 276
#define K_ALTU 278
#define K_ALTV 303
#define K_ALTW 273
#define K_ALTX 301
#define K_ALTY 277
#define K_ALTZ 300

/* cursor keypad */
#define K_HOME   327
#define K_END    335
#define K_PGUP   329
#define K_PGDN   337
#define K_LEFT   331
#define K_RIGHT  333
#define K_UP     328
#define K_DOWN   336

/*cntl + keypad*/
#define K_CHOME    375
#define K_CEND     373
#define K_CPGUP    388
#define K_CPGDN    374
#define K_CLEFT    371
#define K_CRIGHT   372

/*misc special keys*/
#define K_ESC            27
#define K_SPACE          32
#define K_INS            338
#define K_DEL            339
#define K_TAB            9
#define K_BACKTAB        15
#define K_RETURN         13
#define K_ENTER          13
#define K_BACKSPACE  8


#define setatr(fg,bg,blink,bold) ((blink|(bg<<4))|(fg|bold))
#define setsize_w(trow,tcol,brow,bcol) (((brow-trow+1) * (bcol-tcol+1)) * 2)
#define setsize_ws(trow,tcol,brow,bcol) (((brow-trow+2) * (bcol-tcol+3)) * 2)

/* Function prototypes for tcutilx.lib */
void    _Cdecl  beep(unsigned int pitch, unsigned int nticks );
void    _Cdecl  box(int trow, int tcol, int lrow, int lcol, int wattr, int battr);
long    _Cdecl  calc_tots(char *curr_path);
int     _Cdecl  ccolor(int row, int col, int attr, int len);
int     _Cdecl  change_to(char *dir);
void    _Cdecl  clr(int wattr);
int     _Cdecl  clr_eol(int last_col);
void    _Cdecl  clrarea(int trow,int tcol,int lrow,int lcol,int wattr);
void    _Cdecl  clrbox(int trow,int tcol,int lrow,int lcol,int wattr);
int     _Cdecl  cur_dn();
int     _Cdecl  cur_lf();
int     _Cdecl  cur_nl();
int     _Cdecl  cur_rt();
int     _Cdecl  cur_up();
int     _Cdecl  file_exist(char *fn);
int     _Cdecl  find_file(char *dir_str, char *file_spec, char *dir_hit, char *file_hit,char *srch_type);
int     _Cdecl  flush_key();
int     _Cdecl  get_akey(char *ch, char *list);
int     _Cdecl  get_attr(int row, int col);
int     _Cdecl  get_ca(char *ch, char *scan);
int     _Cdecl  get_chars(int row, int col, int leng, char *string);
int     _Cdecl  get_cur(int *row, int *col);
int     _Cdecl  get_line(char *str, int sleng, int attr, int dleng);
int     _Cdecl  get_xa();
long    _Cdecl  getfsize(char *fn);
int     _Cdecl  hide_cur();
char *  _Cdecl  index(char *str, char c);
int     _Cdecl  left_str(int x, char *str, char *new_str);
int     _Cdecl  locate(int row, int col);
int     _Cdecl  make_shadow(int trow, int tcol, int brow, int bcol);
int     _Cdecl  make_window(int trow, int tcol, int brow, int bcol, int wattr, int battr, int shadow, char *title, char *footer);
int     _Cdecl  melt(int attr, char type);
int     _Cdecl  mid_str(int begin, int leng, char *o_str, char *n_str);
int     _Cdecl  parse_fn(char *fspec,char *fdrive, char *fpath, char *fn, char *fe);
int     _Cdecl  put_ca(char ch, int attr, int count);
int     _Cdecl  put_sa(char *ch, int attr);
void    _Cdecl  rcolor(int row, int col, int attr, int len);
int     _Cdecl  rest_scr(int trow, int tcol, int brow, int bcol, char *array);
int     _Cdecl  right_str(int x, char *str, char *new_str);
int     _Cdecl  s_str_lf(int count, char *str);
int     _Cdecl  s_str_rt(int count, char *str);
int     _Cdecl  save_scr(int trow, int tcol, int brow, int bcol, char *array);
int     _Cdecl  scroll_dn(int trow, int lcol,int brow, int rcol,int attr, int lines);
int     _Cdecl  scroll_lf(int trow, int tcol, int brow, int bcol, int attr, int no_col);
int     _Cdecl  scroll_rt(int trow, int tcol, int brow, int bcol, int attr, int no_col);
int     _Cdecl  scroll_up(int trow, int lcol,int brow, int rcol,int attr, int lines);
int     _Cdecl  set_mode(int mode);
int     _Cdecl  show_cur(int size);
int     _Cdecl  sread_a(int row, int col, int *attr);
int     _Cdecl  sread_c(int row, int col, char *area);
int     _Cdecl  sread_ca(int row, int col, char *area);
int     _Cdecl  str_xform(char *str,char from,char to);
int     _Cdecl  strip(char *str, char c);
int     _Cdecl  striprange(char *str,char clo,char chi);
int     _Cdecl  swrite_a(int row, int col, int attr);
int     _Cdecl  swrite_c(int row, int col, char ch);
int     _Cdecl  swrite_ca(int row, int col, char *area);
int     _Cdecl  trim_l(char *str);
int     _Cdecl  trim_r(char *str);
int     _Cdecl  upcase(char *str);
int     _Cdecl  validate(char *goods, char val_field);
int     _Cdecl  video_type();
int     _Cdecl  write_tty(char *ch);
int     _Cdecl  writef(int row, int col, int attr, char *mch);
int     _Cdecl  writef_n(int row, int col, int attr, char *mch, int count);
int     _Cdecl  writefc(int row, int col, int attr, char mch);
long    _Cdecl  get_long_date();
int     _Cdecl  soundex(char *out_pntr, char *in_pntr);
void    _Cdecl  xprintf(int row, int col,int attr,va_list arg_list, ...);
