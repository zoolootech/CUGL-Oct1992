/*
HEADER: CUG 121.;

    TITLE:	files - functions for ambiguous filenames;
    VERSION:	1.0;
    DATE:	09/01/85;
    DESCRIPTION: "This file contains the following functions used by other
		programs on this disk: (1) filelist converts ambiguous file
		descriptors to unambiguous file descriptors, (2) nameok takes
		an input file descriptor (ambiguous or unambiguous) and
		creates a list of unambiguous file descriptors in the supplied
		table, (3) pickout extracts the name from a CP/M fcb.";
    KEYWORDS:	ambiguous, filenames;
    SYSTEM:	CP/M;
    FILENAME:	FILES.C;
    AUTHORS:	Mike W. Meyer, Cal Thixton;
    COMPILERS:	BDS-C 1.50;
*/

#include <bdscio.h>
#include "fcb.h"

#define	DIRFRST	17
#define	DIRNEXT	18

/*
 * filelist - takes a list of afd's in, and returns the
 *	ufd's they generate out.
 *	parms are: in - input list; num - its length;
 *		  out - a pointer to a vector of char *'s.
 *		  max - maximum number of output filenames.
 *
 *	result - the number of files found (or 0...)
 */
filelist(in, num, out, max) char **in, **out; {
	char f, c, buf[20];
	int count;
	fcb con;

	count = 0;
	while (num--)
		{
		setfcb(con, *in++);
		for(f = DIRFRST; (c = bdos(f, &con)) != 255; f = DIRNEXT)
			{
			pickout(0x80 + (c * 32), buf);
			strcpy(buf[count++] = alloc(strlen(buf)) + 1, buf);
			if (count == max)
				return(max);
			}
		}
	return(count);
	}

/*
 * pickout - picks the name out of a cp/m fcb.
 */
pickout(fcon, name) fcb *fcon; char *name; {
	char i;

	if (fcon -> f_entry) {
		*name++ = " ABCD"[fcon -> f_entry & 0x7f];
		*name++ = ':';
		}
	for (i = 0; fcon -> f_name[i] != ' ' && i < 8; i++)
		*name++ = fcon -> f_name[i];
	if (fcon -> f_name[8] != ' ')
		{
		*name++ = '.';
		for (i = 8; fcon -> f_name[i] != ' ' && i < 11; i++)
			*name++ = fcon -> f_name[i];
		}
	*name = 0;
	}

/*
 * nameok - take an afd or ufd in, create list of ufds in table supplied,
 *	return number of last slot used in table.
 *
 *	parms are: filenum - next position to use in following table
 *		   files - ptr to vector of char *'s, containing ufds
 *			with last entry possibly an afd
 *		   filemax - maximum number of filenames in table
 *
 *	result - the next available slot in files
 */
nameok(filenum,files,filemax) char filenum, **files; int filemax; {
	char	f,c,o[20];
	fcb	cpmfcb;

	filenum--;
	if (!strlen(files[filenum])) 
		return(filemax);
	setfcb(cpmfcb,files[filenum]);
	for (f=DIRFRST; (c=bdos(f,&cpmfcb))!=255 && filenum<filemax; f=DIRNEXT)
		{
		pickout(0x80+(c*32),o);
		files[filenum] = alloc(strlen(o)+1);
		strcpy(files[filenum++],o);
		}
	return(filenum);
}
                                                                                                                      