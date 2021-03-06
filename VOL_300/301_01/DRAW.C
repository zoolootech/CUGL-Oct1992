/*
 * this is a very simple drawing program that 
 * illustrates how to build a DCUWCU application
 *
 * Copyright 1989 Mark A. Johnson 
 */

#include <stdio.h>
#include <graphics.h>

#define M_POINTER	0	/* mouse shapes */
#define M_CROSS		1

#define ON		1
#define OFF		0

#define MAX_OBJECT	100
#define ESC	27

#define BOX	'b'	/* object types we support */
#define ELLIPSE	'e'
#define LINE	'l'
#define TEXT	't'

#define M_MAIN	1	/* handles for the menus */
#define M_FILE	2
#define M_OBJ	3
#define M_ACT	4

#define A_COPY	1	/* action requests for button() */
#define A_MOVE	2
#define A_EDIT	3

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

typedef struct { int type, l, t, r, b; char select, *data; } Object;

Object objects[MAX_OBJECT];	/* the table of objects defined so far */
int last_object;		/* the end of the object table	*/

int map[] = { 			/* maps a M_OBJ menu item to an object */
	0, BOX, ELLIPSE, LINE, TEXT };

char *about = 			/* form used on the M_MAIN About item */
	"  Draw This!|     by|Mark A. Johnson|  %{continue}";

char *help = 			/* help message for wrong keyboard input */
 "quit refresh : box line ellipse text : delete copy move edit";

char filename[20];		/* save the filename we're working with */
char text[100];			/* extra buffer for text i/o */

int actn_obj = 0;		/* flag for button(), some action req */
int make_obj = 0;		/* flag for button(), need to create */
int slct_cnt = 0;		/* count of selected objects */
int first;			/* helps make_object collect points */
int grid = 0;			/* grid displayed, snap coords */

extern int Maxx, Maxy, MaxColor;

/* start routine, called by the application driver, gets things going */

start(argc, argv) char **argv; {
	add_menu(M_MAIN, "Main:About|Quit|Refresh|Grid");
	add_menu(M_FILE, "File:Read|Write|Save|Print");
	add_menu(M_OBJ, "Objects:Box|Ellipse|Line|Text");
	add_menu(M_ACT, "Actions:Delete|Copy|Move|Edit");
	menu_state(M_ACT, 0);
	if (argc > 1) {
		strcpy(filename, argv[1]);
		read_objects();
	}
}

/* no timers in this application , but DCUWCU needs an entry anyway */

timer() {}

/* button routine called every time button 1 is depressed */

button(b, x, y) {
	if (make_obj) { /* need points to make an object */
		make_object(x, y);
	}
	else if (actn_obj) { /* got a point for a copy or move */
		action_object(x, y);
	}
	else	{ /* do a selection */
		select_object(in_object(x, y));
	}
	check_menu();
}

/* menu routine called every time a menu item is selected */

menu(m, i) {
	char junk = 0, on;
	switch (m) {
	case M_MAIN: /* main menu */
		switch (i) {
			case 1: form(about, &junk); break;
			case 2: quit(); break;
			case 3: refresh(); break;
			case 4: do_grid(); break;
		}
		break;
	case M_FILE: /* file menu */
		if (i < 3 && !get_name())
			break;
		switch (i) {
			case 1: read_objects(); break;
			case 2: case 3: write_objects(); break;
			case 4: print(); break;
		}
		break;
	case M_OBJ: /* objects */
		start_make(map[i]);
		break;
	case M_ACT: /* actions */
		switch (i) {
			case 1: kill_object(); break;
			case 2: start_actn(A_COPY); break;
			case 3: start_actn(A_MOVE); break;
			case 4: start_edit(); break;
		}
		break;
	}
}

keyboard(c) {
	switch (c) {
		case 'p': print(); break;
		case 'g': do_grid(); break;
		case 'r': refresh(); break;
		case 'q': quit(); break;
		case 'b': start_make(BOX); break;
		case 't': start_make(TEXT); break;
		case 'l': start_make(LINE); break;
		case 'm': start_actn(A_MOVE); break;
		case 'c': start_actn(A_COPY); break;
		case 'd': kill_object(); break;
		case 'e':
			if (slct_cnt)
				start_edit();
			else	start_make(ELLIPSE);
			break;
		default: msg(help);
	}
}

/* time to go, see if they really want to */

quit() {
	char yes = 0, no = 0;
	char *f_exit = "Are you sure?| %{yes} %{no}";
	if (form(f_exit, &yes, &no) && no == 0)
		finish();
}

/*
 * miscellaneous support routines
 */

do_grid() {
	char gridval, ok = 0, nok = 0, x;
	switch (grid) {
		case 8: gridval = 1; break;
		case 16: gridval = 2; break;
		default: gridval = 0; break;
	}
	x = form("Change Grid Size|  %[none:8:16]| %{ok} %{cancel}",
		&gridval, &ok, &nok);
	if (x == 0 || nok) return;
	grid = gridval * 8;
	refresh(); 
}

/* print the current screen somewhere, Epson-compatible graphics mode */ 

print() {
	static char grhd[] = { ESC, 'L', 0, 0 }; /* 960 bit graphics */
	static char grlf[] = { ESC, 'J', 24, '\r' }; /* line feed */
	static char prbuf[960];
	int x, y, i, b, n, any, pixel, max;
	max = min(Maxx, 960);
	grhd[2] = max;
	grhd[3] = max >> 8;
	mouse_state(OFF);
	b = 0x80;
	any = 0;
	for (y = 0; y < Maxy; y++) {
		for (x = 0; x < max; x++) {
			if (getpixel(x, y)) {
				any = 1;
				prbuf[x] |= b;
			}
		}
		b >>= 1;
		if (b == 0) { /* out it goes */
			if (any) {
				prn(grhd, 4);
				prn(prbuf, max);
			}
			prn(grlf, 4);
			b = 0x80;
			any = 0;
			for (x = 0; x < max; x++)
				prbuf[x] = 0;
		}
	}
	mouse_state(ON);
}

/* print the n bytes out the printer port */

prn(s, n) char *s; { while (n--) biosprint(0, *s++, 0); }

/* add the invert the selected item */

select_object(obj) {
	int i;
	Object *o;
	if (obj == -1) { /* de-select all */
		for (i = 0; i < last_object; i++) {
			o = &objects[i];
			if (o->select) {
				o->select = 0;
				highlight(o, 0);
			}
		}
		slct_cnt = 0;
	}
	else	{
		o = &objects[obj];
		o->select = !o->select;
		highlight(o, o->select);
		slct_cnt += o->select ? 1 : -1;
	}
}

/* get a filename from the user, return 0 if abort */

get_name() {
	return form("Path: %20s", filename);
}

check_menu() {
	menu_state(M_ACT, slct_cnt > 0);
	menu_state(M_OBJ, slct_cnt <= 0);
}

/* start to make an object by collection points */

start_make(type) {
	char *s;
	switch (make_obj = type) {
		case BOX: s = "box: top left corner..."; break;
		case ELLIPSE: s = "ellipse: top left corner..."; break;
		case LINE: s = "line: one end..."; break;
		case TEXT: s = "text: starting..."; break;
	}
	msg(s);
	mouse_shape(M_CROSS);
	first = 1;
}

/* if enough points collected, make the object */

make_object(x, y) {
	static int fx, fy;

	if (grid) snap(&x, &y);

	switch (make_obj) {
	case TEXT:
		*text = 0;
		form("text: %20s", text);
		add_object(TEXT, x, y, x + strlen(text)*8, y+8, text);
		make_obj = 0;
		mouse_shape(M_POINTER);
		msg("");
		break;
	default:
		if (first) {
			fx = x;
			fy = y;
			first = 0;
			line(x-3, y, x+3, y);
			line(x, y-3, x, y+3);
			if (make_obj == LINE)
				msg("other end...");
			else	msg("bottom right corner...");
		}
		else	{
			add_object(make_obj, fx, fy, x, y, 0L);
			msg("");
			make_obj = 0;
			mouse_shape(M_POINTER);
		}
	}
}

/* snap the coordinates to the nearest grid point */

snap(xp, yp) int *xp, *yp; {
	int g2 = grid/2, g4 = grid/4, x = *xp, y = *yp;
	x = ((x + g2) / grid) * grid;
	y = ((y + g4) / g2) * g2;
	msg("x %d->%d  y %d->%d", *xp, x, *yp, y);
	*xp = x;
	*yp = y;
}

/* move, copy, or edit a figure */

action_object(x, y) {
	int i, dx, dy;
	Object *o;

	if (grid) snap(&x, &y);

	/* find reference point and compute distance moved */
	dx = dy = (actn_obj == A_EDIT ? 0 : 10000);
	for (i = 0; i < last_object; i++) {
		o = &objects[i];
		if (o->select) {
			if (actn_obj == A_EDIT) {
				dx = max(o->r, dx);
				dy = max(o->b, dy);
			}
			else	{
				dx = min(o->l, dx);
				dy = min(o->t, dy);
			}
		}
	}
	dx = x - dx;
	dy = y - dy;

	/* do it to all selected items, de-selecting as you go */
	for (i = 0; i < last_object; i++) {
		o = &objects[i];
		if (o->select) {
			o->select = 0;
			highlight(o, 0);
			switch (actn_obj) {
			case A_COPY:
				highlight(o, 0);
				add_object(o->type,
					o->l + dx, o->t + dy,
					o->r + dx, o->b + dy,
					o->data);
				break;
			case A_MOVE:
				draw_object(o, 0);
				o->l += dx;
				o->t += dy;
				o->r += dx;
				o->b += dy;
				draw_object(o, 1);
				break;
			case A_EDIT:
				draw_object(o, 0);
				set_coords(o, 
					o->l, o->t, 
					o->r + dx, o->b + dy);
				draw_object(o, 1);
				break;
			}
		}
	}

	/* deselect all and reset the mouse */
	actn_obj = 0;
	slct_cnt = 0;
	mouse_shape(M_POINTER);
	msg("");
	check_menu();
}

/* read objects from a file */

read_objects() {
	int type, t, l, r, b;
	FILE *f = fopen(filename, "r");
	if (f != NULL) {
		last_object = 0;
		while (fgets(text, 100, f)) {
			sscanf(text, "%c %d %d %d %d '%[^']\n", 
				&type, &l, &t, &r, &b, text);
			add_object(type, l, t, r, b, text);
		}
		fclose(f);
		msg("%d objects loaded", last_object);
	}
	else	msg("can't open '%s'", filename);
}

/* write objects to a file */

write_objects() {
	int i;
	Object *o;
	FILE *f;
	if (*filename == 0 && !get_name())
		return;
	if ((f = fopen(filename, "w")) != NULL) {
		for (i = 0; i < last_object; i++) {
			o = &objects[i];
			fprintf(f, "%c %d %d %d %d '%s'\n",
				o->type, o->l, o->t, o->r, o->b, 
				o->type == TEXT ? o->data : "");
		}
		fclose(f);
	}
	else	msg("can't write '%s'", filename);
}

/* save the given string in malloc'ed memory */

char *
strsave(s) char *s; {
	char *malloc();
	char *r = malloc(strlen(s)+1);
	if (r) strcpy(r, s);
	else msg("out of memory!!!");
	return r;
}

/* re-display all the objects on the screen */

refresh() {
	int i, x, y, gy;
	Object *o;
	clearviewport();
	setcolor(MaxColor);
	if (grid) {
		gy = grid/2;
		for (x = grid; x < Maxx; x += grid)
		for (y = gy; y < Maxy; y += gy)
			putpixel(x, y, 1);
	}
	for (i = 0; i < last_object; i++) {
		o = &objects[i];
		draw_object(o, 1);
		if (o->select) highlight(o, 1);
	}
}

/* (de)highlight the current selected item */

highlight(o, color) Object *o; {
	setcolor(color);
	rectangle(o->l-2, o->t-2, o->l+2, o->t+2);
	rectangle(o->r-2, o->b-2, o->r+2, o->b+2);
}

/* give the user some feedback */

msg(fmt, a, b, c, d) char *fmt; {
	static int lastback = 0;
	setfillstyle(EMPTY_FILL, 0);
	bar(0, 0, lastback, 8);
	sprintf(text, fmt, a, b, c, d);
	setcolor(MaxColor);
	outtextxy(0, 0, text);
	lastback = strlen(text) * 8;
}

/* 
 * object handling 
 */

/* see if x,y are in an object, begin looking at start + 1 */

in_object(x, y) {
	static int last = 0;
	int l, r, t, b;
	Object *o;
	int i = last+1, n = last_object;
	while (n--) {
		if (i >= last_object) i = 0;
		o = &objects[i];
		l = min(o->l, o->r);
		r = max(o->l, o->r);
		t = min(o->t, o->b);
		b = max(o->t, o->b);
		if (x >= l && x <= r && y >= t && y <= b)
			return (last = i);
		i++;
	}
	return (last = -1);
}

/* add an object to the object table */

add_object(type, l, t, r, b, data) char *data; {
	Object *o = &objects[last_object++];
	char *s;
	o->type = type;
	set_coords(o, l, t, r, b);
	o->select = 0;
	if (type == TEXT)
		o->data = strsave(data);
	draw_object(o, 1);
}

/* set the coordinates properly */

set_coords(o, l, t, r, b) Object *o; {
	if (o->type == LINE) { /* no fixup on these */
		o->l = l; 
		o->t = t; 
		o->r = r; 
		o->b = b;
	}
	else	{
		o->l = min(l, r);
		o->t = min(t, b);
		o->r = max(l, r);
		o->b = max(t, b);
	}
}

/* draw an object on the screen */

draw_object(o, color) Object *o; {
	int x, y, xrad, yrad;
	setcolor(color);
	switch (o->type) {
	case TEXT:
		x = strlen(o->data) * 8;
		setfillstyle(EMPTY_FILL, 0);
		bar(o->l, o->t, o->l + x, o->t + 8);
		outtextxy(o->l, o->t, o->data);
		break;
	case BOX:
		rectangle(o->l, o->t, o->r, o->b);
		break;
	case LINE:
		line(o->l, o->t, o->r, o->b);
		break;
	case ELLIPSE:
		x = o->l + (o->r - o->l)/2;
		y = o->t + (o->b - o->t)/2;
		xrad = o->r - x;
		yrad = o->b - y;
		ellipse(x, y, 0, 360, xrad, yrad);
		break;
	}
}

/* delete an object */

kill_object() {
	int i, j;
	Object *o;
	for (i = j = 0; i < last_object; i++) {
		o = &objects[i];
		if (o->select) {
			highlight(o, 0);		
			draw_object(o, 0);
			o->select = 0;
		}
		else	{
			if (i > j)
				objects[j++] = objects[i];
			else	j++;
		}
	}
	last_object = j;
	slct_cnt = 0;
	check_menu();
}

/* start an edit on selected objects */

start_edit() {
	int i;
	Object *o;
	/* edit the text objects now */
	for (i = 0; i < last_object; i++) {
		o =  &objects[i];
		if (o->type == TEXT && o->select) {
			o->select = 0;
			highlight(o, 0);
			draw_object(o, 0);
			strcpy(text, o->data);
			if (form("edit: %20s", text)) {
				free(o->data);
				o->data = strsave(text);
				o->r = o->l + strlen(text)*8;
			}
			draw_object(o, 1);
			slct_cnt--;
		}
	}
	if (slct_cnt > 0) { /* must be other stuff */
		start_actn(A_EDIT);
	}
	check_menu();
}

/* initiate an action on selected objects */

start_actn(actn) {
	switch (actn) {
	case A_COPY: 
		msg("copy to..."); 
		break;
	case A_MOVE: 
		msg("move to..."); 
		break;
	case A_EDIT:
		msg("editing...");
		break;
	}
	actn_obj = actn;
	mouse_shape(M_CROSS);
}
