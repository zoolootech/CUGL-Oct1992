@style(rightmargin 1270 micas, leftmargin 1270 micas, bottommargin 1270 micas)
@pageheading(left "Amethyst Users Group; P.O. Box 8173; Austin, Texas 78712",
right "08/19/81")
@pagefooting(right "Page @value[page]", left "Please Answer All Questions")

This is the membership information form for the Amethyst Users Group.
This form serves two purposes, first, it gives the Amethyst Users
Group editor(s) a more clear understanding of who you are and what
your needs are, and also serves to verify legitimate ownership of
Amethysts, Minces and Scribbles. This latter is necessary because of
the limitations of the agreement between the Amethyst Users Group and
Mark of the Unicorn. Please answer all of the questions as fully as
possible. Also attach a photocopy of the license agreement that you
completed for Mark of the Unicorn, or a photocopy of the label(s) of
your original distribution disks, indicating your valid ownership. (Be
careful when photocopying diskettes not to destroy them, and certainly
back them up first.) This information will not be distributed outside
of the Amethyst Users Group.

@begin(verbatim)
Name
------------------------------------------------------------------
Address
------------------------------------------------------------------
City				State		Zip
------------------------------------------------------------------
Telephone AC(    ) 
------------------------------------------------------------------
I Own (only one should apply):
Complete Amethyst ( )			Mince + Source only ( )
Mince + Scribble (no source) ( )	Scribble + Source only ( )
Mince + Scribble (With Source, but BDS C not included) ( )
Version Numbers:
BDS C:		Amethyst:	Mince:		Scribble:
------------------------------------------------------------------
Please Describe the hardware on which you run your Amethyst. (CPU
type, bus structure, manufacturer, CRT terminal type, or Memory Mapped
Video board type, mass storage type and density, number of users &c.)

------------------------------------------------------------------

------------------------------------------------------------------

------------------------------------------------------------------

------------------------------------------------------------------
Please Describe the operating system(s) and support software you tend
to use with your Amethyst: (OASIS, RSX-11M, CP/M, Textwriter III, roff)

------------------------------------------------------------------

------------------------------------------------------------------
Please describe as unambiguously as possible your preferred medium of
software exchange, and one alternate if possible. (e.g. RT-11 Single
density floppy, CP/M 3740 Single density, CDOS 5.25" Single Density):

1
------------------------------------------------------------------
2
------------------------------------------------------------------
Please describe your skill as a potential Amethyst extender, or C
programmer. (e.g. absolute novice, UNIX wizard, 4004 assembler only):

------------------------------------------------------------------

------------------------------------------------------------------
Would you like to help review submissions to the Amethyst Users Group?

------------------------------------------------------------------
Would you object to having any of the information here published in 
the Amethyst Users Group newsletter so that other members may contact
you directly? YES [ ]  NO [ ]  If yes, be specific:(i.e. address only)

------------------------------------------------------------------
What do you expect to get out of the Amethyst Users Group?

------------------------------------------------------------------

------------------------------------------------------------------

------------------------------------------------------------------

------------------------------------------------------------------
How, if at all, do you expect to contribute to Amethyst Users Group?

------------------------------------------------------------------

------------------------------------------------------------------

------------------------------------------------------------------

------------------------------------------------------------------
Other comments (attach as many pages as necessary)

------------------------------------------------------------------

------------------------------------------------------------------

------------------------------------------------------------------

------------------------------------------------------------------@end(verbatim) expect to get out of the Amethyst Users Group?

--------------------lace;	/* M-R */
	functs[128+'S']=functs[128+'s'] = &MCntrLine;/* M-S */
	functs[128+'T']=functs[128+'t'] = &MSwapWord;/* M-T */
	functs[128+'U']=functs[128+'u'] = &MUpWord;	/* M-U */
	functs[128+'V']=functs[128+'v'] = &MPrevPage;/* M-V */
	functs[128+'W']=functs[128+'w'] = &MCopyRgn;	/* M-W */
	functs[128+'['] = &MBPara;				/* M-[ */
	functs[128+'\\'] = &MDelWhite;			/* M-\ /* VIEW.C -- View Mode
   Contributed to the AUG by Mark of the Unicorn 07/20/81 

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
should fail, return it to Mark of the Unicorn, and a new physical
medium with a copy of the purchased software shall be sent.

The seller reserves the right to make changes, additions, and
improvements to the software at any time; no guarantee is made
that future versions of the software will be compatible with any
other version.

The parts of this disclaimer are severable and fault found in
any one part does not invalidate any other parts.

	Copyright (c) 1981 by Mark of the Unicorn
	Created for Version 2.3 10/4/80 JTL

	This file contains the mode set up function */


SetModes()			/* Set the modes according to the bmodes array */
{
	int cnt;
	int MInsert(), MNotImpl();
	
	*mode='\0';
	finit1();					/* Set up the key bindings */
	finit2();
	finit3();
	for (cnt=MAXMODES-1; cnt>=0; --cnt) {
		switch (buffs[cbuff].bmodes[cnt]) {

		case 'v':
			Rebind(&MInsert,&MNotImpl);
			functs[4] = &MNotImpl;			/* C-D */
			functs[9] = &M