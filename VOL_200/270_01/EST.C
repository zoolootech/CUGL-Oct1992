/*	HEADER: CUG270;
	TITLE:	Find largest, smallest, oldest, newest files;
	DATE:	5/16/88;
	DESCRIPTION: "Lists n largest, smallest, oldest, newest files
		from a given path specification under MS-DOS.  Illustrates
		use of _makepath, _splitpath, _dos_findfirst & _dos_findnext;
	KEYWORDS:  _makepath, _splitpath, _dos_findfirst, _dos_findnext;
	SYSTEM: MS-DOS;
	FILENAME: EST.C;
	WARNINGS: Uses library functions specific to Microsoft C5.0;
	COMPILERS: Microsoft;
	AUTHOR: Les Aldridge;
*/
/*
 * Produced by Programming ARTS
 * PO Box 219
 * Milltown, NJ 08850 (201) 846-7242
 * 5/16/88
 * Programmer: Les Aldridge
 *
*/
/*
 * EST Program: i.e. largEST, smallEST, oldEST, newEST files in a group.
 *    Program idea suggested by Chris Chuba of DIALOGIC Corp.
 *       Implemented by Les Aldridge.
 *    This program may be freely distributed but not sold.
 *
 * Copyright (c) 1988 Programming ARTS.
*/
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>

#define OLDX	0
#define NEWX	1
#define LARGEX	2
#define SMALLX	3

struct fdata {
	unsigned	time, date;
	long        size;
	char        path[40];
	char        name[13];
};

static short	cmpold(struct find_t *ptr, struct fdata *fdp)
{
	if ((ptr->wr_date < fdp->date)
	|| ((ptr->wr_date == fdp->date) && (ptr->wr_time < fdp->time)))
		return(1);
	else
		return(0);
}


static short	cmpnew(struct find_t *ptr, struct fdata *fdp)
{
	if ((ptr->wr_date > fdp->date)
	|| ((ptr->wr_date == fdp->date) && (ptr->wr_time > fdp->time)))
		return(1);
	else
		return(0);
}

static short	cmplarge(struct find_t *ptr, struct fdata *fdp)
{
	if (ptr->size > fdp->size) return(1);
	else return(0);
}


static short	cmpsmall(struct find_t *ptr, struct fdata *fdp)
{
	if (ptr->size < fdp->size) return(1);
	else return(0);
}

static struct estobj {
	short			flag;
	char			*ename;
	struct fdata	**eptr;
	short			(*compare)(struct find_t *, struct fdata *);
} 	obj[] = {
		{ 0, "Oldest",   NULL, cmpold },
		{ 0, "Newest",   NULL, cmpnew },
		{ 0, "Largest",  NULL, cmplarge },
		{ 0, "Smallest", NULL, cmpsmall }
	};

static char		drive[8], dir[40], fpath[40], dpath[80];
static char		fil[16], ext[8];
static short	subds, maxf;

static void *stalloc(short count, short size)
{
	void  *p;

	p = calloc(count, size);
	if (p == NULL)
	{
		printf("Memory allocation error.\n");
		exit(1);
	}
	return(p);
}

static void update(struct fdata *fd, struct find_t *ptr, char *path)
{
	/*
	* Fill in the file data.
	*/
	strcpy(fd->name, ptr->name);
	strcpy(fd->path, path);
	fd->size = ptr->size;
	fd->date = ptr->wr_date;
	fd->time = ptr->wr_time;
}

static void chk(struct find_t *ptr, char *path)
{
	/*
	* Check file against n members of selected group for
	* the __est so far.
	*
	* Uses array of ptrs to struct fdata.
	* Each array consists of maxf+1 ptrs.
	* Whenever a file passes the criterion to fit into a group (i.e it is
	* older, newer, etc. one member of the array will be displaced and the
	* new member will fill a slot in the array.  Members are pushed down to
	* accomodate the new member.  The 'maxf'th member is pushed out. 
	*/
	short  i, j, est = 0;
	struct fdata   *fdp, *out;

	for (i = 0; i < 4; i++)
	{
		if (obj[i].flag)
		{
			out = obj[i].eptr[maxf];
			for (j = maxf-1; j >=0; j--)
			{
				fdp = obj[i].eptr[j];
				if ((fdp->date == 0) || (obj[i].compare(ptr, fdp)))
				{
					 est++;
					 obj[i].eptr[j+1] = fdp;
				}
				else
					 break;
			}
			if (est)
			{
				obj[i].eptr[j+1] = out;
				update(out, ptr, path);
			}
		}
	}
}

static void makefp(char *d, char *f, char *fp)
{
	_splitpath(f, NULL, NULL, fil, ext);
	_makepath(fp, NULL, d, fil, ext);
}


static void findfiles(char *dp, char *fp)
{
	/*
	* Possibly-recursive procedure to find files.
	*/
	char           *wkp;
	short          ret;
	struct find_t  *bp;

	bp = stalloc(1, sizeof(struct find_t));
	wkp = stalloc(1, 40);

	if (subds)  /* search subdirectories */
	{
		makefp(dp, "*", wkp);
		ret =_dos_findfirst(wkp, _A_SUBDIR, bp);
		while (ret == 0)
		{
			if (bp->attrib & _A_SUBDIR) 
			{
				 if (*(bp->name) != '.')
				 {
					 makefp(dp, bp->name, wkp);
					 findfiles(wkp, fp);
				 }
			}
			ret = _dos_findnext(bp);
		}
	}
	makefp(dp, fp, wkp);
	ret = _dos_findfirst(wkp, _A_NORMAL, bp);
	while (ret == 0)
	{
		chk(bp, dp);
		ret = _dos_findnext(bp);
	}
	free(bp);
	free(wkp);
}

static void mkdate(struct fdata *fp, char *str)
{
	unsigned yr, mn, da, hr, min, sec, d, t;

	t = fp->time;
	d = fp->date;
	yr = (d >> 9)+80;
	mn = (d >> 5) & 0xF;
	da = d & 0x1F;
	hr = t >> 11;
	min = (t >>5) & 0x3F;
	sec = t & 0x1F;
	sprintf(str, "%02u/%02u/%02u  %02u:%02u:%02u",
				 mn, da, yr, hr, min, sec);
}

static char dtstr[50];
	
static void prt(struct fdata  *fd[])
{
	short i;
	struct fdata *fp;

	for (i = 0; i < maxf; i++)
	{
		fp = fd[i];
		if (fp->date == 0) continue;
		mkdate(fp, dtstr);
		_splitpath(fp->name, NULL, NULL, fil, ext);
		_makepath(dpath, NULL, fp->path, fil, ext);
		printf("%-44s %9lu %s\n", dpath, fp->size, dtstr);
	}
}


static void tballoc(struct fdata *tb[], short count, short size)
{
	short i;

	for (i = 0; i <= count; i++)
	{
		tb[i] = stalloc(1, size);
	}
}

static void    prmsg(void)
{
		printf("\nCopyright 1988 Programming ARTS\n");
		printf("\nUSAGE: est [/onlsd<nn>] <path>\n");
		printf("  Options: /o = list   oldest files.\n");
		printf("           /n = list   newest files.\n");
		printf("           /l = list  largest files.\n");
		printf("           /s = list smallest files.\n");
		printf("           /d = search subdirectories.\n");
		printf("           /<nn> = # of files in group to list.\n");
		printf("NOTE: if <path> is a directory, it must end with \\.\n\n");
		printf("Example: est /old5 \\*.*\n");
		printf("  Will list the 5 oldest and 5 largest files on the\n");
		printf("  current drive searching all subdirectories.\n\n");
}


short	main(short  arity, char  *param[])
{
	short			p, i;
	char			*fptr = NULL, *pp;

	if (arity < 2)
	{
		prmsg();
		exit(0);                                              
	}
	maxf = 10;
	for (p = 1; p < arity; p++) 
	{
		pp = param[p];
		if (*pp == '/' || *pp == '-')
		{
			for (pp++; *pp; pp++)
			{
				 switch (*pp)
				 {
				 case 'o' :
				 case 'O' :
					 obj[OLDX].flag = 1;
					 break;
				 case 'n' :
				 case 'N' :
					 obj[NEWX].flag = 1;
					 break;
				 case 'l' :
				 case 'L' :
					 obj[LARGEX].flag = 1;
					 break;
				 case 's' :
				 case 'S' :
					 obj[SMALLX].flag = 1;
					 break;
				 case 'd' :
				 case 'D' :
					 subds = 1;
					   break;
				 default :
					 if (isdigit(*pp)) maxf = atoi(pp);
					 else
					 {
					    prmsg();
					    exit(1);
					 }
					 while (isdigit(*(pp+1))) { pp++; }
					 if (maxf <= 0)
					 {
					 	printf("\nInvalid numeric parameter.\n");
					    exit(1);
					 }
					 break;
				 }
			}
		}
		else
			fptr = param[p];
	}
	if (fptr == NULL) fptr = "*.*";
	if ((obj[0].flag+obj[1].flag+obj[2].flag+obj[3].flag) == 0)
	{
		prmsg();	/* no criterion specified */
		exit(0);
	}
	for (i = 0; i < 4; i++)
	{
		if (obj[i].flag)
		{
			obj[i].eptr = stalloc(maxf+1, sizeof(struct fdata *));
			tballoc(obj[i].eptr, maxf, sizeof(struct fdata));
		}
	}

	_splitpath(fptr, drive, dir, fil, ext);
	_makepath(dpath, drive, dir, NULL, NULL);
	if (dpath[0] == 0) getcwd(dpath, 30);
	if (fil[0] == 0) strcpy(fil, "*.*");
	_makepath(fpath, NULL, NULL, fil, ext);

	findfiles(dpath, fpath);
	/*
	* print results
	*/
	for (i = 0; i < 4; i++)
	{
		if (obj[i].flag)
		{
			if (maxf == 1) printf("\nThe %s File:\n", obj[i].ename);
			else printf("\nThe %d %s Files:\n", maxf, obj[i].ename);
			prt(obj[i].eptr);
		}
	}	
}

