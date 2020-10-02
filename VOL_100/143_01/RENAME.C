/*
%CC1 $1.C -X 
%CLINK $1 -N
%DELETE	$1.CRL 
*/
/*********************************************************************
*				RENAME				     *
**********************************************************************
*		   COPYRIGHT 1983 EUGENE H. MALLORY		     *
*********************************************************************/
#include "BDSCIO.h"
main(argc,argv)

char **argv;
int argc;

  BEGIN
    char *n,string[MAXLINE];
    FILE fcb;
    if (argc ==	3)
      THEN
	if (strcmp(argv[1],argv[2]) == 0)
	    {
	    printf("RENAME: Duplicate file names,%s and	%s\n"
	    ,argv[1],argv[2]);
	    printf("RENAME: Exiting.");
	    exit(0);
	    }
	testname(argv[1]);
	testnotname(argv[2]);
	if (rename(argv[1],argv[2]) == ERROR)
	    {
	    printf("RENAME: Unable to rename file %s to	%s\n"
	    ,argv[1],argv[2]);
	    }
	else
	    {
	    printf("RENAME: File %s to %s complete\n"
	    ,argv[1],argv[2]);
	    }
      ENDIF
    else
	{
	printf("RENAME:	Usage: RENAME fromfid tofid\n");
	exit(0);
	}
  END

testname(name)
char *name;
  BEGIN
    int	fd;
    if ((fd=open(name,READ)) ==	ERROR)
      THEN
	printf("RENAME:	File %s	does not exist.\n",name);
	    printf("RENAME: Exiting.");
	    exit(0);
      ENDIF
    fabort(fd);
  END


testnotname(name)
char *name;
  BEGIN
    int	fd;
    if ((fd=open(name,READ)) !=	ERROR)
      THEN
	printf("RENAME:	File %s	already	exists.	Return to continue.\n",name);
	if (getchar() != '\n')
	  THEN
	    printf("RENAME: Exiting.");
	    exit(0);
	  ENDIF
      ENDIF
    fabort(fd);
    unlink(name);
  END
