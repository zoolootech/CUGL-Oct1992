
/*
** ungetc.c	by fas 8/30/84
*/

#include <stdio.h>

#define FCBSIZE 36
#define UNGOT 5

ungetc(c,fd) char c, *fd;{

	if(fd[FCBSIZE+UNGOT] != EOF) return(EOF);

	fd[FCBSIZE+UNGOT] = c;
	return(c);
}

