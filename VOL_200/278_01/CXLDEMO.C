/* ÚÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂ¿
   ³³                                                                   ³³
   ³³                     CXL Demonstration Program                     ³³
   ³³                                                                   ³³
   ³³              CXL is Copyright (c) 1987-1989 by Mike Smedley       ³³
   ³³                                                                   ³³
   ³³               Syntax:  CXLDEMO [-switches]                        ³³
   ³³                                                                   ³³
   ³³                        -c = CGA snow elimination                  ³³
   ³³                        -b = BIOS screen writing                   ³³
   ³³                        -m = force monochrome text attributes      ³³
   ³³                                                                   ³³
   ³³               Compiler command lines:                             ³³
   ³³                                                                   ³³
   ³³                MSC:    cl -Ox cxldemo.c cxlmss.lib                ³³
   ³³                QC:     qcl -Ox cxldemo.c cxlmss.lib               ³³
   ³³                TC:     tcc -Z -O cxldemo.c cxltcs.lib             ³³
   ³³                ZTC:    ztc -msi -o -b cxldemo.c cxlzts.lib        ³³
   ³³                                                                   ³³
   ÀÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÙ */

/*---------------------------------------------------------------------------*/

/* header files */
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cxldef.h"
#include "cxlkey.h"
#include "cxlmou.h"
#include "cxlstr.h"
#include "cxlvid.h"
#include "cxlwin.h"

#if defined(__TURBOC__)                             /* Turbo C */
    #include <dir.h>
#else
    #include <direct.h>                             /* all others */
#endif

#if defined(M_I86) && !defined(__ZTC__)             /* MSC/QuickC */
    #define bioskey(a)       _bios_keybrd(a)
    #define findfirst(a,b,c) _dos_findfirst(a,c,b)
    #define findnext(a)      _dos_findnext(a)
    #define ffblk            find_t
    #define ff_name          name
#elif defined(__ZTC__)                              /* Zortech C/C++ */
    #define ffblk             FIND
    #define ff_name           name
    #define ff_attrib         attribute
#endif

/*---------------------------------------------------------------------------*/

/* function prototypes */
static void activate_demo (void);
static void add_shadow    (void);
static void chg_attr_demo (void);
static void chg_bord_demo (void);
static void chg_wind_attr (int battr,int wattr);
static int  check_date    (char *buf);
static void close_windows (void);
static void confirm_quit  (void);
static void copy_demo     (void);
static void cxlinfo       (void);
static void datademo      (void);
static void directory     (void);
static void display_date  (void);
static void disp_sure_msg (void);
static void display_title (int tpos);
static void do_nothing    (void);
static void error_exit    (int errnum);
static void execute       (void);
static int  find_first    (char *filespec,struct ffblk *ffblk);
static int  find_next     (struct ffblk *ffblk);
static unsigned get_key   (int *done);
static void hide_demo     (void);
static void horz_line     (int wrow,int wcol,int count,int attr);
static void initialize    (void);
static void line_inp_demo (void);
static void main_menu     (void);
static void menudemo      (void);
static void move_bar_down (void);
static void move_demo     (void);
static void move_window   (int nsrow,int nscol);
static void normal_exit   (void);
static void open_back_wind(void);
static void open_stat_wind(void);
static void open_titl_wind(void);
static void os_shell      (void);
static void parse_cmd_line(int argc,char *argv[]);
static void parse_date    (char *buf,int *month,int *day,int *year);
static void pick_compiler (void);
static void pickdemo      (void);
static void popup_demo    (void);
static void pre_help      (void);
static void pre_menu1     (void);
static void pre_menu2     (void);
static void pre_pick1     (void);
static void press_a_key   (int wrow);
static void pulldown_demo (void);
static void quit_window   (void);
static void resize_demo   (void);
static void restore_cursor(void);
static void r_margin      (void);
static void scroll_demo   (void);
static void setup_menu    (void);
static void short_delay   (void);
static void size_window   (int nerow,int necol);
static void text_line_demo(void);
static void title_demo    (void);
static void toggle_block  (void);
static void toggle_yn     (void);
static void update_clock  (void);
static void vert_line     (int wrow,int wcol,int count,int attr);
static void you_selected  (char *str);
static void windemo       (void);

/*---------------------------------------------------------------------------*/

/* miscellaneous global variables */
static int *savescrn,crow,ccol;
static WINDOW w[10];

/*---------------------------------------------------------------------------*/

/* fields used in data entry demo */
static char date[7],firstname[16],initial[2],lastname[26],address[41],city[21],
            state[3],zipcode[10],phone[11],compiler[16],amtenc[8],comments[41],
            ssan[10];

/*---------------------------------------------------------------------------*/

/* months table */
static char *months[]= {
    "January","February","March","April","May","June","July","August",
    "September","October","November","December",NULL
};

/* printer ports table */
static char *prn_ports[]= { "PRN","LPT1","LPT2","COM1","COM2",NULL };

/* error message table */
static char *error_text[]= {
    NULL,   /* errnum =  0, no error	    */
    NULL,   /* errnum == 1, windowing error */
    "Syntax:  CXLDEMO [-switches]\n\n"
        "\t -c = CGA snow elimination\n"
        "\t -b = BIOS screen writing\n"
        "\t -m = force monochrome text attributes",
    "Memory allocation error"
};

/* compiler table */
static char *compilers[]=
    { "Microsoft C","Quick C","Turbo C","Zortech C/C++" };

/*---------------------------------------------------------------------------*/

/* help category numbers */
#define H_GLOBAL    1
#define H_WINDOWING 2
#define H_MENUING   3
#define H_LISTPICK  4
#define H_STRPICK   5
#define H_FILEPICK  6
#define H_DATAENTRY 7
#define H_EDITKEYS  8
#define H_DATE      9
#define H_FIRSTNAME 10
#define H_INITIAL   11
#define H_LASTNAME  12
#define H_CITY      13
#define H_STATE     14
#define H_ZIPCODE   15
#define H_COMPILER  16
#define H_AMOUNT    17
#define H_NUMERIC   18
#define H_PULLDOWN  19
#define H_FILE      20
#define H_EDITING   21
#define H_LOAD      22
#define H_SAVE      23
#define H_RENAME    24
#define H_NEW       25
#define H_DIRECTORY 26
#define H_EXECUTE   27
#define H_OSSHELL   28
#define H_QUIT      29
#define H_SCROLL    30
#define H_CHGATTR   31
#define H_CHGBORD   32
#define H_WINTITLE  33
#define H_RESIZE    34
#define H_TEXTLINE  35
#define H_WINMOVE   36
#define H_WINHIDE   37
#define H_ACTIVATE  38
#define H_WINCOPY   39

/*---------------------------------------------------------------------------*/

/* miscellaneous defines */
#define SHORT_DELAY 11

/*---------------------------------------------------------------------------*/

void main(int argc,char *argv[])
{
    initialize();
    parse_cmd_line(argc,argv);
    open_back_wind();
    open_stat_wind();
    open_titl_wind();
    main_menu();
    normal_exit();
}

/*---------------------------------------------------------------------------*/

static void activate_demo(void)
{
    register int i;

    if((w[5]=wopen(9,44,22,78,1,LCYAN|_CYAN,WHITE|_CYAN))==0) error_exit(1);
    whelpcat(H_ACTIVATE);
    wputs("\n  CXL's wactiv() is used to"
          "\n  activate a window, making"
          "\n  it available for windowing"
          "\n  operations.");
    press_a_key(6);

    /* activate windows 0 thru 5 in order, back to front */
    for(i=0;i<6;i++) {
        wactiv(w[i]);
        short_delay();
    }
}

/*---------------------------------------------------------------------------*/

/* this function will add a shadow to the active window */

static void add_shadow(void)
{
    wshadow(LGREY|_BLACK);
}

/*---------------------------------------------------------------------------*/

static void chg_attr_demo(void)
{
    if((w[1]=wopen(9,35,22,65,0,YELLOW|_BLUE,LCYAN|_BLUE))==0) error_exit(1);
    whelpcat(H_CHGATTR);
    wputs("\n  The wchgattr() function"
          "\n  allows you to change the"
          "\n  attribute of the active"
          "\n  window.");
    press_a_key(6);
    chg_wind_attr(LMAGENTA|_BLUE ,WHITE|_GREEN);
    chg_wind_attr(LGREEN|_LGREY  ,LMAGENTA|_RED);
    chg_wind_attr(YELLOW|_BROWN  ,LCYAN|_BLUE);
    chg_wind_attr(YELLOW|_MAGENTA,YELLOW|_MAGENTA);
    chg_wind_attr(LRED|_MAGENTA  ,WHITE|_MAGENTA);
}

/*---------------------------------------------------------------------------*/

static void chg_bord_demo(void)
{
    register int i;

    whelpcat(H_CHGBORD);
    wtextattr(LGREEN|_MAGENTA);
    wputs("\n\n  The wborder() function is"
            "\n  used to change the style"
            "\n  of the window's border.");
    press_a_key(10);
    for(i=5;i>=0;i--) {
        wborder(i);
        short_delay();
    }
}

/*---------------------------------------------------------------------------*/

static void chg_wind_attr(int battr,int wattr)
{
    wchgattr(battr,wattr);
    short_delay();
}

/*---------------------------------------------------------------------------*/

/* this function validates the Date field in the data entry window */

static int check_date(char *buf)
{
    int year,month,day;

    if(!strblank(buf)) {
        parse_date(buf,&month,&day,&year);

        /* validate numeric values */
        if(month<1 || month>12) {
            wperror("Invalid month");
            return(1);
        }
        if(day<1 || day>31) {
            wperror("Invalid day");
            return(3);
        }
    }

    /* return normally */
    return(0);
}

/*---------------------------------------------------------------------------*/

static void close_windows(void)
{
    register int i;

    for(i=0;i<8;i++) {
        wclose();
        short_delay();
    }
}

/*---------------------------------------------------------------------------*/

/* this function pops open a window and confirms that the user really */
/* wants to quit the demo.  If so, it terminates the demo program.    */

static void confirm_quit(void)
{
    struct _onkey_t *kblist;

    kblist=chgonkey(NULL);  /* hide any existing hot keys */
    if(_mouse&MS_CURS) mshidecur();
    if(!wopen(9,26,13,55,0,WHITE|_BROWN,WHITE|_BROWN)) error_exit(1);
    add_shadow();
    wputs("\n Quit demo, are you sure? \033A\156Y\b");
    clearkeys();
    showcur();
    if(wgetchf("YN",'Y')=='Y') normal_exit();
    wclose();
    hidecur();
    if(_mouse&MS_CURS) msshowcur();
    chgonkey(kblist);       /* restore any hidden hot keys */
}

/*---------------------------------------------------------------------------*/

static void copy_demo(void)
{
    whelpcat(H_WINCOPY);
    wtextattr(LCYAN|_CYAN);
    wputs("\n\n  The wcopy() function is used"
            "\n  to copy the active window.");
    press_a_key(9);
    if(!wcopy(1,1)) error_exit(1);
    short_delay();
    if(!wcopy(3,40)) error_exit(1);
    short_delay();
    short_delay();
}

/*---------------------------------------------------------------------------*/

static void cxlinfo(void)
{
    if(!wopen(4,6,20,72,0,LMAGENTA|_RED,LMAGENTA|_RED)) error_exit(1);
    add_shadow();
    wcenters(1,LCYAN|_RED,"CXL Version 5.1");
    wcenters(2,LCYAN|_RED,"Written by Mike Smedley");
    wcenters(3,LCYAN|_RED,"Copyright (c) 1987-1989  All Rights Reserved");
    wgotoxy(5,0);
    wputs("    CXL is a User-Supported C function library  that supports\n"
          "    Microsoft C, QuickC, Turbo C, and Zortech C/C++.  There  \n"
          "    are  over 260  functions  available for  your  use.      \n"
          "    When  you  register you  get  the remaining memory models\n"
          "    supported by  your  compiler  and the  full  source code!\n"
          "    CXL's  routines  are  written  in  99%   well-documented,\n"
          "    easy-to-modify  C code.  See  documentation  for details.\n\n");
    press_a_key(13);
    wclose();
    move_bar_down();
}

/*---------------------------------------------------------------------------*/

static void datademo(void)
{
    register int ch,mode=0;
    unsigned key;

    if(!wopen(2,8,20,70,1,LCYAN|_BLUE,LCYAN|_BLUE)) error_exit(1);
    add_shadow();
    wtitle("[ CXL Order Form ]",TLEFT,LCYAN|_BLUE);
    wmessage(" [F10]=Finished ",BT_BORD, 5,LCYAN|_BLUE);
    wmessage(" [Esc]=Abort "   ,BT_BORD,45,LCYAN|_BLUE);
    whelpushc(H_EDITKEYS);
    do {

        /* Mark beginning of form. */
        winpbeg(LGREEN|_LGREY,WHITE|_LGREY);

        /* Display prompts and define fields. */
        wprints( 1, 3,WHITE|_BLUE,"Date");
        winpdef( 1,16,date,"<01>#'/'<0123>#'/'<89>#",0,mode,check_date,H_DATE);
        winpfba(NULL,display_date);
        wprints( 3, 3,WHITE|_BLUE,"First Name");
        winpdef(3,16,firstname,"MMMMMMMMMMMMMMM",0,mode,NULL,H_FIRSTNAME);
        wprints( 3,37,WHITE|_BLUE,"Initial");
        winpdef( 3,47,initial,"U'.'",0,mode,NULL,H_INITIAL);
        wprints( 5, 3,WHITE|_BLUE,"Last Name");
        winpdef( 5,16,lastname,"MMMMMMMMMMMMMMMMMMMMMMMMM",0,mode,NULL
            ,H_LASTNAME);
        wprints( 7, 3,WHITE|_BLUE,"Address");
        winpdef( 7,16,address,"****************************************",'M'
            ,mode,NULL,0);
        wprints( 9, 3,WHITE|_BLUE,"City");
        winpdef( 9,16,city,"MMMMMMMMMMMMMMMMMMMM",0,mode,NULL,H_CITY);
        wprints( 9,41,WHITE|_BLUE,"State");
        winpdef( 9,50,state,"UU",0,mode,NULL,H_STATE);
        wprints(11, 3,WHITE|_BLUE,"Zip Code");
        winpdef(11,16,zipcode,"#####'-'%%%%",0,mode,NULL,H_ZIPCODE);
        wprints(11,31,WHITE|_BLUE,"Phone");
        winpdef(11,40,phone,"'('###') '###'-'####",0,mode,NULL,0);
        wprints(13, 3,WHITE|_BLUE,"Compiler");
        winpdef(13,16,compiler,"***************",'M',mode,NULL,H_COMPILER);
        winpfba(pick_compiler,NULL);
        wprints(13,37,WHITE|_BLUE,"Amount");
        winpdef(13,46,amtenc,"'$'99999.99",'9',mode,NULL,H_AMOUNT);
        wprints(15, 3,WHITE|_BLUE,"Comments");
        winpdef(15,16,comments,"****************************************",0
            ,mode,NULL,0);

        /* Define alternate keyboard get function. */
        winpkey(get_key,&key);

        /* Mark end of form and process it.  If [Esc] was pressed, */
        /* then don't bother with the confirmation message.        */
        if(winpread()) break;

        /* Test to see if [F10] was pressed.  If   */
        /* so, don't display confirmation message. */
        if(key==0x4400) break;

        /* Display confirmation message. */
        if(!wopen(9,24,13,57,0,WHITE|_CYAN,WHITE|_CYAN)) error_exit(1);
        add_shadow();
        wputs("\n Is this information correct? \033A\076Y\b");
        clearkeys();
        ch=wgetchf("YN",'Y');
        wclose();

        /* Change field mode to "update". */
        mode=1;

    } while(ch!='Y');
    whelpopc();
    line_inp_demo();
    wclose();
    hidecur();
    move_bar_down();
}

/*---------------------------------------------------------------------------*/

/* this function displays a quick 'n' dirty directory of current path */

static void directory(void)
{
    static int busy=NO;
    char filespec[41],file1[13],file2[13];
    int i,done,lines,files;
    struct ffblk ffblk;

    if(busy) return; else busy=YES;     /* avoid recursion */
    strcpy(filespec,"*.*");
    if(_mouse) mshidecur();
    showcur();
    if(!wopen(4,14,20,59,0,LRED|_MAGENTA,LCYAN|_BLUE)) error_exit(1);
    wtitle("[ Directory ]",TCENTER,LRED|_MAGENTA);
    if(!wopen(6,6,8,49,0,LMAGENTA|_RED,LMAGENTA|_RED)) error_exit(1);
    wtitle("[ Filespec ]",TCENTER,LMAGENTA|_RED);
    winpbeg(BLUE|_LGREY,BLUE|_LGREY);
    winpdef(0,1,filespec,"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",'U',2
        ,NULL,H_EDITKEYS);
    i=winpread();
    wclose();
    if(!i) {
        wclear();
        wtextattr(LCYAN|_BLUE);
        lines=files=0;
        done=find_first(filespec,&ffblk);
        for(;;) {
            if(done) break;
            files++;
            strcpy(file1,ffblk.ff_name);
            if(!done) done=find_next(&ffblk);
            strcpy(file2,ffblk.ff_name);
            if(done)
                file2[0]='\0';
            else {
                files++;
                done=find_next(&ffblk);
            }
            if(done)
                ffblk.ff_name[0]='\0';
            else
                files++;
            wprintf(" %-15s%-15s%s\n",file1,file2,ffblk.ff_name);
            lines++;
            if(lines>=14) {
                wtextattr(LGREEN|_BLUE);
                wprintf(" Press a key to continue....");
                wtextattr(LCYAN|_BLUE);
                i=waitkey();
                wprintf("\r");
                if(i==ESC) break;
                lines=0;
            }
            if(!done) done=find_next(&ffblk);
        }
        wtextattr(LGREEN|_BLUE);
        wprintf(" Files found:  %d  Press a key....",files);
        waitkey();
    }
    hidecur();
    wclose();
    if(_mouse) msshowcur();
    busy=NO;
}

/*---------------------------------------------------------------------------*/

/* this function displays the literal equivalent of the MMDDYY */
/* date that the user entered into the Date field.  It is      */
/* called immediately after the user leaves the Date field.    */

static void display_date(void)
{
    struct _field_t *field;
    int month,day,year;

    /* find address of Date field's record */
    if((field=winpffind(1,16))==NULL) error_exit(1);

    /* set cursor and clear to end of window's line */
    hidecur();
    wgotoxy(1,37);
    wclreol();

    /* don't display date string if Date field is empty */
    if(!strblank(field->buf)) {

        /* parse string date into numeric values */
        parse_date(field->buf,&month,&day,&year);

        /* display date */
        wprintf("%s %d, %d",months[month-1],day,year);
    }
    showcur();
}

/*---------------------------------------------------------------------------*/

/* this function is called by the pull-down demo for a prompt */

static void disp_sure_msg(void)
{
    wprints(0,2,WHITE|_BLUE,"Are you sure?");
}

/*---------------------------------------------------------------------------*/

static void display_title(int tpos)
{
    wtitle("[ My Title ]",tpos,LCYAN|_GREEN);
    short_delay();
}

/*---------------------------------------------------------------------------*/

/* this function is used as a dummy function for */
/* several menu items in the pull-down demo      */

static void do_nothing(void)
{
}

/*---------------------------------------------------------------------------*/

/* this function handles abnormal termination.  If it is passed an  */
/* error code of 1, then it is a windowing system error.  Otherwise */
/* the error message is looked up in the error message table.       */

static void error_exit(int errnum)
{
    if(errnum) {
        printf("\n%s\n",(errnum==1)?werrmsg():error_text[errnum]);
	exit(errnum);
    }
}

/*---------------------------------------------------------------------------*/

/* this function is used by the pull-down demo. */
/* It is used to execute a DOS command.         */

static void execute(void)
{
    static int busy=NO;
    char command[61];
    int *scrn;

    if(busy) return; else busy=YES;     /* avoid recursion */
    showcur();
    if(_mouse) mshidecur();
    if(!wopen(8,7,10,70,0,LMAGENTA|_RED,LMAGENTA|_RED)) error_exit(1);
    wtitle("[ Execute DOS Command ]",TCENTER,LMAGENTA|_RED);
    winpbeg(BLUE|_LGREY,BLUE|_LGREY);
    winpdef(0,1,command,"****************************************"
        "********************",0,0,NULL,H_EDITKEYS);
    if(!winpread()) {
        if((scrn=ssave())==NULL) error_exit(3);
        cclrscrn(LGREY|_BLACK);
        system(command);
        printf("\nPress any key to continue....");
        waitkey();
        srestore(scrn);
    }
    hidecur();
    if(_mouse) msshowcur();
    wclose();
    busy=NO;
}

/*---------------------------------------------------------------------------*/

static int find_first(char *filespec,struct ffblk *ffblk)
{
    register int done;

#if defined(__ZTC__)
    struct ffblk *pffblk;
    if((pffblk=findfirst(filespec,0))==NULL)
        done=YES;
    else {
        *ffblk=*pffblk;
        done=NO;
    }
#else
    done=findfirst(filespec,ffblk,0);
#endif
    return(done);
}

/*---------------------------------------------------------------------------*/

static int find_next(struct ffblk *ffblk)
{
    register int done;

#if defined(__ZTC__)
    struct ffblk *pffblk;
    pffblk=findnext();
    if(pffblk==NULL)
        done=YES;
    else {
        done=NO;
        *ffblk=*pffblk;
    }
#else
    done=findnext(ffblk);
#endif
    return(done);
}

/*---------------------------------------------------------------------------*/

/* This function serves as an alternate get key function for the */
/* data entry demo.  In this example, I do a normal keyboard     */
/* get, but if the key is [F10], I want to exit the form.  The   */
/* purpose of this is to demonstrate how to extend winpread().   */

unsigned get_key(int *done)
{
    unsigned int xch;

    xch=getxch();
    *done=(xch==0x4400);    /* [F10] */
    return(xch);
}

/*---------------------------------------------------------------------------*/

static void hide_demo(void)
{
    register int i;

    whelpcat(H_WINHIDE);
    wtextattr(LCYAN|_BROWN);
    wputs("\n\n   The whide() and wunhide()"
            "\n   functions are used to hide"
            "\n   and unhide windows.");
    press_a_key(9);

    /* hide all windows, front to back */
    for(i=0;i<5;i++) {
        if(whide()) error_exit(1);
        short_delay();
    }

    /* unhide most recently hidden first */
    for(i=0;i<5;i++) {
        if(wunhide(0)) error_exit(1);
        short_delay();
    }
}

/*---------------------------------------------------------------------------*/

static void horz_line(int wrow,int wcol,int count,int attr)
{
    whline(wrow,wcol,count,3,attr);
    short_delay();
}

/*---------------------------------------------------------------------------*/

/* this function initializes CXL's video, mouse, keyboard, and help systems */

static void initialize(void)
{
    /* initialize the CXL video system and save current screen info */
    videoinit();
    readcur(&crow,&ccol);
    if((savescrn=ssave())==NULL) error_exit(3);

    /* if mouse exists, turn on full mouse support */
    if(msinit()) {
        mssupport(MS_FULL);
        msgotoxy(12,49);
    }

    /* attach [Alt-X] to the confirm_quit() function */
    setonkey(0x2d00,confirm_quit,0);

    /* initialize help system, help key = [F1] */
    whelpdef("CXLDEMO.HLP",0x3b00,YELLOW|_RED,LRED|_RED,WHITE|_RED,RED|_LGREY
        ,pre_help);
}

/*---------------------------------------------------------------------------*/

static void line_inp_demo(void)
{
    if(!wopen(5,21,15,58,3,LGREEN|_MAGENTA,LGREEN|_MAGENTA)) error_exit(1);
    wtitle("[ Single-Line Input ]",TCENTER,LGREEN|_MAGENTA);
    whelpcat(H_DATAENTRY);
    add_shadow();
    restore_cursor();
    if(winputsf(phone,"'\n\n  Phone Number?    '!R--!"
        "'('!+!###!-!') '!+!###!-!'-'!+!####")) confirm_quit();
    restore_cursor();
    if(winputsf(ssan,"'\n\n  Soc Sec Number?  '!R-!"
        "<01234567>##!-!'-'!+!##!-!'-'!+!####")) confirm_quit();
    restore_cursor();
    if(winputsf(date,"\"\n\n  Today's Date?    \"!R-!"
        "<01>#!-!'/'!+!<0123>#!-!'/'!+!'19'<89>#")) confirm_quit();
    wclose();
}

/*---------------------------------------------------------------------------*/

static void main_menu(void)
{
    /* push global help category onto help stack, so when   */
    /* [F1] is pressed while selection bar is on an item    */
    /* without help, the global help category will be used. */
    whelpushc(H_GLOBAL);

    /* define and process the main menu */
    wmenubeg(9,27,16,53,0,LBLUE|_BLUE,LBLUE|_BLUE,pre_menu1);
    wmenuitem(0,0,"Windowing"   ,'W',1,0,windemo ,0,H_WINDOWING);
    wmenuitem(1,0,"Menuing"     ,'M',2,0,menudemo,0,H_MENUING);
    wmenuitem(2,0,"List picking",'L',3,0,pickdemo,0,H_LISTPICK);
    wmenuitem(3,0,"Data entry"  ,'D',4,0,datademo,0,H_DATAENTRY);
    wmenuitem(4,0,"Information" ,'I',5,0,cxlinfo ,0,0);
    wmenuitem(5,0,"Exit demo"   ,'E',6,0,NULL    ,0,0);
    wmenuend(1,M_VERT,25,6,LCYAN|_BLUE,WHITE|_BLUE,0,BLUE|_LGREY);
    if(wmenuget()==-1) if(_winfo.errno>W_ESCPRESS) error_exit(1);

    /* pop the global help category off of the stack, and into the void */
    whelpopc();
}

/*---------------------------------------------------------------------------*/

static void menudemo(void)
{
    pulldown_demo();
    move_bar_down();
}

/*---------------------------------------------------------------------------*/

/* This function is used to move the selection bar of the Main Menu  */
/* down to the next menu item.  It does this by fetching the address */
/* of the current menu, then setting the current tagid to the tagid  */
/* of the next menu item.  If I had not numbered the menu items'     */
/* tagids sequentially, this would not work.                         */

static void move_bar_down(void)
{
    wmenuinext(wmenuicurr()->tagid+1);
}

/*---------------------------------------------------------------------------*/

static void move_demo(void)
{
    if((w[4]=wopen(4,2,16,35,2,LGREEN|_BROWN,WHITE|_BROWN))==0) error_exit(1);
    whelpcat(H_WINMOVE);
    wputs("\n   To move the active window,"
          "\n   the wmove() function is"
          "\n   used.");
    press_a_key(5);
    move_window(0,0);
    move_window(10,4);
    move_window(6,19);
    move_window(1,13);
    move_window(11,35);
    move_window(2,39);
}

/*---------------------------------------------------------------------------*/

static void move_window(int nsrow,int nscol)
{
    if(wmove(nsrow,nscol)) error_exit(1);
    short_delay();
}

/*---------------------------------------------------------------------------*/

/* this function handles normal termination.  The original screen and cursor */
/* coordinates are restored before exiting to DOS with ERRORLEVEL 0.         */

static void normal_exit(void)
{
    srestore(savescrn);
    gotoxy_(crow,ccol);
    if(_mouse) mshidecur();
    showcur();
    exit(0);
}

/*---------------------------------------------------------------------------*/

static void open_back_wind(void)
{
    register int i;

    if(!wopen(0,0,23,79,5,0,LGREEN|_GREEN)) error_exit(1);
    for(i=1;i<320;i++) wprintf("\033F%cCXL   ",i);
}

/*---------------------------------------------------------------------------*/

static void open_stat_wind(void)
{
    wfillch('\260');
    wopen(24,0,24,79,5,0,LGREY|_RED);
    wprints( 0, 1,LGREY|_RED,"[F1]=Help");
    wcenters(0   ,LGREY|_RED,"Select demonstration to view.");
    wrjusts( 0,78,LGREY|_RED,"[Alt-X]=Quit");
    wfillch(' ');
}

/*---------------------------------------------------------------------------*/

static void open_titl_wind(void)
{
    if(!wopen(1,12,5,67,0,LRED|_MAGENTA,LRED|_MAGENTA)) error_exit(1);
    add_shadow();
    wcenters(0,WHITE|_MAGENTA,"Welcome to the CXL Demonstration Program!");
    wcenters(2,LCYAN|_MAGENTA,"CXL is Copyright (c) 1987-1989 by Mike Smedley");
}

/*---------------------------------------------------------------------------*/

/* this is used by the pull-down demo.  It is the */
/* [O]S Shell function under the [F]ile menu      */

static void os_shell(void)
{
    int *scrn;

    showcur();
    if(_mouse) mshidecur();
    if((scrn=ssave())==NULL) error_exit(3);
    cclrscrn(LGREY|_BLACK);
    printf("Type EXIT to return....\n");
    system("COMMAND");
    srestore(scrn);
    hidecur();
    if(_mouse) msshowcur();
}

/*---------------------------------------------------------------------------*/

/* this function parses the command line for valid invocation switches.  If  */
/* any valid switches are found, the appropriate video variable will be      */
/* set.  If any invalid parameters are found, a syntax message is displayed. */

static void parse_cmd_line(int argc,char *argv[])
{
    register int i,j;
    char *p;

    for(i=1;i<argc;i++) {
        p=argv[i];
        if(*p=='-'||*p=='/') {
            for(j=1;*(p+j);j++) {
                switch(tolower(*(p+j))) {
                    case 'c':
                        setvparam(VP_CGA);
                        break;
                    case 'b':
                        setvparam(VP_BIOS);
                        break;
                    case 'm':
                        setvparam(VP_MONO);
                        break;
                    default:
                        error_exit(2);
                }
            }
        }
        else
            error_exit(2);
    }
}

/*---------------------------------------------------------------------------*/

/* this function parses a MMDDYY date string into separate numeric values */

static void parse_date(char *buf,int *month,int *day,int *year)
{
    char date[7];

    strcpy(date,buf);
    *year=1900+atoi(date+4);
    *(date+4)='\0';
    *day=atoi(date+2);
    *(date+2)='\0';
    *month=atoi(date);
}

/*---------------------------------------------------------------------------*/

static void pick_compiler(void)
{
    register int subscr,i;

    whelpush();
    wmenubeg(9,22,14,38,0,LMAGENTA|_MAGENTA,LMAGENTA|_MAGENTA,pre_menu2);
    for(i=0;i<4;i++) wmenuitem(i,0,compilers[i],*compilers[i],i,0,NULL,0,0);
    wmenuend(0,M_VERT,15,1,LMAGENTA|_MAGENTA,LCYAN|_MAGENTA,0,BLUE|_LGREY);
    if((subscr=wmenuget())==-1) if(_winfo.errno>W_ESCPRESS) error_exit(1);
    kbput(0x4700);  /* [Home] */
    if(subscr!=-1) {
        kbputs(compilers[subscr]);
        kbput(0x1615);  /* [Ctrl-U] */
    }
    kbput(0x1c0d);  /* [Enter] */
    whelpop();
}

/*---------------------------------------------------------------------------*/

/* this function demonstrates the string picker, the file */
/* picker, and the in-place string selector functions.    */

static void pickdemo(void)
{
    register int *scrn;

    if((scrn=ssave())==NULL) error_exit(3);
    cclrscrn(LGREY|_BLUE);
    if(!wopen(4,11,14,68,3,LMAGENTA|_RED,LRED|_MAGENTA)) error_exit(1);
    add_shadow();
    wprintf("\033R\001\033C\003Select a month =>\033R\001\033C\003");
    whelpcat(H_STRPICK);
    you_selected(months[wpickstr(0,32,5,-1,0,LGREEN|_RED,LCYAN|_RED,RED|_LGREY
        ,months,0,pre_pick1)]);
    wprintf("\033R\003\033C\003Now, select a file.\033R\003\033C\003");
    whelpcat(H_FILEPICK);
    you_selected(wpickfile(10,9,20,66,0,LCYAN|_RED,LGREY|_RED,RED|_LGREY
        ,1,"*.*",add_shadow));
    wprintf("\033R\005\033C\003Select a printer port:\033R\005\033C\003");
    showcur();
    you_selected(prn_ports[wselstr(5,27,LMAGENTA|_MAGENTA,prn_ports,0)]);
    hidecur();
    wclose();
    srestore(scrn);
    move_bar_down();
}

/*---------------------------------------------------------------------------*/

static void popup_demo(void)
{
    whelpushc(H_PULLDOWN);
    wmenubeg(7,15,10,65,0,LRED,LCYAN|_BLUE,NULL);
    wmenuitem(0, 0,"Add",   'A',55,0,NULL,0,0);
    wmenuitxt(1,0,LGREEN|_BLUE,"Create a new record");
    wmenuitem(0, 8,"Delete",'D',56,0,NULL,0,0);
    wmenuitxt(1,0,LGREEN|_BLUE,"Delete an existing record");
    wmenuitem(0,19,"Print", 'P',57,0,NULL,0,0);
    wmenuitxt(1,0,LGREEN|_BLUE,"Print hardcopy of existing record");
    wmenuitem(0,28,"Show",  'S',58,0,NULL,0,0);
    wmenuitxt(1,0,LGREEN|_BLUE,"Display an existing record on screen");
    wmenuitem(0,36,"Update",'U',59,0,NULL,0,0);
    wmenuitxt(1,0,LGREEN|_BLUE,"Modify an existing record");
    wmenuitem(0,45,"Quit",  'Q',60,0,NULL,0,0);
    wmenuitxt(1,0,LGREEN|_BLUE,"Quit and return to pull-down menu");
    wmenuend(55,M_HORZ,0,0,LMAGENTA|_BLUE,WHITE|_BLUE,0,YELLOW|_LGREY);
    wmenuget();
    whelpopc();
}

/*---------------------------------------------------------------------------*/

static void pre_help(void)
{
    add_shadow();
    setonkey(0x2d00,confirm_quit,0);
}

/*---------------------------------------------------------------------------*/

/* this function is called during the setup of the main menu.  It is  */
/* called by wmenuget() right after the main menu's window is opened. */

static void pre_menu1(void)
{
    hidecur();
    add_shadow();
}

/*---------------------------------------------------------------------------*/

/* this function is called during the Data Entry demo when */
/* the user enters the Compiler field.  It is called by    */
/* wmenuget() after the compiler choice menu is opened.    */

static void pre_menu2(void)
{
    add_shadow();
    if(_mouse) msgotoxy(11,34);
    hidecur();
}

/*---------------------------------------------------------------------------*/

/* this function is called during the List Picking demo.  It is called */
/* by wpickstr() right after the string picker's window is opened.     */

static void pre_pick1(void)
{
    wmessage("¿ Ú",BT_BORD,4,LGREEN|_RED);
    add_shadow();
}

/*---------------------------------------------------------------------------*/

/* this function displays a pause message then pauses for a keypress */

static void press_a_key(int wrow)
{
    register int attr;

    attr=(BLINK|YELLOW)|((_winfo.active->wattr>>4)<<4);
    wcenters(wrow,attr,"Press a key");
    hidecur();
    if(waitkey()==ESC) confirm_quit();
    wcenters(wrow,attr,"           ");
}

/*---------------------------------------------------------------------------*/

static void pulldown_demo(void)
{
    int selection;

    whelpushc(H_PULLDOWN);
    if(!wopen(0,0,24,79,0,CYAN,CYAN)) error_exit(1);
    whline(1,0,78,0,CYAN);
    setkbloop(update_clock);
    wgotoxy(2,0);
    wtextattr(YELLOW);
    wputs("This is a sample editor interface to demonstrate how CXL's "
          "menuing functions\n");
    wputs("can be used to create a pull-down menu system.  Use the arr"
          "ow keys to move\n");
    wputs("selection bar around and use the [Esc] key to back up to th"
          "e previous menu.\n");
    wputs("Pressing [F1] will bring up context-sensitive help.  Select"
          " [Q]uit from any\n");
    wputs("menu to exit the pull-down demo.  Be sure to try some of th"
          "e menu options in\n");
    wputs("the pull-down menus.");
    if(_mouse)
        wputs("  Since you have a mouse, you can use it to "
              "move around\nand select options!");

    /* define menu structure */
    wmenubeg(1,1,1,78,5,CYAN,CYAN|_BLUE,NULL);
    wmenuitem(0,2,"File",'F',19,M_HASPD,NULL,0,H_FILE);
        wmenubeg(2,1,11,17,0,LGREY|_BLUE,_BLUE,NULL);
        wmenuitem(0,0,"Load"         ,'L',20,0,do_nothing,0,H_LOAD);
        wmenuitem(1,0,"Save"         ,'S',21,0,do_nothing,0,H_SAVE);
        wmenuitem(2,0,"Rename"       ,'R',22,0,do_nothing,0,H_RENAME);
        wmenuitem(3,0,"New"          ,'N',23,0,do_nothing,0,H_NEW);
        wmenuitem(4,0,"Directory  F5",'D',24,0,directory ,0x3f00,H_DIRECTORY);
        wmenuitem(5,0,"Execute    F6",'E',25,0,execute   ,0x4000,H_EXECUTE);
        wmenuitem(6,0,"OS Shell   F7",'O',26,M_CLOSB,os_shell,0x4100,H_OSSHELL);
        wmenuitem(7,0,"Quit"         ,'Q',27,M_CLALL,NULL,0,H_QUIT);
        wmenuiba(quit_window,quit_window);
        wmenuend(20,M_PD|M_SAVE,15,1,YELLOW|_BLUE,LCYAN|_BLUE,0
          ,YELLOW|_MAGENTA);
    wmenuitem(0,15,"Editing",'E',28,M_HASPD,NULL,0,H_EDITING);
        wmenubeg(2,14,10,28,0,LGREY|_BLUE,_BLUE,NULL);
        wmenuitem(0,0,"Cut"        ,'C',29,0      ,do_nothing  ,0,0);
        wmenuitem(1,0,"Paste"      ,'P',30,M_NOSEL,do_nothing  ,0,0);
        wmenuitem(2,0,"cOpy"       ,'O',31,M_NOSEL,do_nothing  ,0,0);
        wmenuitem(3,0,"block Begin",'B',32,0      ,toggle_block,0,0);
        wmenuitem(4,0,"block End"  ,'E',33,M_NOSEL,toggle_block,0,0);
        wmenuitem(5,0,"Insert line",'I',34,0      ,do_nothing  ,0,0);
        wmenuitem(6,0,"Delete line",'D',35,0      ,do_nothing  ,0,0);
        wmenuend(29,M_PD|M_SAVE,13,1,YELLOW|_BLUE,LCYAN|_BLUE,CYAN|_BLUE
            ,YELLOW|_MAGENTA);
    wmenuitem(0,31,"Defaults",'D',36,M_HASPD,NULL,0,0);
        wmenubeg(2,30,13,52,0,LGREY|_BLUE,_BLUE,setup_menu);
        wmenuitem(0,0,"Left margin       0",'L',37,0,do_nothing,0,0);
        wmenuitem(1,0,"Right margin     72",'R',38,0,r_margin  ,0,0);
        wmenuitem(2,0,"Tab width         4",'T',39,0,do_nothing,0,0);
        wmenuitem(4,0,"tab eXpansion   yes",'X',40,0,toggle_yn ,0,0);
        wmenuitem(5,0,"Insert mode     yes",'I',41,0,toggle_yn ,0,0);
        wmenuitem(6,0,"iNdent mode     yes",'N',42,0,toggle_yn ,0,0);
        wmenuitem(7,0,"Word wrap mode   no",'W',43,0,toggle_yn ,0,0);
        wmenuitem(9,0,"Save defaults"      ,'S',44,0,do_nothing,0,0);
        wmenuend(37,M_PD|M_SAVE,21,1,YELLOW|_BLUE,LCYAN|_BLUE,0
            ,YELLOW|_MAGENTA);
    wmenuitem(0,47,"Run"    ,'R',45,0      ,popup_demo,0,0);
    wmenuitem(0,59,"Options",'O',46,M_HASPD,NULL,0,0);
        wmenubeg(2,56,8,77,0,LGREY|_BLUE,_BLUE,NULL);
        wmenuitem(0,0,"screen siZe     43",'Z',47,0,do_nothing,0,0);
        wmenuitem(1,0,"Backup files   yes",'B',48,0,toggle_yn ,0,0);
        wmenuitem(2,0,"bOxed display   no",'O',49,0,toggle_yn ,0,0);
        wmenuitem(3,0,"Load options"      ,'L',50,0,do_nothing,0,0);
        wmenuitem(4,0,"Save options"      ,'S',51,0,do_nothing,0,0);
        wmenuend(47,M_PD|M_SAVE,20,1,YELLOW|_BLUE,LCYAN|_BLUE,0
            ,YELLOW|_MAGENTA);
    wmenuitem(0,73,"Quit",'Q',52,M_HASPD,NULL,0,0);
    wmenuiba(quit_window,quit_window);
        wmenubeg(2,60,7,78,0,LGREY|_BLUE,_BLUE,disp_sure_msg);
        wmenuitem(2,0,"No" ,'N',53,M_CLOSE,NULL,0,0);
        wmenuitem(3,0,"Yes",'Y',54,M_CLALL,NULL,0,0);
        wmenuend(53,M_PD,17,7,YELLOW|_BLUE,LCYAN|_BLUE,WHITE|_BLUE
            ,YELLOW|_MAGENTA);
    wmenuend(19,M_HORZ,0,0,YELLOW|_BLUE,LCYAN|_BLUE,0,YELLOW|_MAGENTA);

    /* process the menu */
    selection=wmenuget();
    if(selection==-1&&_winfo.errno>W_ESCPRESS) error_exit(1);
    wclose();
    setkbloop(NULL);
    whelpopc();
}

/*---------------------------------------------------------------------------*/

/* this function is called by the pull-down menu demo anytime */
/* the  selection bar moves on or off the [Q]uit menu items.  */

static void quit_window(void)
{
    static WINDOW handle=0;

    if(handle) {
        wactiv(handle);
        wclose();
        handle=0;
    }
    else {
        handle=wopen(14,41,17,70,0,YELLOW|_RED,WHITE|_RED);
        wputs(" Quit takes you back to the\n demo program's main menu.");
    }
}

/*---------------------------------------------------------------------------*/

/* this function handles the setting of the [R]ight   */
/* margin setting under the [D]efault pull-down menu. */

static void r_margin(void)
{
    winpbeg(WHITE|_CYAN,WHITE|_CYAN);
    winpdef(1,17,wmenuicurr()->str+16,"%%#",'9',1,NULL,0);
    winpread();
    hidecur();
    wmenuicurr()->redisp=YES;
}

/*---------------------------------------------------------------------------*/

static void resize_demo(void)
{
    whelpcat(H_RESIZE);
    wtextattr(LGREEN|_GREEN);
    wputs("\n\n\n  The wsize() function"
              "\n  allows you to resize"
              "\n  the active window.");
    press_a_key(10);
    size_window(19,60);
    size_window(19,75);
    size_window(24,75);
    size_window(24,60);
    size_window(24,45);
    size_window(19,45);
}

/*---------------------------------------------------------------------------*/

static void restore_cursor(void)
{
    wtextattr(WHITE|_MAGENTA);
    showcur();
}

/*---------------------------------------------------------------------------*/

static void scroll_demo(void)
{
    register int i,j;

    if((w[0]=wopen(1,1,11,41,2,LMAGENTA|_RED,LCYAN|_RED))==0) error_exit(1);
    whelpcat(H_SCROLL);
    wprintf("\n  CXL's window TTY output functions"
            "\n  such as wputs() and wprintf() allow"
            "\n  scrolling and ANSI-like Escape code"
            "\n  sequences from inside of windows.");
    press_a_key(6);
    wgotoxy(8,0);
    for(i=0;i<10;i++) {
        for(j=DGREY;j<=WHITE;j++) {
            wprintf("\033F%cCXL   ",j);
            delay_(1);
        }
    }
}

/*---------------------------------------------------------------------------*/

/* this function sets up the [D]efaults menu for the pull-down demo */

static void setup_menu(void)
{
    whline(3,0,21,0,LGREY|_BLUE);
    whline(8,0,21,0,LGREY|_BLUE);
}

/*---------------------------------------------------------------------------*/

static void short_delay(void)
{
    delay_(SHORT_DELAY);
}

/*---------------------------------------------------------------------------*/

static void size_window(int nerow,int necol)
{
    wsize(nerow,necol);
    short_delay();
}

/*---------------------------------------------------------------------------*/

static void text_line_demo(void)
{
    if((w[3]=wopen(10,3,23,41,3,YELLOW|_BLUE,LMAGENTA|_BLUE))==0)
        error_exit(1);
    whelpcat(H_TEXTLINE);
    wputs("\n  The whline() & wvline() functions"
          "\n  are used for drawing \"smart\" text"
          "\n  lines which will even make their "
          "\n  own corners and intersections!   ");
    press_a_key(6);
    horz_line(6,6,24,LCYAN|_BLUE);
    vert_line(6,6,4,LCYAN|_BLUE);
    horz_line(10,6,24,LCYAN|_BLUE);
    vert_line(6,30,5,LRED|_BLUE);
    vert_line(6,18,5,LRED|_BLUE);
    horz_line(7,6,25,LRED|_BLUE);
    horz_line(9,6,25,LGREEN|_BLUE);
    vert_line(6,11,5,LGREEN|_BLUE);
    vert_line(6,23,5,LGREEN|_BLUE);
    horz_line(8,6,25,LMAGENTA|_BLUE);
}

/*---------------------------------------------------------------------------*/

static void title_demo(void)
{
    if((w[2]=wopen(5,20,19,45,3,LCYAN|_GREEN,WHITE|_GREEN))==0) error_exit(1);
    whelpcat(H_WINTITLE);
    wputs("\n  The wtitle() function"
          "\n  allows you to give a"
          "\n  window a title.");
    press_a_key(5);
    display_title(TLEFT);
    display_title(TRIGHT);
    display_title(TCENTER);
}

/*---------------------------------------------------------------------------*/

/* this function toggles "block marking" on and off */
/* in the sample editor of the pulldown menu demo   */

static void toggle_block(void)
{
    static int block=OFF;

    if(block) {
        block=OFF;
        wmenuienab(32);
        wmenuidsab(33);
        wmenuinext(32);
    }
    else {
        block=ON;
        wmenuidsab(32);
        wmenuienab(33);
        wmenuinext(33);
    }

    if(wopen(11,30,15,49,0,LRED|_MAGENTA,WHITE|_MAGENTA)) {
        wprintf("\n Block is now o%s",block?"n":"ff");
        delay_(10);
        wclose();
    }
}

/*---------------------------------------------------------------------------*/

/* This function toggles the yes/no status of some pull-down menu items.   */
/* This is to show you can modify the menu item's string and redisplay it. */
/* This is done by getting the address of the current menu item's struct,  */
/* then modifying the string pointed to by the str element.  The redisp    */
/* flag is set so that when you return to the menu, the screen will be     */
/* automatically updated.                                                  */

static void toggle_yn(void)
{
    struct _item_t *citem;
    char *str,*yn;

    citem=wmenuicurr();
    str=citem->str;
    yn=str+strlen(str)-3;
    strcpy(yn,strcmp(yn,"yes")?"yes":" no");
    citem->redisp=1;
}

/*---------------------------------------------------------------------------*/

/* this function updates the on-screen clock in the pull-down demo. */

static void update_clock(void)
{
    prints(0,70,LGREEN,systime(1));
}

/*---------------------------------------------------------------------------*/

static void vert_line(int wrow,int wcol,int count,int attr)
{
    wvline(wrow,wcol,count,3,attr);
    short_delay();
}

/*---------------------------------------------------------------------------*/

/* this function is used by the List Picking demo to display a selected    */
/* string, or display an error message if an error occurred.  It also      */
/* prompts the user for a keypress 2 lines below the string/error message. */

static void you_selected(char *str)
{
    int wrow,wcol;

    if(_winfo.errno)
        wprintf("\033EL%s",werrmsg());
    else
        wprintf("\033ELYou selected:  \033F\005%s\033F\004",str);
    whelpcat(H_LISTPICK);
    wreadcur(&wrow,&wcol);
    press_a_key(wrow+2);
}

/*---------------------------------------------------------------------------*/

static void windemo(void)
{
    int *scrn;

    if((scrn=ssave())==NULL) error_exit(3);
    cclrscrn(LGREY|_BLACK);
    scroll_demo();
    chg_attr_demo();
    chg_bord_demo();
    title_demo();
    resize_demo();
    text_line_demo();
    move_demo();
    hide_demo();
    activate_demo();
    copy_demo();
    close_windows();
    srestore(scrn);
    clearkeys();
    move_bar_down();
}
