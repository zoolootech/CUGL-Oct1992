/*******************************************************/

/*@
  @	fexpnd returns a pointer to the next filename.ext which
  @	matches the first parameter (str) or a zero if no file 
  @	matches that parameter.  The given parameter is processed 
  @	to generate a normalized search string and a normalized 
  @	path prefix.  The prefix is returned in the area pointed to 
  @	by the second parameter.  This string may be concatenated 
  @	with the returned value to give a fully-qualified dataset 
  @	name for open, rename, erase, etc.
  @
  @	You should continue calling fexpnd with the same first 
  @	parameter until it returns a zero, then pass it a new first 
  @	parameter.  This can be used to expand a list of ambiguous 
  @	filenames on a parameter list.   */


char mydta[80] = {0};
char fxs[80] = {0};
char fxsrch[80] = {0};
char fxpref[80] = {0};

char *fexpnd(str, path)
char *str, *path;
{
	int  fixpath(), strcpy();
	char *getret, *dirnxt(), *dirfst();

	if (strcmp(str,fxs) == 0)
		getret = dirnxt(mydta);		/* MS-DOS 2.0+ get next function */
	else {
	    strcpy(fxs,str);			/* for detection of a new name */
	    if (!fixpath(str, fxsrch, fxpref))
	    	return (0);
	    strcpy(path, fxpref);		/* give path to caller */
		getret = dirfst(fxsrch, mydta, 0);	/* MS-DOS 2.0+ get first function */
	}

		return(getret);		/* return filename.ext  */
}
