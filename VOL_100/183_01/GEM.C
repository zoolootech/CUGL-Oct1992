/* #include <stdio.h>  ... included here */
/* stdio.h */

/* a 'FILE' is simply an integer is this implementation */
typedef int  FILE;

#define stdin	0
#define stdout	1
#define stderr	2

#define NULL	0
#define TRUE	1
#define FALSE	0
#define EOF	(-1)
#define ERR	(-1)
#define ERROR	(-1)

/* Control Key Translations */
#define UP_CHAR 30
#define DOWN_CHAR 31
#define LEFT_CHAR 29
#define RIGHT_CHAR 28
#define HOME_CHAR 200
#define END_CHAR 201
#define PAGEUP_CHAR 202
#define PAGEDOWN_CHAR 203
#define CHOME_CHAR 204	    /* ^HOME */
#define CEND_CHAR 205	    /* ^END  */
#define INS_CHAR 206
#define DEL_CHAR 207
#define CLEFT_CHAR 208	    /* ^LEFT  */
#define CRIGHT_CHAR 209     /* ^RIGHT */

/* Function Keys */
#define M1 210
#define M2 211
#define M3 212
#define M4 213
#define M5 214
#define M6 215
#define M7 216
#define M8 217
#define M9 218
#define M10 219

/* Screen Attributes */
#define R_VIDEO 0x70
#define BLINK	0x80
#define LIGHT	0x08
#define NRML	0x07
#define F_BLK	0x00
#define F_BLU	0x01
#define F_GRN	0x02
#define F_CYA	0x03
#define F_RED	0x04
#define F_MAG	0x05
#define F_BRN	0x06
#define B_BLK	0x00
#define B_BLU	0x10
#define B_GRN	0x20
#define B_CYA	0x30
#define B_RED	0x40
#define B_MAG	0x50
#define B_BRN	0x60

/* MS-DOS 'INT 21H' Functions */
#define AUXI_FUNC  3	     /* Auxiliary Input 	*/
#define AUXO_FUNC  4	     /* Auxiliary Output	*/
#define PRTR_FUNC  5	     /* Printer Output		*/
#define SCH1_FUNC 17	     /* Search First		*/
#define SCHN_FUNC 18	     /* Search Next		*/
#define CURD_FUNC 24	     /* Current Disk		*/
#define ATBL_FUNC 26	     /* Allocation Tbl. Addr.	*/
#define SVEC_FUNC 37	     /* Set Vector		*/
#define GDAT_FUNC 42	     /* Get Date		*/
#define SDAT_FUNC 43	     /* Set Date		*/
#define GTIM_FUNC 44	     /* Get Time		*/
#define STIM_FUNC 45	     /* Set Time		*/

/* End of stdio.h */

#define BANNER_1  "Printer ATTRibutes"
#define BANNER_1A " gemini-10X "
#define BANNER_1B "            "
#define AUTHOR	  "by Robert J. Molloy"

/* right arrow character */
#define OPTION_ON   "\020"
/* space */
#define OPTION_OFF  "\040"

/* Note: '\377' (0xFF) converted to 0x00 in print
	 routine. */
#define STR_INIT	"\021\033@"
#define STR_10CPI	"\033B\001"
#define STR_12CPI	"\033B\002"
#define STR_17CPI	"\033B\003"
#define STR_DBLW_ON	"\033W\001"
#define STR_CMP_ON	"\017"
#define STR_SUP_ON	"\033S\377"
#define STR_SUB_ON	"\033S\001"
#define STR_ITA_ON	"\033\064"
#define STR_DBLS_ON	"\033G"
#define STR_EMP_ON	"\033E"
#define STR_LF16	"\033\062"
#define STR_LF18	"\033\060"
#define STR_LF772	"\033\061"

main()
{
char c;
unsigned opt_cpi10   =	 FALSE;
unsigned opt_cpi12   =	 FALSE;
unsigned opt_cpi17   =	 FALSE;
unsigned opt_dblw    =	 FALSE;
unsigned opt_cmp     =	 FALSE;
unsigned opt_sup     =	 FALSE;
unsigned opt_sub     =	 FALSE;
unsigned opt_ita     =	 FALSE;
unsigned opt_dbls    =	 FALSE;
unsigned opt_emp     =	 FALSE;
unsigned opt_lf16    =	 FALSE;
unsigned opt_lf18    =	 FALSE;
unsigned opt_lf772   =	 FALSE;

    scr_clr();
    scr_setmode(1);	/* 40X25 Color */
    scr_cursoff();

    scr_puts(1,(40-strlen(BANNER_1))/2,(LIGHT|NRML),BANNER_1);
    scr_puts(6,(40-strlen(AUTHOR))/2,(LIGHT|NRML),AUTHOR);
    scr_puts(2,(40-strlen(BANNER_1B))/2,(NRML|B_RED|LIGHT),BANNER_1B);
    scr_puts(3,(40-strlen(BANNER_1A))/2,(NRML|B_RED|LIGHT),BANNER_1A);
    scr_puts(4,(40-strlen(BANNER_1B))/2,(NRML|B_RED|LIGHT),BANNER_1B);
    scr_puts(8,2,NRML,"[A] 10 CPI");
    scr_puts(10,2,NRML,"[B] 12 CPI");
    scr_puts(12,2,NRML,"[C] 17 CPI");
    scr_puts(14,2,NRML,"[D] Double-wide");
    scr_puts(16,2,NRML,"[E] Compress");
    scr_puts(18,2,NRML,"[F] Superscript");
    scr_puts(20,2,NRML,"[G] Subscript");
    scr_puts(8,22,NRML,"[H] Italics");
    scr_puts(10,22,NRML,"[I] Double Strike");
    scr_puts(12,22,NRML,"[J] Emphasize");
    scr_puts(14,22,NRML,"[K] LF=1/6\"");
    scr_puts(16,22,NRML,"[L] LF=1/8\"");
    scr_puts(18,22,NRML,"[M] LF=7/72\"");

    opt_query();
    while ((c=toupper(ci())) != 'X')
	switch (c)  {
	case 'A':
	    if (opt_cpi10=~(opt_cpi10))
		opt_onturn(8,1);
	    else
		opt_offturn(8,1);
	    opt_cpi12=FALSE;
	    opt_offturn(10,1);
	    opt_cpi17=FALSE;
	    opt_offturn(12,1);
	    opt_query();
	    break;
	case 'B':
	    if (opt_cpi12=~(opt_cpi12))
		opt_onturn(10,1);
	    else
		opt_offturn(10,1);
	    opt_cpi10=FALSE;
	    opt_offturn(8,1);
	    opt_cpi17=FALSE;
	    opt_offturn(12,1);
	    opt_query();
	    break;
	case 'C':
	    if (opt_cpi17=~(opt_cpi17))
		opt_onturn(12,1);
	    else
		opt_offturn(12,1);
	    opt_cpi12=FALSE;
	    opt_offturn(10,1);
	    opt_cpi10=FALSE;
	    opt_offturn(8,1);
	    opt_query();
	    break;
	case 'D':
	    if (opt_dblw=~(opt_dblw))
		opt_onturn(14,1);
	    else
		opt_offturn(14,1);
	    opt_query();
	    break;
	case 'E':
	    if (opt_cmp=~(opt_cmp))
		opt_onturn(16,1);
	    else
		opt_offturn(16,1);
	    opt_query();
	    break;
	case 'F':
	    if (opt_sup=~(opt_sup))
		opt_onturn(18,1);
	    else
		opt_offturn(18,1);
	    opt_sub=FALSE;
	    opt_offturn(20,1);
	    opt_query();
	    break;
	case 'G':
	    if (opt_sub=~(opt_sub))
		opt_onturn(20,1);
	    else
		opt_offturn(20,1);
	    opt_sup=FALSE;
	    opt_offturn(18,1);
	    opt_query();
	    break;
	case 'H':
	    if (opt_ita=~(opt_ita))
		opt_onturn(8,21);
	    else
		opt_offturn(8,21);
	    opt_query();
	    break;
	case 'I':
	    if (opt_dbls=~(opt_dbls))
		opt_onturn(10,21);
	    else
		opt_offturn(10,21);
	    opt_query();
	    break;
	case 'J':
	    if (opt_emp=~(opt_emp))
		opt_onturn(12,21);
	    else
		opt_offturn(12,21);
	    opt_query();
	    break;
	case 'K':
	    if (opt_lf16=~(opt_lf16))
		opt_onturn(14,21);
	    else
		opt_offturn(14,21);
	    opt_lf18=FALSE;
	    opt_offturn(16,21);
	    opt_lf772=FALSE;
	    opt_offturn(18,21);
	    opt_query();
	    break;
	case 'L':
	    if (opt_lf18=~(opt_lf18))
		opt_onturn(16,21);
	    else
		opt_offturn(16,21);
	    opt_lf16=FALSE;
	    opt_offturn(14,21);
	    opt_lf772=FALSE;
	    opt_offturn(18,21);
	    opt_query();
	    break;
	case 'M':
	    if (opt_lf772=~(opt_lf772))
		opt_onturn(18,21);
	    else
		opt_offturn(18,21);
	    opt_lf18=FALSE;
	    opt_offturn(16,21);
	    opt_lf16=FALSE;
	    opt_offturn(14,21);
	    opt_query();
	    break;
	default:
	    break;
	}
    lprintf(STR_INIT);
    if (opt_cpi12)
	lprintf(STR_12CPI);
    else if (opt_cpi17)
	lprintf(STR_17CPI);
    if (opt_dblw)
	lprintf(STR_DBLW_ON);
    if (opt_cmp)
	lprintf(STR_CMP_ON);
    if (opt_sup)
	lprintf(STR_SUP_ON);
    else if (opt_sub)
	lprintf(STR_SUB_ON);
    if (opt_ita)
	lprintf(STR_ITA_ON);
    if (opt_dbls)
	lprintf(STR_DBLS_ON);
    if (opt_emp)
	lprintf(STR_EMP_ON);
    if (opt_lf18)
	lprintf(STR_LF18);
    else if (opt_lf772)
	lprintf(STR_LF772);
    scr_setmode(3);	/* 80X25 Color */
}

opt_query()
{
    scr_puts(24,10,F_GRN,"  (X to Exit)");
    scr_puts(23,10,F_GRN,"Select Option: ");
    scr_curson();
}

opt_onturn(row,col)
int row, col;
{
    scr_puts(row,col,(LIGHT|BLINK|F_BLU),OPTION_ON);
}

opt_offturn(row,col)
int row, col;
{
    scr_puts(row,col,NRML,OPTION_OFF);
}

/*
	LPRINTF     formatted output to the list device.
		    Usage:  lprintf(format,arg1,arg2,...)
			    char *format;
*/
lprintf(format)
char *format;
{
    char txtlin[132];
    _spr(txtlin,&format);
    lputs(txtlin);
}

/*
	LPUTS	    put a line to the list device.
		    Usage:  lputs(str)
			    char *str;
*/
lputs(str)
char *str;
{
    char c;
    while (c = *str++)	{
	if (c == '\n') _os(PRTR_FUNC,'\r');
	if (c == '\377') _os(PRTR_FUNC,'\000');
	else _os(PRTR_FUNC,c);
    }
}

/*
	SCR_PUTS    write string with attribute at location.
		    Usage:  scr_puts(row,col,attr,p)
			    int row, col, attr;
			    char *p;
*/
scr_puts(row,col,attr,p)
int row, col, attr;
char *p;
{
    extern char scr_attr;
    char c, save;

    save = scr_attr;
    scr_attr = attr;
    while (c=*p++)  {
	scr_rowcol(row,col++);
	scr_putch(c);
    }
    scr_attr = save;
}
