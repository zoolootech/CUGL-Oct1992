@pageheading(left "Notes on AUG Disk # 1 by B. Dobyns")
This disk isn't nearly as full as I had intended it to be, but the
release is occuring now, so that everyone can begin to share the
benefits of the code contained herein. 

There should be little trouble getting most of these functions to work
in your Amethyst package, with the exception of Jeffrey Stone's and
Oscar Goldman's Scribble/Crayon hacks. These are strictly for
Scrobble/Crayon 1.0 only and will not work elsewhere.

For Mince extenders, most of the Mince source is very liberally
commented, and easy to use. You may have some trouble geting some of
these things going in your Mince, though, if you have not tried any
extenson yet. 

I have included SD for two reasons: One, they both work under
CP/M 2.2 (while many older extended directory listers don't). Two, I
am planning on turning one into an overlay for Mince (heh, heh).

Don't forget that if you #define LARGE in MINCE.GBL you can get two
more modes 'Save' and 'C'. Neither of these are really 'complete' yet,
but the code for them is scattered about the source files, disabled by
#ifdef LARGE ... and you can be tinkering with them now. 

Hereare some notes on how to go about extension:

If the extensions that you want on this disk are already
built into a Bindings.C file (recognizable by the presence of
finit1(), finit2(), finit3(), SetModes() and UInit() in the file) then
you merely need to pip the file over to a disk with the rest of the
Mince code on it, and edit MC.SUB or LMC.SUB to reflect the new name
of the bindings source file. Take out the old references to Bindings.C
and put in the new ones (e.g. Demo.C). Submit (L)MC and lean back. You
should get a new Mince shortly. 

Note that you can have lines in a submit file that start with a ';'
and these are taken by CP/M (all versions) to be comment lines, and
are printed on the terminal, but ignored. Comment lines in your submit
files can greatly ease the pain of figuring out what the little
nasties do weeks later (especially when you named it with a VERY
arcane name (QQX.SUB)). 

If you do not find that the code that you want is already in a
Bindings file, you must do some work. Take your current Bindings.C and
edit it thusly: Take the functions you want and place them at the end
of the file, just before the /* END OF mumble.C */. Place a
modification notice at the head of the file. If this loses you, look
at what I did (in MACBIND.C) for an example of one way to go about
this.  Decide which command names (the letters you type on the
keyboard) you want to go with which command. Look in the file for the
bindings similar to it (all the C- commands are in finit1, all the M-
in finit2, all the C-X in finit3). Near the the beginning of finitX
there will be a number of function declarations. Put the new functions
in after the ones there (e.g. int MMumble();) Put the assignments for
the function names in the body of finitX, preserving the order. Once
again, if this loses you, look at the way I did it.  

Once you have modified your bindings, you may find that there is
insufficient space for all of the code to fit before the externals.
You can comment out page mode in the Bindings, and this will give you
some space, but the functions will still be linked in by L2. To insure
that the page mode (or any other) functions you never reference will
NOT be linked in, edit them out of COMMx and into another file. Put
this file after the -l on the L2 command line. I provide a sample of
this, COMM4.C in which I have placed all the page mode functions, and
the MSearch and MRSearch (I wrote new ones). If I include it after the
-l on the L2 line (with appropriately modified COMM(1-3)), when
compiled with the 'factory standard' bindings, I get a 'factory
standard' mince. When compiled with a bindings without references to
these functions, they do not get linked in, and save me space (in
which I can do other things.)

I keep all of my .CRL files on a separate disk in B:, and keep a
mince, all the source code, the C compiler, and L2 on another in A:.
This way i have enough space to get things done in. I have provided a
few submit files that i use so that you can see how i have things
organized and how i get new minces built.

@address{22 September 1981

Barry A. Dobyns, Editor, Slave &c.
Amethyst Users Group
P.O. Box 8173
Austin, Texas 78712-8173
(512)441-9466}
them out e contains the function table initializers and mode set up
functions */

#include "mince.gbl"

finit1()			/* initialize the@heading{Letter from Oscar Goldman, 7-15-81:}

The first issue of the newletter mentions the problem in Crayon
which arises when the pause option is used with a printer that sends
out a line only after receiving a line feed. (My Epson MX-80 is one such.)

Clearly the solution is to supply the necessary line feed.  One
place to do this is in the function 'PagePause' in the file Crayon.C .
As it is now, that function looks like:

@verbatim[ PagePause()
           {            /* pause, get fresh page from user */
      			    if (PrVPos () != 0) PrFF;
			    PrFlush();
			    puts ("Insert fresh page; type any character when ready -->");
			    getchar();
		}]

Using Mince (of course), edit the file by adding the line

@verbatim[    OSend ('/012');]

just after the line 'PrFlush ();'.  Then compile and relink (the
pieces to link are in the file cc.sub).

By making this change, if the pause option is used, the last
thing sent to the printer after printing a page is the needed line
feed.  The extra line feed does no harm; it will merely help push the
paper out of the printer!  Also, if the pause option is not used, then
the function PagePause is not called, and the change has no effect.
Care should be taken not to use the pause option with continuous
forms, because then the extra line-feed will make a mess.

As far as I have been able to determine, there are no undesirable
side effects arising from this modification.  However, this fix is not
very elegant, since it modifies Page Pause even for printers that
don't have the buffer problem (?) of the Epson.  I suppose that in
some future version on Crayon, the creators of Amethyst will make the
required modification in the printer configuration routine.

@display{Oscar Goldman
1221 Knox Road
Wynnewood, PA  19096}lush ();'.  Then compile and relink (the
pieces to link are in the file cc.sub).

By makl, but ignored. Comment lines in your submit
files can greatly ease the pain of figuring out what the little
nasties do weeks /* QUARTZ.C - Alternate Command Set (a mode)
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

The seller reserves the right to make changes, add