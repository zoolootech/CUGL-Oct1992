
/* mgetch() and mgets() were added 9/89 by T Clune. */
/* clear_input_devices() made externally-visible 9/89 by T Clune */

/* print_headline() added 8/89, because I always use this construct anyway. */
/* added by T Clune. */

/* menu_mouse_config() added 8/89 by T. Clune to support user-configuration */
/* of mouse parameters, as per Frans VdV request. */

/* get_file() was added to menu.c [it was formerly in grabmain.c] and */
/* dir_menu() was eliminated 12/88, and get_dir() in dos_func.c was */
/* rewritten to use string pointers instead of character arrays.  Now */
/* get_dir() is compatible with menu(), so dir_menu() is superfluous. */
/* Modified by T Clune. */

/* Modified by T Clune 12/88 to support mouse selection of menu items. */
/* menu.obj must be linked with mouselib.obj and sound.obj and the */
/* Microsoft library mouse.lib now.  Sound.obj is used to generate the */
/* pause between mouse queries to give a pause between menu */
/* selections, analogous to the pause in keyboard repeats.  Mouselib.obj */
/* is my mouse library front end. */

/* pathprint() added to menu.c 11/20/88 by T. Clune.  Moved from */
/* tmbr_gp.c for conceptual unity. */

/* c_dir fname_unused() and get_dir() functions moved to dos_func.c */
/* 11/88 by T. Clune. */

/* c_dir.c functions added to menu.c 6/88 by T. Clune. */
/* the dir_menu() function supports a menu() -style selection of */
/* directory files. */
/* written by T. Clune for R. Webb, 5/18/88. */
/* Copyright (c) 1988, Eye Research Institute, 20 Staniford St., Boston, MA */
/* All rights reserved. */

/* menu.c is a menu-creation utility for use with large memory model */
/* Microsoft C routines.  It expects to run on an IBM PC or At with */
/* either monchrome (including Hercules in text mode) or CGA adaptor */
/* menu() and reset_menu() are externally visible, highlight() is an */
/* internal routine called by menu() only.  Written by Thomas Clune */
/* for the Eye Research Institute, 20 Staniford St., Boston, MA 02114 */
/* Copyright 1987, E.R.I.  All rights reserved. */


#include "msc_hdrs.h"   /* the usual gang of MS C headers */
#include "menu.h"
#include "ansi.h"
#include "mouselib.h"
#include "keys.h"
#include "sound.h"
#include "dos_func.h"

#define HERC_BASE       0XB0000000      /* base address of mono card */
#define CGA_BASE        0XB8000000      /* base address of color card */


static unsigned char off = 7;      /* unlit character attribute code */
static unsigned char on = 112;     /* lit character attribute code */

static int mouse_flag=KEYBOARD_ONLY;    /* flag for whether mouse-based selection is  */
	/* active.  Set to NO for compatibility with old version of menu.c */
	/* See mouse_flag_toggle() for mouse_flag values */
static double off_time=OFF_TIME;
static double duty_time=DUTY_TIME;
static int mouse_sensitivity=MOUSE_SENSITIVITY;

static char *screen_ptr;  /* base of screen memory to be named later */
static char *mode_ptr = (char *)0X00000410; /* mode memory address */

static void highlight();    /* local function for turning reverse */
				/* video on and off */
static int get_response();  /* the operator-selection function, added 12/88 */
	/* to support mouse input option as well as keyboard input */
static int mouse_read();    /* mouse support function for get_response() */





/* +++++++++++++++++ clear_input_devices() +++++++++++++++++++++++++ */
/* clear_input_devices() makes sure that no keypresses are pending  */
/* or mouse buttons depressed before the menu selection is active */

void clear_input_devices()
{
    int delay=0;

	/* clear the keyboard bufffer */
    if((mouse_flag != MOUSE_ONLY) && kbhit())
	while(kbhit())
	   getch();
 /* if the mouse is active, make sure the buttons are not depressed on entry */
    if((mouse_flag !=KEYBOARD_ONLY) && (button_read().status != 0))
    {
	do
	{
	    pause(0.1);
	    delay++;
	}while((button_read().status != 0) && (delay<20));

    /* after 2 secs, tell 'em to get their finger off the damn button */
	if(delay>=20)
	{
	    while(button_read().status !=0)
		mouse_warning_sound();
	}
    }
}





/* +++++++++++++++++++++ get_file() +++++++++++++++++++++++++++++++++ */
/* get a filename by selecting from highlighting menu. Val returned is 0 */
/* unless there are too many files for this routine to process, in which case  */
/* the f.error_flag is -1, or unless QUIT is the menu item selected, in which */
/* case f.error_flag is set to 1. The memory freed in get_file() was allocated */
/* in get_dir() (ind dos_func.c) except for the QUIT string area, which was */
/* allocated here. */


string_struc get_file()
{

    int i,j,k;
    int length;
    static char *fnames[MAX_FILES+1];
    static string_struc f;
    static char dir_name[40]="";
    int default_flag=0;
    static char spec[14]="*.*";
    char search_name[54];
    static int last_choice=0;
    char c;

    f.error_flag=0;
    CLS;
    if(!strlen(dir_name))
    {
	strcpy(search_name, "Default directory");
	default_flag=1;
    }
    else
	strcpy(search_name, dir_name);
    printf("Current search path is %s\n",search_name);
    printf("Enter new path, or <CR> to keep current path\n");
    if(mouse_flag != KEYBOARD_ONLY)
	mouse_gets(dir_name);
    else
	gets(dir_name);
    if((!strlen(dir_name)) && (default_flag == 0))
	strcpy(dir_name, search_name);
    printf("Enter file format for search, or <CR> to keep default %s\n", spec);
    if(mouse_flag != KEYBOARD_ONLY)
	mouse_gets(f.string);
    else
	gets(f.string);
    if(strlen(f.string))
	strcpy(spec, f.string);

    strcpy(search_name, dir_name);
    length=strlen(search_name);
    if(length && (!(search_name[length-1]=='\\')))
	strcat(search_name, "\\");
    strcat(search_name, spec);

    i=get_dir(search_name,fnames,MAX_FILES);
    if(i>MAX_FILES)
    {
	printf("Too many files for array size\n");
	printf("Maximum number of file entries supported=%d\n", MAX_FILES);
	printf("Try using a more restrictive file format for the search\n");
	printf("Press any key to return to menu\n");
	if(mouse_flag != KEYBOARD_ONLY)
	    inpause();
	else
	    getch();
	f.error_flag=-1;
	i=MAX_FILES-1;  /* the number of buffers to free() */
    }
    else
    {
	k=MAX_FILES/20;  /* 20 lines for display */
	fnames[i]=malloc(5);
	if(fnames[i]==NULL)
	{
	    printf("Error allocating menu buffer memory.  Program aborting.\n");
	    exit(-1);
	}
	strcpy(fnames[i], "Quit");  /* add an ABORT option to menu */
	CLS;
	printf("Number of matching files: %d\n",i);
	    /* 80/k=number of spaces per field width */

	reset_menu(last_choice);
	j=menu(2,0,80/k,k,1,i+1,0,fnames);
	last_choice=j;
	CLS;
	if(j != i)  /* if not QUIT */
	{
	    strcpy(f.string, dir_name);
	    if((strlen(f.string))&&(!(f.string[strlen(f.string)-1]=='\\')))
		strcat(f.string, "\\");
	    strcat(f.string, fnames[j]);
	}
	else
	    f.error_flag=1;
    }
    for(j=0;j<=i;j++)
	free(fnames[j]);

    return(f);

}



/* ++++++++++++++++ get_mouse_flag() +++++++++++++++++++++++++++ */
/* get_mouse_flag() returns the current status of the mouse flag. */
/* it is useful in making other i/o operations conform to the menu */
/* style, i.e., if the mouse is active for the menu, you may want */
/* other input options to support mouse input, etc. Any non-zero */
/* value means that the mouse is active.  Any value >=0 means that */
/* the keyboard is active. */

int get_mouse_flag()
{
    return mouse_flag;

}





/* +++++++++++++++++++ menu() +++++++++++++++++++++++++++++++++++++++++ */
/*                                                                      */
/*              top_mar = top margin spacing (0<=t_m<=24)               */
/*              left_mar = left margin spacing (0<=l_m<=79)             */
/*              tab = total spaces per column                           */
/*              columns = number of columns in menu                     */
/*              line_feed = number of spaces between lines in menu      */
/*              entries = number of items in menu array                 */
/*              label = no auto numbering of menu items if 0,           */
/*                      auto number but no number-key select if = 1     */
/*                      auto number and number-key select if > 1        */
/*                      MAX. ENTRIES FOR AUTO NUMBERING = 10            */
/*                      auto capital lettering but no select if = -1    */
/*                      auto lettering with keyboard select if < -1     */
/*                      MAX. ENTRIES FOR AUTO LETTERING = 26            */
/*              str_ptr = pointer to menu entries array                 */
/*                                                                      */
/*              menu() returns the subscript of the menu item selected  */
/*              i.e., first item = 0, last item = ENTRIES - 1.          */
/*                                                                      */
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


int menu(top_mar,left_mar,tab,columns,line_feed,entries,label,str_ptr)


int top_mar, left_mar, tab, columns, line_feed, entries, label;
char *str_ptr[];


{

    static int index = 0;      /* menu array element currently active */
    int x, y, i, j, disp_num;  /* x,y screen coords, i,j menu array elements */
			       /* disp_num menu auto number */
    unsigned char inchar;       /* input key selection variable */


    if(columns==0)      /* reset index flag from reset_menu() */
    {
	index=line_feed;
	return;
    }

    if(index>=entries)  /* make sure index is not larger than the array */
	index=0;


    outp(0X3B4, 10);        /* turn off cursor */
    outp(0X3B5, 32);

    i = *mode_ptr;          /* is the system monitor monochrome? */
    if((i & 48) == 48)      /* if so, set mono screen base address */
	    screen_ptr = (char *)HERC_BASE;
    else                    /* if not, set color screen base address */
	    screen_ptr = (char *)CGA_BASE;


    for(i=0;i<entries;i++)                          /* print menu */
    {
	    j = 0;                     /* first element in menu[i] entry */
	    x = left_mar + (i % columns) * tab;   /* x-location for print */
	    y = top_mar + (i/columns)*line_feed;  /* screen y for print */
	    if(label > 0)             /* use numbers if appropriate */
	    {
		disp_num = i + '1';      /* "ASCII-ize" the number */
		if(disp_num > '9')       /* >=10 prints as 0 */
		       disp_num = '0';
	    }
	    if(label < 0)      /* use capital letters if appropriate */
	    {
		disp_num = 'A' + i;
		if(disp_num > 'Z')
			disp_num = 'Z';
	    }
	    if(label != 0)     /* for auto numbers or letters */
	    {
		*(screen_ptr + 160*y + 2*x) = disp_num;
		if(index == i)             /* highlight first number */
			*(screen_ptr + 160*y + 2*x + 1) = on;
		else                    /* but not the others */
			*(screen_ptr + 160*y + 2*x + 1) = off;
		x += 1;         /* go to next screen memory location */
		*(screen_ptr + 160*y + 2*x) = 32;  /* print space after # */
		if(index == i)               /* highlight first space */
			*(screen_ptr + 160*y + 2*x + 1) = on;
		else                    /* but not the others */
			*(screen_ptr + 160*y + 2*x + 1) = off;
		x += 1;         /* go to next screen memory location */
	    }
	    while(str_ptr[i][j]!='\0')  /* print the string to the end */
	    {
		*(screen_ptr + 160*y + 2*x) = str_ptr[i][j];
		if(index == i)      /* highlight as appropriate */
		    *(screen_ptr + 160*y + 2*x + 1) = on;
		else
		    *(screen_ptr + 160*y + 2*x + 1) = off;
		x += 1;     /* next screen memory location */
		j += 1;     /* next menu[i] character */
	    }
	    *(screen_ptr + 160*y + 2*x) = str_ptr[i][j]; /* use '\0' */
				/* to mark end of menu entry on screen */
    }

    clear_input_devices();

    while((inchar=get_response())!=CARRIAGE_RETURN)  /* move highlight around selections */
    {
	inchar = toupper(inchar);   /* caps only for alpha entries */

	highlight(index,top_mar,left_mar,tab,columns,line_feed,off);

	if(inchar==UP_MOVE)     /* going up? */
	    if((index-columns)>=0)
		index -= columns;     /* increment selection */

	if(inchar==RIGHT_MOVE)  /* right? */
	    if(((index%columns)<(columns-1)) && ((index+1)<entries))
		index++;

	if(inchar==DOWN_MOVE)   /* going down? */
	    if((index+columns)<entries)
		index  += columns;

	if(inchar==LEFT_MOVE)  /* going left? */
	     if((index%columns)>0)
		 index--;

	highlight(index,top_mar,left_mar,tab,columns,line_feed,on);

	if(label>1)   /* are number keys active for selection? */
	{
	    if(entries != 10)
		j='1';
	    else
		j='0';
	    if(inchar>=j && inchar <=(j+entries-1))
	    /* if num-key selection */
	    {
		highlight(index,top_mar,left_mar,tab,columns,line_feed,off);
		index=inchar-'1';   /* "Numberize" ASCII and */
		    /* subtract 1 for array starting at 0 */
		if(index<0)   /* menu 0 = array 9 */
		   index = 9;
		highlight(index,top_mar,left_mar,tab,columns,line_feed,on);
		break;
	    }
	}

	if(label<-1)  /* are letter keys active? */
	{
	   if(inchar>='A' && inchar<=('A'+entries-1))
	    /* if letter key selected */
	    {
		   highlight(index,top_mar,left_mar,tab,columns,line_feed,off);
		   index = inchar-'A';
		   highlight(index,top_mar,left_mar,tab,columns,line_feed,on);
		   break;
	    }
	}


    }


    outp(0X3B4, 10);            /* turn cursor back on */
    outp(0X3B5, 11);

    return index;         /* return selected number */
}



/* +++++++++++++++++++ menu_mouse_config() ++++++++++++++++++++++++++++++ */
/* menu_mouse_config() sets the number of mickeys needed for a mouse movement */
/* to register as significant, and the duty cycle for evaluating the mouse */
/* motion.  duty_on is the time in second when the mouse is accumulating */
/* data, and duty_off is the time when the data is ignored.  The need for */
/* an "off" time is to give a positive feel to making a selection. */

void menu_mouse_config(sensitivity,duty_on,duty_off)
int sensitivity;
double duty_on, duty_off;
{
    mouse_sensitivity=sensitivity;
    duty_time=duty_on;
    off_time=duty_off;
}





/* +++++++++++++++++++++++++ mgetch() +++++++++++++++++++++++++++++++++ */
/* mgetch() is the generic mouse-and-keyboard or just-keyboard getch() */
/* function.  It decides whether to use the mouse on the basis of the status */
/* of mouse_flag.   inpause() is in mouselib.c */

int mgetch()
{
    int ret_val;

    if(mouse_flag)
	ret_val=inpause();
    else
	ret_val=getch();

    return ret_val;

}



/* +++++++++++++++++++++ mgets() ++++++++++++++++++++++++++++++++++++ */
/* mgets() is the generic mouse-plus keuyboard or just-keyboard gets() */
/* function.  The decision on whether to use the mouse is based on the */
/* status of mouse_flag. mouse_gets() is in mouselib.c */

char * mgets(strptr)
char *strptr;
{
    clear_input_devices();
    if(mouse_flag)
	strptr=mouse_gets(strptr);
    else
	strptr=gets(strptr);

    return strptr;

}





/* +++++++++++++++++++++++++ mouse_flag_toggle() +++++++++++++++++++++++++++++++ */
/* mouse_flag_toggle() sets the variable mouse_flag on or off to enable and disable */
/* mouse-based selection of menu items in menu() and dir_menu().  Added 12/88 */
/* by T Clune. if VAL=0, mouse is disabled.  If VAL>0, mouse and keyboard */
/* are enabled.  IF mouse<0, mouse only is enabled. */

void mouse_flag_toggle(val)
int val;
{
    if(val>0)
	mouse_flag=MOUSE_PLUS_KEYBOARD;
    if(val==0)
	mouse_flag=KEYBOARD_ONLY;
    if(val<0)
	mouse_flag=MOUSE_ONLY;
}




/* ++++++++++++++++++++++++++++ pathprint() ++++++++++++++++++++++++++++ */
/* pathprint() prints your current position in the menu structure on the */
/* bottom of the screen.  Pathprint() must be called at each menu in the */
/* heirarchy in order to have the correct information.  MENU_NAME is the */
/* current menu's pathprint name.  All pathprint() does is determine whether */
/* that name is new to the list or not.  If yes, it is added to the path, */
/* if no, the path list is truncated at that name. */

void pathprint(menu_name)
char menu_name[];
{
    static char path_name[80]="";
    static char divider[]="->";
    char *strptr;

    strptr=strstr(path_name, menu_name);

	/* if the name is not currently on the printed path */
    if(strptr==NULL)
    {
	if(strlen(path_name))  /* if not the first name, add name separator */
	    strcat(path_name, divider);
	strcat(path_name, menu_name);  /* add the name */
    }
	/* if the name is already on the path */
    else
    {
	*strptr='\0'; /* delete everything from the name on in the path */
	strcat(path_name, menu_name); /* and re-append the name at the end */
    }
    POSITION(1,24);
    LINE_CLEAR;
    POSITION(1,24);
    printf("You are here: %s\n", path_name);

}



/* +++++++++++++++++++++++ print_headline() +++++++++++++++++++++++++++++ */
/* print_headline() writes an underlined string starting at x, y (1<=x<=80, */
/* 1<=y<=25).  It is intended for use printing the title of a menu, which is */
/* passed as STRING.  Added by T. Clune, 8/89. */

void print_headline(string, x, y)
char string[];
int x, y;
{

	    /* POSITION and CHAR_ATTRIBUTE are ANSI.H macros */

	POSITION(x,y); /* center heading */
	CHAR_ATTRIBUTE(UNDERSCORE);
	printf("%s\n",string);

	CHAR_ATTRIBUTE(NORMAL);
}






/* ++++++++++++++++ reset_menu() ++++++++++++++++++++++++++++++ */
/* reset_menu() resets the static variable that "remembers" which */
/* selection was last made on the current menu.  It should be called */
/* whenever leaving a menu level for a new menu level that may call */
/* menu(), or an error condition can result if the next menu has */
/* fewer entries than the previous menu. SETTING may be set to the */
/* value that the menu being returned to last had or to 0 as a reset */
/* without "memory." */


void reset_menu(setting)
int setting;
{
	char *dummy;

	menu(0,0,0,0,setting,0,0,dummy);      /* reset menu static variable to setting */
}





/* ++++++++++++++++++++++++++++++ get_extended_key() ++++++++++++++++++++++ */
/* get_extended_key() gets the ALT:KEY arrow codes and maps them onto single */
/* character values (The arrow keys are mapped onto four low-usage ASCII */
/* vals: CTL-A, CTL-B, CTL-D, and CTL-E.) */

static int get_extended_key()
{
    int inchar;

    inchar=getch();     /* get the second key */
    switch(inchar)
    {
	case UPARROW:        /* UP=CNTL-A */
	    inchar=UP_MOVE;
	    break;
	case LEFTARROW:        /* LEFT=CNTL-B */
	    inchar=LEFT_MOVE;
	    break;
	case RIGHTARROW:        /* RIGHT=CNTL-D */
	    inchar=RIGHT_MOVE;
	    break;
	case DOWNARROW:        /* DOWN=CNTL-E */
	    inchar=DOWN_MOVE;
	    break;
	default:        /* if it was anything else, "erase" it */
	    inchar=0;
    }
    return inchar;
}




/* ++++++++++++++++++++++++ get_response() +++++++++++++++++++++++++++ */
/* get_response() is the menu() and dir_menu() operator selection function */
/* that returns the character selected by keyboard or the mouse selection */
/* if mouse_flag is not zero */

static int get_response()
{
    int ret_val= -1;

    while(ret_val == -1)
    {
	if((mouse_flag != MOUSE_ONLY) && kbhit())
	{
	    ret_val=getch();
	    if(ret_val==ALT)
		ret_val=get_extended_key();
		/* clear the keyboard buffer */
	    while(kbhit())
		getch();
	}
	else
	    if(mouse_flag != KEYBOARD_ONLY)
		ret_val=mouse_read();

    }
    return ret_val;

}



/* +++++++++++++++++++ highlight() +++++++++++++++++++++++++++ */


static void highlight(index,top_mar,left_mar,tab,columns,line_feed,value)


int index,top_mar,left_mar,tab,columns,line_feed;
unsigned char value;

{
    /* Highlight selected entry and un-highlight deselected entry */
    /* only menu attributes are modified */


    int x, y;

    x = left_mar + (index % columns) * tab;
    y = top_mar + (index/columns)*line_feed;


    while(*(screen_ptr + 160*y + 2*x) != '\0')
    {
	    *(screen_ptr + 160*y + 2*x + 1) = value;
	    x += 1;
    }


}


/* ++++++++++++++++++++++++++ mouse_read() +++++++++++++++++++++++++++++ */
/* mouse_read() reads the current condition of the mouse, and returns -1 */
/* if there is "nothing to report", the value is the keyboard equivalent */
/* to the mouse reading significance otherwise. */

static int mouse_read()
{
    int ret_val= -1;
    mstruc m;
    if(button_read().status == RIGHT_BUTTON)
	ret_val=CARRIAGE_RETURN;
    else
    {
	/* clear mouse position counter, pause to let user feel positive lock */
	/* on selected position, like keyboard repeat delay */
	pause(off_time);
	mdpos_read();
	pause(duty_time);
	m=mdpos_read();
	m.dx /=mouse_sensitivity;
	m.dy /=mouse_sensitivity;
    /* the mouse will only respond left/right or up/down on a given read */
	if(m.dx || m.dy)
	{
	    if(abs(m.dx) >= abs(m.dy))
	    {
		 if(m.dx>0)
		     ret_val=RIGHT_MOVE;
		  else
		     ret_val=LEFT_MOVE;
	    }
	    else
	    {
		if(m.dy>0)
		    ret_val=DOWN_MOVE;
		else
		    ret_val=UP_MOVE;
	    }
	}
    }

    return ret_val;
}
