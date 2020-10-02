typedef void fn();

/* check malloc's answer, if no memory then exit graphics before aborting */

void *g_malloc(int siz);


extern struct submenu *rootmenu;
extern long int the_key;		/* function key value */
extern int keys_ptr;			/* key stack depth */

void get_stack_key();
long int lkeyof(char ch);

/* define how to wait for keys */
void wait_key(fn funct);


struct submenu {
	unsigned char direction;	/* HORIZONTAL, VERTICAL, INVISIBLE */
	long int choicekey;	/* simple key to activate this submenu */
	int lastchoice;	/* offset of last submenu of this menu that was used */
	char *shortname;	/* name of this menu/function */
	char *longname;
	fn *funct;
	struct submenu *next;	/* right pointer */
	struct submenu *prev;	/* left pointer */
	struct submenu *sub;	/* down pointer */
	struct submenu *esc;	/* up pointer */
	int belowx, belowy;	/* where longname would be put for submenu */
	int sizex, sizey;	/* graphics size of shortname */
};

void def_submenu(char choice, char *shortname, char *longname);
void endef_submenu(char *matchshort); /* repeat shortname to match & debug */
void def_entry(char choice, char *shortname, fn funct );
void menu_fn(fn funct );
void menu_horizontal();
void menu_invisible();

void nofn();		/* default function does nothing */

#define MAXKEYS 500



/* This routine allows the user to define a function-key.
   The keynum is "made up" by the user.
   It is a value between 1 and MAXKEYS and is used to match a keyboard entry
   to the key.

   The keyboard entry assigned to this function key is passed as an ASCII name.
   The name for this keyboard entry is passed as keyname.  
   Any value may be entered and the fkey may be
   re-defined later and saved in a config. file.

   From now on, whenever keyname is typed on the keyboard, fkey will be 
   returned as a value.  The name of fkey is fkeyname.
*/ 
int def_fkey(int fkey, char *keyname, char *fkeyname);


/* define a user defined key as being a 'hot' key.
   Whenever the key is encountered by the decoding routine, the
   given function will be executed.
*/
void hot_key(int keynum, fn funct);

/* close down the graphics system 
   print an error message
   stop the program.
*/
void g_failure(char *text);

void user_menus();	/* this is defined by the user */

/* function to initialize the graphics system for whatever hardware
   the user's computer happens to have.
   Used by restorescr() to 'undo' the effect of closegraph() from savescr();
   Something like this should be called by the user before menu_init() to turn
   on the graphics system.
*/
void initgr();
/* This is called after initgr() or some equivalent to it.
   This must be called before *ANYTHING* else may be called. 
*/
void menu_init();



void menu_run();		/* how to run the menu system */


/* save and restore graphics screen */
void savescr();
void restorescr();
void initgr();

/* Get a key from the stack.
   Check to see if it is a hot key, if so do it's routine.
   Save key value in global location.
*/
void call_for_key(struct submenu *cursor);


/* routine to print a text in a box
   The text will have BOX_X-1 pixels between it and the horiz. walls.
   The text will have BOX_Y-1 pixels between it and the vert. walls.
   The text will be white pixels on a black background.
*/
void boxtext(int x, int y, char *data);


/* 1=key available
   0=no key pending
*/
int keyready();



