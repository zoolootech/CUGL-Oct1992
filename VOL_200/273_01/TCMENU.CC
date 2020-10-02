#include <tcutil.h>
#include <tcmenu.h>
#include <string.h>




menubar(struct menu_struc *menu1, struct pull_down *pulls)
/* 	This function will display a menu-bar at the top of the screen.
	Before calling this function you must set up the two menu stuctures
	needed my the function. The two structures are:
	1) menu_struc
	2) pull_down
	The structures are defined in tcmenu.h.  To initialize do the following:

	struct menu_struc menu = {4,setatr(WHITE,BLUE,BOLD,0),
							"Application\0Gen\0Help\0Last\0"};

	item-1 = number of menu selections.
	item-2 = attr of menu bar.
	item-3 = string of menu items seperated by a null character.

	struct pull_down pulls[] = {4,20,"app1\0app2\0app3\0app4\0",
								4,20,"gen1\0gen2\0gen3\0gen4\0",
								4,20,"help1\0help2\0help3\0help4\0",
								10,20,"l1\0l2\0l3\0l4\0l5\0l6\0l7\0l8\0l9\0la\0"};

	Define structure pull_down with the same number of pull down menus as there
	are menu items.

	item-1 = number of pull down selections.
	item-2 = length of the biggest pull down selection.
	item-3 = string of pull down selections seperated by a null character.

	To call the function do as follows:

	item_num=menubar(&menu,&pulls[0]);

	return = 0 if no items where selected.
	return !=0 if an item was selected.
			   (the returned value will = the number of the menu item
			   selected. I.E. 1-x;

    ALSO menu.selected will = the main menu item number selected and
		 menu.p_sel will = the pull down item number under that main menu item.

*/

{
int attr2, attr3;
int p, i, start, interval;
int x, key_val, rc, rc2;
int cur_state, orow, ocol;
char *items, save_1[160];
static int first = 1;
	if(first) {
		video_type();
		first=0;
	}
	items=menu1->menu_string;
	cur_state=get_cur(&orow,&ocol);
	hide_cur();
	attr2=_rotr(menu1->attr,4) & 0x0007;
	attr3=_rotl(menu1->attr,4) & 0x0070;
	attr2 = attr2 | attr3 | 0x0008;
	menu1->rattr=attr2;
	save_scr(0,0,0,79,&save_1);
	x=0;
	clrarea(0,0,0,79,menu1->attr);

	interval=80 / menu1->nsels;

	i = 0;
	start=1;
	for(p=0; p < menu1->nsels; p++) {
		menu1->icol[x]=start;
        menu1->ilen[x]=strlen(items);
		x++;
		writef(0,start,menu1->attr,items);
		i = strlen(items) + 1;
		items += i;
		start += interval;
	}
    x=0;
	rc2=0;
	for(;;) {
		rcolor(0,menu1->icol[x],attr2,menu1->ilen[x]);
		if(rc2 == 0)
			key_val=get_xa();
		else  {
			key_val=rc2;
			if(rc2 == K_ENTER) rc2=0;
		}
		switch (key_val) {
			case K_ESC:
				rc=0;
				menu1->selected = 0;
				menu1->p_sel=0;
				goto pass_back;
			case K_RIGHT:
				rcolor(0,menu1->icol[x],menu1->attr,menu1->ilen[x]);
				x++;
				if(x == menu1->nsels) x=0;
				if(rc2 == K_RIGHT) rc2=K_ENTER;
				break;
			case K_LEFT:
				rcolor(0,menu1->icol[x],menu1->attr,menu1->ilen[x]);
				x--;
				if(x < 0) x = menu1->nsels -1;
				if(rc2 == K_LEFT) rc2=K_ENTER;
				break;
			case K_ENTER:
				rc=x + 1;
				menu1->selected = rc;
				rc2=do_pull_down(pulls+x,menu1);
				if(rc2 == K_ESC) {
					rc2 = 0;
					break;
				}
				if(rc2 != K_ENTER) break;
				goto pass_back;
			default: break;
		}
	}
pass_back:
	rest_scr(0,0,0,79,&save_1);
	if(!cur_state) show_cur(1);
	locate(orow,ocol);
	return(rc);
}


do_pull_down(struct pull_down *pull, struct menu_struc *menu1)
{
int p, i, start;
int x, key_val, rc;
int wleft, wright, wbottom;
int y;
char *save_2;
char *items;
    save_2=(char *)malloc(4000);
	y = menu1->selected - 1;
	items=pull->pull_items;
	x=0;
	if(menu1->icol[y] > 39) {
		wright=menu1->icol[y] + menu1->ilen[y];
		wleft=wright - pull->mlen - 2;
	}
	else {
		wleft=menu1->icol[y] - 1;
		wright=wleft + pull->mlen + 2;
	}

	wbottom=2 + pull->npulls;

	save_scr(1,wleft,wbottom,wright,save_2);
	make_window(1,wleft,wbottom,wright,menu1->attr,menu1->attr,0,"","");
	if(menu1->icol[y] > 39) {
		writefc(1,wright,menu1->attr,'\xb4');
		writefc(1,wright - menu1->ilen[y] - 1,menu1->attr,'\xc1');
	}
	else {
		writefc(1,wleft,menu1->attr,'\xc3');
		writefc(1,wleft + menu1->ilen[y] + 1,menu1->attr,'\xc1');
	}

	i = 0;
	start = 2;
	for(p=0; p < pull->npulls; p++) {
		writef(start,wleft + 1,menu1->attr,items);
		i = strlen(items) + 1;
		items += i;
		start ++;
	}

    x=1;
	for(;;) {
		rcolor(x+1,wleft+1,menu1->rattr,pull->mlen);
		key_val=get_xa();
		switch (key_val) {
			case K_ESC:
				menu1->p_sel = 0;
				rc=key_val;
				goto pass_back;
			case K_DOWN:
				rcolor(x+1,wleft+1,menu1->attr,pull->mlen);
				x++;
				if(x > pull->npulls) x=1;
				break;
			case K_UP:
				rcolor(x+1,wleft+1,menu1->attr,pull->mlen);
				x--;
				if(x < 1) x = pull->npulls;
				break;
			case K_ENTER:
				menu1->p_sel = x;
				rc=key_val;
				goto pass_back;
			case K_RIGHT:
			case K_LEFT:
				menu1->p_sel = 0;
				rc=key_val;
				goto pass_back;
		}
	}
pass_back:
	rest_scr(1,wleft,wbottom,wright,save_2);
	free(save_2);
	return(rc);
}


