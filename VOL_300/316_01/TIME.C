/*	time, ctime for mark williams C, version 3.0.2		*/

char *index();
char *rindex();

/* simulate time  ctime */

char *dumtim = "";

long time()
{
	return(0L);
}

char *ctime()
{
	return(dumtim);
}

char *strchr(ptr, c)
char *ptr;
int c;
{
	return( index(ptr, c) );
}

char *strrchr(ptr, c)
char *ptr;
int c;
{
	return( rindex(ptr, c) );
}
