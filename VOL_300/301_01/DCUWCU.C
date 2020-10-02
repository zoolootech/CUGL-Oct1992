/*
 * the Hollywood principle: don't call us, we'll call you
 * 
 * This package uses pd mouse routines and Turbo C graphics
 * to implement a mouse, menu, and form handler.  An application
 * program provides the following routines:
 *	start(ac, av, ep)	- initialization
 *	menu(m, i)		- menu m, item i was selected
 *	button(b, x, y)		- unbound button was pressed 
 *      keyboard(key)		- keyboard character struck
 *      timer(t)                - a timer has gone off
 *
 * This package also provides the following interface routines:
 *	finish()		- all done
 *	add_menu(m, mdef)	- add a menu to the system
 *      menu_state(m, on)       - turn a menu on or off
 *	menu_item(m, i, str)	- change a menu item's string
 *	mouse_state(on)		- turn the mouse on or off
 *      mouse_shape(type, ...)	- set the mouse bitmap
 *      add_timer(t, wait)	- add a timer to the system
 *	form(fdef, argptr,...)	- have user fill out a form
 * 
 * Form definition is pretty complicated.  It's somewhat like scanf in
 * that it uses a format string and %Nd or %Ns to define an input
 * field.  A %[str] defines an exit button and a %(str) defines a regular
 * button.  If you select an exit button, you leave the form.  A
 * regular button has will toggle a value.  A '|' in the definition
 * string starts a new line.
 *
 * If no Microsoft compatible mouse driver is installed, it works
 * with keyboard input with the following conventions:
 *	cursor keys control the mouse
 *	F1 simulates a button 1 key depression (and release)
 *	F2 will enter a menu, a second F2 leaves it
 *
 * Copyright Mark A. Johnson, 1989
 */

#include <stdio.h>
#include <graphics.h>
#include <bios.h>

#define SAVE		5000	/* constant sizes			*/
#define MAXITEM		30
#define MAXMENU		10
#define MAXFORM		20
#define MAXTIMER	10

#define TXT_WD		8	/* text height and width		*/
#define TXT_HT		8

#define TITLE	't'		/* form item types			*/
#define NUMBER	'd'
#define STRING	's'
#define EXIT	'e'
#define BUTTON	'b'
#define RADIO	'r'

#define ESC	27		/* constants for keyboard handling	*/
#define ENTER	'\r'
#define TAB	'\t'
#define F1	256
#define F2	257
#define UP	258
#define DOWN	259
#define LEFT	260
#define RIGHT	261

#define INS_CHR	17		/* editing cursor for forms		*/

#define MS_PER_TICK	64	/* milliseconds per tick (from bios)	*/

typedef struct { int wid, ht; char buf[10]; } Mbits;
typedef struct { char type, row, col, radio, size, *str, *data; } Form;
typedef struct { char **item, size, handle, on; } Menu;
typedef struct { char handle; long wait; } Timer;

static char mouse_ptr[] = { 
	0xFC, 0xFC, 0xF0, 0xF8, 0xDC, 0xCE, 0x07, 0x02 
};
static char mouse_cross[] = {
	0x18, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0x18
};
static Mbits mbits = {
	7, 7, { 0xFC, 0xFC, 0xF0, 0xF8, 0xDC, 0xCE, 0x07, 0x02, 0, 0 }
};
int Mx, My;			/* current mouse coordinates		*/
static int Mhotx=0, Mhoty=0;	/* offset for mouse cursor hotspot	*/
static int Mshow;		/* is mouse showing or not		*/
static int Mpresent;		/* is a mouse present or not		*/

int Maxx, Maxy;			/* graphics driver position range	*/
int MaxColor;			/* graphics driver color range		*/

int MaxSave;			/* keep track of max size save needed	*/
static char save[SAVE];		/* buffer to save patches of screen	*/

static char *items[MAXITEM];	/* menu item table			*/
static int last_item;		/* size of menu table			*/
static Menu menus[MAXMENU];	/* menu descriptor table		*/
static int last_menu;		/* size of menu descriptor table	*/

static Form forms[MAXFORM];	/* form table				*/
static int fcnt;		/* number of items in the form table	*/
static char fbuf[100];		/* buffer for the form def'tion string	*/
static int fwid;		/* max width of the form		*/
static int fht;			/* max height of the form		*/
static int fl, ft, fr, fb;	/* form box coordinates			*/
static char ebuf[100];		/* edit buffer for form item		*/
static int ebufx;		/* index into the edit buffer		*/
static int edit;		/* index of current editable item	*/

static Timer timers[MAXTIMER];	/* a set of timers			*/
static int last_timer;		/* number of active timers		*/
static long last_tick;		/* last tick of the old ticker		*/

main(argc, argv, envp) char **argv, **envp; {
	int b, x, y;
	int driver, mode;

	Mx = My = 0;
	if ((Mpresent = ms_init()) == 0)
		printf("no mouse!\n");

	driver = mode = DETECT; /* detect */
	initgraph(&driver, &mode, ".");
	if (driver < 0) {
		printf("cannot open graph driver, driver=%d\n", driver);
		exit(1);
	}
	start(argc, argv, envp);
	Maxx = getmaxx();
	Maxy = getmaxy();
	MaxColor = getmaxcolor();
	last_tick = biostime(0, 0L);
	if (Mpresent)
		mouse_appl();
	else	key_appl();
}

/* drive the application from the mouse */

static
mouse_appl() {
	int b, x, y;
	ms_setspeed(10, 10);
	ms_hbounds(0, Maxx);
	ms_vbounds(0, Maxy);
	mouse_state(1);
	while (1) {
		do_timer();
		if (kbhit()) {
			mouse_state(0);
			keyboard(getch());
			mouse_state(1);
		}
		if (ms_button()) {
			delay(50);
			if ((b = ms_button()) != 0) {
				mouse_state(0);
				if (b == 2)
					do_menu();
				else	{
					button(b, Mx, My);
					while (ms_button())
						;
				}
				mouse_state(1);
			}
		}
		ms_motion(&x, &y);
		if (x || y) move_mouse(x, y);
	}
}

/* drive the application from the keyboard */

static
key_appl() {
	int c, x, y;
	mouse_state(1);
	while (1) {
		do_timer();
		if (kbhit()) {
			c = key();
			if (c == F1) {
				mouse_state(0);
				button(1, Mx, My);
				mouse_state(1);
			}
			else if (c == F2) {
				mouse_state(0);
				do_menu();
				mouse_state(1);
			}
			else if (is_arrow(c, &x, &y))
				move_mouse(x, y);
			else	{
				mouse_state(0);
				keyboard(c);
				mouse_state(1);
			}
		}
	}
}

/* change the mouse bitmap */

mouse_shape(type, data, hx, hy) char *data; {
	int i;
	char *p;
	int m_on = Mshow;
	if (m_on) mouse_state(0);
	switch (type) {
	case 0: 
		p = mouse_ptr; 
		Mhotx = Mhoty = 0;
		break;
	case 1: 
		p = mouse_cross; 
		Mhotx = Mhoty = 3;
		break;
	case 2: 
		p = data; 
		Mhotx = hx;
		Mhoty = hy;
		break;
	}
	for (i = 0; i < 8; i++)
		mbits.buf[i] = p[i];
	if (m_on) mouse_state(1);
}

/* handle keyboard input, map F1, etc to single keys */

static
key() {
	int c;
	static int back = 0;
	if (back) {
		c = back;
		back = 0;
	}
	else	{
		c = getch();
		if (c == 0) {
			switch (c = getch()) {
			case 59: c = F1; break;
			case 60: c = F2; break;
			case 72: c = UP; break;
			case 80: c = DOWN; break;
			case 77: c = RIGHT; break;
			case 75: c = LEFT; break;
			default: back = c; c = 0; break;
			}
		}
	}
	return c;
}

/* is the given key an arrow key, if so, set the delta x,y pointers */

static
is_arrow(c, xp, yp) int c, *xp, *yp; {
	static int lastc, cnt;
	if (c != lastc) {
		lastc = c;
		cnt = 1;
	}
	else	cnt++;
	*xp = *yp = 0;
	switch (c) {
	case UP:
		*yp = -cnt;
		break;
	case DOWN:
		*yp = cnt;
		break;
	case LEFT:
		*xp = -cnt;
		break;
	case RIGHT:
		*xp = cnt;
		break;
	default:
		return 0;
	}
	return 1;
}

/* the application requests termination */

finish() {
	closegraph();
	exit(0);
}

/* move the mouse a bit */

static
move_mouse(dx, dy) {
	mouse_state(0);
	Mx += dx;
	if (Mx < 0) Mx = 0;
	if (Mx > Maxx) Mx = Maxx;
	My += dy;
	if (My < 0) My = 0;
	if (My > Maxy) My = Maxy;
	mouse_state(1);
}

/* turn the mouse picture on or off */

mouse_state(on) {
	if (on ^ Mshow)
		putimage(Mx-Mhotx, My-Mhoty, &mbits, XOR_PUT);
	Mshow = on;
}

/* add a timer to the system, wait is in milliseconds */

add_timer(handle, wait) long wait; {
	int i, j;
	long total = 0L;

	/* find the insertion point */
	for (i = 0; i < last_timer; i++) {
		if (wait < total + timers[i].wait)
			break;
		total += timers[i].wait;
	}

	/* shuffle things down if necessary */
	if (i < last_timer) { 
		for (j = last_timer; j > i; j--)
			timers[j] = timers[j-1];
	}
	
	/* insert the new timer, adjust wait to incr over previous */
	wait -= total;
	timers[i].handle = handle;
	timers[i].wait = wait;

	/* fix up the timers that follow it */
	for ( ; i < last_timer; i++) 
		timers[i].wait -= wait;
	last_timer++;

#ifdef DEBUG
	/* debugging */
	for (i = 0; i < last_timer; i++)
		message(300, i * 10, "%d: h=%d w=%ld",
			i, timers[i].handle, timers[i].wait);
#endif
}

/* parse a menu definition string and add it to menu table */

add_menu(handle, menu_def) char *menu_def; {
	int start = last_item;
	char *s, *strtok();
	if (last_menu >= MAXMENU || last_item >= MAXITEM) {
		trouble("too many menu's");
		return 0;
	}
	s = strtok(menu_def, ":");
	menus[last_menu].item = &items[last_item];
	menus[last_menu].handle = handle;
	menus[last_menu].on = 1;
	while (s && last_item < MAXITEM) {
		items[last_item++] = s;
		s = strtok(NULL, "|");
	}
	menus[last_menu].size = last_item - start;
	last_menu++;
}

/* find a menu index, given a handle */

static
find_menu(handle) {
	int i;
	for (i = 0; i < last_menu; i++)
		if (menus[i].handle == handle)
			return i;
	return -1;
}

/* activate or deactivate a menu */

menu_state(handle, new_state) {
	Menu m;
	int i = find_menu(handle);
	if (i >= 0) {
		if (new_state == 1 && !menus[i].on) {
			menus[i].on = 1;
			menu_top(i);
			return 1;
		}
		else if (new_state == 0 && menus[i].on) {
			menus[i].on = 0;
			m = menus[i];
			while (i < last_menu-1 && menus[i+1].on) {
				menus[i] = menus[i+1];
				i++;
			}
			menus[i] = m;
			return 0;
		}
		else	return menus[i].on;
	}
	else	return -1;
}

/* move a menu item to the top spot */

static
menu_top(i) {
	Menu m;
	m = menus[0];
	menus[0] = menus[i];
	while (i > 0 && !menus[i-1].on) {
		menus[i] = menus[i-1];
		i--;
	}
	menus[i] = m;
}

/* change a menu item */

menu_item(handle, index, item) char *item; {
	int i = find_menu(handle), j;
	char **p;
	if (i >= 0 && index > 0 && index < menus[i].size) {
		p = menus[i].item;
		p[index] = item;
		return 1;
	}
	return 0;
}

/* handle the application timers */

static
do_timer() {
	int i, h;
	long ticks = (biostime(0, 0L) - last_tick) * MS_PER_TICK;
	Timer *t = &timers[0];

	if (last_timer > 0) {
		t->wait -= ticks;
		while (last_timer > 0 && t->wait <= 0) {
			h = t->handle;
			last_timer--;
			for (i = 0; i < last_timer; i++)
				timers[i] = timers[i+1];
			timer(h);
		}
	}
	last_tick = biostime(0, 0L);
}

/* handle a menu selection */

static
do_menu() {
	int i, x, y, c, lasti, w, h;
	int mcnt = 0, mwid = 0, handle = menus[0].handle;
	int ml, mt, mr, mb; 	/* basic menu coordinates */
	int tl, tt, tr, tb; 	/* total menu coordinates */
	int active;		/* active menu's */
	char **item = menus[0].item; /* pointer to menu items */

	/* figure out how many menu's are active */
	for (i = 0; i < last_menu && menus[i].on; i++)
		;
	active = i;
			
	/* figure out size of the base menu */
	mcnt = menus[0].size;
	for (i = 0; i < mcnt; i++) {
		c = strlen(item[i]) + 1;
		if (c > mwid) mwid = c;
	}
	for (i = 1; i < active; i++) {
		c = strlen(*menus[i].item) + 1;
		if (c > mwid) mwid = c;
	}

	/* compute base menu screen size */
	ml = Mx;
	mt = My;
	mr = ml + (mwid * TXT_WD);
	mb = mt + (mcnt * (TXT_HT+1)) + 2;

	/* compute total menu patch size and save it */
	tl = ml;
	tt = mt - ((active - 1) * (TXT_HT+1));
	tr = mr + (active * (TXT_WD/2));
	tb = mb;
	if (tl < 0 || tr > Maxx || tt < 0 || tb > Maxy)
		return;
	if ((i = imagesize(tl, tt, tr, tb)) > SAVE) {
		trouble("SAVE needs to be > %d", i);
		return;
	}
	else if (i >= MaxSave) MaxSave = i;

	getimage(tl, tt, tr, tb, save);
	setfillstyle(EMPTY_FILL, 0);
	bar(tl, tt, tr, tb);
	
	/* display menu items */
	setcolor(MaxColor);
	setfillstyle(CLOSE_DOT_FILL, MaxColor);
	bar(ml, mt, mr, mt+TXT_HT);
	rectangle(ml, mt, mr, mb);
	line(ml, mt+9, mr, mt+(TXT_HT+1));
	y = mt + 2;
	x = ml + (TXT_WD/2);
	for (i = 0; i < mcnt; i++, y += (TXT_HT+1))
		outtextxy(x, y, item[i]);

	/* display other menus (headers only) */
	y = mt - (TXT_HT+1);
	x = ml + (TXT_WD/2);
	w = mr - ml;
	h = mb - mt;
	for (i = 1; i < active; i++, x += (TXT_WD/2), y -= (TXT_HT+1)) {
		bar(x, y, x+w, y+TXT_HT);
		outtextxy(x+(TXT_WD/2), y+1, *menus[i].item);
		moveto(x, y+(TXT_HT+1));
		linerel(0, -(TXT_HT+1));
		linerel(w, 0);
		linerel(0, h);
		linerel(-(TXT_WD/2), 0);
	}

	/* now track the mouse and look for a release */
	lasti = i = -1;
	mouse_state(1);
	while (1) {
		if (Mpresent) {
			if (ms_button() != 2)
				break;
			ms_motion(&x, &y);
		}
		else	{
			c = key();
			if (c == F2)
				break;
			is_arrow(c, &x, &y);
		}
		if (x || y) {
			move_mouse(x, y);
			i = mouse_in_item(Mx, My, ml, mt+2, mr, mb);
			if (i != lasti) {
				menu_invert(lasti, mcnt, ml, mr, mt);
				menu_invert(i, mcnt, ml, mr, mt);
				lasti = i;
			}
		}
	}

	/* check to see if other menu was chosen */
	y = mouse_in_item(Mx, My, tl, tt, tr, mt);
	if (y >= 0) {
		y = active - 1 - y;
		menu_top(y);
	}

	/* put things back the way they were */
	mouse_state(0);
	putimage(tl, tt, save, COPY_PUT);

	/* call the application menu routine if selected */
	if (i >= 1 && i < mcnt) menu(handle, i);
}

/* highlight (or de-highlight) a menu item by inverting it */

static
menu_invert(i, mcnt, l, r, t) {
	if (i >= 1 && i < mcnt) {
		t = t + (i * (TXT_HT+1)) + 2;
		invert(l+2, t-1, r-2, t+(TXT_HT-1));
	}
}

/* see if the given mouse coordinates are on top of a menu item */

static
mouse_in_item(x, y, l, t, r, b) {
	if (x >= l && x <= r && y >= t && y <= b) {
		return (y - t) / (TXT_HT+1);
	}
	else	return -1;
}

/* invert a (small) patch of the screen */

static
invert(l, t, r, b) {
	static char buf[100];
	getimage(l, t, r, b, buf);
	putimage(l, t, buf, NOT_PUT);
}

/* parse a form definition string */

static
parse_form(frm, argp) char *frm, **argp; {
	Form *fp;
	char *s = fbuf;
	int i, c = 0, n, b;
	fcnt = fht = fwid = 0;
	strcpy(fbuf, frm);
	while (*s && fcnt < MAXFORM) {
		if (*s == ' ') { /* skip spaces */
			*s++ = 0;
			c++;
		}
		else if (*s == '|') { /* start a new line */
			*s++ = 0;
			if (c > fwid) fwid = c;
			fht++;
			c = 0;
		}			
		else if (*s == '%') { /* data field */
			*s++ = 0;
			fp = &forms[fcnt++];
			fp->row = fht;
			fp->col = c;
			fp->size = 0;
			fp->data = *argp++;
			if ((b = *s) == '[' || b == '{') { /* buttons */
				fp->type = (b == '{' ? EXIT : BUTTON);
				fp->str = s;
				b = (b == '[' ? ']' : '}');
				while (*s && *s++ != b) {
					if (*s == ':')
						fp->type = RADIO;
					c++;
				}
				if (fp->type == RADIO)
					radio_set(fp);
				c += 2;
			}
			else	{ /* number or a string */
				n = 0;
				while (*s && *s >= '0' && *s <= '9')
					n = n * 10 + (*s++ - '0');
				fp->size = n;
				c += (n + 2);
				if (*s == 'd' || *s == 's')
					fp->type = *s++;
				else	trouble("bad form: '%s'\n", frm);
			}
		}
		else	{ /* title field */
			fp = &forms[fcnt++];
			fp->row = fht;
			fp->col = c;
			fp->type = TITLE;
			fp->str = s;
			fp->size = 0;
			while (*s && *s != '|' && *s != '%') {
				c++;
				s++;
			}
		}
	}
	if (c > fwid) fwid = c;
	fht++;
	return 1;
}

/* do a form for the user */

form(frm, args) char *frm; {
	int c, i, x, y;
	char *s;

	/* create the form */
	if (!parse_form(frm, &args)) {
		trouble("bad form: '%s'\n", frm);
		return 0;
	}

	/* convert text counts to pixel sizes */
	fht = fht * (TXT_HT+1) + 2;
	fwid = fwid * TXT_WD + 6;

	/* center the form, save area underneath, and clear it out */
	fl = Maxx/2 - fwid/2;
	ft = Maxy/2 - fht/2;
	fb = ft + fht;
	fr = fl + fwid;
	if ((i = imagesize(fl, ft, fr, fb)) > SAVE) {
		trouble("SAVE needs to be > %d", i);
		return 0;
	}
	else if (i > MaxSave) MaxSave = i;
	getimage(fl, ft, fr, fb, save);
	setfillstyle(EMPTY_FILL, 0);
	bar(fl, ft, fr, fb);
	setcolor(MaxColor);
	rectangle(fl, ft, fr, fb);

	/* draw the form */
	edit = -1;
	for (i = 0; i < fcnt; i++) {
		item_get(ebuf, &forms[i]);
		switch (forms[i].type) {
		case NUMBER: case STRING:
			if (edit == -1) edit = i;
			break;
		}
		item_show(ebuf, &forms[i], edit == i);
	}
	if (edit != -1) {
		item_get(ebuf, &forms[edit]);
		ebufx = strlen(ebuf);
	}

	/* get the data */
	mouse_state(1);
	while (1) {
		if (Mpresent) {
			if (ms_button() == 1 && form_button())
				break;
			ms_motion(&x, &y);
			if (x || y) move_mouse(x, y);
			if (kbhit() && (c = form_key(getch())) != 0)
				break;
		}
		else	{
			c = key();
			if (is_arrow(c, &x, &y))
				move_mouse(x, y);
			else if (c == F1 && form_button())
				break;
			else if ((c = form_key(c)) != 0)
				break;
		}
	}

	/* finish up */
	mouse_state(0);
	putimage(fl, ft, save, COPY_PUT);
	return c == ESC ? 0 : 1;
}

/* handle a mouse button 1 push during form processing */

static
form_button() {
	int i, l, t, r, b;
	Form *fp;
	for (i = 0; i < fcnt; i++) {
		fp = &forms[i];
		switch (fp->type) {
		case BUTTON: case EXIT: case RADIO:
			l = fl + fp->col * TXT_WD;
			t = ft + fp->row * (TXT_HT+1);
			r = l + strlen(fp->str) * TXT_WD;
			b = t + (TXT_HT+1);
			if (Mx >= l && Mx <= r && My >= t && My <= b) {
				if (fp->type == RADIO)
					radio_button(fp, (Mx - l)/TXT_WD);
				else	*fp->data = !*fp->data;
				mouse_state(0);
				item_show(fp->str, fp, 0);
				mouse_state(1);
				return (fp->type == EXIT);
			}
			break;
		}
	}
	return 0;
}

/* handle selection of a radio button item */

static
radio_button(fp, cx) Form *fp; {
	int i, n;
	char *s = fp->str;
	for (i = n = 0; s[i] && i < cx; i++) {
		if (s[i] == ':')
			n++;
	}
	*fp->data = n;
	radio_set(fp);
}

/* set up the .radio and .size fields for a radio button */

static
radio_set(fp) Form *fp; {
	char *s = fp->str;
	int i, n = *fp->data;
	for (i = 1; s[i] && n; ) {
		if (s[i++] == ':') 
			n--;
	}
	if (s[i] == 0) { /* bogus data value */
		fp->radio = fp->size = 0;
	}
	else	{
		fp->radio = i;
		while (s[i] && s[i] != ':' && s[i] != ']')
			i++;
		fp->size = i;
	}
}

/* handle a keyboard character pressed during form processing */

static
form_key(c) {
	int r = 0;
	mouse_state(0);
	if (c == ESC || c == ENTER) {
		if (edit != -1) item_update(ebuf, &forms[edit]);
		r = c;
	}
	else if (c == TAB) {
		if (edit != -1) {
			item_update(ebuf, &forms[edit]);
			item_show(ebuf, &forms[edit], 0);
			while (1) {
				if (++edit >= fcnt) edit = 0;
				if (forms[edit].type == STRING ||
				    forms[edit].type == NUMBER)
					break;
			}
			item_get(ebuf, &forms[edit]);
			ebufx = strlen(ebuf);
			item_show(ebuf, &forms[edit], 1);
		}
	}
	else if (edit != -1) {
		if (c == '\b' && ebufx > 0)
			ebuf[--ebufx] = 0;
		else if (c >= ' ') {
			ebuf[ebufx++] = c;
			ebuf[ebufx] = 0;
		}
		item_show(ebuf, &forms[edit], 1);
	}
	mouse_state(1);
	return r;
}

/* get the string of the form item value into the buffer */

static
item_get(buf, fp) char *buf; Form *fp; {
	switch (fp->type) {
	case TITLE: case BUTTON: case EXIT: case RADIO:
		strcpy(buf, fp->str);
		break;
	case STRING:
		strcpy(buf, fp->data);
		break;
	case NUMBER:
		sprintf(buf, "%d", *(int *)fp->data);
		break;
	}
}

/* show the given item, string rep in buf */

static
item_show(buf, fp, edit) char *buf; Form *fp; {
	int l, t, r, b, n;
	static char sbuf[100];
	strcpy(sbuf, buf);
	n = strlen(sbuf);
	if (edit) sbuf[n++] = INS_CHR;
	if (fp->type == NUMBER || fp->type == STRING) {
		while (n < fp->size) sbuf[n++] = '_';
	}
	sbuf[n] = 0;
	l = fl + 3 + fp->col * TXT_WD;
	t = ft + 2 + fp->row * (TXT_HT+1);
	r = l + n * TXT_WD;
	b = t + TXT_HT;
	if (fp->type == NUMBER || fp->type == STRING) /* extra blank */
		r += TXT_WD; 
	bar(l, t, r, b);
	outtextxy(l+1, t+1, sbuf);
	switch (fp->type) {
	case RADIO:
		if (fp->radio) 
			invert(l + fp->radio * TXT_WD, t, 
			       l + fp->size * TXT_WD, b);
		break;
	case BUTTON: case EXIT:
		if (*fp->data) invert(l+TXT_WD, t, r-TXT_WD, b);
		break;
	}
}

/* move the item's new data back to the application */

static
item_update(buf, fp) char *buf; Form *fp; {
	switch (fp->type) {
	case STRING:
		strncpy(fp->data, buf, fp->size);
		break;
	case NUMBER:
		*((int *)fp->data) = atoi(buf);
		break;
	}
}

/* we had some trouble, complain and get out */

trouble(fmt, args) char *fmt; {
	int mode = getgraphmode();
	restorecrtmode();
	vprintf(fmt, &args);
	printf("\n type any key to continue..."); getch();
	setgraphmode(mode);
}

#ifdef DEBUG
/* debugging routine */

message(x, y, fmt, a, b, c, d, e) char *fmt; {
	static char buf[100];
	sprintf(buf, fmt, a, b, c, d, e);
	setfillstyle(EMPTY_FILL, 0);
	bar(x, y, x+((strlen(buf)+1)*TXT_WD), y+TXT_HT);
	setcolor(1);
	outtextxy(x, y, buf);
}
#endif
