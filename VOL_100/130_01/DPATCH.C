/*
	Disk Patcher

	written for BDS "C" v1.3, 1.44 by
	Rick Hollinbeck
	Box C
	Norwood, CO  81423
*/

#include <a:bdscio.h>
#include <diskio.dcl>

#define SD_MAXTRK 76
#define SD_MAXSEC 26
#define DD_MAXTRK 76
#define DD_MAXSEC 52
#define DS_MAXTRK 153
#define DS_MAXSEC 52

int maxtrk, maxsec;
struct dskadr dadr;
int trk, sec;
char dbuf[128];
char cmd, ans[132];
int ncmd,modloc,modval;

main()
{
 puts("Disk Patch Utility");
 puts(" v1.2\n");

getdsk:
 puts("Enter Drive (A:, B:): ");
 gets(ans);
 bios(SELDSK, toupper(ans[0]) - 'A');

 printf("\nEnter type of disk [SD, DD, DS]: ");
 gets(ans);
 if (strcmp(ans, "SD") == 0) {
	maxtrk = SD_MAXTRK;
	maxsec = SD_MAXSEC;
 }
 else if (strcmp(ans, "DD") == 0) {
	maxtrk = DD_MAXTRK;
	maxsec = DD_MAXSEC;
 }
 else if (strcmp(ans, "DS") == 0) {
	maxtrk = DS_MAXTRK;
	maxsec = DS_MAXSEC;
 }
 else {
	puts("\nTry Again.");
	goto getdsk;
 }

  /* Sector select loop */

sec_select:
 while (1)
 {
   printf("\nEnter track, sector: ");
   gets(ans);

trksec:
   if (sscanf(ans,"%d,%d",&trk,&sec) != 2) goto sec_select;
   dadr.track = trk;
   dadr.sector = sec;

rdnxt:
   puts(CLEARS);
   printf("\nTrack %u, Sector %u\n\n", dadr.track, dadr.sector);
   diskio(READ, &dadr, &dbuf, 1, maxtrk, maxsec);

   dmpbuf(dbuf);

/* command loop */

   while (1)
    {
     puts("\nCommand [M xx,yy; W; D; E]: ");
     gets(ans);
     if (strlen(ans) == 0) goto rdnxt;
     if (isdigit(ans[0]))  /* Take a plain number pair as a new trk,sec */
	goto trksec;
     ncmd = sscanf(ans,"%c%x,%x", &cmd, &modloc, &modval);

     switch (toupper(cmd))
      {
       case 'M':

      /* Modify buffer */

	if (ncmd != 3) {
	 puts("Illegal Modify Command");
	 break;
	}
        dbuf[modloc & 0x7f] = modval;
        break;

       case 'W':

      /* Write the buffer */

        dec_dskadr(&dadr, maxsec);  /* offset diskio's increment */
        diskio(WRITE, &dadr, &dbuf, 1, maxtrk, maxsec);

       case 'E':

      /* Exit to sector select level */

        goto sec_select;

       case 'D':

      /* Display dbuf */

        dmpbuf(dbuf);
	break;

       default:
	puts("What?");
   }
  }
 }
}

dmpbuf(buf)
char buf[];
{
  char i, j, *bufp, *bufp1;

  bufp = dbuf;
  for (i=0; i<8; ++i)
   {
    printf("\n%c0: ", hexd(i));
    bufp1 = bufp;
    for (j=0; j<16; ++j)
    {
     printf("%c%c ", hexd(*bufp>>4),
                     hexd(*bufp & 0x0f));
     ++bufp;
    }
    printf(" |");
    bufp = bufp1;
    for (j=0; j<16; ++j) {
	putchar( printable(*bufp) ? *bufp : '_');
	++bufp;
    }
    printf("|");
   }
}

char hexd(dbyte)
char dbyte;
{
  return
    ( ((dbyte = dbyte & 0x0f) > 9) ? (dbyte + ('A'-10)) :
                                   (dbyte + '0') );
}

char printable(chr)
char chr;
{
 if (chr & 0x80) return 0;
 if (chr < ' ') return 0;
 if (chr == 0x7f) return 0;
 return 1;
}
