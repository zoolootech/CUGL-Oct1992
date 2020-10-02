/* Tabs.C extra mince commands to set and clear tab stops at irregular
   intervals. Contributed to AUG by Mark of the Unicorn 07/20/81 */

/* BINDINGS.C - Set up the key bindings for Mince

The seller of this software hereby disclaims any and all
guarantees and warranties, both express and implied.  No
liability of any form shall be assumed by the seller, nor shall
direct, consequential, or other damages be assumed by the seller.
Any user of this software uses it at his or her own risk.

Due to the ill-defined nature of "fitness for purpose" or similar
types of guarantees for this type of product, no fitness for any
purpose whatsoever is claimed or implied.

The physical medium upon which the software is supplied is
guaranteed for one year against any physical defect.  If it
should fail, return it to the seller, and a new physical medium
with a copy of the purchased software shall be sent.

The seller reserve the right to make changes, additions, and
improvements to the software at any time; no guarantee is made
that future versions of the software will be compatible with any
other version.

The parts of this disclaimer are severable and fault found in any
one part does not invalidate any other parts.

	Copyright (c) 1981 by Mark of the Unicorn
	Created for Version 2.3  10/4/80  JTL
	Updated to version three  1/7/81  JTL

	This file contains the function table initializers and mode set up
functions */

#include "mince.gbl"

finit1()			/* initialize the control and self insert key bindings */
{
	int cnt;
	int MInsert(), MSetMrk(), MBegLin(), MPrevChar();
	int MNotImpl(), MDelChar(), MEndLin(), MNextChar();
	int MAbort(), MDelLin(), NewDsp(), MNewLin();
	int MNextLin(), MOpenLin(), MPrevLin(), MQuote();
	int MRSearch(), MSearch(), MSwapChar(), MArg();
	int MNextPage(), MDelRgn(), MCtrlX(), MYank();
	int MMeta(), MDelIndent(), MRDelChar(), MIndent();
/*	int MTab(); */

	for (cnt=0; cnt<128; ++cnt) functs[cnt] = &MInsert;
	TKbChk();
	functs[0] = &MSetMrk;		/* C-@ */
	functs[1] = &MBegLin;		/* C-A */
	functs[2] = &MPrevChar;		/* C-B */
	functs[3] = &MNotImpl;		/* C-C */
	functs[4] = &MDelChar;		/* C-D */
	functs[5] = &MEndLin;		/* C-E */
	functs[6] = &MNextChar;		/* C-F */
	functs[7] = &MAbort;		/* C-G */
	functs[8] = &MRDelChar;		/* C-H */
/*	functs[9] = &MTab;			/* C-I */ */
	functs[10] = &MIndent;		/* C-J */
	functs[11] = &MDelLin;		/* C-K */
	functs[12] = &NewDsp;		/* C-L */
	functs[13] = &MNewLin;		/* C-M */
	functs[14] = &MNextLin;		/* C-N */
	functs[15] = &MOpenLin;		/* C-O */
	functs[16] = &MPrevLin;		/* C-P */
	functs[17] = &MQuote;		/* C-Q */
	functs[18] = &MRSearch;		/* C-R */
	functs[19] = &MSearch;		/* C-S */
	functs[20] = &MSwapChar;		/* C-T */
	functs[21] = &MArg;			/* C-U */
	functs[22] = &MNextPage;		/* C-V */
	functs[23] = &MDelRgn;		/* C-W */
	functs[24] = &MCtrlX;		/* C-X */
	functs[25] = &MYank;		/* C-Y */
	functs[26] = &MNotImpl;		/* C-Z */
	functs[ESC] = &MMeta;		/* ESC */
	functs[28] = &MDelIndent;	/* C-\ */
	functs[29] = &MNotImpl;		/* C-] */
	functs[30] = &MNotImpl;		/* C-^ */
	functs[31] = &MNotImpl;		/* C-_ */
	functs[DEL] = &MRDelChar;	/* DEL */
	}

finit2()			/* set up the Meta key bindings */
{
	int cnt;
	int MNotImpl(), MAbort(), MDelELin(), MQryRplc();
	int MMakeDel(), MToStart(), MToEnd(), MBSent();
	int BWord(), MCapWord(), MDelWord(), MFSent();
	int FWord(), MMrkPara(), MDelSent(), MLowWord();
	int MFillPara(), MReplace(), MCntrLine(), MUpWord();
	int MPrevPage(), MCopyRgn(), MBPara(), MDelWhite();
	int MFPara(), MRDelWord(), MSetMrk(), MSwapWord();
/**/ int MScrnSiz();

	for (cnt=128; cnt<256; ++cnt) functs[cnt] = &MNotImpl;
	TKbChk();
	functs[128+7] = &MAbort;					/* M-C-G */
	functs[128+8] = &MRDelWord;				/* M-C-H */
	functs[128+11] = &MDelELin;				/* M-C-K */
	functs[128+18] = &MQryRplc;				/* M-C-R */
/**/ functs[128+19] = &MScrnSiz;				/* M-C-S */
	functs[128+23] = &MMakeDel;				/* M-C-W */
	functs[128+'<'] = &MToStart;				/* M-< */
	functs[128+'>'] = &MToEnd;				/* M-> */
	functs[128+' '] = &MSetMrk;				/* M-  */
	functs[128+'A']=functs[128+'a'] = &MBSent;	/* M-A */
	functs[128+'B']=functs[128+'b'] = &BWord;	/* M-B */
	functs[128+'C']=functs[128+'c'] = &MCapWord;	/* M-C */
	functs[128+'D']=functs[128+'d'] = &MDelWord;	/* M-D */
	functs[128+'E']=functs[128+'e'] = &MFSent;	/* M-E */
	functs[128+'F']=functs[128+'f'] = &FWord;	/* M-F */
	functs[128+'H']=functs[128+'h'] = &MMrkPara;	/* M-H */
	functs[128+'K']=functs[128+'k'] = &MDelSent;	/* M-K */
	functs[128+'L']=functs[128+'l'] = &MLowWord;	/* M-L */
	functs[128+'Q']=functs[128+'q'] = &MFillPara;/* M-Q */
	functs[128+'R']=functs[128+'r'] = &MReplace;	/* M-R */
	functs[128+'S']=functs[128+'s'] = &MCntrLine;/* M-S */
	functs[128+'T']=functs[128+'t'] = &MSwapWord;/* M-T */
	functs[128+'U']=functs[128+'u'] = &MUpWord;	/* M-U */
	functs[128+'V']=functs[128+'v'] = &MPrevPage;/* M-V */
	functs[128+'W']=functs[128+'w'] = &MCopyRgn;	/* M-W */
	functs[128+'['] = &MBPara;				/* M-[ */
	functs[128+'\\'] = &MDelWhite;			/* M-\ 