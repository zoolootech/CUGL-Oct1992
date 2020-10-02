
#include <undelete.h>

init(disk)
char disk;
{
	char ch;
	int *ptr, *tbl;
	ptr =  1;
	badr = *ptr -3;
	tbl = call(badr+(SELDSK*3),0,0,disk);
	transtbl = *tbl;
}

translate(sec)
char sec;
{
	return call(badr+(SECTRAN*3),0,0,sec,transtbl);
}




dump(sec,trk,index)
int sec, trk, index;
{
	int count, lines;
	char *secx;
	char c;
	secx = sector[index].dir;
	printf("\nTrack %d, Sector %d\n",trk,sec);

	for(lines=0;lines<8;lines++)
	{
		printf("\n%d\t",(lines*16));
		for(count=0;count<16;count++)
			printf("%2x ",secx[(lines<<4)+count]);
		for(count=0;count<16;count++)
		{
			c = secx[(lines << 4)+count] & 0x7f;
			if (c < 0x20) putchar('.');
			else putchar(c);
		}
	}
	putchar('\n');
}

/* dump(sec,trk) */

fillarray(disk)
int disk;
{
	int dircnt;
	int track, logical;
	init(disk);
	for(dircnt=0;dircnt<DIRSIZ;dircnt++)
	{
		track = DTRACK+(dircnt/NSECS);
		logical = dircnt % NSECS;
		bios(SETTRK,track);

		bios(SETSEC,translate(logical));

		bios(SETDMA,sector[dircnt].dir);
		bios(DREAD);
		if(Dump) dump(translate(logical),track,dircnt);
	}
}

cmp(len,s1,s2)
int len; char *s1, *s2;
{
	while(len--) if((*s1 != '?') && ((*s1++ & 0x7f) != 
		 (*s2++ & 0x7f))) return 0;
	return 1;
}


findfcb(fd)    /* Given a pointer to an fcb return the nescessary indices */
char *fd;
{
	int s, c;

	fcb = fd; /* Map into structure */
	for(s=0;s<16;s++)
		for(c=0;c<4;c++)
			if(cmp(11,fcb->name,sector[s].dir[c].name) &&
			 (sector[s].dir[c].ext == fcb->ext))
			{
				marker.entry = c;
				marker.sec = s;
				marker.ptr = sector[s].dir[c];
				return 1;
			}

	return 0;   /* Directory entry not found */
}

_mvx(s)
char *s;
{
	char *p;
	p = s+2;
	while(*s++ = *p++);
}
rk) */

fillar