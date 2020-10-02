/* hdwlib.c -- hardware library -- contents:	1984 may 4
	read & set date/time - Newclock80
     	primitives for graphics peripheral - TMS9918a
	cursor-control for TRS-80 screen w/ Omikron bios
	PM Krasno 17813 Kiowa Trail Los Gatos CA 95030
*/

#include "bdscio.h"
#define short char
#define byte char
#define K 1024

/* -- fcns to read & set Newclock80	*/

unsigned tim_dif (d, t1, t2) char *d, *t1, *t2 ; 
/* time difference in seconds, up to 18hrs or so */ {
	char *t ; int n ; unsigned dif ;
	unsigned val[12] ; /* seconds, each digit-position */
	val[6] = 36000 ;	val[7] =  3600 ; /* hrs */
	val[8] =   600 ;	val[9] =    60 ; /* min */
	val[10] =   10 ;	val[11] =    1 ; /* sec */
 	n = 0 ; /* digit counter */
	while ( t1[n] && t1[n] == t2[n] ) /* leading zeroes */
		++n ; 
	if ( t1[n] < t2[n] ) /* if negative, swap pointers */
		{ t = t1 ; t1 = t2 ; t2 = t  ; }
	if ( n < 6 ) return 65535 ; /* error - day changed */
	for (dif = 0 ; n < 12 ; ++n)
		dif += val[n] * (t1[n] - t2[n]) ;	
	return dif ;
} /* tim_dif */

#define clock_port 0xB0		/* Newclock80 base port */
#define clock_digs 13		/* # data digits	*/

syst_d (d) char *d ; { /* system date/time, "yymddhhmmss" */
/* read Newclock80, ASCII format	*/
#define day_dig 6
	/* compare tens of seconds before & after,
		in case of rollover */
	byte bef, aft ; int i, j ;

	aft = 0xF & inp (clock_port + 1) ;
	do {	bef = aft ;
		for (i = clock_digs - 1, j = 0 ; 0 <= i ; 
			--i, ++j )
		    if ( i == day_dig ) --j ; /* skip weekday*/
		    else d[j] = '0' + (inp (clock_port + i) &
			(((j==4) | (j==6)) ? 0x3 : 0xF) ) ;
		aft = 0xF & inp (clock_port + 1) ;
	} while ( bef != aft ) ;
	d[clock_digs-1] = 0 ;
#undef day_dig
} /* sys_d */

read_clock (d_t) byte *d_t ;	{
	/* compare tens of seconds before & after,
		in case of rollover */
	byte tens_bef, tens_aft ; int i, j ;

	tens_aft = 0xF & inp (clock_port + 1) ;
	do {	tens_bef = tens_aft ;
		for (i = clock_digs - 1, j = 0 ; 0 <= i ; 
			--i, ++j )
			d_t[j] = 0xF & inp (clock_port + i) ;
		tens_aft = 0xF & inp (clock_port + 1) ;
	} while ( tens_bef != tens_aft ) ;
} /* read_clock */

show_clock (d_t) byte *d_t ;	{
	int i, j, digit ; char *ptr ;
	ptr = "SunMonTueWedThuFriSat" ;	/* quasi-static */
	for (i = clock_digs, j = 0 ; 0 < i ; --i, ++j)	{
		digit = d_t[j] ;
		switch (j)	{ 
		case 4:        digit &= 0x3 ;
		case 2:          puts ("/") ; break ;
		case 7:        digit &= 0x3 ; break ;
		case 9: case 11: puts (":") ; break ;
		default : ;	} /* punctuation, masking */
		if ( j != 6 ) putchar (0x30 + digit) ;
		else printf (" %3.3s ",
			ptr + 3 * (0x7 & digit)) ;
	} /* for all digits */
	putchar ('\n') ;
} /* show_clock */

set_clock (d_t, n) byte *d_t ; int n ;	{ /* n = offset */
	int i, j ;
	for (i = clock_digs - 1 - n, j = n ; 0 <= i ; --i, ++j)
		outp (clock_port + j, 0xF & d_t[i]) ;
} /* set_clock */
#undef clock_digs
#undef clock_port

/*  - primitives for CHROMAtrs/TMS9918A graphics */
/* after an .asm package marked 
	"(c) 1982 South Shore Computer Concepts, Hewlett, NY"
	rewritten (NOT copied!) in BDS C 1983 Sep pmk */
/* vram addresses cleaned up, text mode added 1984 may pmk */
#define stat_port 0x79		/* CHROMAtrs port address */
#define reg_port  0x79
#define add_port  0x79
#define data_port 0x78		/* CHROMAtrs vram read/write */
#define STICK     0x7D		/* joystick */
#define ena_port  0xEC		/* TRS-80 MOD III I/O BUS */
#define PAT_GEN	0	/* pattern gen vram loc    */
#define PAT_NAM	0x1800	/* pattern name table      */
#define SPR_ATT	0x1C00	/* sprite attribute table  */
#define PAT_COL	0x2000	/* pattern color table     */
#define SPR_PAT	0x3800	/* sprite pattern table    */
#define WH 15	/* colors */
#define BL 1

vram_out (addr, data)	/* output to vram thru tms9918a */
unsigned addr, data ;	{
/*M III	outp (ena_port, 0x10) ; */	inp (stat_port) ;
	outp (reg_port, 0xFF & addr) ;	/* LSB */
	outp (reg_port, 0x40 | (0x3F & (addr>>8 ))) ;
		/* MSB, flags (asm needs NOP for timing ? */
	outp (data_port, data) ;	/* 1st data byte */
} /* vram_out */

vram_o_m (addr, data, count) /* out data byte count times */
unsigned addr, data, count ;	{
/*M III	outp (ena_port, 0x10) ; */	inp (stat_port) ;
	outp (reg_port, 0xFF & addr) ;
	outp (reg_port, 0x40 | (0x3F & (addr>>8) ) ) ;
	while ( count-- ) outp (data_port, data) ;
} /* vram_o_m */

vram_o_2 (addr, data1, data2) /* out 2 data bytes */
unsigned addr, data1, data2 ;	{
/*M III	outp (ena_port, 0x10) ; */	inp (stat_port) ;
	outp (reg_port, 0xFF & addr) ;
	outp (reg_port, 0x40 | (0x3F & (addr>>8) ) ) ;
	outp (data_port, data1) ;
	outp (data_port, data2) ;
} /* vram_o_2 */

vram_o_a (addr, pdata, count) /* out count bytes from array */
unsigned addr, count ; char *pdata ;	{
/*M III	outp (ena_port, 0x10) ; */	inp (stat_port) ;
	outp (reg_port, 0xFF & addr) ;
	outp (reg_port, 0x40 | (0x3F & (addr>>8) ) ) ;
	while ( count-- ) outp (data_port, *pdata++) ;
} /* vram_o_a */

int vram_in (addr)	/* input from vram thru tms9918a */
unsigned addr ;	{
/*M III	outp (ena_port, 0x10) ; */	inp (stat_port) ;
	outp (reg_port, 0xFF & addr) ;	/* LSB */
	outp (reg_port, 0x3F & (addr>>8) ) ;
	return inp (data_port) ;
} /* vram_in */

vram_i_a (addr, pdata, count)	/* input count bytes */
unsigned addr, count ; char *pdata ;	{
/*M III	outp (ena_port, 0x10) ; */	inp (stat_port) ;
	outp (reg_port, 0xFF & addr) ;
	outp (reg_port, 0x3F & (addr>>8) ) ;
	while ( count-- ) *pdata++ = inp (data_port) ;
} /* vram_i_a */

vreg_out (reg, data)	/* output to vreg in tms9918a */
unsigned reg, data ;	{
/*M III	outp (ena_port) ; */	inp (stat_port) ;
	outp (reg_port, data) ;		/* data */
	outp (reg_port, 0x80 | reg) ;	/* reg #, flag */
} /* vreg_out */

txt_mod ()	/* set up tms9918 text mode */	{
	byte reg[8] ;	int i, j ;
	reg[0] = 0x00 ;	/* M3 = 0, no video in */
	reg[1] = 0x80 | 0x10 | 0x02 ;
		/* 4116, blank, M1=1, size 8, mag 1X */
	reg[2] = PAT_NAM / 0x400 ;	/* PAT_NAM msb */
	reg[3] = PAT_COL / 0x40 ;	/* PAT_COL msb */
	reg[4] = PAT_GEN / 0x800 ;	/* PAT_GEN msb */
	reg[5] = SPR_ATT / 0x80 ;	/* SPR_ATT msb */
	reg[6] = SPR_PAT / 0x80 ;	/* SPR_PAT msb */
	reg[7] = (WH << 4) | BL ;	/* text, bkgr color */
	for (i = 7 ; 0 <= i ; --i)
		vreg_out (i, reg[i]) ;
	vram_o_m (0, 0, 16 * K) ;
	vram_o_m (PAT_NAM, ' ', 40 * 24) ;
	txt_font ("b:font.def") ; 
	vreg_out (1, reg[1] | 0x40) ; /* un-blank */
} /* txt_mod */

txt_font (file)	char *file ; {
#define FSIZ 0x60
	byte font[FSIZ][6] ; 
	/* vertical dot-matrix images, 6 bytes/char */ 
	char fontbuf[BUFSIZ] ;	int ifont[6], c, i, s ;
	if (fopen (file, fontbuf) == ERROR)
		printf ("%s ?\n", file) ;
	while (TRUE)	{    /* one pattern per line, to EOF */
		s = fscanf (fontbuf,
			"%x: %x, %x, %x, %x, %x, %x, %x, %x ;",
			& c, ifont, ifont+1, ifont+2,
			 ifont+3, ifont+4, ifont+5) ;
		if (s == EOF || s == CPMEOF || c ==0xff) break;
		else if (s == 7 && ' ' <= c && c <= 0x7f)  {
			for (i = 5 ; i >= 0 ; i--)
				font[c-' '][i] = ifont[i] ;
		} /* if good char */
		else	printf ("trouble in %s at %02x\n",
				file, c) ;
	} /* loop until EOF */
	fclose (fontbuf) ;
	wr_font (font) ;
} /* txt_font */

wr_font (font) byte font[FSIZ][6] ; { /* write font to vram */
	int i, j, k ; byte f[FSIZ][8] ;
	for (i = FSIZ-1 ; 0 <= i ; --i)	/* transpose */
	    for (k = 0 ; k < 8 ; ++k)	/* row */
		for (f[i][k] = j = 0 ; j < 6 ; ++j) /* col */
		    f[i][k] |=
 			( (font[i][j] << (7-k) ) & 0x80) >> j ;
	vram_o_a (PAT_GEN + 8 * ' ', f, FSIZ * 8) ;
} /* wr_font */
#undef FSIZ

txt_ch (l, c, ch) int l, c ; char ch ; { /* write text char */
	vram_out (PAT_NAM + 40 * (l-1) + c-1, ch) ;
} /* txt_ch */

sh_ch (f) char *f ; {
	int r, c ;
	for (printf("\N"),r=0 ; r<8 ; ++r, printf("\N") )
		for (c=0 ; c<8 ; ++c)
			printf("%s", f[r] & (0x80 >> c) ? 
				"**":"  ") ;
} /* sh_ch */

mode2 ()	/* set up tms9918 graphics mode 2 */	{
	byte reg[7] ;	int i ;
	reg[0] = 0x02 ;	/* M3 = 1, no video in */
	reg[1] = 0x80 | 0x40 | 0x02 ;
		/* 4116, active, size 8, mag 1X */
	reg[2] = PAT_NAM / 0x400 ;	/* PAT_NAM msb */
	reg[3] = 0xFF ; /* ??? */
	reg[4] = 0x03 ; /* ??? */
	reg[5] = SPR_ATT / 0x80 ;	/* SPR_ATT msb */
	reg[6] = SPR_PAT / 0x80 ;	/* SPR_PAT msb */
	for (i = 6 ; i >= 0 ; i--)
		vreg_out (i, reg[i]) ;
} /* mode2 */

cls (back_gr, fore_gr)		/* clear screen */
unsigned back_gr, fore_gr ;		{
	unsigned i, color ;
	color = (fore_gr << 4) | back_gr ;
	vram_out (PAT_NAM, 0) ;	/* clear pattern name table */
	for (i = 1 ; i < 0x0300 ; i++) outp (data_port, i) ;
	vram_o_m (PAT_GEN, 0, 0x1800) ;
		/* clear pattern generator table */
	vram_o_m (PAT_COL, color, 0x1800) ;
		/* clear pattern color table */
	vreg_out (7, 0x0C) ;	/* backdrop color */
} /* cls */

spr_init ()	/* clear sprite pattern table */	{
	vram_o_m (SPR_PAT, 0, 2048) ;
} /* spr_init */

spr_def (pat_num, pat)	/* define sprite pattern */
char pat_num, pat[32];	{	int i ;
	if ( pat_num > 63 ) return ;
	vram_o_a (SPR_PAT + (pat_num<<5), pat, 32) ;
} /* spr_def */

spr_patc (sp_num, pat_num, col)	/* set sprite pattern, color */
char sp_num, pat_num, col;	{
	if ( (sp_num > 31) | (pat_num > 63) ) return ;
	vram_o_2 (SPR_ATT + (sp_num<<2) + 2, pat_num<<2, col) ;
} /* spr_patc */	

spr_mov (sp_num, x, y)	/* move sprite to x, y */
char sp_num, x, y ;	{
	if ( sp_num > 31 ) return ;
	vram_o_2 (SPR_ATT + (sp_num<<2), y-1, x) ;
			/* hdw bug ? needs y-1 ! */
} /* spr_mov */

ld_font (font)	/* load font.def, define characters */	
short font[0x60][6] ; /* dot-matrix images, 6 bytes/char */ {
	char fontbuf[BUFSIZ] ;
	int ifont[6], c, i, s ;
	if (fopen ("font.def", fontbuf) == ERROR)
		printf ("font.def ?\n") ;
	while (TRUE)	{    /* one pattern per line, to EOF */
		s = fscanf (fontbuf,
			"%x: %x, %x, %x, %x, %x, %x, %x, %x ;",
			& c, ifont, ifont+1, ifont+2,
			 ifont+3, ifont+4, ifont+5) ;
		if (s == EOF || s == CPMEOF || c ==0xff) break;
		else if (s == 7 && ' ' <= c && c <= 0x7f)  {
			for (i = 5 ; i >= 0 ; i--)
				font[c-' '][i] = ifont[i] ;
		} /* if good char */
		else	printf ("%s %02x:\n",
				"trouble in font.def at", c) ;
	} /* loop until EOF */
	fclose (fontbuf) ;
} /* ld_font */

cbar ()	/* draw colored bars */	{
	unsigned i, x, y, color ; char *addr, bit_m ;
	for (x = 0, y = 56, color = 1 ;
	    color < 15 ;
	    x += 8, y += 8, color++)	{
		xycalc (x, y, &addr, &bit_m) ;
		for (i = 0 ; i < 8*18 ; i++)
		    vram_out (PAT_COL+addr++,color);
	} /* for x, y */
} /* cbar */

wr_str (font, x, y, foregr, text)	/* write string */
unsigned x, y, foregr ;	char **font, *text ;	{
	while (*text) {
		wr_ch (font, x, y, foregr, *text++) ;
		x += 6 ;
	} /* while */
} /* wr_str */

wr_ch(font, x, y, foregr, ch)	/* write char to screen, G2 */
short font[0x60][6] ; unsigned x, y, foregr, ch ;	{
	/* doit the hard way - bit-by-bit in GR2 mode */
	/* this means 48*6 outp's PER CHARACTER - optimise ! */
	unsigned xi, i, yj, ym, col ;
	for (xi = x, i = 0 ; i < 6 ; xi++, i++)
		for (yj = y, ym = 1 ; ym ; yj++, ym <<= 1)
			if (ym & font[ch-' '][i])
			        set (xi, yj, foregr) ;
			else  reset (xi, yj) ;
} /* wr_ch */

follow (sprite, stk, pat, off, col, x, y)
/* track joystick stk with sprite */
unsigned sprite, stk, pat, off, col, x, y ;	{
	short stick, dir, fire, oldf, draw ;
	unsigned xdel, ydel, paddle, loop ;
	spr_patc (sprite, pat, col) ;	/* display it	*/
	oldf = draw = FALSE ;
	while ( TRUE )	{
	    spr_mov (sprite, x-off, y-off) ;
	    stick = ~inp (STICK + stk) ;  fire = stick & 0x10 ;
	    if ( fire && (! oldf) ) draw = ( ! draw ) ;
	    oldf = fire ;  /* flip draw when fire pushed */
	    if (stick & 1) x++ ;	if (stick & 2) x-- ;
	    if (stick & 4) y++ ;	if (stick & 8) y-- ;
	    if (x == 0xF8) x = 0x08 ;  if (x == 0x07) x = 0xF7;
	    if (y == 0xB8) y = 0x08 ;  if (y == 0x07) y = 0xB7;
	    if ( draw ) set(x, y, col); else printf (" \b \b");
	} /* forever */
} /* follow */

line (x1, y1, x2, y2, color) /* draw line (x1,y1) to (x2,y2) */
short x1, x2, y1, y2 ; int color ; {
	/* Bresenham's Algorithm, MiniMicro Systems Nov 1983 */
	int t, a, b, xd, yd, dx, dy, x, y ;
	set (x1, y1, color) ;		/* plot end pt */
	dx = x2 - x1 ;	dy = y2 - y1 ; xd = yd = 1 ;
	if ( (dx | dy) == 0 ) return ;
	if ( dx < 0 ) { dx = - dx ; xd = -1 ; }
	if ( dy < 0 ) { dy = - dy ; yd = -1 ; }
	if ( dy < dx )	{
		a = dy + dy ; t = a - dx ; b = t - dx ;
		for (y = y1, x = x1 + xd ; x != x2 ; x += xd) {
/*-> */			if ( t < 0 )	t += a ;
			else { t += b ; y += yd ; }
			set (x, y, color) ;
		} /* for x */
	} /* flat */
	else {	a = dx + dx ; t = a - dy ; b = t - dy ;
		for (x = x1, y = y1 + yd ; y != y2 ; y += yd) {
/*-> */			if ( t < 0 )	t += a ;
			else { t += b ; x += xd ; }
			set (x, y, color) ;
		} /* for y */
	} /* steep */
	set (x2, y2, color) ;
} /* line */

set (h, v, col)		/* point on */
unsigned h, v, col ;	{
	unsigned offset, by, bit_m ;
	by = xycalc (h, v, &offset, &bit_m) ;
	vram_out (PAT_GEN + offset, by | bit_m) ;
	if ( col <= 15 ) {  /* set color */
	    by = vram_in (PAT_COL + offset) ;
	    vram_out (PAT_COL + offset, 
		(col<<4) | (0x0F & by)) ;
	} /* color */
} /* set */

reset (h, v)		/* point off */
unsigned h, v ;	{
	unsigned offset, by, bit_m ;
	by = xycalc (h, v, &offset, &bit_m) ;
	vram_out (offset, by & ~bit_m) ;
} /* reset */

test (h, v)		/* test point */
unsigned h, v ;	{
	unsigned offset, by, bit_m ;
	by = xycalc (h, v, &offset, &bit_m) ;
	return (by & bit_m) ;
} /* test */

xycalc (h, v, offset, b_m)    /* find byte & bit for (h, v) */
unsigned h, v, *offset, *b_m ;	{
	*b_m = 0x80 >> (h & 0x07) ;
	*offset = ((v & 0xF8) << 5) | (h & 0xF8) | (v & 0x07) ;
	return (vram_in (PAT_GEN + *offset)) ;
} /* xycalc */

#define TEST 0xA5
vram_test () 	/* write vram, read back */	{
	int i, in;
	vram_out (0, TEST) ;
	for (i = 1 ; i < 16*1024; i++)
		outp (data_port, TEST) ;
	if ((in = vram_in (0)) != TEST)
		vram_err (0, in, TEST);
	for (i = 1 ; i < 16*1024 ; i++)
		if ((in = inp (data_port)) != TEST)
			vram_err (i, in, TEST) ;
} /* vram_test */
#undef TEST

vram_err (l, r, w)	{
	printf ("vram err-loc %04x: wrote %02x, read %02x\n",
		l, w, r) ;
} /* vram_err */
#undef stat_port
#undef reg_port
#undef add_port
#undef data_port
#undef STICK
#undef ena_port
#undef PAT_GEN
#undef PAT_NAM
#undef SPR_ATT
#undef PAT_COL
#undef SPR_PAT
#undef WH
#undef BL

/* TRS-80 screen cursor control, etc */
clear ()	{	/* clear screen, home cursor */
	bios (4, 0x1b) ; bios (4, '*') ;	return;
} /* clear */

cursor (l, c) int l, c ;	{
/* position cursor to line l, col c */
	bios (4, 0x1b) ;   bios (4, '=') ;	
	bios (4, 0x20+l) ; bios (4, 0x20+c) ;
} /* cursor */

curpr (l, c, s)	int l, c; char *s ; {
/* position cursor & print string */
	cursor (l,c) ; puts (s) ; return c+strlen(s) ;
} /* curpr */

or (i = 1 ; i < 16*1024; i++)
		outp (data_por