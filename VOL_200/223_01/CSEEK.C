#include stdio.h

/*
** cseek(fd,offset,base)	by F.A.Scacchitti	3/22/84
**
** Position fd to the 128-byte record indicated by
** "offset" relative to the point indicated by "base."
** 
**     BASE     OFFSET-RELATIVE-TO
**       0      first record
**       1      current record
**       2      end of file (last record + 1)
**
** Returns NULL on success, else EOF.
*/

extern char zzmxsc;

static char zzzext, zzzrec;
static char *fcbdata;
static int tempofs, unused;

cseek(fd, offset, base) int fd, offset, base; {

  fcbdata = fd;
  switch (base) {
    case 0:  getval(offset);
             break;
    case 1:  tempofs = gettof(fcbdata);
             if(tempofs >= 8) tempofs -= (8 - zzmxsc);
             getval(tempofs + offset);
             break;
    case 2:  while(bdos(20, fd) == NULL);
             tempofs = gettof(fcbdata);
             getval(tempofs - offset);
             break;
    default: return (EOF);
    }
  
   fcbdata[12] = zzzext;   fcbdata[32] = zzzrec;

   if(fcbdata[33] != 1){
       fcbdata[33] = 0;
       if(cpmio(20,fd) == EOF);
          if(zzmxsc == 8) return(EOF);
          unused = 1024 - (zzmxsc * 128);
   }else{
      unused = 0;
   }
   *(fd + 38) = unused;
   *(fd + 36) = fd + 42;
   return (NULL);
}

getval(offset) int offset;{
  zzzext = offset / 80;
  zzzrec = offset % 80;
}

gettof(fcbdata) char *fcbdata; {
int tempofs;

   return(fcbdata[12] * 80 + fcbdata[32]);

}

