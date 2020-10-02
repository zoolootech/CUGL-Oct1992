/* WILDEXPR.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:37:31 PM */
/*
%CC1 $1.C -X
*/
/* 
Description:

Modification of Van Nuys Toolkit - BDS C distribution version of WILDEXP:
	use of <du:>, by conversion to <u/d:> form;
	expansion of <d?:> and <?/d:>;
	all output given uu/d: prefix, even files in current drive/user area;
	restructuring of much of the code.

Size of code:			hex bytes
	this version:		CC1 		wildcard nos & du: (d?:, ?/d:)
	Van Nuys:		C8E		user numbers (u/d: form only)
	BDS C v1.50a:		A8E		no user numbers (d: form only)
(Inclusion of wildcard user numbers costs a little but not much.)
(The size of the du_ud() function is not included in the value cited for the
	memory requirement for this version)

The number of !names allowed is 40 (= 2 !d?: + 8 !du:).

NOTE:
There must be a <du:> --> <u/d:> conversion function = du_ud().
If the conversion is required only for WILDCARD, then it is sufficient
	to compile and link the du_ud function given at the end of this file.
A more general approach is to modify DEFF.CRL and DEFF2.CRL, 
	by adding the functions of STDLIB3R to DEFF.CRL, and 
	by patching the functions of DEFF2.CRL, as described in STDLIB3R.
If this is done, then all file opening, creation etc operations accept the
	<du:> form.


By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/


        /* page eject */

/*********************************************************************
*                               WILDEXP                              *
*********************************************************************/
/*
	WILDEXP.C 	v2.0	12/26/83
	BDS C Command-line Wild-card expansion utility
	
	Revised by JA Rupley -- to include expansion of ?/ = d?:, and
				conversion of du: -> u/d:
	Revised by Gene Mallory
	Written by Leor Zolman

	Lets ambiguous file names appear on the command line to C programs,
	automatically expanding the parameter list to contain all files that
	fit the afn's. Returns ERROR if something went wrong; usually, this
	is due to MAXITEMS being set too low; the "MAXITEMS" defined constant
	sets the maximum number of matches that WILDEXP can handle. If you
	need to be able to process directories with more entries than this,
	be sure to modify MAXITEMS to be bigger than the largest expected
	directory size.

	An afn preceded by a "!" causes all names matching the given afn to
	be EXCLUDED from the resulting expansion list. Thus, to yield a
	command line containing all files except "COM" files, you'd say:

		A>progname !*.com <cr>

	Another example: to get all files on B: except .C files, say:

		A>prognam b:*.* !b:*.c <cr>

	When giving a "!" afn, "*" chars in the string matches to the end of
	either the filename or extension, just like CP/M, but "?" chars match
	ONE and ONLY ONE character in either the filename or extension.

	To use WILDEXP, begin your "main" function as follows:

	---------------------------------------------
	main(argc,argv)
	char **argv;
	{
	...				   	/* local declarations  */
		dioinit(&argc,argv);		/* if using DIO	  */
		wildexp(&argc,&argv)		/* 1st state. in prog. */
		...
	---------------------------------------------

	and link WILDEXP.CRL in with your program. That's all there is to
	it; note that "wildexp" uses the "sbrk" function to obtain storage,
	so don't go playing around with memory that is outside of the
	external or stack areas unless you obtain the memory through "sbrk"
	or "alloc" calls.
*/

        /* page eject */

#include "BDSCIO.H"

#define MAXITEMS 1333 /* max no. of items after expansion */
#define MAXUSERS 16 /* max user number in expansion of ?/ */
#define NEED_DU_UD FALSE /* set true if du_ud() not in DEFF.CRL */

#define SEARCH_FIRST 17 /* BDOS calls */
#define SEARCH_NEXT 18
#define GET_DRIVE 25
#define GET_USER 32
#define SET_DISK 14
#define SET_USER 32
#define SET_DMA 26

wildexp(argcptr, argvptr)
int *argcptr;        /* pointer to argc */
char ***argvptr;        /* pointer to argv */
{
	int newargc;        /* new argc */
	char **newargv;        /* new argv */
	char **oldargv;        /* old argv */
	int oldargc;        /* old argc */
	int expargc;        /* expansion of ?/ and d? */
	char **expargv;
	char fcb[36];        /* fcb used for search for first/next calls */
	char dmapos;        /* value returned by search calls */
	char first_time;        /* used in search routine */
	char tmpfn[20];        /* temporary file name */
	char *p_tmp;        /* temporary pointer */
	char *p_exp;        /* another temporary pointer */
	char *notfns[40];        /* list of !<afn> entries */
	int notcount;        /* count of entries in notfns */
	char cur_drive;        /* current drive number */
	char cur_user;        /* current user number */
	char usrno;        /* this file user number */
	char driveno;        /* this file drive number */
	char buffer[128];        /* directory buffer */
	int i, j, k;

	cur_drive = bdos(GET_DRIVE);
	cur_user = bdos(GET_USER, 0xff);
	bdos(SET_DMA, buffer);

	oldargv = *argvptr;
	oldargc = *argcptr;
	expargc = 1;
	newargc = 1;
	notcount = 0;

	        /* page eject */

	/* get newargv and expargv space  */
	if (((newargv = sbrk(MAXITEMS * 2 + 2)) == ERROR) ||
	    ((expargv = sbrk(MAXITEMS + 2)) == ERROR))
		goto err_exit;

	/* change du: to #/d: form	*/
	/* expand all_users flag = ?/ = d?: */
	/* at end of loop, handle..first name is a !name */
	for (i = 1; i < oldargc; i++)
	{
		if (*(p_tmp = oldargv[i]) == '!')
			p_tmp++;

		if (du_ud(p_tmp, tmpfn))
			p_tmp = tmpfn;

		for (j = 0; j <= MAXUSERS; j++)
		{
			if ((p_exp = expargv[expargc++] = sbrk(20)) == ERROR)
				goto err_exit;
			if (oldargv[i][0] == '!')
				*p_exp++ = '!';
			if ((*p_tmp == '?') && (*(p_tmp + 1) == '/'))
			{
				*p_exp++ = j / 10 + '0';
				*p_exp++ = j % 10 + '0';
				strcpy(p_exp, (p_tmp + 1));
			}
			else
			    {
				strcpy(p_exp, p_tmp);
				break;
			}
		}
		if (oldargv[1][0] == '!')
		{
			fsplit(&expargv[1][1], tmpfn, &usrno, &driveno,
			cur_drive, cur_user);
			strcpy(&tmpfn[5], "*.*");
			oldargv[1] = tmpfn;
			if (expargc > 2)
				*++oldargv[1] = '?';
			i--;
		}
	}
	oldargv = expargv;
	oldargc = expargc;

	/* if not_flag on oldargv then add to not list .... */
	/* else get directory files that match wildcard oldargv */
	for (i = 1; i < oldargc; i++)
	{
		if (oldargv[i][0] == '!')
			notfns[notcount++] = &oldargv[i][1];
		else
		    {
			p_tmp = fsplit(oldargv[i], tmpfn, &usrno, &driveno,
			cur_drive, cur_user);
			setfcb(fcb, p_tmp);
			bdos(SET_DISK, driveno);
			bdos(SET_USER, usrno);
			first_time = TRUE;
			while (1)
			{
				if ((dmapos = bdos((first_time ? SEARCH_FIRST
				    : SEARCH_NEXT), fcb)) == 255)
					break;
				first_time = FALSE;
				if (((newargv[newargc] = sbrk(20)) == ERROR) ||
				    (newargc >= MAXITEMS))
					goto err_exit;
				makename(newargv[newargc++], tmpfn,
				(buffer + dmapos * 32));
			}
		}
	}

	/* restore current drive and user */
	bdos(SET_DISK, cur_drive);
	bdos(SET_USER, cur_user);

	/*  Now eliminate all the NOT names  */
	for (i = 0; i < notcount; i++)
	{
		p_tmp = fsplit(notfns[i], tmpfn, &usrno, &driveno,
		cur_drive, cur_user);
		strcpy(&tmpfn[5], p_tmp);
		for (j = 1; j < newargc; j++)
			while (match(tmpfn, newargv[j]))
			{
				if (j == --newargc)
					break;
				for (k = j; k < newargc; k++)
					newargv[k] = newargv[k + 1];
			}
	}

	/* Replace argc and argv with new values....and exit*/
	*argcptr = newargc;
	*argvptr = newargv;

	return 0;

err_exit :
	exit(puts("WILDEXP: Out of RAM space"));
}


        /* page eject */

void makename(dest, tmpfn, source)
char *dest, *tmpfn, *source;
{
	int i;

	/* user and drive */
	while (*tmpfn)
		*dest++ = *tmpfn++;

	/* file name and type */
	for (i = 1; i < 12; i++)
	{
		if ((source[i] &= 0x7f) == ' ')
			continue;
		if (i == 9)
			*dest++ = '.';
		*dest++ = source[i];
	}
	*dest = '\0';
}


int match(f1ptr, f2ptr)
char *f1ptr, *f2ptr;
{
	char c;

	while (c = *f1ptr++)
	{
		switch (c)
		{
		case '?' :
			if (*f2ptr && *f2ptr != '.')
				f2ptr++;
			else
			    return FALSE;
			break;
		case '*' :
			while (*f1ptr && (*f1ptr++ != '.'))
				;
			while (*f2ptr && (*f2ptr++ != '.'))
				;
			break;
		default :
			if (c != *f2ptr++)
				return FALSE;
		}
	}

	if (!*f2ptr)
		return TRUE;
	else
	    return FALSE;
}


char *fsplit(bigname, dest, nu, nd, cur_drive, cur_user)
char *bigname, *dest, *nu, *nd, cur_drive, cur_user;
{
	/* parse or set drive/user and set pointer to return filename */
	if ((isdigit(bigname[0]) && (bigname[1] == '/'))
	    || (isdigit(bigname[0]) && isdigit(bigname[1]) 
		&& (bigname[2] == '/')))
	{
		*nu = atoi(bigname);
		while (*bigname++ != '/')
			;
	}
	else
	    *nu = cur_user;
	if (isalpha(bigname[0]) && (bigname[1] == ':'))
	{
		*nd = toupper(bigname[0]) - 'A';
		bigname += 2;
	}
	else
	    *nd = cur_drive;

	/* put drive/user as uu/d: into dest array */
	*dest++ = *nu / 10 + '0';
	*dest++ = *nu % 10 + '0';
	*dest++ = '/';
	*dest++ = *nd + 'A';
	*dest++ = ':';
	*dest = '\0';

	return bigname;
}


        /* page eject */

#if NEED_DU_UD
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

	/* Set i = pointer to 1st user digit or to '?' */
	/* if 'd:' only, then no userno and return <0>*/
	for (i = 0; (isdigit(fname[i]) == FALSE) && (fname[i] != '?'); i++)
		if (i == 0 && j == 1)
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
#endif

