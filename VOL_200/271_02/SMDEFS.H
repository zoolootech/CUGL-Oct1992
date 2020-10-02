/*
**          SM.LIB  function source code for Turbo-C
**  Copyright 1988, S.E. Margison
**
**  FUNCTION: smdefs.h
** extra defines for compiling programs by S. Margison.
** 1-28-88 A
*/

/* uncomment the following line if you are using Turbo-C Version 1.0 */
/* #define TC10 1 */

#define MAXFN	32	/* max filename space */
#define EXTMARK	'.'	/* delimiter for extension of filename */
#define MAXLINE	256	/* maximum text line space */
#define TRUE	1
#define YES	TRUE
#define FALSE	0
#define NO	FALSE
#define BELL	7	/* control-G ASCII bellcode */


/* following are english language equivalents to logical operators */
#define is ==
#define isnot !=
#define and &&
#define or ||
#define ever (;;)    /* as in "for ever { }" */

#define LP1 0    /* printer numbers */
#define LP2 1
#define LP3 2
#define SER1 0    /* COM1 port */
#define SER2 1    /* COM2 port */
#define SER3 2    /* COM3 port */
#define SER4 3    /* COM4 port */

#define GAMEPORT 0x201        /* gameport address */
#define IDLE 0                /* state definitions for debouncer */
#define INPROG 1
#define WAITOPEN 2

/* function definitions */
extern void _Cdecl aabort(int);
extern void _Cdecl init_game(unsigned char);
extern void _Cdecl clear_game(void);
extern void _Cdecl debounce(void);
extern void _Cdecl eraok(char *);
extern void _Cdecl error(char *);
extern void _Cdecl setport(int, int);
extern void _Cdecl setdtr(int, int);
extern void _Cdecl setrts(int, int);
extern void _Cdecl writechar(int, int);
extern void _Cdecl badext(char *);
extern void _Cdecl basename(char *, char *, char *);
extern void _Cdecl cant(char *);
extern void _Cdecl setcaps(void);
extern void _Cdecl clrcaps(void);
extern void _Cdecl setnumlock(void);
extern void _Cdecl clrnumlock(void);
extern void _Cdecl cls(void);
extern void _Cdecl clrfield(int, int, int, int);
extern void _Cdecl cursor_style(int, int, int);
extern void _Cdecl d_cls(void);
extern void _Cdecl d_pos(int, int, int);
extern void _Cdecl d_say(int, int, char *);
extern void _Cdecl d_saypag(int, int, char *, int);
extern void _Cdecl enterfn(char *, int, int, int, int);
extern void _Cdecl exit2dos(void);
extern void _Cdecl fillch(char *, char, int);
extern void _Cdecl get_code_adr(unsigned int *, unsigned int *, unsigned int *);
extern void _Cdecl get_data_adr(unsigned int *, unsigned int *, unsigned int *);
extern void _Cdecl gtodsub(FILE *);
extern void _Cdecl gtodstr(char *);
extern void _Cdecl loadstr(char *, char *, int);
extern void _Cdecl mkbox(int, int, int, int, int);
extern void _Cdecl mktone(int, int, int);
extern void _Cdecl newext(char *, char *, char *);
extern void _Cdecl pr_putline(char *);
extern void _Cdecl pr_puts(char *);
extern void _Cdecl pr_set(int);
extern void _Cdecl pr_nl(void);
extern void _Cdecl pr_eject(void);
extern void _Cdecl pr_carret(void);
extern void _Cdecl repchar(char, int, FILE *);
extern void _Cdecl rmlead(char *);
extern void _Cdecl rmtrail(char *);
extern void _Cdecl rmallws(char *);
extern void _Cdecl soundon(void);
extern void _Cdecl soundoff(void);
extern void _Cdecl strip(char *);
extern void _Cdecl installtick(int *);
extern void _Cdecl removetick(void);
extern void _Cdecl vidpage(int);
extern void _Cdecl dvid_attrib(int);
extern void _Cdecl dvid_bios(void);
extern void _Cdecl dvid_box(int, int, int, int, int);
extern void _Cdecl dvid_char_at(int, int, char);
extern void _Cdecl dvid_char_atr(int, int, char);
extern void _Cdecl dvid_clrfield(int, int, int);
extern void _Cdecl dvid_cls(void);
extern void _Cdecl dvid_e2eol(void);
extern void _Cdecl dvid_e2eos(void);
extern void _Cdecl dvid_flush(void);
extern void _Cdecl dvid_init(void);
extern void _Cdecl dvid_done(void);
extern void _Cdecl dvid_move(int, int);
extern void _Cdecl dvid_putchr(char);
extern void _Cdecl dvid_sync(int);
extern void _Cdecl dvid_say(int, int, char *);
extern void _Cdecl dvid_sayr(int, int, char *);
extern void _Cdecl dvid_scroll(int, int, int, int, int, int);
extern void _Cdecl dvid_enterfn(char *, int, int, int, int);
extern void _Cdecl dvid_setpage(int, int);
extern void _Cdecl dvid_chgattrib(int, int, int, int, int);
extern void _Cdecl vid_color(int);
extern void _Cdecl vid_border(int);
extern void _Cdecl vid_wrpix(int, int, int);
extern void _Cdecl vmode(int);

extern int _Cdecl argval(char *, int *, int);
extern int _Cdecl dvid_enterdata(char *, int, int, int, int);
extern int _Cdecl dvid_getchr(int, int);
extern int _Cdecl dvid_getattr(void);
extern int _Cdecl vid_rdpix(int, int);
extern int _Cdecl readchar(int);
extern int _Cdecl ready_rcv(int);
extern int _Cdecl ready_xmt(int);
extern int _Cdecl chkdsr(int);
extern int _Cdecl chkdcd(int);
extern int _Cdecl chkcts(int);
extern int _Cdecl chkring(int);
extern int _Cdecl blpr(char, int);
extern int _Cdecl blprstat(int);
extern int _Cdecl center(char *, int);
extern int _Cdecl daynum(int, int, int);
extern int _Cdecl dstr_i(char *, int *);
extern int _Cdecl exists(char *);
extern int _Cdecl enterdata(char *, int, int, int, int);
extern int _Cdecl exttyp(char *, char *);
extern int _Cdecl getdi(int);
extern int _Cdecl getkey(void);
extern int _Cdecl getpath(char *);
extern int _Cdecl ibmtype(void);
extern int _Cdecl iscons(FILE *);
extern int _Cdecl i_dstr(char *, int);
extern int _Cdecl isleap(int);
extern int _Cdecl lexcmp(char *, char *);
extern int _Cdecl openp(char *, int);
extern int _Cdecl opend(char *, int, char *);
extern int _Cdecl openg(char *, int, char *);
extern int _Cdecl pr_putc(char);
extern int _Cdecl _stuff(void);
extern int _Cdecl stuff(int);
extern int _Cdecl weekday(int, int, int);
extern int _Cdecl _kbstate(void);
extern int _Cdecl kbstatus(int);

#ifndef TC10
extern int _Cdecl expand_args(int, char **);
extern int _Cdecl untab(char *, char *, int, int);
extern char **nargv;
#endif

extern unsigned int _Cdecl getpos(int);
extern unsigned int _Cdecl hstr_i(char *);

extern unsigned char _Cdecl crc_clear(void);
extern unsigned char _Cdecl crc_update(unsigned char, unsigned char);
extern unsigned char _Cdecl crc_finish(unsigned char);
extern unsigned char _Cdecl get_press(void);

extern FILE *_Cdecl fopenp(char *, char *);
extern FILE *_Cdecl fopeng(char *, char *, char *);
extern FILE *_Cdecl fopend(char *, char *, char *);

extern char *_Cdecl monthis(int);
extern char *_Cdecl wkdayname(int);

