/* STDLIB3R.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:36:48 PM */
/*
%CC1 $1.C -X 
*/
/* 
Description:

STDLIB3R has file-handling functions that allow use of du: in addition 
to u/d: drive/user prefix.
The functions have been added to DEFF.CRL
They replace functions of same name in DEFF2.CRL, which must be 
patched (DEFF2.CRL: open -> xpen, etc).

Functions:
	open(), creat(), unlink(), rename(), execv(), exec(), du_ud().

In DEFF2.CRL:
	patched header for routines (open...exec -> xpen...xxec);
      	patched body of routines open and unlink;
	edfile or similar program used to make changes.

Execl is not patched and does not take du: form.

An alternate route, consuming less memory:
	revise "setfcu" routine of ccc.asm,
	then recompile: 
		   ccc.asm....c.ccc
		   bds.lib
		   deff2*.csm...deff2.crl
		   
By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

#include "bdscio.h"

/* #include <ctrlcnst.h> */

int open(filename, mode)
char *filename;
int mode;
{
	char temp[20];
	int dchk, fd;

	if ((dchk = du_ud(filename, temp)) < 0)
		return ERROR;
	if (dchk == 0)
		fd = xpen(filename, mode);
	else
		fd = xpen(temp, mode);
	return fd;
}

int creat(filename)
char *filename;
{
	char temp[20];
	int dchk, fd;

	if ((dchk = du_ud(filename, temp)) < 0)
		return ERROR;
	if (dchk == 0)
		fd = xreat(filename);
	else
		fd = xreat(temp);
	return fd;
}

int unlink(filename)
char *filename;
{
	char temp[20];
	int dchk, fd;

	if ((dchk = du_ud(filename, temp)) < 0)
		return ERROR;
	if (dchk == 0)
		fd = xnlink(filename);
	else
		fd = xnlink(temp);
	return fd;
}

int rename(old, new)
char *old, *new;
{
	char oldtemp[20];
	char newtemp[20];
	int oldchk, newchk, fd;

	if ((oldchk = du_ud(old, oldtemp)) < 0)
		return ERROR;
	if ((newchk = du_ud(new, newtemp)) < 0)
		return ERROR;
	if (oldchk == 0)
		if (newchk == 0)
			fd = xename(old, new);
		else
			fd = xename(old, newtemp);
	else
		if (newchk == 0)
			fd = xename(oldtemp, new);
		else
			fd = xename(oldtemp, newtemp);
	return fd;
}

int execv(filename, argvector)
char *filename;
char *argvector;
{
	char temp[20];
	int dchk, fd;

	if ((dchk = du_ud(filename, temp)) < 0)
		return ERROR;
	if (dchk == 0)
		fd = xxecv(filename, argvector);
	else
		fd = xxecv(temp, argvector);
	return fd;
}

int exec(filename)
char *filename;
{
	char temp[20];
	int dchk, fd;

	if ((dchk = du_ud(filename, temp)) < 0)
		return ERROR;
	if (dchk == 0)
		fd = xxec(filename);
	else
		fd = xxec(temp);
	return fd;
}

int du_ud(fname, temp)
char *fname, *temp;
{
	int i, j, l;

	/* Set j = pointer to ':'		*/
	/* Exit if no du: or #/d: prefix	*/
	for (j = 0; fname[j] != ':'; j++)
		if (fname[j] == NULL)
			return 0;

	/* Exit if prefix already #/d:		*/
	for (i = 1; i < j; i++)
		if (fname[i] == '/')
			return 0;

	/* Set i = pointer to 1st user digit 
							or to '?'		*/
	for (i = 0; (isdigit(fname[i]) == FALSE) && (fname[i] != '?'); i++)
		if (i == 0 && j == 1)        /* 'd:' only, no userno		*/
			return 0;
	/* errors in du: designator:	*/
	/* 1st char not alpha or digit 	*/
	/* neither 1st or 2nd char digit */
	else
	    if (i == 1 && !isalpha(fname[0]))
		return ERROR;
	else
	    if (i > 1)
		return ERROR;

	/* Now construct new file image in temp	*/
	/* First copy the user number		*/
	l = 0;
	while (i < j)
		temp[l++] = fname[i++];
	temp[l++] = '/';
	/* Then the drive designator if present	*/
	if (isalpha(fname[0]))
	{
		temp[l++] = fname[0];
		temp[l++] = ':';
	}
	/* Then the filename			*/
	strcpy(&temp[l], &fname[j + 1]);

	return 1;
}
/* END OF DU_UD		*/
