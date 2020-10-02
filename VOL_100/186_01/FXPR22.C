/*
04/08/83
FXPR2.C (ver 2) EPSON FX-80 printer utility by Thomas E. McCormick derived
from PRINTER.C for the Espon MX printers by Gary P. Novosielski (c) 1982.

Requires only BDSCIO.H in addition to this FXPR.C source to compile.

************************************************************************
05/09/85 Two small changes have been made to enable compilation under
	 BDS C ver. 1.50a.		R. W. Odlin
************************************************************************

FXPR2.DOC documentation file explains the options more thoroughly.

DEN1  DEN2  DEN4  MX-80/100 features from PRINTER.C have been removed.

There are more than 50 keyword options, and God knows how many combinations.
They have NOT all been tested !!  Be aware that the FX-80 treats various
combinations differently than "you thought".  The features have priorities
with regard to others, and some negate others.	The 150+ page manual has
several tables that will help, but you will have to experiment a little too.

It is a good idea to use RESET as your first keyword to "baseline" the
subsequent commands.  If you don't, you will be asking "why it behaves
differently even though the same command is entered". For example, some
of the features are "toggles"; the condition will remain set until unset 
with the same command. Other features require a specific reset command.
The "RESET" command mentioned above will reset ALL modes to the power-on
configuration you have chosen via the DIP switches at installation.

Look for FXPARMS.BAS MBASIC source file.  It contains many of the examples
from the FX-80 Operation Manual, and allows you to fiddle with them and
print them immediately to fine tune.

Fifteen unduplicated print modes/pitches have been added with the
ESCAPE;"!";CHR$(n) commands where n = 0 to 63.	These are the
P5, P5B, P10VB, etc commands setting pitch and intensity at once.

All rights reserved.
Permission is hereby granted for unrestricted non-commercial
use.  Any use for commercial advantage without prior written
consent of the author (G.P.N.) is prohibited.

This public domain program compiles under BDS C ver 1.44 or 1.5.
As of March, 1983, you should use 1.44 if you want to apply NOBOOT2
to the FXPR2.COM file since NOBOOT2 does NOT work if compiled with
BDS C  ver 1.5.

To Recompile with 1.44 and apply NOBOOT2:

	A>CC1 FXPR2.C

	A>CLINK FXPR2

	A>NOBOOT2 FXPR2.COM


*/

#include <bdscio.h>	/* brackets instead of quotes, so that 1.50a looks for
			   header file on the default drive */

/*	Return code definitions   */
#define RESET	1
#define PITCH	2
#define LPI	3
#define DRAFT	4
#define TEXT	5
#define EJECT	6
#define TINY	7
#define TABS	8
#define SKIP	9
#define NOSKIP	10
#define WIDTH	11
#define ITALIC	12
#define ROMAN	13
#define PROPOR	14
#define LTRITA	15
#define LTRITB	16
#define LTRELI	17
#define EMPHAS	18
#define ELITE	19
#define PICA	20
#define QUIET	21
#define ENLARG	22
#define CONDEN	23
#define P5	24
#define P5B	25
#define P5VB	26
#define P6	27
#define P6B	28
#define P6VB	29
#define P9B	30
#define P9VB	31
#define P10	32
#define P10B	33
#define P10VB	34
#define P12	35
#define P12B	36
#define P17	37
#define P17B	38
#define LPI10	39
#define TINYER	40
#define LEFTM	41
#define RIGHTM	42
#define LPI3	43
#define LPI6	44
#define LPI8	45
#define LPI12	46
#define PLINCH	47
#define DOUBLE	48
#define XDOUBL	49
#define XEMPHA	50
#define LPI4	51
#define LPI5	52
#define XENLAR	53
#define XCONDE	54

#define LST	5	/* Bdos function for list output */

#define ESC	27	/* This equate needed to compile under BDS ver. 1.50a,
			   as the def. has been moved in this version from 
			   BDSCIO.H to HARDWARE.H */

main(argc, argv)
int  argc;
char **argv;
{
    int  value, oldvalue;
    char xlate();

    if (argc < 2)
    {
	puts("FXPR2 (ver 2).......by Tom McCormick  04/08/83\n");
	puts("      from PRINTER.C (c) 1982 G.P. Novosielski\n");
	puts("FXPR2 <opt> [<opt>.....]   where <opt> may be:\n");
	puts("                                              \n");
	puts("RESET      QUIET       PITCH 10    PITCH 17   \n");
	puts("LPI3       LPI4        LPI5        LPI6       \n");
	puts("LPI8       LPI10       LPI12       PLINCH 1-22\n");
	puts("WIDTH 80 [... or any value between 2 and 137 ]\n");
	puts("SKIP       NOSKIP      EJECT       TABS n n n \n");
	puts("PROPOR     DRAFT       TEXT                   \n");
	puts("PICA       ELITE       ITALIC      LEFTM  n   \n");
	puts("TINY       TINYER      ROMAN       RIGHTM n   \n");
	puts("LTRITA     LTRITB      LTRELI                 \n");
	puts("EMPHAS     CONDEN      ENLARG      DOUBLE     \n");
	puts("XEMPHA     XCONDE      XENLAR      XDOUBL     \n");
	puts("Various pitches (CPI) and bold or very bold...\n");
	puts(" P5     P6            P10     P12     P17     \n");
	puts(" P5B    P6B    P9B    P10B    P12B    P17B    \n");
	puts(" P5VB   P6VB   P9VB   P10VB                   \n");
	puts("                                              \n");
	puts("\n  Example: FXPR2 RESET LPI6 PITCH 10 ELITE  \n");
	exit();
    }

    while (--argc > 0)
    {
	switch (xlate(*++argv))
	{
	case RESET:
	    bdos(LST,ESC);
	    bdos(LST,'@');
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,0);
	    bdos(LST,11);
	    break;
	case PITCH:
	    value = atoi((--argc,*++argv));
	    if (value == 10)
		bdos(LST,18);
	    else if (value == 17)
		bdos(LST,15);
	    else
		abort ("%s--Invalid pitch.  Not 10 or 17.",*argv);
	    break;
	case LPI:
	    if (!(value = atoi((--argc,*++argv))) || value > 216)
		abort ("%s--Invalid Lines per Inch\nMust be 1 - 216.",*argv);
	    bdos(LST,ESC);
	    bdos(LST,'3');
	    bdos(LST,216/value);
	    break;
	case DRAFT:
	    bdos(LST,ESC);
	    bdos(LST,'H');
	    bdos(LST,ESC);
	    bdos(LST,'T');
	    break;
	case TEXT:
	    bdos(LST,ESC);
	    bdos(LST,'G');
	    bdos(LST,ESC);
	    bdos(LST,'U');
	    bdos(LST,1);
	    break;
	case EJECT:
	    bdos(LST,12);
	    break;
	case TINY:
	    bdos(LST,ESC);
	    bdos(LST,'S');
	    bdos(LST,0);
	    bdos(LST,ESC);
	    bdos(LST,'1');
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,110);
	    bdos(LST,ESC);
	    bdos(LST,'N');
	    bdos(LST,20);
	    break;
	case TABS:
	    oldvalue = 0;
	    bdos(LST,ESC);
	    bdos(LST,'D');
	    while(--argc)
	    {
		value = atoi(*++argv);
		if(value && value <= oldvalue)
		{
		    bdos(LST,0);
		    abort("Tabs not in increasing order");
		}
		if (!value)
		    --argv, ++argc;
		else
		    bdos(LST,(oldvalue = value));
	    }
	    bdos(LST,0);
	    break;
	case SKIP:
	    bdos(LST,ESC);
	    bdos(LST,'N');
	    bdos(LST,6);
	    break;
	case NOSKIP:
	    bdos(LST,ESC);
	    bdos(LST,'N');
	    bdos(LST,0);
	    break;
	case WIDTH:
	    if (!(--argc))
		break;
	    if (!(value = atoi(*++argv)))
		abort("%s--Invalid width.",*argv);
	    bdos(LST,ESC);
	    bdos(LST,'Q');
	    bdos(LST,value);
	    break;
	case ITALIC:
	    bdos(LST,ESC);
	    bdos(LST,'4');
	    break;
	case ROMAN:
	    bdos(LST,ESC);
	    bdos(LST,'5');
	    break;
	 case PROPOR:
	    bdos(LST,ESC);
	    bdos(LST,'p');
	    bdos(LST,1);
	    break;
	case LTRITA:
	    bdos(LST,ESC);
	    bdos(LST,'@');
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,ESC);
	    bdos(LST,0);
	    bdos(LST,11);

	    bdos(LST,ESC);
	    bdos(LST,'4');

	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,3);

	    bdos(LST,ESC);
	    bdos(LST,'Q');
	    bdos(LST,132);
	    break;

	case LTRITB:

	    bdos(LST,ESC);
	    bdos(LST,'@');
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,ESC);
	    bdos(LST,0);
	    bdos(LST,11);

	    bdos(LST,ESC);
	    bdos(LST,'4');

	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,17);

	    bdos(LST,ESC);
	    bdos(LST,'Q');
	    bdos(LST,132);
	    break;

	case LTRELI:
	    bdos(LST,ESC);
	    bdos(LST,'@');
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,ESC);
	    bdos(LST,0);
	    bdos(LST,11);

	    bdos(LST,ESC);
	    bdos(LST,'5');
	    bdos(LST,'M');

	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,3);

	    bdos(LST,ESC);
	    bdos(LST,'Q');
	    bdos(LST,132);
	    break;

	 case EMPHAS:
	    bdos(LST,ESC);
	    bdos(LST,'E');
	    break;
	case ELITE:
	    bdos(LST,ESC);
	    bdos(LST,'M');
	    break;
	case PICA:
	    bdos(LST,ESC);
	    bdos(LST,'P');
	    break;
	case QUIET:
	    bdos(LST,ESC);
	    bdos(LST,'s');
	    bdos(LST,1);
	    break;
	case ENLARG:
	    bdos(LST,14);
	    break;
	case CONDEN:
	    bdos(LST,15);
	    break;
	case P5:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,32);
	    break;
       case P5B:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,40);
	    break;
       case P5VB:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,62);
	    break;
       case P6:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,35);
	    break;
       case P6B:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,47);
	    break;
       case P6VB:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,63);
	    break;
       case P9B:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,36);
	    break;
       case P9VB:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,52);
	    break;
	case P10:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,2);
	    break;
       case P10B:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,8);
	    break;
       case P10VB:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,24);
	    break;
       case P12:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,3);
	    break;
       case P12B:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,17);
	    break;
       case P17:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,4);
	    break;
       case P17B:
	    bdos(LST,ESC);
	    bdos(LST,'!');
	    bdos(LST,22);
	    break;
	case LPI10:
	    bdos(LST,ESC);
	    bdos(LST,'1');
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,110);
	    bdos(LST,ESC);
	    bdos(LST,'N');
	    bdos(LST,20);
	    break;
	case TINYER:
	    bdos(LST,ESC);
	    bdos(LST,'S');
	    bdos(LST,0);
 
	    bdos(LST,ESC);
	    bdos(LST,'3');
	    bdos(LST,18);

	    bdos(LST,ESC);
	    bdos(LST,'N');
	    bdos(LST,26);
	    break;
	case LEFTM:
	    if (!(--argc))
		break;
	    if (!(value = atoi(*++argv)))
		abort("%s--Invalid width.",*argv);
	    bdos(LST,ESC);
	    bdos(LST,'l');
	    bdos(LST,value);
	    break;
	case RIGHTM:
	    if (!(--argc))
		break;
	    if (!(value = atoi(*++argv)))
		abort("%s--Invalid width.",*argv);
	    bdos(LST,ESC);
	    bdos(LST,'Q');
	    bdos(LST,value);
	    break;

	case LPI3:
	    bdos(LST,ESC);
	    bdos(LST,'3');
	    bdos(LST,72);
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,33);
	    bdos(LST,ESC);
	    bdos(LST,'N');
	    bdos(LST,6);

	case LPI6:
	    bdos(LST,ESC);
	    bdos(LST,'2');
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,66);
	    bdos(LST,ESC);
	    bdos(LST,'N');
	    bdos(LST,12);
	    break;

	case LPI8:
	    bdos(LST,ESC);
	    bdos(LST,'0');
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,88);
	    bdos(LST,ESC);
	    bdos(LST,'N');
	    bdos(LST,16);
	    break;
	case LPI12:
	    bdos(LST,ESC);
	    bdos(LST,'3');
	    bdos(LST,18);
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,126);
	    bdos(LST,ESC);
	    bdos(LST,'N');
	    bdos(LST,18);
	    break;
	case PLINCH:
	    if (!(value = atoi((--argc,*++argv))) || value > 22)
		abort ("%s--Page Length Inches\nMust be 1 - 22.",*argv);
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,'0');
	    bdos(LST,value);
	    break;
	case DOUBLE:
	    bdos(LST,ESC);
	    bdos(LST,'G');
	    break;
	case XDOUBL:
	    bdos(LST,ESC);
	    bdos(LST,'H');
	    break;
	case XEMPHA:
	    bdos(LST,ESC);
	    bdos(LST,'F');
	    break;
	case LPI4:
	    bdos(LST,ESC);
	    bdos(LST,'3');
	    bdos(LST,54);
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,44);
	    bdos(LST,ESC);
	    bdos(LST,'N');
	    bdos(LST,8);
	case LPI5:
	    bdos(LST,ESC);
	    bdos(LST,'3');
	    bdos(LST,43);
	    bdos(LST,ESC);
	    bdos(LST,'C');
	    bdos(LST,55);
	    bdos(LST,ESC);
	    bdos(LST,'N');
	    bdos(LST,10);
	    break;
	case XENLAR:
	    bdos(LST,20);
	    break;
	case XCONDE:
	    bdos(LST,18);
	    break;

       default:
	    abort("%s--Unknown keyword.",*argv);
	}
    }  
}

char xlate(string)
  char *string;
{
    if (!strcmp(string,"RESET"))
	return RESET;
    else if (!strcmp(string,"PITCH"))
	return PITCH;
    else if (!strcmp(string,"LPI"))
	return LPI;
    else if (!strcmp(string,"DRAFT"))
	return DRAFT;
    else if (!strcmp(string,"TEXT"))
	return TEXT;
    else if (!strcmp(string,"EJECT"))
	return EJECT;
    else if (!strcmp(string,"TINY"))
	return TINY;
    else if (!strcmp(string,"TABS"))
	return TABS;
    else if (!strcmp(string,"TAB"))
	return TABS;
    else if (!strcmp(string,"SKIP"))
	return SKIP;
    else if (!strcmp(string,"NOSKIP"))
	return NOSKIP;
    else if (!strcmp(string,"WIDTH"))
	return WIDTH;
    else if (!strcmp(string,"ITALIC"))
	return ITALIC;
    else if (!strcmp(string,"ROMAN"))
	return ROMAN;
    else if (!strcmp(string,"PROPOR"))
	return PROPOR;
    else if (!strcmp(string,"LTRITA"))
	return LTRITA;
    else if (!strcmp(string,"LTRITB"))
	return LTRITB;
    else if (!strcmp(string,"LTRELI"))
	return LTRELI;
    else if (!strcmp(string,"ENLARG"))
	return ENLARG;
    else if (!strcmp(string,"ELITE"))
	return ELITE;
    else if (!strcmp(string,"PICA"))
	return PICA;
    else if (!strcmp(string,"QUIET"))
	return QUIET;
    else if (!strcmp(string,"EMPHAS"))
	return EMPHAS;
    else if (!strcmp(string,"CONDEN"))
	return CONDEN;
    else if (!strcmp(string,"P5"))
	return P5;
    else if (!strcmp(string,"P5B"))
	return P5B;
    else if (!strcmp(string,"P5VB"))
	return P5VB;
    else if (!strcmp(string,"P6"))
	return P6;
    else if (!strcmp(string,"P6B"))
	return P6B;
    else if (!strcmp(string,"P6VB"))
	return P6VB;
    else if (!strcmp(string,"P9B"))
	return P9B;
    else if (!strcmp(string,"P9VB"))
	return P9VB;
    else if (!strcmp(string,"P10"))
	return P10;
    else if (!strcmp(string,"P10B"))
	return P10B;
    else if (!strcmp(string,"P10VB"))
	return P10VB;
    else if (!strcmp(string,"P12"))
	return P12;
    else if (!strcmp(string,"P12B"))
	return P12B;
    else if (!strcmp(string,"P17"))
	return P17;
    else if (!strcmp(string,"P17B"))
	return P17B;
    else if (!strcmp(string,"LPI10"))
	return LPI10;
    else if (!strcmp(string,"TINYER"))
	return TINYER;
    else if (!strcmp(string,"LEFTM"))
	return LEFTM;
    else if (!strcmp(string,"RIGHTM"))
	return RIGHTM;
    else if (!strcmp(string,"LPI3"))
	return LPI3;
    else if (!strcmp(string,"LPI6"))
	return LPI6;
    else if (!strcmp(string,"LPI8"))
	return LPI8;
    else if (!strcmp(string,"LPI12"))
	return LPI12;
    else if (!strcmp(string,"PLINCH"))
	return PLINCH;
    else if (!strcmp(string,"DOUBLE"))
	return DOUBLE;
    else if (!strcmp(string,"XDOUBL"))
	return XDOUBL;
    else if (!strcmp(string,"XEMPHA"))
	return XEMPHA;
    else if (!strcmp(string,"LPI4"))
	return LPI4;
    else if (!strcmp(string,"LPI5"))
	return LPI5;
    else if (!strcmp(string,"XENLAR"))
	return XENLAR;
    else if (!strcmp(string,"XCONDE"))
	return XCONDE;

    else
	return ERROR;
}

abort(p1,p2,p3,p4,p5,p6,p7,p8)
unsigned p1,p2,p3,p4,p5,p6,p7,p8;
{					     
    printf(p1,p2,p3,p4,p5,p6,p7,p8);
    unlink("A:$$$.SUB");
    puts("...ABORTED");
    exit();
}
