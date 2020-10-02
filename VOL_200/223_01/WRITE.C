/*
** write.c	by F. A. Scacchitti	11/24/84
*/

#include <stdio.h>

extern int zzbuf;

static int i, n;
static char *tbuff, flag;

write(fd,buffer,cnt) int fd, cnt; char *buffer; {

   tbuff = &zzbuf;
   n = fd + 33;		/* prevents fcb buffer flush */
   *n = NULL;

   n=0;
	while(cnt >0){
	   i = 0;
	   while(i <= 127 && cnt > 0){ tbuff[i] = buffer[n];
			   i++; cnt--; n++; }

	   if((flag = bdos(21,fd)) != NULL) {
	       *(fd + 40) = flag;
	       return(n);
	   }
	}
	return(n);
}

