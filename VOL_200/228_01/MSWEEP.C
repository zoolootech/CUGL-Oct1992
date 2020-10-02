/*
HEADER:         CUGXXX;
TITLE:          MS-DOS file utility;
DATE:           6-2-85;
DESCRIPTION:    MS-DOS file utility based on CP/M NSWEEP;
KEYWORDS:       File organizer, file utility;
FILENAME:       MSWEEP.C;
WARNINGS:       None;
AUTHORS:        Ferdinand Knerlich;
COMPILER:       DeSmet C;
REFERENCES:     US-DISK 1308;
ENDREF
*/
/* mswp  ver. 1.00  release prototype	 6-2-85

	 ms-dos file manipulation utility
	 based on the cp/m nsweep program

	 written by ferdinand knerlich, 6-2-85

*/


#define SP 32
#define CR 13
#define DC3 19
#define EOF -1
#define ERR -1
#define unsn unsigned

struct {
  char dta_reserved[21];
  char dta_attribute;
  short int dta_time;
  short int dta_date;
  long int dta_file_size;
  char dta_name[13];
  } DTA;

struct d_entry{
  char	  e_attribute;
  short int  e_time;
  short int  e_date;
  long int  e_file_size;
  char	  e_name[13];
  char e_tag;
  int e_num;
  } entry[500];

int entries;
int num;
int fil1,fd1,fd2;
char e_buf[40];
char f_buf[40];
char dest_buf[40];

char files[] = "*.*";
char drv[5] = " :";
char ddrv[5] = " ";
int long_format = 0;
int time_sort = 0;
int reverse_order = 0;
int single_column = 0;

int showhide = 0;
char volume[13];

unsn avail,total,sectors,bytes;

main(argc, argv)
  short int argc; char *argv[];

{
  char devarg[85];
  char fname[20];
  int c,d,tag,prpt;
  int xit,j,lncnt,def_drv;
  long int tag_tot,totu,tota;
  char fc,nc,cur_dsk;

  char tag,prpt;
  char ver;

  /* set the Data Transfer Address to the local block */
#asm
  mov	 dx, offset DTA_
  mov	 ah, 01AH      ; set disk transfer address
  int	 21H
#

  scr_clr();
  scr_rowcol(0,0);

  printf("\n* msweep file utility *  ver 1.00  6-2-85  ferdinand knerlich\n");
  printf("            [press '?' key for help : 'X' key to eXit]");

  entries = xit = num = 0;

  ver = 0;

  def_drv = (drv[0] = cur_drv()) - 'A';


  strcat(devarg,files);

  if (argc > 1)
  {
    drv[0] = toupper(argv[1][0]);
  }


  tag_tot = 0;

  login(drv[0]);

  while(!(xit))
  {

    if(entries > 0) display_e(num);
    else printf("\nno files\n");

    fc = tolower(ci()); if (!fc) nc = tolower(ci());

    switch(fc) {

      case SP: case CR : /* forward */

		if (num > entries-2) {
		  num = 0;
		  co('\n'); }
		else num++;

		break;
      case 'a': /* retag */
		for( j=0; j<=entries;j++)
		{
		  if(entry[j].e_tag == 1)
		  {   entry[j].e_tag = 2;
		      tag_tot += entry[j].e_file_size;
		      fmtebuf(j);
		      printf("\n %14s %6D retagged",
		      e_buf,
		      tag_tot);
		  }
		}
		break;

      case 'b': /* back one */

		if (num < 1) {
		  num = entries-1;
		  co('\n'); }
		else num--;

		break;

      case 'c': /* copy */

		printf("to drive ? ");
		do
		  (c=toupper(getchar()));
		while (!(c >= 'A' && c <= 'Z' ));

		ddrv[0] = c; ddrv[1] = ':'; ddrv[2] = '\0';

		fmtebuf(num);

		cope(num);

		break;

      case 'd': /* delete */
		printf(" really (y/n) ? ");
		c=toupper(getchar());

		if(c=='Y')
		{
		  fmtebuf(num);
		  del(e_buf,num);
		}
		break;

      case 'e': /* erase t/u */

		tag = 0;
		printf("\ntagged or untagged (t/u) ? ");
		  tag = toupper(getchar());

		if (tag)
		{
		  printf("\nprompt (y/n) ? ");
		    prpt = toupper(getchar());
		    if (prpt != 'N') prpt = 'Y';

		  for( j=0; j<=(entries-1);j++)
		  {
		    if(entry[j].e_tag == 2 &&  tag=='T')
		    {
			tag_tot -= entry[j].e_file_size;
			entry[j].e_tag = 1;

			fmtebuf(j);
			if (prpt == 'Y')
			{
			  printf("\ndelete %14s (y/n) ? ",e_buf);
			  c=toupper(getchar());

			  if(c=='Y')
			  {
			  printf("\ndeleting %14s",e_buf);
			  del(e_buf,j);
			  j--;
			  }
			}
			else {
			       printf("\ndeleting %14s",e_buf);
			       del(e_buf,j);
			       j--;
			     }
		    }
		    else if(entry[j].e_tag != 2 &&  tag=='U')
		    {
			fmtebuf(j);
			if (prpt == 'Y')
			{
			  printf("\ndelete %14s (y/n) ? ",e_buf);
			  c=toupper(getchar());

			  if(c=='Y')
			  {
			    printf("\ndeleting %14s",e_buf);
			    del(e_buf,j);
			    j--;
			  }
			}
			else {
			       printf("\ndeleting %14s",e_buf);
			       del(e_buf,j);
			       j--;
			     }

		     }

		  }
		}

		break;

      case 'f': /* find */

		break;
      case 'g': /* chg attributes */

		break;

      case 'l': /* log new dir */
		printf(" drive ? ");
		do
		  (c=toupper(getchar()));
		while (!(c >= 'A' && c <= 'Z' ));

		drv[0] = c ;

		login(drv[0]);

		tag_tot = 0;
		break;

      case 'm': /* mass file copy */

		printf("to drive ? ");
		do
		  (c=toupper(getchar()));
		while (!(c >= 'A' && c <= 'Z' ));

		ddrv[0] = c; ddrv[1] = ':'; ddrv[2] = '\0';

		tag = 0;
		printf("\ntagged or untagged (t/u) ? ");
		  tag = toupper(getchar());

		if (tag)
		{
		  printf("\nprompt (y/n) ? ");
		    prpt = toupper(getchar());
		    if (prpt != 'N') prpt = 'Y';

		  for( j=0; j<=(entries-1) ;j++)
		  {
		    if(entry[j].e_tag == 2 &&  tag=='T')
		    {
			tag_tot -= entry[j].e_file_size;
			entry[j].e_tag = 1;

			fmtebuf(j);
			if (prpt == 'Y')
			{
			  printf("\ncopy %14s (y/n) ? ",e_buf);
			  c=toupper(getchar());

			  if(c=='Y') cope(j);
			}
			else cope(j);
		    }
		    else if(entry[j].e_tag != 2 &&  tag=='U')
		    {
			fmtebuf(j);
			if (prpt == 'Y')
			{
			  printf("\ncopy %14s (y/n) ? ",e_buf);
			  c=toupper(getchar());

			  if(c=='Y') cope(j);
			}
			else cope(j);

		     }

		  }
		}

		break;

      case 'r': /* rename */
		if(get_f(fname))
		{
		  fmtebuf(num);
		  if(rename(e_buf,fname) != -1)
		  {
		    printf(" rename successful !\n");
		    strcpy(entry[num].e_name,fname);

		  }
		  else printf(" error, not renamed \n");
		}

		break;
      case 's': /* stats */

		cur_dsk = cur_drv();

		printf(" drive ? ");
		do
		  (c=toupper(getchar()));
		while (!(c >= 'A' && c <= 'Z' ));


		chg_drv( (int)c - 'A');

		totu = tota = avail = total = sectors = bytes = 0;

		free_drv(0);

		totu = ((long)total - (long)avail) * (long)sectors * (long)bytes;
		tota = (long)avail * (long)sectors * (long)bytes;


		printf("\n\ndrive %c: has %8D used / %8D free \n",
			   c,
			   totu,
			   tota );

		chg_drv( (int)cur_dsk - 'A' );

		break;

      case 't': /* tag file */
		if( (entry[num].e_tag==0) || (entry[num].e_tag==1) )
		{
		  entry[num].e_tag = 2;
		  tag_tot += entry[num].e_file_size;
		  printf(" %6D ",tag_tot);
		}
		else if( entry[num].e_tag==2 )
		  printf(" %6D",tag_tot);

		if (num > entries-2)
		{
		  num = 0;
		  co('\n');
		}
		else num++;

		break;

      case 'u': /* untag file */
		if( entry[num].e_tag==2 )
		{
		  entry[num].e_tag = 0;
		  tag_tot -= entry[num].e_file_size;
		  printf(" %6D ",tag_tot);
		}
		else if(( entry[num].e_tag==0 ) || ( entry[num].e_tag==1) )
		  printf(" %6D",tag_tot);

		if (num > entries-2)
		{
		  num = 0;
		  co('\n');
		}
		else num++;

		break;

      case 'v': /* view file */

		fmtebuf(num);

		if (strlen(drv)>1)
		{ strcpy(f_buf,drv);
		  strcat(f_buf,e_buf);}
		else strcpy(f_buf,e_buf);

		scr_clr();
		scr_rowcol(0,0);

		if (!(fil1 = fopen(f_buf,"r")))
		{
		  printf("\nopen error\n");
		  break;
		}
      printf("Press 'S' to freeze/unfreeze display and 'X' to quit view.\n");

		putchar('\n');putchar('\n');
		while ((c=fgetc(fil1)) != EOF && d != 'x')
		{
		  d=tolower(scr_csts());
		  if (d=='s') while(!scr_csts());
		  putchar(c);
		}
		d=' ';
		putchar('\n');
		fclose(fil1);
		break;

      case 'w': /* wildcard tag */

		break;

      case 'q':case 'x': /* exit */
		xit = 1;
		break;

      case 'y': /* set verifY on */
		ver = 1 - ver;
		ver = set_ver(ver);
		printf("\n\nverify is now: ");
		(ver ? printf("on\n") : printf("off\n"));
		break;

      case '?': /* display help file */

scr_clr();
scr_rowcol(0,0);

printf("\n\n* msweep file utility *  ver 1.00  6-2-85  ferdinand knerlich\n");
printf("\nms-dos file manipulation utility command summary\n");
printf("\nA = retAg file     ");
printf("\nB = Back one file  ");
printf("\nC = Copy one file  ");
printf("\nD = Delete one file  ");
printf("\nE = Erase (t/u) files");
printf("\nL = Login new drive");
printf("\nM = Mass copy (t/u) files");
printf("\nQ = Quit to dos");
printf("\nR = Rename one file");
printf("\nT = Tag one file (t)");
printf("\nU = Untag one file (u)");
printf("\nV = View one file ");
printf("\nX = eXit to dos");
printf("\nY = toggle verifY  ");
printf("\nSPACE or RETURN = forward one file");
printf("\n? = display this summary\n");

   }
 }
   chg_drv(def_drv);
   exit(0);
}

int find_first(ptr)
  char *ptr; {

#asm
  mov	 dx,[bp+4]	; ptr
  mov	 cx,  0FFH
  mov	 ah, 04EH
  int	 21H
  mov	 ax,0
  jc	ff_over
  inc	 ax
ff_over:
  pop	 bp
  ret
#
  }

int find_next() {

#asm
  mov	 ah, 04FH      ; find next
  int	 21H
  mov	 ax,0
  jc	fn_over
  inc	 ax
fn_over:
  pop	 bp
  ret
#
  }

int set_ver(on)
int on;
{

#asm

  mov	 dl,0
  mov	 al,[bp+4]    ; set ax to on

  mov	 ah,02EH      ; toggle verify
  int	 21H

  mov	 ah,54H
  int	 21H
#
}


int opposite(val)
  int val; {

  if (reverse_order)
    return -val;
  return val;
  }


int comp(ptr1, ptr2)
  struct d_entry *ptr1, *ptr2; {
  long *time1, *time2;
  int val;

  val = 0;
  if (time_sort) {
    time1 = &ptr1->e_time;
    time2 = &ptr2->e_time;
    if (*time1 > *time2) val = -1;
    else if (*time1 < *time2) val = 1;
    return opposite(val);
    }
  else {
    return opposite(strcmp(ptr1->e_name, ptr2->e_name, 13));
    }
  }

fmtebuf(i)
int i;
{

*e_buf = '\0';
strcat(e_buf, entry[i].e_name);
strcat(e_buf, "                   ");
e_buf[14] = '\0';

}

cur_drv()	/* GET THE CURRENT DRIVE SELECTED  "A","B", ETC... */
{
#asm
	mov	ah,19h
	int	21h
	mov	ah,0
	add	al,'A'
#
}

chg_drv(d)
int d;
{
#asm

  mov  dl,byte [bp+4]
  mov  ah,0EH
  int  21H
#
}

free_drv(device)
	int device; {

#asm
	mov		dl, byte [bp+4] 	; device
	mov		ah, 36H
	int		21H
	cmp		AX,0FFFFH
	jne		over
	mov		DX,0FFFFH			; return -1 if bad device
	pop		bp
	ret
over:
	mov		word avail_,bx
	mov		word total_,dx
	mov		word sectors_,ax
	mov		word bytes_,cx
#
	}


int del(ptr,numb)
  char *ptr;
  int numb;
{
int i;

#asm
  mov	 dx,[bp+4]	; ptr
  mov	 ah, 041H
  int	 21H
#

if(numb+1 >= entries) num--;
else{
  for(i=numb+1;i<entries; i++)
  {
   entry[i-1].e_attribute	=  entry[i].e_attribute;
   entry[i-1].e_time		=  entry[i].e_time;
   entry[i-1].e_date		=  entry[i].e_date;
   entry[i-1].e_file_size	=  entry[i].e_file_size;
   strcpy(entry[i-1].e_name,entry[i].e_name);
   entry[i-1].e_tag		=  entry[i].e_tag;
   entry[i-1].e_num		=  entry[i].e_num-1;

  }
}
  entries--;
}

get_f(name)
char name[];
{
  int i,c_cnt,flg;
  char c;

  flg = 1;
  while(flg)
  {
    puts("filename ? :");
    gets(name);

    for(i=0;(name[i]=(toupper(name[i]))) ;i++);

    if (name[0] == SP || name[0]==CR || name[0]=='\0') break;
    c_cnt = strlen(name);
    if(c_cnt>12) puts("\nfilename too long\n");
    else if(c_cnt == 12 && name[8] != '.')
	   puts("\nfilename too long\n");
    else  flg = 0;

  }
  return(1);
}

cope(numb)
int numb;
{
unsn bytes_r;
char *buf;
unsn bufsiz;

  strcpy(dest_buf,ddrv);
  strcat(dest_buf,e_buf);

  printf("\n  copying %14s to %14s ",
	  e_buf,
	  dest_buf);

  bufsiz = (unsn)_showsp() - (unsn)_memory() - 2000;
  buf = 0;
  while(!(buf = malloc(bufsiz) ))	bufsiz-=100;
  if(!buf)	printf("\nno memory for a buffer\n");
  else
  {

   fd2=0;

   if( (fd1 = fopen(e_buf,"r")) >0 )
   {
     while(bytes_r = fread(buf,1,bufsiz,fd1))
     {
       if (!fd2)
       { if( (fd2 = fopen(dest_buf,"w")) <1 )
	 {  printf("\ncan\'t create \"%s\"\n",dest_buf);
		goto endc;
	 }
       }
       if ( fwrite(buf,1,bytes_r,fd2) != bytes_r)
	 printf("\ndestination disk full\n");
     }
   }
   else printf("\ncan\'t open \"%s\"\n",e_buf);

endc:
   free(buf);

   fclose(fd2);
   fclose(fd1);
   set_dt(dest_buf,entry[numb].e_date,entry[numb].e_time);
  }

}

set_dt(ptr,dat,tim)
char	*ptr;
short int dat,tim;
{
#asm
  mov  dx,[bp+4]
  mov  al,2
  mov  ah,3dh
  int  21h
  push ax

  mov  bx,ax	     ; file handle
  mov  dx,[bp+6]     ; dx=date
  mov  cx,[bp+8]     ; cx=time
  mov  al,01H	     ; al=1 for set
  mov  ah,57H	     ; ah=57
  int  21H

  pop  bx
  mov  ah,3eh
  int  21h

#
}

login(drive)
char drive;
{
int i;
long int totu,tota;


  entries = 0;
  total = 0;

  totu = tota = avail = total = sectors = bytes = 0;

  chg_drv((int)drive - 'A');

  ld_es(files);
  qsort(&entry, entries, 25, comp);

  for(i=1; i<= (entries ); i++)
  {
    entry[i-1].e_num = i;
  }

  free_drv(0);

	totu = ((long)total - (long)avail) * (long)sectors * (long)bytes;
	tota = (long)avail * (long)sectors * (long)bytes;


  printf("\n\ndrive %c%c has %d files with %8D used / %8D free \n",
			   drv[0],drv[1],
			   i-1,
			   totu,
			   tota );

  num = 0;

}

ld_es(dev_arg)
char dev_arg[];
{

      if (!find_first(dev_arg)) return(-1);
      do {
	   if(DTA.dta_attribute & 0x08)
	     strcpy(volume,&DTA.dta_name);
	   else if(showhide || !(DTA.dta_attribute & 0x02))
	   {
	     _move(22, &DTA.dta_attribute, &entry[entries]);
	     entry[entries++].e_tag = 0;
	   }
	} while (find_next());

}

display_e(i)
int i;
{
int hour;
char access_attr,type_attr;

fmtebuf(i);

hour = (entry[i].e_time >> 11);

if (entry[i].e_attribute & 0x01)
  access_attr = '-';
else access_attr = 'w';

if (entry[i].e_attribute & 0x10)
  type_attr = 'd';
else if (entry[i].e_attribute & 0x02)
  type_attr = 'h';
else if (entry[i].e_attribute & 0x04)
  type_attr = 's';
else type_attr = '-';


printf("\n%3d. %c%c%14s %c%c%c %2d:%02d %2d-%02d-%02d %8D %c ",
	entry[i].e_num,
	drv[0],drv[1],
	e_buf,
	type_attr, 'r', access_attr,
	hour,
	((entry[i].e_time >> 5) & 0x003F),
	(entry[i].e_date >> 5) & 0x000f,
	(entry[i].e_date & 0x001f),
	((entry[i].e_date >> 9) + 1980),
	entry[i].e_file_size,
	(entry[i].e_tag ==2) ? '*' : ':');

}

