/* ..make a global argv[] by parsing command line.. */

#include <stdio.h>
#include <dos.h>
#include <ctype.h>

#define MAXARGS 30
int argc;
extern unsigned _pspbase;
char *cl, tail[0x80], **argv, *getarg(), *argbuf[MAXARGS];

makeargv()
{
	/* ..initialize argc/argv[].. */
	argc = 1;
	argv = argbuf;
	argv[0] = "";

	/* ..get command line from DOS.. */
	_copy(PTR(tail),0x80,_pspbase,0x80);
	cl = &tail[1];
	cl[tail[0]] = '\0';

	/* ..parse arguments.. */
	while ((argv[argc++] = getarg()) != '\0') ;
	argv[argc--] = NULL;
}

char *getarg()
{
	char arg[0x80], *q;
	int i;

	while (*cl != '\0' && isspace(*cl))
		++cl;

	if (*cl == '\0')
		return NULL;

	i = 0;

	/* ..process quoted argument.. */
	if (*cl == '"')
	{
		++cl;		/* ..eat open-quote.. */
		while (*cl != '\0' && *cl != '"')
			arg[i++] = *cl++;
		if (*cl == '"')
			++cl;	/* ..eat close-quote.. */
	}
	else
	/* ..process unquoted argument.. */
		while (*cl != '\0' && !isspace(*cl))
			arg[i++] = *cl++;

	arg[i] = '\0';
	q = (char *) malloc(strlen(arg)+1);
	strcpy(q,arg);
	return q;
}
