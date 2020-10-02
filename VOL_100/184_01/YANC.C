/* YANC version 1.0 -- Yet Another New Catalog program */

/* Master disk catalog system */
/* Based on the original catalog programs by Ward Christensen */
/* (FMAP, UCAT, CAT, and QCAT) */
/* and incorporating revisions by Lewis Moseley, Jr. */
/* (NEWCAT and CROSSREF) */
/* Rewritten into a single C program for ease of maintainance. */
/* Menu-driven and extended for ease of use. */

/* Written by Ken Presser */
/* Converted to BDS-C by David Welch */
/* 06/15/83, Minor corrections and alterations, J.E. Byram */

/* Original C dialect...CW/C from The Code Works */
/* Current C dialect...BDS-C from BD Software */

#include "bdscio.h"

#define LISTCHAR	5	/* bdos function list char to printer */
#define RETURNVER	12	/* bdos function return version no */
#define RESETDISK	13	/* bdos function reset disk system */
#define SELDISK 	14	/* bdos function select disk */
#define SRCHFIRST	17	/* bdos function search for first */
#define SRCHNEXT	18	/* bdos function search for next */
#define DELETEFILE	19	/* bdos function delete file */
#define RENAMEFILE	23	/* bdos function rename file */
#define GETDISK 	25	/* bdos function get current disk */
#define SETDMA		26	/* bdos function set dma addr */
#define GETALLOC	27	/* bdos function get addr (alloc) */
#define GETDPB		31	/* bdos function get disk parameter *
				 * block address */
#define FCBSIZE 	16	/* cpm directory entry size */
#define NUMDRIVES	16	/* number of cpm drives */
#define DIRSIZ		256	/* max # of cpm dir entries */
#define DMADDR		0x80	/* bdos default dma addr */
#define MAXLINES	60	/* max lines before sending FF */
#define FF		0x0c	/* form feed character */
#define CR		0x0d	/* carriage return character */
#define ESC		0x1b	/* escape */

#define YES		TRUE
#define NO		FALSE
#define EOS		NO
#define BOOLEAN 	int
#define GETS_SIZE	128
#define UP		0xC1
#define DOWN		0xC2
#define HOME		0xC8

int	printer;
char	mdrive,ddrive;
char	*pfcb[DIRSIZ],volname[12],ibuffer[BUFSIZ],obuffer[BUFSIZ];
int	nfcb,cfcb,block,remsz,disksz,dirmax,linecount,itemcount;
struct	dontcat {
		char	fname[13];
		struct	dontcat *next;
		} *dcs;
struct	dontcat *head;

main()
{
	int i,cmd;
	char matchstr[GETS_SIZE];

	if (bdos(RETURNVER,0) == 0) {
		puts("Can't use this program with CP/M 1.4");
		exit();
		}
	mdrive = 'A';
	ddrive = 'B';
	printer = NO;
	while (1) {
		cmd = menu();
		switch (cmd) {
		case 1: updatedir();
			break;
		case 2: strcpy(matchstr,"*.*");
			listmast(matchstr);
			break;
		case 3: getstr(matchstr);
			listmast(matchstr);
			break;
		case 4: getstr(matchstr);
			listvol(matchstr);
			break;
		case 5: dispdir();
			break;
		case 6: initmast();
			break;
		case 7: getdefaults();
			break;
		case 8: makedirfile();
			break;
		case 9: exit();
			break;
		}
	}
}

menu()
{
	char x,y;
	char c;
	int i;

	clear();
	gotoxy(29,0);
	printf("Master Catalog System");
	gotoxy(5,2);
	printf("Defaults:  ");
	printf("Master Catalog on drive %c:",mdrive);
	printf("  Disk to Add on drive %c:",ddrive);
	gotoxy(16,3);
	printf("Printer: ");
	if (printer)
		printf("on");
	else	printf("off");
	gotoxy(16,5);
	printf("...use arrow keys to move cursor to selection...");
	gotoxy(17,7); revvid(); putchar(' '); norvid(); gotoxy(21,7);
	printf("1) Update the master catalog with above defaults");
	gotoxy(17,9); revvid(); putchar(' '); norvid(); gotoxy(21,9);
	printf("2) List the entire master catalog");
	gotoxy(17,11); revvid(); putchar(' '); norvid(); gotoxy(21,11);
	printf("3) List specific files with a match key");
	gotoxy(17,13); revvid(); putchar(' '); norvid(); gotoxy(21,13);
	printf("4) List a volume from the master catalog");
	gotoxy(17,15); revvid(); putchar(' '); norvid(); gotoxy(21,15);
	printf("5) List the disk directory");
	gotoxy(17,17); revvid(); putchar(' '); norvid(); gotoxy(21,17);
	printf("6) Initialize MAST.CAT");
	gotoxy(17,19); revvid(); putchar(' '); norvid(); gotoxy(21,19);
	printf("7) Update the defaults");
	gotoxy(17,21); revvid(); putchar(' '); norvid(); gotoxy(21,21);
	printf("8) Make directory volume label file");
	gotoxy(17,23); revvid(); putchar(' '); norvid(); gotoxy(21,23);
	printf("9) Quit this program");
	gotoxy(17,7);
	while (1) {
		c = hinchar();
		switch (c) {
		case UP:	getxy(&x,&y);
				if (y == 7) break;
				y = y - 2;
				gotoxy(x,y);
				break;
		case DOWN:	getxy(&x,&y);
				if (y == 23) break;
				y = y + 2;
				gotoxy(x,y);
				break;
		case HOME:	gotoxy(17,7);
				break;
		case CR:	getxy(&x,&y);
				clear();
				x = y/2-2;
				if ( x<1 || x>9 )
				    abort("Invalid menu response = %d"
						,x);
				return x;
				break;
		}
	}
}

updatedir()
{
	char *top;

	top = alloc(1);
	printf("\r\nInsert disk to be cataloged in %c:\r\n",ddrive);
	if (mdrive != ddrive)
		printf("Be sure the master disk is in %c:\r\n",mdrive);
	hold();
	readdir();
	if (strcmp(volname,"n/a"))
		updatemast();
	else {
		printf("No volume label file on disk\r\n");
		hold();
		}
	free(top);
}

dispdir()
{
	printf("Load disk to list in drive %c:  ",ddrive);
	hold();
	puts("\r\nReading directory...\r\n\n");
	readdir();
	listdir();
}

readdir()
{
	char fcb[FCBSIZE];
	char dmapos;
	BOOLEAN firstime;
	char *alloc;
	char *ptr,bsh;
	int i;

	bdos(RESETDISK,0);
	bdos(SELDISK,(ddrive-'A'));
	bdos(SETDMA,DMADDR);
	ptr = bdos(GETDPB,0);
	ptr += 2;
	bsh = *ptr;
	block = 1 << (bsh-3);
	ptr += 3;
	disksz = ((*ptr++)+(*ptr++)*256)*block-block;
	remsz = disksz;
	dirmax = (*ptr+(*(ptr+1)*256))+1;
	firstime = YES;
	for (i=0; i<13; i++)
		fcb[i] = '?';
	for (i=13; i<FCBSIZE; i++)
		fcb[i] = '\0';
	nfcb = 0;
	volname[0] = 'n';
	volname[1] = '/';
	volname[2] = 'a';
	volname[3] = EOS;
	while (1) {
		dmapos = bdos(firstime ? SRCHFIRST : SRCHNEXT,fcb);
		if (dmapos == 255) break;
		firstime = NO;
		remsz -= copyentry(DMADDR + 32*(dmapos%4));
		}
}

copyentry(fcb)
char *fcb;
{
	int i,j,val;
	char *ptr;

	if ((*fcb < 32) && (*(fcb+1) != '-')) {
		ptr = fcb + 1;
		for (i=0; i<11; i++) {
			*ptr = (*ptr) & 0x7f; 
			++ptr;
			}
		for (i=0; i<nfcb; i++) {
			val = comparefcb(pfcb[i],fcb);
			switch (val) {
			case -2: break;
			case -1: return copyfcb(i,fcb);
				 break;
			case 0: abort("duplicate directory entry\r\n");
				break;
			case  1: return (*(fcb+15)/(8*block)*block +
				    ((*(fcb+15)%(8*block)>0)*block));
				 break;
			case  2: for (j=nfcb++; j>i; j--)
					pfcb[j] = pfcb[j-1];
				 pfcb[i] = alloc(FCBSIZE);
				 return copyfcb(i,fcb);
				 break;
				}
			}
		i = nfcb++;
		pfcb[i] = alloc(FCBSIZE);
		return copyfcb(i,fcb);
		}
	else if ((*fcb < 32) && (*(fcb+1) == '-')) {
		j = 0;
		for (i=2; i<9; i++)
			volname[j++] = fcb[i];
		volname[j++] = ' ';
		for (i=9; i<12; i++)
			volname[j++] = fcb[i];
		volname[j] = EOS;
		return 0;
		}
	else	return 0;
}

copyfcb(ptr,fcb)
char *fcb;
int ptr;
{
	char *tfcb,*sfcb;
	int i;

	tfcb = sfcb = pfcb[ptr];
	for (i=0; i<FCBSIZE; i++)
		*tfcb++ = *fcb++;
	return (*(sfcb+15)/(8*block)*block +
		((*(sfcb+15)%(8*block)>0)*block));
}

comparefcb(fcb1,fcb2)
char *fcb1,*fcb2;
/* Compare two fcb's  -- return:			*
 *   -2 name1,user1 < name2,user2			*
 *   -1 name1,user1 = name2,user2 & extent1 < extent2;	*
 *    0 name1,user1,extent1 = name2,user2,extent2;	*
 *   +1 name1,user1 = name2,user2 & extent1 > extent2;	*
 *   +2 name1,user1 > name2,user2;			*/

{
	char s1[14],s2[14];
	int i,k,val;

	for (i=0,k=1; k<12; i++,k++) {
		s1[i] = fcb1[k];
		s2[i] = fcb2[k];
		}
	s1[i] = fcb1[0] + '0';
	s2[i] = fcb2[0] + '0';
	i++;
	s1[i] = EOS;
	s2[i] = EOS;
	val = strcmp(s1,s2);
	if (val < 0)
		return -2;
	else if (val > 0)
		return 2;
	else
		if (fcb1[12] < fcb2[12])
			return -1;
		else if (fcb1[12] > fcb2[12])
			return 1;
		else	return 0;
}

listdir()
{
	int i,j,k,size;
	char s[50],stemp[5];
	char *fcb;
	
	sprintf(s,"        Directory for Volume: %s\r\n",volname);
	printf("%s",s);
	if (printer) {
		bdos(LISTCHAR,FF);
		for (i=0; i<strlen(s); i++)
			bdos(LISTCHAR,s[i]);
		}
	for (i=0; i<nfcb; i++) {
		fcb = pfcb[i];
		k = 0;
		s[k++] = ddrive;
		s[k++] = '0' + *fcb++;
		s[k++] = ':';
		for (j=1; j<9; j++)
			if (*fcb == ' ')
				fcb++;
			else	s[k++] = *fcb++;
		if (*fcb != ' ')
			s[k++] = '.';
		for (j=9; j<12; j++)
			if (*fcb == ' ')
				fcb++;
			else	s[k++] = *fcb++;
		while (k<16)
			s[k++] = ' ';
		size = *fcb*16 + *(fcb+3)/(8*block)*block +
			((*(fcb+3)%(8*block)>0)*block);
		sprintf(stemp,"%d",size);
		for (j=0; j<strlen(stemp); j++)
			s[k++] = stemp[j];
		s[k++] = 'K';
		while (k < 21)
			s[k++] = ' ';
		s[k] = EOS;
		if ((i % 3) == 2)
			printf("%s\r\n",s);
		else	printf("%s | ",s);
		if (printer) {
			if ((i % 3) == 0)
				for (k=0; k<5; k++)
					bdos(LISTCHAR,' ');
			for (k=0; k<strlen(s); k++)
				bdos(LISTCHAR,s[k]);
			if ((i % 3) == 2) {
				bdos(LISTCHAR,'\r');
				bdos(LISTCHAR,'\n');
				}
			else {
				bdos(LISTCHAR,' ');
				bdos(LISTCHAR,'|');
				bdos(LISTCHAR,' ');
				}
			}
		}
	sprintf(s,"\r\n->Drive %c: %d files (%dk free, %dk capacity)\r\n",
		ddrive,nfcb,remsz,disksz);
	printf("%s",s);
	if (printer)
		for (i=0; i<strlen(s); i++)
			bdos(LISTCHAR,s[i]);
	hold();
}

updatemast()
{
	char fcb[33];
	char rec[GETS_SIZE],fname[13],rfname[13],rvolname[12];
	int i,ptr,size,rsize,recsize,fc,vc;

	if (mdrive == ddrive) {
		printf("Insert master catalog disk in drive %c:\r\n",
			mdrive);
		hold();
		}
	bdos(RESETDISK,0);
	bdos(SELDISK,(mdrive - 'A'));
	bdos(SETDMA,DMADDR);
	strcpy(fcb,"\1MAST    BAK\1\1\1\1");
	for (i=1; i<16; i++)
		if (fcb[i] == '\1')
			fcb[i] = '\0';
	fcb[0] = mdrive - '@';
	bdos(DELETEFILE,fcb);
	strcpy(fcb,"\1MAST    CAT\1\1\1\1\1MAST    BAK\1\1\1\1");
	for (i=1; i<32; i++)
		if (fcb[i] == '\1')
			fcb[i] = '\0';
	fcb[0] = mdrive - '@';
	if (bdos(RENAMEFILE,fcb) == 255) {
		puts("MAST.CAT not on default drive -- ");
		hold();
		return 0;
		}
	if (fopen("MAST.BAK",ibuffer) == ERROR) {
		puts("Can't reopen MAST.BAK -- ");
		hold();
		return 0;
		}
	if (fcreat("MAST.CAT",obuffer) == ERROR) {
		puts("Can't reopen MAST.CAT -- ");
		hold();
		return 0;
		}
	copydont(obuffer,ibuffer);
	cfcb = -1;
	size = bldfree(fname,remsz);
	recsize = readline(ibuffer,rec);
	if (recsize > 0)
		rsize = extract(rfname,rvolname,rec);
	while (recsize >= 0 && cfcb < nfcb) {
		fc = strcmp(fname,rfname);
		vc = strcmp(volname,rvolname);
		if (fc < 0) {
			putentry(obuffer,fname,volname,size);
			printf("ADD:%s\r\n",fname);
			++cfcb;
			if (cfcb < nfcb)
				size = bldentry(fname);
			}
		else if (fc == 0) {
			if (vc < 0) {
				putentry(obuffer,fname,volname,size);
				printf("ADD:%s\r\n",fname);
				++cfcb;
				if (cfcb < nfcb)
					size=bldentry(fname);
				}
			else if (vc == 0) {
				putentry(obuffer,fname,volname,size);
				printf("CHG:%s\r\n",fname);
				++cfcb;
				if (cfcb < nfcb)
					size=bldentry(fname);
				recsize = readline(ibuffer,rec);
				if (recsize > 0)
					rsize=extract(rfname,rvolname,
						rec);
				}
			else /* (vc > 0) */ {
				putentry(obuffer,rfname,rvolname,rsize);
				recsize = readline(ibuffer,rec);
				if (recsize > 0)
					rsize=extract(rfname,rvolname,
						rec);
				}
			}
		else /* (fc > 0) */ {
			if (vc == 0) {
				printf("DEL:%s\r\n",rfname);
				recsize = readline(ibuffer,rec);
				if (recsize > 0)
					rsize=extract(rfname,rvolname,
						rec);
				}
			else	{
				putentry(obuffer,rfname,rvolname,rsize);
				recsize = readline(ibuffer,rec);
				if (recsize > 0)
					rsize=extract(rfname,rvolname,
						rec);
				}
			}
		}
	if (recsize < 0)
		while (cfcb < nfcb) {
			putentry(obuffer,fname,volname,size);
			printf("ADD:%s\r\n",fname);
			cfcb++;
			if (cfcb < nfcb)
				size = bldentry(fname);
			}
	if (cfcb == nfcb)
		while (recsize >= 0) {
			vc = strcmp(volname,rvolname);
			if (vc == 0)
				printf("DEL:%s\r\n",rfname);
			else
				putentry(obuffer,rfname,rvolname,rsize);
			recsize = readline(ibuffer,rec);
			if (recsize > 0)
				rsize=extract(rfname,rvolname,rec);
			}
	putc(CPMEOF,obuffer);
	fflush(obuffer);
	fclose(ibuffer);
	fclose(obuffer);
	hold();
}

copydont(fdo,fdi)
FILE *fdi,*fdo;
{
	char rec[GETS_SIZE];

	head = NULL;
	if (readline(fdi,rec) < 0)
		abort("\r\nFile MAST.CAT is null\r\n");
	else if (rec[0] != '(')
		abort("\r\nNo valid exclude files\r\n");
	while (rec[strlen(rec)-1] != ')') {
		writeline(fdo,rec);
		dontadd(rec);
		if (readline(fdi,rec) < 0)
			abort("\r\nPremature End of File\r\n");
		}
	writeline(fdo,rec);
	rec[strlen(rec)-1] = EOS;
	dontadd(rec);
}

dontadd(rec)
char *rec;
{
	int i,j;
	struct dontcat *ptr,*nptr;

	i=0;
	if (rec[0] == '(')
		i = 1;
	nptr = ptr = head;
	while (nptr != NULL) {
		ptr = nptr;
		nptr = ptr->next;
		}
	if (diff(nptr,ptr) == 0)
		nptr = head = alloc(15);
	else	nptr = ptr->next = alloc(15);
	nptr->next = NULL;
	j = 0;
	while (j<8 && rec[i] != '.' && rec[i] != EOS)
		nptr->fname[j++] = rec[i++];
	while (j<8)
		nptr->fname[j++] = ' ';
	if (rec[i] == '.')
		i++;
	while (j<11 && rec[i] != EOS)
		nptr->fname[j++] = rec[i++];
	while (j<11)
		nptr->fname[j++] = ' ';
	nptr->fname[j] = EOS;
}

writeline(fd,buff)
FILE *fd;
char *buff;
{
	char c;
	char *cp;

	cp = buff;
	while ((c = *cp++) != EOS)
		putc(c,fd);
	putc('\r',fd);
	putc('\n',fd);
}

readline(fd,buff)
FILE *fd;
char *buff;
{
	char *cp;
	int c;

	for (cp = buff; (c=getc(fd)) != '\n' && c != CPMEOF; )
		if (c != '\r') *cp++ = c;
	*cp = '\0';
	if (cp != buff)
		return cp-buff;
	else if (c != CPMEOF)
		return 0;
	else	return -1;
}

bldfree(buff,n)
char *buff;
int n;
{
	char *cp;

	buff[5] = 'k';
	buff[6] = ' ';
	buff[7] = ' ';
	buff[8] = 'F';
	buff[9] = 'R';
	buff[10] = 'E';
	buff[11] = '0'; 	/* free space is always user 0 */
	buff[12] = EOS;
	cp = &buff[5];
	do {	*--cp = n%10 + '0';
		n = n/10;
		} while (n != 0);
	while (cp > buff)
		*--cp = '+';
	return n;
}

bldentry(fname)
char *fname;
{
	int i,j;
	char *fcb;
	struct dontcat *ptr;

	while (cfcb < nfcb) {
		fcb = pfcb[cfcb];
		for (j=0,i=1; i<12; j++,i++)
			fname[j] = fcb[i];
		fname[11] = EOS;
		ptr = head;
		while (ptr != NULL && strcmp(ptr,fname))
			ptr = ptr->next;
		if (ptr == NULL) {
			fname[11] = fcb[0] + '0';
			fname[12] = EOS;
			return (*(fcb+12)*16+ *(fcb+15)/(8*block)*block
				+ ((*(fcb+15)%(8*block)>0)*block));
			}
		else	cfcb++;
		}
	return 0;
}

putentry(fd,name,vol,size)
FILE *fd;
char *name,*vol;
int size;
{
	char c,s[10],*cp;

	putfcb(fd,name);
	putc(',',fd);
	putfcb(fd,vol);
	sprintf(s,",%d",name[11] - '0');
	cp = s;
	while (c = *cp++)
		putc(c,fd);
	sprintf(s,",%dK",size);
	cp = s;
	while (c = *cp++)
		putc(c,fd);
	putc('\r',fd);
	putc('\n',fd);
}

putfcb(fd,name)
FILE *fd;
char *name;
{
	int i;

	for (i=0; i<8; i++) {
		if (name[i] == ' ')
			break;
		putc(name[i],fd);
		}
	if (name[8] != ' ')
		putc('.',fd);
	for (i=8; i<11; i++) {
		if (name[i] == ' ')
			break;
		putc(name[i],fd);
		}
}

extract(name,vol,rec)
char *name,*vol,*rec;
{
	int i,j,n,size;

	i = j = 0;
	while (j<8 && rec[i] != '.' && rec[i] != ',' && rec[i] != EOS)
		name[j++] = rec[i++];
	while (j<8)
		name[j++] = ' ';
	if (rec[i] == '.')
		i++;
	while (j<11 && rec[i] != ',' && rec[i] != EOS)
		name[j++] = rec[i++];
	while (j<11)
		name[j++] = ' ';
	if (rec[i] == ',')
		i++;
	j = 0;
	while (j<8 && rec[i] != '.' && rec[i] != ',' && rec[i] != EOS)
		vol[j++] = rec[i++];
	while (j<8)
		vol[j++] = ' ';
	if (rec[i] == '.')
		i++;
	while (j<11 && rec[i] != ',' && rec[i] != EOS)
		vol[j++] = rec[i++];
	while (j<11)
		vol[j++] = ' ';
	vol[11] = EOS;
	if (rec[i] == ',')
		i++;
	n = 0;
	while (isdigit(rec[i]))
		n = n*10 + (rec[i++] - '0');
	while (!isdigit(rec[i]) && rec[i] != EOS)
		i++;
	name[11] = n + '0';
	name[12] = EOS;
	size = 0;
	while (isdigit(rec[i]))
		size = size*10 + (rec[i++] - '0');
	return size;
}

listmast(str)
char *str;
{
	int size;
	char c;
	char hstr[GETS_SIZE],rec[GETS_SIZE];
	char file[13],vol[13],nstr[13];
	char buffer[BUFSIZ];

	expandstr(nstr,str);
	strcpy(hstr,"FILES: ");
	strcat(hstr,str);
	bdos(RESETDISK,0);
	bdos(SELDISK,(mdrive - 'A'));
	bdos(SETDMA,DMADDR);
	if (fopen("MAST.CAT",buffer) == ERROR) {
		puts("Can't open MAST.CAT -- ");
		hold();
		return 0;
		}
	linecount = 99;
	itemcount = 0;
	do c=getc(buffer); while (c != ')' && c != CPMEOF);
	do c=getc(buffer); while (c != '\n' && c != CPMEOF);
	while (readline(buffer,rec) > 0) {
		size = extract(file,vol,rec);
		if (filecompare(file,nstr) == 0)
			listentry(file,vol,size,hstr);
		}
	if (printer) {
		bdos(LISTCHAR,'\r');
		bdos(LISTCHAR,'\n');
		}
	fclose(buffer);
	hold();
}

listvol(str)
char *str;
{
	int size;
	char c;
	char hstr[GETS_SIZE],rec[GETS_SIZE];
	char file[13],vol[13],nstr[13];
	char buffer[BUFSIZ];

	expandstr(nstr,str);
	strcpy(hstr,"VOLUME: ");
	strcat(hstr,str);
	bdos(RESETDISK,0);
	bdos(SELDISK,(mdrive - 'A'));
	bdos(SETDMA,DMADDR);
	if (fopen("MAST.CAT",ibuffer) == ERROR) {
		puts("Can't open MAST.CAT -- ");
		hold();
		return 0;
		}
	linecount = 99;
	itemcount = 0;
	do c=getc(ibuffer); while (c != ')' && c != CPMEOF);
	do c=getc(ibuffer); while (c != '\n' && c != CPMEOF);
	while (readline(ibuffer,rec) > 0) {
		size = extract(file,vol,rec);
		if (filecompare(vol,nstr) == 0)
			listentry(file,vol,size,hstr);
		}
	if (printer) {
		bdos(LISTCHAR,'\r');
		bdos(LISTCHAR,'\n');
		}
	fclose(ibuffer);
	hold();
}

expandstr(nstr,str)
char *nstr,*str;
{
	int i,j;

	i = j = 0;
	if (str[j] == '*') {
		j++;
		while (i<8)
			nstr[i++] = '?';
		}
	else
		while (i<8 && str[j] != '.' && str[j] != EOS)
			nstr[i++] =str[j++];
	while (i<8)
		nstr[i++] = ' ';
	if (str[j] == '.')
		j++;
	if (str[j] == '*')
		while (i<11)
			nstr[i++] = '?';
	else
		while (i<11 && str[j] != EOS)
			nstr[i++] = str[j++];
	while (i<11)
		nstr[i++] = ' ';
	nstr[11] = '?';
	nstr[12] = EOS;
}

filecompare(file,str)
char *file,*str;
{
	int i,diff;

	for (i=0; i<strlen(file); i++) {
		if (str[i] == '?')
			diff = 0;
		else
			diff = file[i] - str[i];
		if (diff)
			break;
		}
	return diff;
}

listentry(file,vol,size,str)
char *file,*vol,*str;
int size;
{
	int i,j,k;
	char s[50],stemp[5];
	char *pfile,*pvol;
	
	if (linecount > MAXLINES) {
		printf("%s\r\n\n",str);
		if (printer) {
			bdos(LISTCHAR,FF);
			for (i=0; i<5; i++)
				bdos(LISTCHAR,' ');
			for (i=0; i<strlen(str); i++)
				bdos(LISTCHAR,str[i]);
			bdos(LISTCHAR,'\r');
			bdos(LISTCHAR,'\n');
			bdos(LISTCHAR,'\n');
			}
		strcpy(s,
			"       NAME            DISK        SIZE  | ");
		printf("%s",s);
		if (printer)
			for (i=0; i<strlen(s); i++)
				bdos(LISTCHAR,s[i]);
		strcpy(s,"  NAME            DISK        SIZE\r\n\n");
		printf("%s",s);
		if (printer)
			for (i=0; i<strlen(s); i++)
				bdos(LISTCHAR,s[i]);
		linecount = 4;
		itemcount = 0;
		}
	itemcount++;
	pfile = file;
	pvol = vol;
	k = 0;
	s[k++] = file[11];
	s[k++] = ':';
	for (j=1; j<9; j++)
		if (*pfile == ' ')
			pfile++;
		else	s[k++] = *pfile++;
	if (*pfile != ' ')
		s[k++] = '.';
	for (j=9; j<12; j++)
		if (*pfile == ' ')
			pfile++;
		else	s[k++] = *pfile++;
	while (k<16)
		s[k++] = ' ';
	for (j=1; j<9; j++)
		if (*pvol == ' ')
			pvol++;
		else	s[k++] = *pvol++;
	if (*pvol != ' ')
		s[k++] = '.';
	for (j=9; j<12; j++)
		if (*pvol == ' ')
			pvol++;
		else	s[k++] = *pvol++;
	while (k<30)
		s[k++] = ' ';
	sprintf(stemp,"%d",size);
	for (j=0; j<strlen(stemp); j++)
		s[k++] = stemp[j];
	s[k++] = 'K';
	while (k < 35)
		s[k++] = ' ';
	s[k] = EOS;
	if (itemcount % 2)
		printf("     %s | ",s);
	else {
		printf("%s\r\n",s);
		linecount++;
		}
	if (printer) {
		if (itemcount % 2)
			for (i=0; i<5; i++)
				bdos(LISTCHAR,' ');
		for (i=0; i<strlen(s); i++)
			bdos(LISTCHAR,s[i]);
		if (itemcount % 2) {
			bdos(LISTCHAR,' ');
			bdos(LISTCHAR,'|');
			bdos(LISTCHAR,' ');
			}
		else {
			bdos(LISTCHAR,'\r');
			bdos(LISTCHAR,'\n');
			}
		}
}

initmast()
{
	char c,str[GETS_SIZE];
	int i,n;

	bdos(RESETDISK,0);
	bdos(SELDISK,(mdrive - 'A'));
	bdos(SETDMA,DMADDR);
	if (fopen("MAST.CAT",ibuffer) != ERROR)
		while (1) {
			printf("\r\nMAST.CAT already exists on %c:",
				mdrive);
			printf(" shall I release it? (y/n) - ");
			c = getchar();
			if (c == 'y' || c == 'Y') {
				printf("\r\n");
				fclose(ibuffer);
				break;
				}
			if (c == 'n' || c == 'N') {
				fclose(ibuffer);
				return;
				}
			}
	if (fcreat("MAST.CAT",obuffer) == ERROR)
		abort("can't build MAST.CAT");
	printf("\r\nEnter files to be EXCLUDED from the Master ");
	printf("Catalog.\r\nUse a null file name to end entry.\r\n");
	printf("There must be at least one EXCLUDE file.\r\n");
	printf("If you wish no files to be excluded then ");
	printf("enter a dummy file name.\r\n");
	putc('(',obuffer);
	n = 0;
	while (1) {
		printf("Enter EXCLUDE file - ");
		gets(str);
		if (strlen(str) == 0)
			if (n == 0) {
				printf("\r\nmust be at least one ");
				printf("EXCLUDE file\r\n");
				}
			else	break;
		else {
			if (n != 0)
				putw('\r\n',obuffer);
			n = n + 1;
			for (i=0; i<strlen(str); i++)
				putc(toupper(str[i]),obuffer);
			}
		}
	putc(')',obuffer);
	putw('\r\n',obuffer);
	putc(CPMEOF,obuffer);
	fflush(obuffer);
	fclose(obuffer);
}

getstr(s)
char *s;
{
	int i;

	puts("Enter match string using wildcards --");
	puts("\n     '?' to match a single character");
	puts("     '*' to match whole file name or type");
	printf("\n\n          Enter string - ");
	gets(s);
	for (i=0; i<strlen(s); i++)
		s[i] = toupper(s[i]);
}

makedirfile()
{
	char buf[GETS_SIZE],buf2[GETS_SIZE];
	BOOLEAN notdone;

	notdone = YES;
	while (notdone) {
		printf("Enter volume name as a cp/m file name\r\n");
		printf(" -- don't forget to use a dash as the ");
		printf("first character  ");
		gets(buf);
		if (buf[1] == ':')
			if (toupper(buf[0]) != ddrive)
				printf("invalid drive\r\n");
			else {
				strcpy(buf2,buf);
				notdone = NO;
				}
		else {
			buf2[0] = ddrive;
			buf2[1] = ':';
			buf2[2] = EOS;
			strcat(buf2,buf);
			notdone = NO;
			}
		if (!notdone)
			if (strlen(buf2) > 14) {
				printf("file name too long\r\n");
				notdone = YES;
				}
		}
	bldfile(buf2);
}

bldfile(buf)
char *buf;
{
	char str[GETS_SIZE];

	if(fcreat(buf,obuffer) == ERROR) {
		sprintf(str,"unable to build file %s\r\n",buf);
		abort(str);
		}
	fclose(obuffer);	/* an empty file has now been built */
	puts("Volume label file built");
}

getdefaults()
{
    char c;

    do {
	printf("\r\nEnter drive where Master Catalog will reside - ");
	mdrive = toupper(getchar());
    } while ( mdrive < 'A' || mdrive >= (NUMDRIVES+'@'));
    do {
	printf("\r\nEnter drive of Disk to be cataloged - ");
	ddrive = toupper(getchar());
    } while ( ddrive < 'A' || ddrive >= (NUMDRIVES+'@'));
    do {
	printf("\r\nEcho displays to the CP/M list device? (y/n) - ");
	c = toupper(getchar());
    } while (c != 'Y' && c != 'N');
    if (c == 'Y')
	printer = YES;
    else
	printer = NO;
}

hold()
{
	printf("Press <return> to continue ");
	while(bdos(6,255) != CR) /* do nothing */ ;
	printf("\r\n");
}

abort(str)
char *str;
{
	printf("%s",str);
	printf("\r\n ----- aborting -----\r\n");
	exit();
}

diff(p1,p2)
char *p1,*p2;
{
	return p1-p2;
}

#include "v200.l"
                          