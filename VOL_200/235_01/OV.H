/*  040  14-Feb-87  ov.h

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif

#ifndef NULL
#define NULL (0)
#endif

#define SCREEN_COLS (80)
#define SCREEN_ROWS (25)

/* file name display constants */

#define VOL_ROW (1)
#define FILE_STAT_ROW (VOL_ROW+2)
#define TAG_STAT_ROW (FILE_STAT_ROW+1)
#define MASK_ROW (VOL_ROW+2)
#define UP_BOUND (TAG_STAT_ROW+1)
#define FIRST_NROW (UP_BOUND+1)
#define NAME_ROWS (STATUS_ROW-FIRST_NROW)
#define STATUS_ROW (MENU_ROW - 1)
#define MENU_ROW (PROMPT_ROW - 1)
#define PROMPT_ROW (SCREEN_ROWS - 1)

#define PATH_COL (13)
#define VOL_STAT_COL (2)
#define NUM_FILES_COL (31)
#define SPACE_USED_COL (44)
#define MASK_COL (69)

/* view display constants */

#define FIRST_VROW (1)
#define VIEW_ROWS (STATUS_ROW-FIRST_VROW)

/* help display constants */

#define FIRST_HROW (1)
#define HELP_ROWS (STATUS_ROW-FIRST_VROW)

/* configuration parameters */

#define MAX_REPLY (SCREEN_COLS-4)      /* max user reply length to prompt */
#define MAX_DIR 256                    /* max # dir's processed */
#define MAX_FILES 1000                 /* max # files processed in files[] */
#define MAX_PATHLEN 65                 /* max dir pathname length */
#define MAX_VOLLEN 11                  /* max volume label length */
#define MAX_NAMELEN 12                 /* max length of file name */
#define MASK_LEN 10                    /* max len of file selection mask */
#define MAX_DRIVES 20                  /* max drives we will handle */

/* display attribute for file names */

#define DIS_NORM 0             /* normal display */
#define DIS_HIGH 1             /* highlighted display (current file) */
#define DIS_BOX  2             /* dialog box display */
#define DIS_HIBOX 3            /* highlight in dialog box */
#define DIS_HEAD 4             /* header text */
#define DIS_TEXT 5             /* static text display */
#define DIS_TAGD 6             /* tagged display */

/* keyboard mapping */

#define LEFT 128               /* left arrow */
#define RIGHT 129              /* right arrow */
#define UP 130                 /* up arrow */
#define DOWN 131               /* down arrow */
#define PGUP 132               /* page up */
#define PGDN 133               /* page dn */
#define HOME 134               /* home */
#define END 135                /* end */
#define TAG 136                /* tag the current file toggle */
#define HELP 137               /* Help via function key */
#define OPENW 138              /* open file display window */
#define CLOSEW 139             /* close file display window */
#define NEXTW 140              /* goto next file display window */
#define PREVW 141              /* goto prev file display window */
#define INS 142                /* insert toggle key */
#define DEL 143                /* delete char key */
#define GOPAR 144              /* goto parent directory */
#define GOSUB 145              /* goto subdirectory */
#define NEXTT 146              /* goto next tagged file */
#define PREVT 147              /* goto prev taggd file */
#define RUBOUT 8               /* rubout last char key */
#define RETURN 13              /* return key */
#define EOF_CH (0x1A)          /* eof character which will quit overview */
#define ESC_KEY (0x1b)         /* escape char */

/* file attribute flags */

#define TAGGED  (128)          /* file has been tagged */
#define ARCHIVE (32)           /* file modified since backup */
#define DIR     (16)           /* file is a subdirectory */
#define SYSTEM  (4)            /* system file */
#define HIDDEN  (2)            /* hidden file */
#define RDONLY  (1)            /* file is read only */

/* Window update flags */

#define W_PACK (1)             /* pack the files before redisplay */
#define W_DISP (2)             /* just redisplay */


/* internal drive entry structure */

typedef struct drive_ent {
   struct drive_ent *next;             /* pointer to next DRIVE_ENT in list */
   int drive;                          /* drive letter, 'A' - ? */
   unsigned int clustersiz;            /* size of a cluster on volume */
   unsigned long vol_size;             /* size of volume */
   unsigned long vol_free;             /* free space on volume */
   char volbuf[MAX_VOLLEN+1];          /* current volume name buffer */
} DRIVE_ENT;

/* internal file entry structure */

typedef struct file_ent {
   char name[MAX_NAMELEN+1];           /* the file name */
   unsigned char flags;                /* flags for this file */
   unsigned index;                     /* sequential index number */
   unsigned date;                      /* date last modified */
   unsigned time;                      /* time last modified */
   char *dirp;                         /* ptr to dir name for file if showall */
   long size;                          /* file size in bytes or blocks */
} FILE_ENT;

/* window structure */

typedef struct window {
   struct window *next;                /* ptr to next winwow in list */
   struct window *prev;                /* ptr to prev window in list */

   int curidx;                         /* current files[] index */

   int fwrow;                          /* first window display row */
   int wrows;                          /* # window display rows */
   int fnrow;                          /* first name display row */
   int ndrows;                         /* # name display rows */

   int maxlen;                         /* size of the largest file name */
   int colsiz;                         /* size of a name column in chars */

   int ncols;                          /* number of logical columns on screen */
   int nrows;                          /* number of logical rows used 4 files */
   int nbase;                          /* files[] idx of first name displayed */
   int nfiles;                         /* number entries in files[] */

   int updated;                        /* NZ if window needs redisplay */

   DRIVE_ENT *drivep;                  /* pointer to DRIVE_ENT for window */

   unsigned int num_files;             /* number files selected in directory */
   unsigned int num_tagged;            /* number of files tagged */
   unsigned long tag_size;             /* size of tagged files */
   unsigned long files_size;           /* # bytes used by files */

   char far *save_files;               /* ptr to files[] data for window */

   int sortopt;                        /* window sort option */
   int (*sortfunc)();                  /* pointer to sorting function */

   unsigned char info_display;         /* NZ when exteneded file info shown */
   unsigned char showall;              /* NZ when showall mode active */

   unsigned char selatrs;              /* selection attribute flags */
   unsigned char maskcmp;              /* include/exclude meaning of mask */
   char mask[MASK_LEN+1];              /* current file selection mask */

   char dirbuf[MAX_PATHLEN+1];         /* current dir name buffer */
} WINDOW;

/* menu selection entry structure */

typedef struct menu_selection {
   char *choice;
   char *prompt;
   int (*func)();
   struct menu_selection *sub_menu;
} MENU;

/* menu state control block */

#define MAX_MENU_SELECTIONS 20

typedef struct {
   MENU *current_menu;
   int number_selections;
   int current_selection;
   struct {
      unsigned char position;
      unsigned char length;
   } selection[MAX_MENU_SELECTIONS];
} MENU_STATE;

/* misc macros to translate coordinates */

#define min(a,b) ((a < b) ? a : b)             /* min of two values */
#define yes(c) (c == 'Y' || c == 'y')          /* is char == Yy? */

#define lr2sr(r) (cw.fnrow + (r - cw.nbase))   /* logical row to screen row */
#define lc2sc(c) (c * cw.colsiz)               /* logical col to screen col */
#define idx2lr(i) (i % cw.nrows)               /* idx to logical row */
#define idx2lc(i) (i / cw.nrows)               /* idx to logical col */
#define idx2sr(i) (lr2sr(idx2lr(i)))           /* idx to screen row */
#define idx2sc(i) (lc2sc(idx2lc(i)))           /* idx to screen col */

/* function declaration stuff */

#define ALTCALL pascal         /* alternate calling method reduces overhead */

int ALTCALL disp_file(FILE_ENT *, int);        /* common func declarations */
int ALTCALL disp_vol_stats(void);
int ALTCALL disp_file_stats(void);
int ALTCALL setvattrib(int);
int ALTCALL disp_str_at(char *, int, int);
int ALTCALL disp_char_at(int, int, int);
int ALTCALL disp_rep_at(int, int, int, int);
int ALTCALL out_int(int, int, int);
int ALTCALL out_long(long, int, int);
int ALTCALL out_str(char *, int, int);
int ALTCALL clr_eol(void);
int ALTCALL adjust_window(void);
int ALTCALL update_window(int);
int ALTCALL on_screen(int);
int ALTCALL fp_on(int);
int ALTCALL fp_off(int);
char * ALTCALL prompt(char *, char *, char *, int, int);
char * ALTCALL read_str(int, char *, int);
char * ALTCALL fname(FILE_ENT *);

#define Malloc(i) mustalloc(i)         /* for cases where we must alloc */
#define Strdup(s) mustdup(s)           /* memory or we die */
#define Strndup(s,n) mustndup(s,n)

char * ALTCALL mustalloc(int);         /* Malloc() */
char * ALTCALL mustdup(char *);        /* Strdup() */
char * ALTCALL mustndup(char *, int);  /* Strndup() */

#ifdef LINT_ARGS       /* include common function declarations if wanted */
#include <io.h>
#include <malloc.h>
#include <string.h>
#endif
