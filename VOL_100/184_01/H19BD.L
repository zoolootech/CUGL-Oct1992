/* Heath H-19 terminal specific routines */
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
	char c;

	putchar(ESC);
	putchar('n');
	while ((c=bdos(6,255)) == 0) ;
	while ((c=bdos(6,255)) == 0) ;
	while ((*y=bdos(6,255)) == 0) ;
	while ((*x=bdos(6,255)) == 0) ;
	*y -= 32;
	*x -= 32;
}
savexy()
/* issue the heath cursor save command */
{
	putchar(ESC);
	putchar('j');
}
restxy()
/* restore the heath cursor saved with savexy() */
{
	putchar(ESC);
	putchar('k');
}
clear()
/* clear the screen and home cursor */
{
	putchar(ESC);
	putchar('E');
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
	putchar('K');
}
revvid()
/* enter heath reverse video mode */
{
	putchar(ESC);
	putchar('p');
}
norvid()
/* enter heath normal video mode */
{
	putchar(ESC);
	putchar('q');
}
termset()
/* set up the terminal to operate */
{
	putchar(ESC);
	putchar('t');		/* keyboard shifted */
	putchar(ESC);
	putchar('y');
	putchar('5');		/* cursor on */
	putchar(ESC);
	putchar('x');
	putchar('4');		/* block cursor */
	putchar(ESC);
	putchar('y');
	putchar('8');		/* no auto-LF */
	putchar(ESC);
	putchar('y');
	putchar('9');		/* no auto-CR */
}
termreset()
/* reset terminal to whatever the power-up mode was */
{
	int i;

	putchar(ESC);
	putchar('z');		/* back to power-up config */

	for (i=0; i<20; i++)	/* send nulls to allow terminal */
		putchar('\0');	/* time to initialize itself    */
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
C) {
		      while ((c=bdos(6,255))==0);
		      return(c+128);
                      }
	else	return(