/*
** read.c	by F. A. Scacchitti	3/22/86
*/

#include <stdio.h>

extern int zzbuf;

static int i, n;
static char *tbuff, flag;

read(fd,buffer,cnt) int fd, cnt; char *buffer; {

   tbuff = &zzbuf;
   n=0;

	while(cnt >0){
	   if((flag = bdos(20,fd)) != NULL) {
              *(fd + 40) = flag;
	      return(n);
           }else 
	      i = 0;
	   while(i <= 127 && cnt > 0){ buffer[n] = tbuff[i];
			   i++; cnt--; n++; }
	}
	   return(n);
}

