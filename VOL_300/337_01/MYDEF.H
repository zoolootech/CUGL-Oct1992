/**********************   MYDEF.H   ***************************/

/*
Set the following define statement to "#define TURBOC" if you are
using Borland "Turbo C", set it to "#define QUICKC" if you are using
Microsoft "QuickC".
*/

#define TURBOC

#define MAX_WINDOW 20

#define MAX_STRING 500   /* maximum length of strings.
                            (includes '\0') */

#define MAKE_FP(seg,off) \
       ((char far *) (((unsigned long)(seg) << 16)|(unsigned)(off)))

#define set_color(foreground,background)\
                 (((background)<<4)|(foreground))
#define set_intense(attribute)((attribute)|8)


/* Screen mode definations (the most common)*/

#define BW_40      0  /* 40 column B&W */
#define COLOR_40   1  /* 40 column color */
#define BW_80      2  /* 80 column B&W */
#define COLOR_80   3  /* 80 column color */
#define MED_COLOR  4  /* 320x200 4 color graphics mode */
#define MED_BW     5  /* 320x200 4 shade levels */
#define HI_RES     6  /* 640x200 B&W */
#define MONOCHROME 7  /* monochrome display */

#define FALSE 0          /* logic values */
#define TRUE  1

#define ON 1
#define OFF 0

#define BLACK   0         /* text attributes for color cards */
#define BLUE    1
#define GREEN   2
#define CYAN    3
#define RED     4
#define MAGENTA 5
#define BROWN   6
#define WHITE   7
#define YELLOW 14   /* intensity set on */

/* definitions of box types */

#define STD_FRAME    "\xda\xbf\xc0\xd9\xc4\xb3"  /* Ú ¿ À Ù Ä ³ */
#define PD_FRAME     "\xc2\xc2\xc0\xd9\xc4\xb3"  /* Â Â À Ù Ä ³ */
#define TOP_FRAME    "\xda\xbf\xc3\xb4\xc4\xb3"  /* Ú ¿ Ã ´ Ä ³ */
#define MIDDLE_FRAME "\xc3\xb4\xc3\xb4\xc4\xb3"  /* Ã ´ Ã ´ Ä ³ */
#define BOTTOM_FRAME "\xc3\xb4\xc0\xd9\xc4\xb3"  /* Ã ´ À Ù Ä ³ */
#define NO_FRAME  ""

#define UP        0x48
#define DOWN      0x50
#define LEFT      0x4b
#define RIGHT     0x4d
#define HOME      0x47
#define END       0x4f
#define INSERT    0x52
#define DELETE    0x53
#define PGDN      0x51
#define PGUP      0x49
#define ESCAPE    0x1b
#define RETURN    0x0d
#define BACKSPACE 0x08
#define BELL      0x07
#define F1        0x3b

#if defined QUICKC
 #include <stddef.h>   /* for NULL definition */
#endif

#define UNDERLINE 1     /* ATTRIBUTES FOR MONOCHROME CARDS */
#define NORMAL    7
#define BOLD      15
#define INVERSE   112

/* cursor types */
#define BIG_CURSOR    2
#define NORMAL_CURSOR 1
#define NO_CURSOR     0


/* for input screens */

#define OK     0
#define REDO   1
#define REDRAW 2

/* window structure */

struct screen_structure{
 /* Screen buffer related variables */

 char far *buffer;  /* pointer to screen buffer */
 int rows;    /* number of rows the monitor can display */
 int columns; /* number of columns the monitor can display */
  int top;    /* the top margin of active screen area (row) */
  int bottom; /* the bottom margin of active screen area(row) */
  int left;   /* the left margin of active screen area(column) */
  int right;  /* the right margin of active screen area(column)*/
 int mode;    /* screen mode (as reported by BIOS) */
 int page;    /* the text page in use (color cards only) */
 int snow;    /* flag to indicate if snow avoidance ia used */

 /* Attribute related variables */

 char current; /* current text attribute used when printing text*/
 char inverse; /* default inverse attribute (predefined for mode) */
 char normal;  /* default normal attribute (predefined for mode */
 int bold_caps; /* flag to indicate if upper case letters are
                   printed with intensity set high */

 /* window related variables */

 int active;    /* the handle (number) of the active window */
 int ptr;       /* pointer to the window list (how many windows) */
 int list[MAX_WINDOW+1];  /* the list of windows (sequence)  */
 int update;    /* flag to indicate if cursor should be updated */
};

struct window_structure{
 int frame; /* flag to indicate if window is framed or unframed */
 int x,y;         /*cursor position (column,row) */
 char attribute;  /* The attribute specified for the window */
 int start, end;  /* cursor scan lines */
 int top,bottom,left,right; /* window margins */
 char  *buffer;             /* buffer to store image */
};

/* information for menus */

#define MAX_BAR 10  /* The maximum number of options in the */
                    /* moving light bar menu. */


/* menu structure for the pop-up menu */

struct pop_struc{
 char *name;       /* the menu option */
 int (*fun)();     /* the pointer to function */
 int select_id;    /* the list-select return code */
};

/* menu structure for moving light bar menu */

struct bar_struc{
 char *name;         /* the name of the function */
 char *info;         /* the info line appearing under options */
 int  (*fun)();      /* the pointer to function */
 int select_id;      /* the list-select return code */
};

/* menu structure for the pull-down menus */

#define MAIN_OPT   5  /* the actual number of options appearing
                         in the main menu */
#define PD_SUB     3  /* the maximum number of sub-options in the
                         pull-down menus */
#define PD_SUB_ROW 4  /* the row that the pull-down window appears */

#define PD_MAIN_FRAME STD_FRAME /* frame used for the main menu
                                   window */
#define PD_SUB_FRAME  PD_FRAME  /* frame used for the pull-down
                                   menu window */

struct pd_str{
 char *main;            /* option to appear in main menu */
 char *sub[PD_SUB];     /* array of options to appear in Pull-down*/
 int (*fun[PD_SUB])();  /* array of function pointers for pull-down*/
 int select_id[PD_SUB]; /* array of list-select return code */
};

struct window_colors{
 char main_frame;       /* attribute used for main menu frame */
 char main_interior;    /* " " " " menu interior */
 char main_inverse;     /* " " " " menu highlighter */

 char pd_frame;         /* " " " pull-down frame */
 char pd_interior;      /* " " " pull-down interior */
 char pd_inverse;       /* " " " pull-down highlighter */
};


/* structure for help files */

struct help_structure{
     char filename[80];  /* the current help file */
     char message[80];   /* the window title */
     int x;              /* the column for the upper left corner of
                            the help window */
     int y;              /* the row  for the upper left corner of
                            the help window  */
     int  page;          /* page within file to use */
     char frame_attr;    /* character attribute for help interior */
     char interior_attr; /* character attribute for help frame */
     };



/* structure for input screens */
struct in_struc {
 int  x;      /* x position for data input field (start of label)*/
 int  y;      /* y position for data input field (start of label)*/

 char *prompt;/* the prompt for the field */
 char *ptr;   /* pointer to string to edit */
 int  length; /* the maximum length of the field */

 unsigned int label_f,label_b;  /* label foreground,background color */
 unsigned int input_f,input_b;  /* input field foreground,background
                                   color */
 };



/* function prototypes */

int start(void);   /* for start function */


/***********     l_main.c     ***********/

int main(int argc,char *argv[]);
static void init_window(void);
static void set_screen_attr(void);
static void test_dv(void);
void update_margins(void);


/***********     l_scrn1.c     ***********/

void ceol(int x, int y);
void cls(void);
void gotoxy(int x,int y);
void scroll_up(int lines);
void scroll_down(int lines);
void set_mode(int mode);
void what_mode(int *mode,int *page);


/***********     l_scrn2.c     ***********/

void wherexy(int *x,int *y);
void readxy(char *ch,char *attr);
void what_cursor(int *start, int *end);


/***********     l_scrn3.c     ***********/

void cursor(int size);
void set_cursor(int start, int end);


/***********     l_scrn4.c     ***********/

void alt_screen(int action);


/***********     l_win1.c     ***********/

int win_make(int x,int y,int width,int height,char *array,\
               char *title, char frame_attr, char win_attr);
void win_save(void);
void win_delete(int handle);
int win_validate (int handle);
void win_delete_top(void);
void draw_frame (int x,int y,int width,int height,\
                 char *array,char *title,char attribute);
int win_center(int width,int height,char *array,char *title,\
               char frame_attr, char win_attr);


/***********     l_win2.c     ***********/

void win_pop_top(int handle);
void display_cursor(void);
static void win_redraw(int handle);
void win_redraw_all(void);


/***********     l_win3.c     ***********/

void win_ceol(int handle, int x, int y);
void win_cls(int handle);
void win_scroll_up(int handle,int lines);
void win_scroll_down(int handle,int lines);
void win_gotoxy(int handle,int x,int y);
void win_print( int handle, int x,int y, char *string);
static void win_point(int handle);


/***********     l_win4.c     ***********/

void win_up(int handle, int amount);
void win_right(int handle, int amount);
void win_left(int handle, int amount);
void win_down(int handle, int amount);
void win_insert(int handle, int position);


/***********     l_win5.c      ***********/

char win_what_attr(int handle);
void win_set_attr(int handle, char attribute);


/***********     l_print.c     ***********/

void print(int x,int y,char *string);
void print_here(char *string);
void dma_print(int *x, int *y,char *string);
void move_scr_mem (char far *string,char far *video,int number);


/***********     l_getkey.c      ***********/
void get_key(char *ch, char *ext);


/***********   l_popup.c    ***********/

int pop_up(struct pop_struc pop_menu[],int x,int y,\
           char normal, char inverse);


/***********     l_bar.c    ***********/

int bar_menu(struct bar_struc menu[], char normal, char inverse);


/***********     l_chip.c    ***********/

void chip_left(char *chip,char *block,int number);
void chip_right(char *chip,char *block, int number);


/***********     l_copy.c    ***********/

void copy (char *from,char *to,int first,int length);


/***********     l_getfld.c     ***********/

char getfield(char *string, int inlength, int start, char attribute);
void hilight_field (int x, int y, int length, char attribute);


/***********     l_input.c     ***********/

int  input(struct in_struc in_scrn[]);


/***********     l_string.c    ***********/

int pos(char *string,char *pattern);
void caps(char *string);


/***********    l_list.c     ***********/

int list_select(char *ptr[]);


/***********      l_dir.c     ***********/

int dir(char *filespec, char *selection);
int file_count(char *filespec);


/***********       pd.c    ***********/

int pull_down (struct pd_str m_menu[], struct window_colors color);
static int pull_down_sub(struct pd_str m_menu[],struct window_colors\
                          color,int option, char *ext, int *expert);
