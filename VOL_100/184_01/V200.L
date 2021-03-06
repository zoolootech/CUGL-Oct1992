/* Visual 200 terminal specific routines */

/* Hardware specific module for YANC10.C */
/* Based on H19BD.C by Ken Presser */
/* Current C dialect...BDS-C from BD Software */

gotoxy(x,y)
/* position cursor at line y column x */
char x,y;
{
	putchar(ESC);
	putchar('Y');
	putchar(y+32);
	putchar(x+32);
}
getxy(x,y)
/* get the current cursor line and column */
char *x,*y;
{
	putchar(ESC);
	putchar('r');
	while ((*y=bdos(6,255)) == 0) ;
	while ((*x=bdos(6,255)) == 0) ;
	*y -= 32;
	*x -= 32;
}
clear()
/* clear the screen and home cursor */
{
	putchar(ESC);
	putchar('v');
}
delline()
/* delete the line the cursor is on and move it to far left */
{
	putchar(CR);
	deol();
}
deol()
/* delete to end of line */
{
	putchar(ESC);
	putchar('x');
}
revvid()
/* enter background video mode */
{
	putchar(ESC);
	putchar('4');
}
norvid()
/* enter foreground (normal) video mode */
{
	putchar(ESC);
	putchar('3');
}
hinchar()
/* input characters and translate the heath ESC sequences *
 * into one character as character + 128                  */
{
	char c;

	while ((c = bdos(6,255)) == 0) /* do nothing */ ;
	if (c == ESC) {
		      while ((c=bdos(6,255))==0);
		      return(c+128);
                      }
	else	return(c);
}
/* #endif */
                                                                                                      