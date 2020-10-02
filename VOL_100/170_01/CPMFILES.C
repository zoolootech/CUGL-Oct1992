#include <stdio.h>
#include <telink.h>

/* This is a set of replacement functions to make the BDS 'C' compiler
(for CP/M) "unix" compatible.  Each function is described below. 

NOTE: You MUST call xinit() at the start of your program to initialize
this file system.

All of the names begin with 'x', like xread, xwrite, etc. to distinguish 
them from the BDS functions. You can of course change them to read, write
etc.

Change MAXFILES to change the maximum number of files open.

The CP/M search first/search next is also supported. It will support only
128 names. Change MAXNAMES below if necessary.

		Tom Jennings 30 June 83

	The maximum number of open files is defined by MAXFILES, below.

xinit()				Initialize the file system by clearing
				all handles and counts. MUST be called
				before any routines are used. Calling it
				later will abort (not close) all open files.

handle= xopen(name,access)	Open the file for reading or writing.
int handle;			'access' is ignored; no text translation
char *name;			is done. Returns -1 if error or file not
				found.

handle= xcreate(name,access)	Similar to above, creates a new file,
				whose length is zero. Any previous file
				is deleted.

val= xread(handle,buffer,count) Read or write chars to the specified
int val,count,handle;		file. 'val' returns the number of bytes
char *buffer;			actually read; will be == to count if OK,
				0 if none read.

val= xwrite(handle,buffer,count)Same as above, except writes to the open
int val,count,handle;		file. Same returns.
char *buffer;

error= xclose(handle)		Close the file. Returns -1 if error. Flushes
				any data left in the buffer.


found= xfind(path,name,flag)	Search for filenames. 'flag' should be
int found,flag;			0 for the first call, non-zero there-
char *path,*name;		after. If 'found' returns non-zero, then
				a filename matching 'path' was found on the
				disk, and is returned in 'name'. 'path'
				may contain wildcards and a drive spec.
				'name' must be 14 chars long. A maximum of
				MAXNAMES (below) will be handled. The
				two names are "normal" null terminated names,
				with a dot if appicable. The drive spec 
				is stripped off. Upper and lower case is
				handled by the function.

				NOTE: This works by making the ENTIRE list
				the 1st time called, and then just returns
				a previously found name when 'flag' != 0.
				You CAN make any file open, close, create,
				read or write calls in between calls to xfind.
*/
#define MAXFILES 8		/* max. number of files open at once, */

struct _xiobuf {		/* Define the stuff for each file. */
	int hndle;		/* the associated file handle, */
	char fcb[36];		/* The opened FCB, */
	int bufcnt;		/* # bytes in the buffer, */
	char buff[128];		/* the data buffer, */
	int dirty;		/* 1 if written, */
} xfiles[MAXFILES];

/* List of found names from the search first/next. */

#define MAXNAMES 128		/* maximum number of names for search 
				first/next. NOTE: 11 bytes per name. */
int numnames;			/* number of names in pile, */
int namenum;			/* current name from pile, */
char names[MAXNAMES][11];	/* where the names are, */

/* Initialize the file system. */

xinit() {
int i;

	for (i= 0; i < MAXFILES; i++)
		xfiles[i].hndle= -1;
	return;
}

/* Open a file. Return -1 if file not found, or no handles available. */

xopen(name,access)
char *name;
int access;
{
int handle;			/* handle to return to caller, */
int i;

	handle= gethandle(-1);	/* find a free handle, */
	if (handle == -1)
		return(handle);	/* none available. */

	for (i= 0; i < 36; i++)	/* clear out the FCB, */
		xfiles[handle].fcb[i]= 0;

	xfiles[handle].fcb[0]= 0; /* default drive, */
	if (name[1] == ':') {	/* if a drive spec, */
		xfiles[handle].fcb[0]= name[0] - '@';
		++name; ++name;	/* install it, */
	}
	to_fcb(name,&xfiles[handle].fcb[1]);
	if (bdos(15,&(xfiles[handle].fcb[0])) == 255)
		return(-1);

	xfiles[handle].hndle= handle;	/* mark which handle, */
	xfiles[handle].bufcnt= 0;	/* empty buffer, */
	xfiles[handle].dirty= 0;
	return(handle);
}
/* Close the specified file. */

xclose(handle)
int handle;
{
int err;

	if (gethandle(handle) == -1)
		return(-1);		/* check handle, */

	err= 0;				/* assume no error, */
	if (xfiles[handle].dirty) {	/* if stuff in buffer, */
		bdos(26,&xfiles[handle].buff[0]);
		err= bdos(21,&xfiles[handle].fcb[0]) ? -1 : 0;
	}
	xfiles[handle].dirty= 0;
	xfiles[handle].bufcnt= 0;
	xfiles[handle].hndle= 0;
	bdos(16,&xfiles[handle].fcb[0]);	/* close file. */
	return(err);
}
/* Create the specified file. */

xcreate(name,access)
char *name;
int access;
{
int handle;
int i;
	handle= gethandle(-1);
	if (handle == -1)
		return(-1);

	for (i= 0; i < 36; i++)			/* clear the FCB, */
		xfiles[handle].fcb[i]= 0;

	if (name[1] == ':') {			/* if :, then drive, */
		xfiles[handle].fcb[0]= name[0] - '@';
		++name; ++name;
	} else
		xfiles[handle].fcb[0]= 0;	/* default drive, */

	to_fcb(name,&xfiles[handle].fcb[1]);
	xfiles[handle].fcb[12]= '?';		/* delete all extents, */
	bdos(19,&xfiles[handle].fcb[0]);	/* delete file, */
	xfiles[handle].fcb[12]= 0;		/* zero extent, */
	if (bdos(22,&xfiles[handle].fcb[0]) == 255) /* create file, */
		return(-1);
	xfiles[handle].hndle= handle;
	xfiles[handle].bufcnt= 0;
	xfiles[handle].dirty= 0;
	return(handle);
}

/* Read from a file. */

xread(handle,buffer,count)
int handle;
char *buffer;
int count;
{
int err,i;

	i= 0;
	if (gethandle(handle) == -1)
		return(0);
	while (count) {
		if (xfiles[handle].bufcnt) { /* if not empty, */
			*buffer++= xfiles[handle].buff[128 - xfiles[handle].bufcnt--];
			i++;			/* read from buffer, */
			--count;		/* read one, */
		} else {
			bdos(26,&xfiles[handle].buff[0]); /* DMA address, */
			err= bdos(20,&xfiles[handle].fcb[0]);/* fill buffer, */
			if (err)
				break;
			xfiles[handle].bufcnt= 128;	/* set new count, */
		}
	}
	return(i);
}

/* Write chars to the specified file. */

xwrite(handle,buffer,count)
int handle;
char *buffer;
int count;
{
int i,err,j;

	if (gethandle(handle) == -1)	/* find it first, */
		return(0);		/* doesnt exist! */

	i= 0;				/* # byte written, */
	while (count) {
		if (xfiles[handle].bufcnt < 128) {
			xfiles[handle].buff[xfiles[handle].bufcnt++]= *buffer++;
			xfiles[handle].dirty= 1;
			++i;
			--count;
		} else {
			xfiles[handle].bufcnt= 0; /* buffer empty, */
			xfiles[handle].dirty= 0;  /* buffer written, */
			bdos(26,&xfiles[handle].buff[0]); /* set DMA address, */
			if (bdos(21,&xfiles[handle].fcb[0]))
				break;
		}
	}
	return(i);			/* return number written, */
}

/* Search first/next. The first time causes a disk search, and returns one
name. Search nexts just return one name from the pile. */

xfind(path,name,flag)
char *path,*name;
int flag;
{
int i,v;
char *p;

if (flag == 0) {			/* if first time, */

	numnames= 0;			/* empty list, */
	namenum= 0;			/* start with the 1st. */
	for (i= 0; i < 32; i++)		/* clear out FCB, */
		xffcb[i]= 0;
	for (i= 1; i < 12; i++)		/* blank name, */
		xffcb[i]= ' ';

	setfcb(xffcb,path);		/* make an FCB, */
	if ((v= bdos(17,xffcb)) == 255)	/* search first, */
		return(0);		/* no match. */
	do {
		p= cast(0x80 + 1 + (32 * v));
		for (i= 0; i < 12; i++)	/* copy in name only, */
			names[numnames][i]= p[i];
		++numnames;		/* find all others */
		v= bdos(18,xffcb);	/* search next, */
	} while ((v != 255) && (numnames < MAXNAMES));
}
/* Search next. Just get a name from the list. */

if (numnames) {
	from_fcb(names[namenum],name);
	--numnames; ++namenum;
	return(1);
}
return(0);
}
/* Do a type conversion: BDS has no cast operator. */

cast(obj)
char *obj;
{	return(obj);
}
/* Look for the handle number in the file structure. Assigned handles
start at 0; unallocated handles are -1. Returns either the handle # 
(index into the structure) or -1 if no match. */

gethandle(handle) 
int handle;
{
int i;
	for (i= 0; i < MAXFILES; i++) {
		if (xfiles[i].hndle == handle)
			return(i);
	}
	return(-1);
}
/* Convert a normal asciz string to MSDOS/CPM FCB format. Make the filename
portion 8 characters, extention 3 maximum. */

to_fcb(inname,outname)
char *inname;
char outname[];
{
char c;
int i;

	for (i= 0; i < 11; i++)
		outname[i]= ' ';		/* clear out name, */
	outname[i]= '\0';			/* null terminate, */
	for (i= 0; i < 11; i++) {
		if (*inname == '\0')		/* if null, */
			outname[i]= ' ';	/* pad with blanks, */
		else if (*inname == '.') {	/* if a dot, */
			++inname;		/* skip it, */
			i= 7;			/* skip to extention, */
		} else {
			outname[i]= toupper(*inname);
			++inname;
		}
	}
	return;
}
/* Convert a CP/M like filename to a normal ASCIZ name. */

from_fcb(inname,outname)
char *inname,*outname;
{
int i;
char c;

	for (i= 8; i != 0; i--) {		/* do name portion, */
		c= filtchar(*inname++);		/* filter it, */
		if (c != ' ') 			/* if not space, */
			*outname++= c;		/* set it, */
	}					/* do all 8 chars, */
	*outname++= '.';			/* add the dot, */

	for (i= 3; i != 0; i--) {		/* do extention, */
		c= filtchar(*inname++);		/* filter it, */
		if (c == ' ')
			break;
		*outname++= c;
	}
	*outname= '\0';				/* terminate it, */
	return;
}
/* Process a filename character. */

char filtchar(c)
char c;
{	c&= 0x7f;
	return(toupper(c));
}
