#include <tcutil.h>
#include <conio.h>


print_init(int lptnum)
/* Initialize the printer identified by lptnum. lptnum = 1-3 */
{
	return(biosprint(1,0,lptnum - 1) & 0x29);
}

print_stat(int lptnum)
/* query the status of the printer attached as lptnum. lptnum = 1-3.
	RETURNED = 0 if printer is ready for work.
			 = 1 if printer is not ready.
*/
{
int rc, rc2, ocol, orow;
char opt;
char saver[740];
	rc2=0;
	ocol=wherex();
	orow=wherey();
	window(29,7,65,16);
	gettext(29,7,65,16,saver);
	textbackground(blue);
	textcolor(white);
    highvideo();
ptype_ok:
	rc=biosprint(2,0,lptnum - 1);
	if(rc & 0x2f || !rc) {
		clrscr();
		gotoxy(12,1);
		cprintf("[PRINTER ERROR]");
		gotoxy(12,10);
		cprintf("[ESC TO EXIT]");
		gotoxy(2,3);
		cprintf("The Printer is not responding.");
		gotoxy(2,4);
		cprintf("Make sure you entered the correct");
		gotoxy(2,5);
		cprintf("printer number, that the printer");
		gotoxy(2,6);
		cprintf("power is on, and it has paper.");
		gotoxy(2,7);
		cprintf("Press a space bar to retry or");
		gotoxy(2,8);
		cprintf("Press ESC to cancel print.");
		get_akey(&opt," \x1b");
		if(opt=='\x1b') {
			rc2=1;
			goto end_it;
		}
		goto ptype_ok;
	}
end_it:
	puttext(29,7,65,16,saver);
	window(1,1,80,25);
	textbackground(black);
	textcolor(cyan);
	gotoxy(ocol,orow);
	return(rc2);
}



print_str(char *str, int lptnum)
/* Print the string pointed to by *str on printer lptnum where
   lptnum = 1-3.
   RETURNED = 0 if ok.
			= 1 if error.
*/
{
	int rc;

	while(*str) {
		rc=biosprint(0,*str,lptnum - 1);
		if(rc & 0x29) return(1);
		str++;
	}
	return(0);
}



print_char(char ch, int lptnum)
/* Print the character ch on printer lptnum where lptnum = 1-3.
   RETURNED = 0 if ok.
			= 1 if error.
*/
{
	int rc;

	rc=biosprint(0,ch,lptnum - 1);
	if(rc & 0x29) return(1);
	return(0);
}

