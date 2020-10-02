                  Doing Sneaky Things with CP/M


	When CP/M version 1.4 was all the rage, we didn't have
too many problems with incompatibility between disk formats.
Almost everyone used 8inch single density IBM formatted disks
and the standard skewing factor built in to the CP/M bdos.
People who wrote those clever little utilities to manipulate
the directory or copy system tracks and needed access to the
bios routines, could do so in the knowledge that they were
dealing with a standard disk configuration.

	CP/M version 2.2 changed all that.  By putting all of
the hardware dependencies in the bios and specifying standard
routines for accessing these, the operating system could be
configured for a wide variety of disk formats from the humble
mini-floppy to the giant hard disk.  Unfortunately, many of
the utilities haven't caught up with the change.  I met a very
sad man the other day who had backed up an important disk
using COPY.COM - a track to track copier which has been around
for a long time.  Unfortunately, he did it on a double-density
system and is still trying to unscramble the result.

	One way of handling the problem is to #define all the
important parameters in your programs to produce versions
specially tailored for different formats.  This is not too
difficult - STAT DSK: will give you most of the numbers - but
it does cause problems if you have a number of different
formats.  The Altos multi-user system on which I sometimes
work, for example, has a hard disk and both single and
double density floppies.  Trying to keep track of three
different versions of a number of utilities is more than I
want to do.

	A better way is to use CP/M itself.  After all,
the information is in the bios and Digital Research have
specified the way to get to it.  The Version 2.0 alteration
guide has all the data but, in the manner of most Digital
Research publications, the explanations are less than lucid.
To make life easier, the collection of functions below are
what I have been using for some time.  They have been tested
with both versions of CP/M and on a variety of disk formats
and have (in their present form) not caused any major
disaster of which I am aware.  Caveat emptor, though - If
you use these to write a disk hacking program, make sure
you test it on an unimportant disk first!

	The functions are split into two groups.  The first
accesses the bdos in the approved manner and are pretty
straightforward.  Getparams() determines which version
of CP/M it is running under and then sets the external
parameter veriables accordingly.  For version 1.4, it
uses 'canned' values appropriate to 8inch disks - these
will need altering for special 1.4 versions modified
for mini-floppies or other formats.  Note that the
parameter values are set for the current CP/M default
drive.  If your program is to work in an environment
where there are mixed types, you must select each
prior to calling the function. 

	The second group uses the bios low-level routines
and can easily get you into trouble if you don't know what
you are doing. I will assume that you do.  Sectran() is
the critical routine and performs translation between the
logical and physical sector numbers to take account of the
skew factor.  Note again that canned values are used for
CP/M version 1.4.

	Before sectran() is called, the disk on which you
are going to operate must be selected using seldsk().  It
is not good enough to use the currently selected drive as
seldsk() is the only way to access the address of the 
sector translation table in the bios and initialize the
value of transtable which sectran uses.  All this is a bit
messy, but then you weren't supposed to do this sort of thing
with CP/M, were you?

						Nick Hammond


/********* BDOSCOM - Routines for accessing the BDOS *********/

/*	the external variables below are set by getparams() 	*/
/*	to the appropriate value for the current default    	*/
/*	drive. They may then be accessed by the host program	*/ 

int	offset,		/* no of reserved system tracks */
	secpertrk,	/* 128 byte records per track */
	entries,	/* no of 32 byte directory entries */
	blocksize,	/* size of a disk block in bytes */
	capacity;	/* disk capacity in blocks */


getparams()
{	/* set parameters for current default drive */
	struct dpb{		/* disk parameter block */
		int	spt;
		char	bsh,
			blm,
			exm;
		int	dsm,
			drm,
			all,
			cks,
			off;
		}
			*bp;
	if (cpmvsn() == 1){	/* set for std 8in disk */
		secpertrk = 26;
		blocksize = 1024;
		capacity = 243;
		entries = 64;
		offset = 2;
		}
	else{			/* get data from dpb */
		bp = dpbaddr();
		secpertrk = bp -> spt;
		blocksize = 1024 << ((bp -> bsh) - 3);
		capacity = (bp -> dsm) + 1;
		entries = (bp -> drm) + 1;
		offset = bp -> off;
		}		
}

bdos16(funct,param)
int	funct,param;
{	/* perform bdos call and return value of HL */
	/* note: bdos library function returns value of A */
	return call(0005,0,0,funct,param);
}
cpmvsn()
{	/* return CP/M version no */
	return (bdos16(12,0) ? 2 : 1);
}

seldrive(drive)
int	drive;
{	/* select drive as current default */
	bdos(14,drive);
}


defdrive()
{	/* return no of current default disk */
	return bdos(25,0);
}

dpbaddr()
{	/* return address of dpb of current default drive */
	return bdos16(31,0);
}


/********** BIOSCOM - Low Level CP/M Communications **********/

/*	transtable is a pointer to the logical/physical     */
/*	sector translation table for the currently selected */
/*	disk drive.  It is used by sectran() in CP/M V2.2   */ 

char	*transtable;	/* ^ sector translation table */

bios16(offset,param)
int	offset,		/* ordinal number in bios vector */
	param;		/* parameter for bios call */
{	/* perform bios call and return the value of HL */
	/* note: bios library function returns value of A */
	int	*wbp;
	char	*addr;
	wbp = 0x0001;
	addr = *wbp + (--offset) * 3;
	return call(addr,0,0,param,0);
}

home()
{	/* disk to trk 0 */
	bios(8);
}

seldsk(disk)
char	disk;
{	/* select disk and initialize transtable */
	struct dph{		/* disk parameter header */
		char	*xlt;
		int	scratch[3];
		char	*dirbuf;
		struct
		   dpb	*dpbptr;
		char	*csv,
			*alv;
		}
			*hp;
	hp = bios16(9,disk);
	transtable = hp -> xlt;
}

settrk(trk)
int	trk;
{	/* set track */
	bios(10,trk);
	return trk;
}

setsec(sec)
int	sec;
{	/* set sector */
	bios(11,sec);
	return sec;
}

setdma(dma)
char	*dma;
{	/* set dma transfer address */
	bios(12,dma);
	return dma;
}

readsec()
{	/* read sector */
	return bios(13,0;
}

writesec()
{	/* write sector */
	return bios(14,0);
}

writedsec()
{	/* write a directory sector */
	return bios(14,1);
}

sectran(lsec,vsn)
int	lsec,		/* logical sector number */
	vsn;		/* CP/M bios version */
/* note that this routine must be called with the version #
   as a parameter.  It is not good practice to obtain the
   version by a bdos call while in a bios level routine
   since there is no guarantee that the bdos has not been
   overlayed.  The version should be obtained by the calling
   program by a previous call to cpmvsn() */
{	/* logical to physical sector translation */
	int	incr;
	int	*wbp;
	char	*addr;
	if (vsn == 1){	/* use standard skew factor */
		incr = lsec >= 13 ? 2 : 1;
		return ((lsec * 6) + incr) % 26;
		}
	else{
		if (transtable == 0) return lsec;
		wbp = 0x0001;
		addr = *wbp + 45;
		return call(addr,0,0,lsec,transtable) & 0xFF;
		}
}
