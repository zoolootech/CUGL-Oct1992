/*

This program rewritten Sept 9, 1981 by Robert Ward.  All but some low
level functions and the command line argument handling have been 
redone.  The object was to create code that would work with double
density and other controllers. 

To compile correctly for your system. You must know the number of 
sectors per track (SPT), the number of tracks per disk (TPD),
the maximum blocks on a drive (MAXBLKS), the size of each sector in 
bytes (SECSIZE), the size of each block in sectors (BLKSIZE), and the
location of your sector translate table (we are assuming cpm 2.2x), and
the size of the directory(DIRSIZE). 

All of these items are set in #define statements on or about line 100.
Most of the information is available by "stat" on the drive. The 
TXTABLE value may be found by loading ddt, loading a program that 
does file I/O, putting a break (FF?) at vector 16 (sectran call) in
the bios. Once the breakpoint is set, execute the program and 
the value in DE when the breakpoint is encountered is TXTABLE for
that density disk only!
*/
/*
	DISK UTILITY PROGRAM

	Written by Richard Damon
	Version 1.0 May 1980

    This program allows the operator to examine and modify
a CPM diskette.

    The commands available in this package are :

Tn  set current track to n (0-76)
Sn  set current sector to n (1-SECS_PER_TRACK)
Dn  set current disk number to n (0-3)
Bn  set current track and sector to point to block n (0-F2)
N   set current track and sector to next sector/block
	next sector if last set command was for track or sector
	next block if last set command was for block
I   increment sector, if sector>SECS_PER_TRACK set to 1 and increment track
R   read sector/block into buffer
W   write sector/block from buffer
P   print out contents of buffer, allong with track/sector/block information
Ea n n n n
    edit buffer starting from location a filling with values n n n.
Fn  Fill buffer with value n
X   exit program
M   print disk allocation map

Notes:
    1)  Multiple commands may be specified on a line except for X
which must be the only command on the line followed by return.
    2)  Commands may be in upper or lower case letters
    3)  Spaces are ignored except in the E command where they are used
	as separaters for the numbers

Typical commands:
d0t0s1rp	read in the track 0 sector 1 of disk 0 (Drive A) and print it
e1A 4F 20 11	set buffer location 1A to 4F, 1B to 20, and 1C to 11.
e0a 00w		set buffer location 0a to 0 and write buffer
			Note no space after last data byte
nrp		get next buffer and print it

Disk Allocation Map
    The M command is designed to allow the directory blocks (blocks 0 and 1)
to be printed out in a convient format. The directory is print out in the
following format:

 Section 1:
	The top half of the directory listing is a listing of the name
    inforamtion of the directory entries.  Each line corresponds to 1 sector
    of the directory.  A typical entry would be f=DISKTESTCOM/1 4c
    The first letter is a code letter used to referance into section 2.
    The equal sign indicats that the file exists, a star here indicates
      that this entry is a deleted file.
    Next comes the filename and extension.
    The following /n is printed if this is other then the first extent 
      (extent 0) of a file where n is the extent number of this entry.
    The following number is the hex record count for this extent.

 Section 2:
	The bottom half of the directory listing is a disk allocation map
    showing which blocks are in use and by which file. Free blocks are
    indicated by a dot while used blocks are marked by the file control
    letter asigned to a file in section 1.  This listing has been blocked off
    in groups of 8 and 16 to ease reading.

CPM FILE STRUCTURE

    To help the user of this program the following is a brief description
of the format used in CPM.  The first 2 tracks of a disk are reserved
for the bootstrap and the copy of the CPM operating system.  Tracks 2
through 76 store the data.  To speed up disk access CPM does not store
consecutive data in consecutive sectors.  Insteed it uses every 6th sector
for data. Thus to 