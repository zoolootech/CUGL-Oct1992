/*@================================================================ */
/*@
  @  From Dr. Dobbs #108 October 1985.   */

/*@ fixpath() processes a DOS pathname for two different uses.
  @	The input path, *ip, is presumably a command operand like the 
  @	first operand of DIR.  One output, the search path *sp, is 
  @	the input possibly augmented with "*.*" or "\*.*" so that it 
  @	will work reliably with DOS functions 4E/$f.  The other output
  @	is a lead-in path, *lip, which can be prefixed to the simple 
  @	filename.ext returned by fuctions 4E/4F to make a complete path 
  @	for opening or erasing a file.
  @
  @	The function returns 1 if it is successful, but 0 if the 
  @	input path can't be processed and should not be used.
  @
  @	Some input paths can be processed here yet be invalid or 
  @	useless.  The search path produced from such an input wwill
  @	cause an error return from function 4E (search first match). */
/* ================================================================ */

int fixpath(ip, sp, lip)
register char 	*ip,	/* input path */
				*sp,	/* the search path */
				*lip;	/* the lead-in or prefix path */
{
	char *cp;			/* work pointer for scanning paths */
	int attr;			/* attribute for chgattr */
	int ret, strlen(), strcpy(), strcmp(), strcat(), chgattr();

/* ================================================================ */
/*	Pick off 4 special cases:
	(1)	the NULL string, which we take to mean "*.*"
	(2)	the simple "d:" reference to a drive, which we also take
		to mean "d:*.*"
	(3)	the root-dir reference "d:\" which is mishandled by 
		function 0x4300 of both DOS 2.1 and 3.0.
	(4)	any reference that ends in "\"

	In all cases, the input path is ok as a lead-in, but it needs
	the global qualifier *.* added for searching.

/* ================================================================ */

	if ((strlen(ip) == 0)				/* null string */
		|| (strcmp(ip+1, ":") == 0)		/* d: only */
		|| (ip[strlen(ip)-1] == '\\'))	/* ends in backslash */
		{
			strcpy(lip, ip);			/* input is ok as prefix */
			strcpy(sp, ip);
			strcat(sp, "*.*");			/* add *.* for search */
			return (1);
		}

/* ================================================================ */
/*	Ok, we have a non-null string not ending in \ and not a lone
	drive letter.  Four possibilities remain:
	(1)	an explicit file reference, b:\mydir\mydat
	(2)	an explicit directory reference, \mydir
	(3)	an ambiguous file reference, *.* or b:\mydir\*.dat
	(4)	an unknown name, a:noway or b:\mydir\nonesuch.dat

	We can separate this with fair reliability using DOS function
	0x4300, get attributes from path.


/* ================================================================ */

	attr = 0x0000;						/* output area for get command */
	ret = chgattr('G', ip, &attr);		/* get attributes for this path */

	if (ret == 0x0002) 

	/*   the path is valid, in that all directories and drives
		named in it are valid, but the final name is unknown.  No
		files will be found in a search, so quit now.   */

		return (0);

	if ((ret == 0x0003) 
		|| ((ret == 0) && ((attr & 0x0010) == 0))) {

	/* Error 3, path not found, could mean total junk or a 
		misspelt directory name, but most likely it just means
		the path ends in an ambiguous name.  If there's an error
		the initial search (function 4Eh) will fail.

		With no error and reg cx not showing the directory 
		attribute flags 0010, we have a normal, unambiguous file
		pathname like "b:\mydir\mydat" or just "myprog.com".

		In either case the search path is the whole input
		path.  The lead-in is that shorn of whatever follows the 
		rightmost \ or :, whichever comes last -- or just a null
		string if there is no \ or :.		*/

		strcpy(sp, ip);			/* working copy of ip in sp */
		cp = sp + strlen(sp) -1;
		for (; cp > sp; --cp)
			if (('\\' == *cp) || (':' == *cp)) 
				break;
		if (cp > sp)
			++cp;				/* retain colon or slash */
		*cp = '\0';				/* make a null string */
		strcpy(lip, sp);
		strcpy(sp, ip);			/* whole input for search */
		return (1);
	}

	if ((ret == 0) && (attr & 0x0010)) {

	/* No error and the directory attribute returned in cx
		shows an unambiguous path that happpens to end in the 
		name of a directory, for instance "..\..\bin" or
		"b:\mydir".  Applying the rules of DIR or COPY, we
		have to treat these as global refs to all files named in 
		the directory.  The search path is the input with 
		"\*.*" tacked onto it.  The lead-in path is just the 
		input plus a backslash.			*/

		strcpy(sp, ip);
		strcpy(lip, ip);
		strcat(sp, "\\*.*");
		strcat(lip, "\\");
		return (1);
	}

	else {
		/* unexpected events make me nervous, so give up */
		return (0);
	}
}
