/*
   Disk I/O utilities
*/

#include <diskio.dcl>


diskio(rdwt,dadrptr,bufadr,nblks,maxtrk,maxsec)
struct dskadr *dadrptr;
char *bufadr, rdwt;
int nblks;
char maxtrk, maxsec;
{
  int i;

  if (nblks == 0) return;
  if ((dadrptr->track > maxtrk) ||
   (dadrptr->sector == 0 || dadrptr->sector > maxsec))
   {
    printf("\nAttempt to read/write track %u, sector %u\n",
           dadrptr->track, dadrptr->sector);
    return;
   }
  for (i=0; i<nblks; ++i)
   {
/**    printf("read/write(%u) trk %u sec %u into %x\n", rdwt,
       trk, dadrptr->sector, bufadr); /*debug*/
**/
    if (dadrptr->track > maxtrk) break;

    bios(SETTRK,dadrptr->track);
    bios(SETSEC,dadrptr->sector);
    inc_dskadr(dadrptr,maxsec);
    bios(SETDMA,bufadr);
    bios(rdwt,2);
    bufadr += 128;
   }
}

inc_dskadr(dadrptr,maxsec)
struct dskadr *dadrptr;
char maxsec;
{
    if (++dadrptr->sector > maxsec)
     {
      dadrptr->sector = 1;
      ++dadrptr->track;
     }
}

dec_dskadr(dadrptr,maxsec)
struct dskadr *dadrptr;
char maxsec;
{
    if (--dadrptr->sector == 0)
     {
      dadrptr->sector = maxsec;
      if (--dadrptr->track == 0) dadrptr->track = 0xff;
     }
}

/*bios(fnum,bcreg,dereg)
int fnum, bcreg, dereg;
{
  /* call the bios entry point associated with fnum */

  int *bbase;

  call( *(bbase = 0x0001) + (fnum * 3),
        0,  0,  bcreg,  dereg);
}
*/
