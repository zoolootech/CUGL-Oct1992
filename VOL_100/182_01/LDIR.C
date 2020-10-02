/*	Paul Homchick 31 Aug 84
 *
 *	directory display routines from lu86
 *
 *	will compile with Computer Innovations C86, or
 *			  Digital Research C
 *
 *	v 1.03	added 'close(lbr)'s' in error return code in get_directory()
 *      v 1.04  expanded input list definitions to 128 from 30
 *		checked for wildexp error in CI C86 version
 */

#include <stdio.h>

/************************************************************************/
/*			PREPROCESSOR DEFINITIONS			*/
/************************************************************************/

#define VERNO		" 1.04"			/* VERSION number	*/
#define VERDATE		"26 Sep 84"		/* VERSION date		*/

#define ERROR		-1
#define FLAG		int

#define ACTIVE		0	/* active, non-deleted file,		*/
#define UNUSED		0xff	/* never used slot,			*/
#define DELETED		0xfe	/* deleted file slot,			*/

#define MAXNAMES	128	/* maximum number of names in cmnd line	*/
#define MAXFILES	256	/* max files per LBR file		*/
#define NOCOLS		4	/* number of colums on 80 col screen	*/
#define JBASE		1965	/* for julian date routines		*/
#define DATEADJ		4748	/* to conform to DRI julian date	*/

#define DRC		1	/* using Dig Res Compiler		*/

#ifdef DRC
#include <ctype.h>
#define BREAD		0
#define UNSIGNED
#define OPEN		openb
#else				/* else use CI C86			*/
#define OPEN		open
#define UNSIGNED	unsigned
#endif

/************************************************************************/
/*			  STRUCTURES					*/
/************************************************************************/

/* This is the structure of the LBR file directory. One per slot.	*/

struct _ludir {
	UNSIGNED char stat;	/* file status,				*/
	char name[11];	/* FCB type filename,				*/
	unsigned off;	/* offset to data, (sectors)			*/
	unsigned len;	/* length (sectors) of data ,			*/
	unsigned crc;	/* CRC checksum for member			*/
	unsigned cdate;	/* file create date, uses DRI julian dt format	*/
			/* (jan 1, 1978 = 1; july 4, 1984 =2273, etc.)	*/
	unsigned udate;	/* file update date, uses Dig Res format	*/
	unsigned ctime; /* file create time, uses MSDOS format		*/
	unsigned utime;	/* file update time, uses MSDOS format		*/
	UNSIGNED char padchar;	/* number of pad bytes in last sector	*/
	UNSIGNED char filler;	/* fill out to 32 bytes			*/
	int fill[2];	/* fill out to 32 bytes				*/
} ludir; /* (dummy ludir so we can sizeof() it) */

struct dtbl {
	struct _ludir ldir[MAXFILES];
};

struct _date {
	int	day;
	int	month;
	int	year;
	int	yearday;
};					/* date	*/

struct _tod {
	int	hr;
	int	min;
	int	sec;
};					/* time	*/

static int day_tab[2][13] = {
	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
	};

/************************************************************************/
/*			   TYPE DECLARATIONS				*/
/************************************************************************/

struct dtbl	 *dtblPtr;
struct _date	 *datePtr;
struct _tod 	 *todPtr;

char names[MAXNAMES][30];	/* list of names from command line, */
char *substr, *index();

FLAG crcflag;

int lbr;			/* LBR file handle,		*/
int num_names;			/* number of names in above list */
int num_slots;			/* number of slots in LBR file, */

unsigned updcrc(),checksum;	/* crc support for LU 3.0 compatibility */

/************************************************************************/
/*				main()					*/
/************************************************************************/

main(argc,argv)
int argc;
char *argv[];
{
int i;

#ifndef DRC
	if (wildexp(&argc, &argv) == ERROR)	/* parse ambig. names */
		abort("ldir: wildexp error.");
#endif
	++argv; --argc;
	if (argc < 1) {
		fprintf(stderr,"ldir Ver%s, %s; Paul Homchick\n",VERNO,VERDATE);
		fprintf(stderr,"Use:  ldir lbrfile ... [ >output ]\n");
		fprintf(stderr,"      displays Novosielski LBR directories.\n");
		fprintf(stderr,"examples:\tldir one.lbr\t\tldir one\n");
		fprintf(stderr,"\t\tldir *.lbr\t\tldir a:*.lbr\n");
		fprintf(stderr,"\t\tldir one two three\tldir a*.lbr >a-lbr.dir\n");
		exit(0);
	}
	num_names= argc;
	for( i= 0; i < argc; i++) {
		cpyarg(names[i],argv[i]);
		stoupper(names[i]);		/* add ".lbr" if not already there */
		if ( ((substr= index(names[i],'.')) != 0) &&
		(cmdeq(substr,".LBR")) )
			;			/* do nothing in this case */
		else {
		    if (substr) {
			fprintf(stderr,"ldir:\tError- illegal filetype for library file.\n");
			fprintf(stderr,"\tIf a filetype is supplied, it must be \".LBR\".\n");
			fprintf(stderr,"\tIf no filetype is supplied, \".LBR\" is assumed.\n");
			exit(1);
		    } else
			strcat(names[i],".LBR"); /* add .lbr type */
		}
	}
	if (argc > MAXNAMES) {
		fprintf(stderr,"ldir: Program only allows 128 input names,\n");
		fprintf(stderr,"      and %d names are present.  Please cut\n",argc);
		fprintf(stderr,"      down on size of input list.\n");
		exit(1);
	}

	dtblPtr= calloc(sizeof(struct dtbl),1);
	todPtr= calloc(sizeof(struct _tod),1);
	/* note 2000 byte insurance */
	if ( ! (datePtr= calloc((sizeof(struct _date)+2000),1)) )
		abort("ldir: CALLOC returned NULL, not enough memory.");
	list();
}

list()
{
  int i,j;
  long size, unused;
  char name[14];
  int active,deleted,empty;

    for(j= 0; j < num_names; j++) {
	size= unused= 0L;
	active= deleted= empty= 0;

	if (get_directory(names[j]) == -1)
	{
		fprintf(stderr,"ldir: Error  %s, invalid or not found\n",names[j]);
		close(lbr);
		return;
	}
	printf("\nLibrary: %s",names[j]);
	if (!(dtblPtr->ldir[0].udate))
		slist();
	else {
		printf("\nFilename       Bytes  Sectors  Index");
		printf("   --- Created ---    --- Updated ---");
	for (i= 0; i < num_slots; i++)
	{
		if (dtblPtr->ldir[i].stat == ACTIVE)
		{
			cvt_from_fcb(dtblPtr->ldir[i].name,name);
			if(!i)
				printf("\ndirectory    %7lu   %6u  %5u",
				  (long)dtblPtr->ldir[i].len * 128L,dtblPtr->ldir[i].len,
				  dtblPtr->ldir[i].off);
			else
				printf("\n%-12s %7lu   %6u  %5u",name,
				  (((long)dtblPtr->ldir[i].len * 128L)-dtblPtr->ldir[i].padchar),
				  dtblPtr->ldir[i].len,dtblPtr->ldir[i].off);
			if(dtblPtr->ldir[i].cdate) {
				pdate(dtblPtr->ldir[i].cdate);
				ptime(dtblPtr->ldir[i].ctime);
			}
			if(!(dtblPtr->ldir[i].cdate) && dtblPtr->ldir[i].udate)
				printf("%19s"," ");
			if(dtblPtr->ldir[i].udate) {
				pdate(dtblPtr->ldir[i].udate);
				ptime(dtblPtr->ldir[i].utime);
			}
		size+= (((long)dtblPtr->ldir[i].len * 128L)-dtblPtr->ldir[i].padchar);
			++active;
		}
		if (dtblPtr->ldir[i].stat == DELETED) {
			unused+= dtblPtr->ldir[i].len * 128;
			++deleted;
		}
		if (dtblPtr->ldir[i].stat == UNUSED) ++empty;
	}
	if (unused != 0L)
		printf("\n(%6lu bytes unused)",unused);
	else
		printf("\n");
	printf("\n%7lu bytes used;",size);
	printf("%5u active entries; %5u deleted; %5u empty.\n",
		active,deleted,empty);
      }
    close(lbr);
   }
}

/*
 * short list for pre version 4.0 lbrs
 */

slist()
{
long tot_siz, unused;
int i, room, ndir, deleted, empty, newline;
char name[14];

	tot_siz= unused= 0;
	ndir= deleted= empty= newline= 0;
	room = (NOCOLS); 
	printf("\n");
	while(room--)
		printf("Name      Sectors  ");
	printf("\n\n");

	room = (NOCOLS);
	for (i = 0; i < num_slots; i++) {
		if (dtblPtr->ldir[i].stat == ACTIVE) {
			newline= 0;
			ndir++;
			tot_siz += dtblPtr->ldir[i].len;
			cvt_from_fcb(dtblPtr->ldir[i].name,name);
			if (!i)
				printf("%-12s %4d  ","directory",dtblPtr->ldir[i].len);
			else
				printf("%-12s %4d  ",name,dtblPtr->ldir[i].len);
			if(--room == 0) {
				room = (NOCOLS);
				printf("\n");
				newline= 1;
			}
		}
		if (dtblPtr->ldir[i].stat == DELETED) {
			unused+= dtblPtr->ldir[i].len * 128;
			++deleted;
		}
		if (dtblPtr->ldir[i].stat == UNUSED) ++empty;
	}
	if (!newline)
		printf("\n");
	if (unused != 0L)
		printf("(%6lu bytes unused)\n",unused);
	else
		printf("\n");

	printf("%7lu bytes used;",tot_siz*128);
	printf("%5u active entries; %5u deleted; %5u empty.\n",
	    ndir,deleted,empty);
}

/* Read the directory. */

get_directory(lbrname)
char *lbrname;
{
unsigned size, checksum, dirchk;
int i, j, count;
UNSIGNED char *bp;

	lbr= OPEN(lbrname,BREAD);
	if (lbr == -1) return(-1);

	if (read(lbr,&dtblPtr->ldir[0],sizeof(ludir)) != sizeof(ludir)) {
		return(-1);
	}
	num_slots= (dtblPtr->ldir[0].len * 128) / sizeof(ludir);
	size= (num_slots - 1) * sizeof(ludir);	/* already read one, */
	if (num_slots > MAXFILES) {
		fprintf(stderr,"Directory error: %s is bad or not an LBR file\n"
		    ,lbrname);
		return(-1);
	}
	if ( ! ((dtblPtr->ldir[0].stat == 0) &&
		(!strncmp(dtblPtr->ldir[0].name,"           ",11)) &&
		(dtblPtr->ldir[0].off == 0)) ) {
		fprintf(stderr,"%s is not a Library.\n",lbrname);
		return(-1);
	}
	if (read(lbr,&dtblPtr->ldir[1],size) != size) {
		fprintf(stderr,"Directory error: is %s an LBR file?\n",lbrname);
		return(-1);
	}
	/* disable CRC if master entry is 0 */
	crcflag = !(dtblPtr->ldir[0].crc == 0);
	checksum= 0;
	count= size + sizeof(ludir);
	dirchk= dtblPtr->ldir[0].crc;
	dtblPtr->ldir[0].crc= 0;
	bp = &dtblPtr->ldir[0];			/* start of directory */
	for(j = 0;j < count;j++)	/* do crc */
		checksum = updcrc(*bp++,checksum);
	checksum= updcrc(0,updcrc(0,checksum));
	dtblPtr->ldir[0].crc= dirchk;
	if (crcflag && (checksum != dirchk)) {
		printf("Warning: Directory CRC error in %s.\007\n",lbrname);
		printf("Directory seems to have %d entries.\n",num_slots);
	}
	return(1);
}


/*
 * Convert a CP/M like filename to a normal ASCIZ name.
 * Filter out characters undersireable on MSDOS.
 */

cvt_from_fcb(inname,outname)
char *inname,*outname;
{
int i;
char c;

	for (i= 8; i != 0; i--) {		/* do name portion, */
		c= toupper(*inname);
		++inname;
		if (c != ' ') 			/* if not space, */
			*outname++= c;		/* set it, */
	}					/* do all 8 chars, */

	if (*inname != ' ') {			/* if there is an extention, */
		*outname++= '.';		/* add the dot, */
		for (i= 3; i != 0; i--) {	/* do extention, */
			c= toupper(*inname);
			++inname;
			if (c == ' ')
				break;
			*outname++= c;
		}
	}
	*outname= '\0';				/* terminate it, */
	return;
}

/* Convert a string to all upper case. */

stoupper(s)
char *s;
{
	while (*s) {
		*s= toupper(*s);
		++s;
	}
}

cpyarg(to,from)
char *to,*from;
{
char *cp;
	cp = from;
	while(*cp)
		*to++ = *cp++;
}

cmdeq(s,p)
char *s, *p;
{
	while(*p)
		if(*s++ != *p++)
			return(0);
	return(1);
}

/*
 *  convert MS-DOS time to hh:mm:ss
 */
word2hms(mstime)
register unsigned int mstime;
{
	todPtr->hr=  (mstime&0xf800) >> 11;
	todPtr->min= (mstime&0x07e0) >> 5;
	todPtr->sec= (mstime&0x001f) << 1;
}

ptime(time)
unsigned time;
{
	word2hms(time);
	printf("%02d:%02d:%02d",todPtr->hr,
				todPtr->min,
				todPtr->sec);
}

pdate(date)
unsigned date;
{
	jul2greg(date);
	printf("  %02d/%02d/%02d ",datePtr->month,
				   datePtr->day,
				   datePtr->year-1900);
}

/*
** julian to calendar date conversion
**
** (with greatful acknowledgement to Steve Passe)
*/

jul2greg(jdate)
unsigned jdate;
{
	register unsigned days, years;
	int x, leap;

	jdate += DATEADJ;
	for (years = days = 0; days < jdate; )
		days += (++years % 4 ? 365 : 366);
	days -= (years-- % 4 ? 365 : 366);
	datePtr->year = years + JBASE;
	datePtr->yearday =  (int)jdate - days;
	leap = datePtr->year % 4 == 0 && datePtr->year % 100 != 0
			|| datePtr->year % 400 == 0;
	datePtr->day = datePtr->yearday;
	for (x = 1; datePtr->day > day_tab[leap][x]; ++x)
		datePtr->day -= day_tab[leap][x];
	datePtr->month = x;
}

/*
 * update CRC
 * Copyright 1983 Computer Development Inc Beaverton Or USA
 *  All rights reserved
 */
unsigned short updcrc(c, crc)
register c;
register unsigned short crc;
{
	register count;

	for(count=8; --count>=0;) {
		if(crc & 0x8000) {
			crc <<= 1;
			crc += (((c<<=1) & 0400)  !=  0);
			crc ^= 0x1021;
		}
		else {
			crc <<= 1;
			crc += (((c<<=1) & 0400)  !=  0);
		}
	}
	return crc;	
}

#ifdef DRC

abort(s)
char *s;
{
	fprintf(stderr,"ABORT: %s\007\n",s);
	exit(1);
}
#endif
                                                                                                                  