/*
**          SM.LIB  function source code
**  Copyright 1986, S.E. Margison
**
**  FUNCTION: smdefs.h
** extra defines for compiling programs by S. Margison.
** 08-18-87 A
*/

#define MAXFN	32	/* max filename space */
#define EXTMARK	'.'	/* delimiter for extension of filename */
#define MAXLINE	192	/* maximum text line space */
#define TRUE	1
#define YES	TRUE
#define FALSE	0
#define NO	FALSE
#define BELL	7	/* control-G ASCII bellcode */

/* following are modes for open() commands */

#define O_READ 0
#define O_WRITE 1
#define O_RDWR 2

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
extern void aabort(int),
            init_game(unsigned char),
            clear_game(void),
            debounce(void),
            eraok(char *),
            error(char *),
            setport(int, int),
            setdtr(int, int),
            setrts(int, int),
            writechar(int, int),
            badext(char *),
            basename(char *, char *, char *),
            cant(char *),
            setcaps(void),
            clrcaps(void),
            setnumlock(void),
            clrnumlock(void),
            cls(void),
            clrfield(int, int, int, int),
            disp_clrfield(int, int, int),
            common_exit(int),
            cursor_style(int, int, int),
            disp_box(int, int, int, int, int),
            disp_cls(void),
            disp_say(int, int, char *),
            disp_sayr(int, int, char *),
            disp_char_at(int, int, char),
            disp_char_atr(int, int, char),
            d_cls(void), d_pos(int, int, int),
            d_say(int, int, char *),
            d_saypag(int, int, char *, int),
            enterfn(char *, int, int, int, int),
            disp_enterfn(char *, int, int, int, int),
            exit2dos(void),
            fillch(char *, char, int),
            get_code_adr(unsigned int *, unsigned int *, unsigned int *),
            get_data_adr(unsigned int *, unsigned int *, unsigned int *),
            get_filetime(struct tm *, int),
            gtodsub(FILE *),
            gtodstr(char *),
            loadstr(char *, char *, int),
            mkbox(int, int, int, int, int),
            mktone(int, int, int),
            newext(char *, char *, char *),
            onexit(int *),
            pr_putline(char *),
            pr_puts(char *),
            pr_set(int),
            pr_nl(void),
            pr_eject(void),
            pr_carret(void),
            repchar(char, int, FILE *),
            rmlead(char *),
            rmtrail(char *),
            rmallws(char *),
            soundon(void),
            soundoff(void),
            strip(char *),
            installtick(int *),
            removetick(void),
            vidpage(int),
            vid_border(int),
            vid_palette(int),
            vid_wrpix(int, int, int),
            vmode(int);

extern int argval(char *, int *, int),
           readchar(int),
           ready_recv(int),
           ready_xmt(int),
           chkdsr(int),
           chkdcd(int),
           chkcts(int),
           chkring(int),
           blpr(char, int),
           blprstat(int),
           center(char *, int),
           criterr(),
           ctlbrk(),
           daynum(int, int, int),
           dstr_i(char *, int*),
           dup(int),
           exists(char *),
           enterdata(char *, int, int, int, int),
           disp_enterdata(char *, int, int, int, int),
           exttyp(char *, char *),
           _getdi(int),
           getkey(void),
           getpath(char *),
           ibmtype(void),
           iscons(FILE *),
           i_dstr(char *, int),
           isleap(int),
           kbhit(void),
           kbstatus(int),
           _kbstate(void),
           lexcmp(char *, char *),
           openp(char *, int),
           opend(char *, int, char *),
           openg(char *, int, char *),
           pr_putc(char),
           _stuff(void),
           stuff(int),
           vid_rdpix(int, int),
           weekday(int, int, int);

extern unsigned int getpos(int),
                    hstr_i(char *);
           

extern unsigned char crc_clear(void),
                     crc_update(unsigned char, unsigned char),
                     crc_finish(unsigned char),
                     get_press(void);

extern FILE *fopenp(char *, char *),
            *fopeng(char *, char *, char *),
            *fopend(char *, char *, char *);

extern char *getenv(),
            *monthis(int),
            *wkdayname(int);

