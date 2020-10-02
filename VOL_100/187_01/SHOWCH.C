/*@*****************************************************/
/*@                                                    */
/*@ showch - display a character on the screen.        */
/*@        The character is displayed normally, if     */
/*@        possible, displayed as a reverse video      */
/*@        name (e.g. NUL for 0x00), if appropriate,   */
/*@        or displayed as \xxx\ where xxx is the      */
/*@        decimal value of the character.             */
/*@                                                    */
/*@   Usage:     showch(ch);                           */
/*@       where ch is a character.                     */
/*@                                                    */
/*@   Returns the number of screen positions used to   */
/*@       display the character.                       */
/*@                                                    */
/*@*****************************************************/

#define EOS '\0'
#define     REVVID  0x70    /* reverse video attribute */
#define     NORM    0x07    /* normal video attribute */

int
showch(c)
char c;
{
	static char *cntlchr[] = {"NUL","SOH","STX","ETX","EOT","ENQ","ACK","BEL",
        "BS ","HT ","LF ","VT ","FF ","CR ","SO ","SI ","DLE",
        "DC1","DC2","DC3","DC4","NAK","SYN","ETB","CAN","EM ",
        "SUB","ESC","FS ","GS ","RS ","US "};

	int ret, strbuf[4];
	ret = 0;

	if ((c >= 0) && (c <= 31)) {   /* HANDLE CONTROL CHARS */
		ret = 3;   /* number of positions used */
		conout(cntlchr[(int)c],REVVID);
	}
	else if ((c >= ' ') && (c <= '~')) { /* HANDLE NORMAL CHARS */
	    ret = 1;  /* number of positions used */
	    strbuf[0] = c;		/* display as normal video */
	    strbuf[1] = EOS;
		conout(strbuf,NORM);
	}
	else if (c == 127) {		/* HANDLE 0X7F (DEL) */
		ret = 3;   /* number of positions used */
		conout("DEL",REVVID);
	}
	else {				/* HANDLE FUNNY CHARS */
		conout("\\",REVVID);
		itoa(c,strbuf);
		conout(strbuf,REVVID);
		conout("\\",REVVID);
		ret = strlen(strbuf) + 2;  /* number of positions used */
	}
	conout(" ",NORM);         /* separate displays */
	return(++ret);
}

