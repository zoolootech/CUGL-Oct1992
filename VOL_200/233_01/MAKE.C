/* 
 * 	Program Name	: MAKE.C
 *	Author		: Allen Holub
 *	Implementor	: Kenji Hino
 *	Compiler	: Microsoft ver. 4.0, Lattice C ver. 3
 *	Description	: MAKE is the program that maintains programs.
 *			  MAKE takes resposibility for recompiling or linking
 *			  any source or object codes if any of them has 
 *			  updated.
 */ 
 
#include "make.h"

void main(argc, argv)
char 	**argv;
int 	argc;
{
int dependancies(), make(), err();

	if( !(Makefile = fopen(MAKEFILE, "r")) )
		err("can't open %s\n", MAKEFILE);
		
	if( !dependancies() )
		err("Nothing to make");
	else
		make( argc > 1 ? argv[1] : First);
	
} /* main end */


char	*gmem( numbytes )
int 	numbytes;
{
	/* Get numbytes from malloc. Print an error messages and 
	   abort if malloc fails, otherwise return a pointer to
	   the memory.
	*/
	
	char 	*calloc();
	char 	*p;
	int	err();
	
	if(  !( p = calloc(1, numbytes) ))
		err("Out of memory");
	return p;
} /* gmem end */

char 	**stov(str, maxvect)
char	*str;
int 	maxvect;
{
	/* "Str" is a string of words seperated from each other by
	   white space. Stov returns an argv-like array of pointers 
	   to chracter pointers, one to each word in the original
	   string. The white-space in the original string is replaced
	   with nulls. The array of pointers is null-terminated
	   array. The program is aborted of it can't get memory.
	*/
	
	char **vect, **vp;
	char *gmem();

	vp = vect = (char **) gmem( (maxvect + 1) * sizeof(str) );
	while( *str && --maxvect >= 0 )
	{
		skipwhite(str);
		*vp++ = str;
		skipnonwhite(str);
		if(*str)
			*str++ = 0;
	}
	*vp = 0;
	return(vect);
} /* stov end */

long	gtime( file )
char 	*file;
{
	/* Return the time and date for file.
	   
	  The DOS time and date are concatenated to form one
	  large number. Note that the high bit of this number
	  will be set to 1 for all dates after 2043, which will
	  cause the date comparisons done in make() to not work.
	  THIS ROUTINE IS NOT PORTABLE (because it assumes a 32 bit
	  long).
	  
	*/

	long time;
	int fd;
	int open(), err(), close();

#ifdef	MICRO_C
	struct stat buf;
	int fstat();
	int result;
#endif

#ifdef	LATTICE_C
	long getft();
#endif

	if ( (fd = open (file, O_RDONLY)) == -1 )
		return DEFTIME;

#ifdef	MICRO_C
	if ( (result = fstat (fd, &buf)) == -1 )
		err("DOS returned error from date/time request");
	time = buf.st_atime;
#endif

#ifdef	LATTICE_C
	/* LATTICE uses a different time structure from one returned by */
	/* gmtime() so that it doesn't show the same file time as one you get */
	/* by "dir" command. However, it still can tell the difference */
	/* between new and old files. */


	if ( (time = getft (fd)) == -1 )
		err("DOS returned error from date/time request");
#endif

	if ( close (fd) )
		err("DOS returned error from file close request");

	return time;
} /* gtime end */

TNODE	*makenode()
{
	/* Create a TNODE, filling it from the makefile
	   and return a pointer to it. Return NULL if there are no more
	   objects in the makefile.
	*/
	
	char 	*line, *lp;
	TNODE	*nodep;
	char *getline(), **stov(), **getblock(), *gmem();
	int	err();
	long	gtime();

	/* First, skip past any blank lines or comment lines.
	   Return  NULL if we reach the end of file
	*/
	
	do{
		if( (line = getline(MAXLINE, Makefile)) == NULL )
			return( NULL );
	} while ( *line == 0 || *line == COMMENT );
	
	/* At this point we've gotten what should be the dependancy
	   line. Position lp to point at the colon.
	*/

	for( lp  = line; *lp && *lp != ':'; lp++)
		;
		
	/* If we find the colon position, lp to point at the first
	   non-white character following the colon.
	*/
	
	if( *lp != ':' )
		err("missing ':'");	/* This will abort the program */
	else
		for( *lp++ = 0; iswhite(*lp) ; lp++)
		;
	
	/* Allocate and initialize the TNODE */
	
	nodep			= (TNODE *) gmem( sizeof(TNODE) );
	nodep->being_made	= line;
	nodep->time		= gtime( line );
	nodep->depends_on	= stov( lp, MAXDEP );
	nodep->do_this		= getblock( Makefile );
	
	return( nodep );
} /* makenode end */

int dependancies()
{
	/* Manufacture the binary tree of objects to make. First
	   is a pointer to the first target file listed in the 
	   makefile (i.e. the one to make if one isn't explicitly
	   given on the command line). Root is the tree's root pointer.
	*/
	
	TNODE	*node, *makenode();
	int 	err(), tree();

	if( node = makenode() )
	{
		First = node->being_made;
		
		if( !tree(node, &Root) )
			err("Can't insert first node into tree !!!\n");
		
		while( node = makenode() )
			if( !tree(node, &Root) )
				free(node);
			return 1;
	}
	return 0;
} /* dependancies end */


char 	*getline( maxline, fp)
int maxline;
FILE *fp;
{
	/* Get a line from the stream pointed to by fp.
	   "Maxline" is the maximum input line size ( including the
	   terminating null. A \ at the end of line is
	   recognized as a line continuation, ( the lines
	   are concatenated). Buffer space is gotten from malloc.
	   If a line is longer than maxline it is truncated (i.e.
	   all characters from the maxlineth until a \n or EOF is
	   encountered are discarded.
	   
	  Return: NULL on a malloc failure or the end of file.
	  	  A pointer to the malloced buffer on success.
	*/
	
	static char	*buf;
	register char 	*bp;
	register int 	c, lastc;
	char 		*malloc(), *strcpy();


	/* Two buffers are used, Here, we are getting a worst-case buffer
	   that will hold the longest possible line. Later on we'll copy
	   the string into a buffer that's the correct size.
	*/
	
	if( !(bp = buf = malloc(maxline)) )
		return  NULL;
	while(1)
	{
		/* Get the line from fp. Terminate after maxline
		   characters and ignore \n following a \.
		*/
		
		Inputline++; 		/* Update input line number */
		for( lastc=0; (c = fgetc(fp)) != EOF && c!='\n'; lastc = c)
			if( --maxline > 0 )
				*bp++ = c;
		if( !(c == '\n' && lastc == '\\') )
			break;
		else if(maxline > 0)	/* erase the \ */
			--bp;
	}
	*bp = 0;
		
	if( (c == EOF && bp == buf) || !(bp = malloc((bp-buf) + 1)) )
	{
		/* If EOF was the first character on the line or
		   malloc fails when we try to get a buffer, quit.
		*/
			
		free(buf);
		return( NULL );
			
	}
		
	strcpy (bp, buf);	/* Copy the worst-case buffer to the one */
				/* that is the correct size and ... */
	free ( buf );		/* free the original, worst-case buffer, */
	return ( bp );		/*  returning a pointer to the copy */
} /* getline end */
	

char 	**getblock( fp )
FILE *fp;
{
	/* Get a block from standard input. A block is a sequences of
	   lines terminates by a blank line. The block is returned as
	   an array of pointers to strings. At most MAXBLOCK lines can
	   be in a block. Leading white space is stripped.
	*/
	
	char	*p, *lines[MAXBLOCK], **blockv = lines, *gmem();
	int 	blockc  = 0;
	char 	*memcpy(), *getline(), *gmem();
	int 	err();

	do {
		if( !( p = getline(MAXLINE, Makefile) ))
			break;
			
		skipwhite(p);
		
		if( ++blockc <= MAXBLOCK )
			*blockv++ = p;
		else
			err("action too long (max = %d lines)", MAXBLOCK);
	} while(*p);
	
	/* Copy the blockv array into a safe place. Since the array
	   returned by getblock is NULL terminated, we need to 
	   increment block first.
	*/
	
	blockv = (char **) gmem( (blockc + 1) * sizeof(blockv[0]) );
	memcpy( blockv,lines, blockc * sizeof(blockv[0]) );
	blockv[blockc] = NULL;
	
	return blockv;
} /* getblock end */


int err( msg, param)
char *msg;
int param;
{
	/* Print the error message and exit the program. */
	
	fprintf(stderr, "Mk (%s line %d): ", MAKEFILE, Inputline);
	fprintf(stderr, msg, param);
	exit(1);
	
} /* err end */

int serr( msg, param )
char *msg, *param;
{
	/* same as err() except the parameter is a string pointer
	   instead pf an int.
	*/
	
	fprintf(stderr, "Mk (%s line %d): ", MAKEFILE, Inputline);
	fprintf(stderr, msg, param);
	exit(1);
} /* serr end */

int make(what)
char *what;
{	
	/* Actually do the make. the dependancy tree is descended
	   recursively and of required, the dependancies are 
	   adjusted. Return 1 if anything was done, 0 otherwise.
	*/
	
	TNODE		*snode;		/* Source file node pointer */
	TNODE		*dnode;		/* Dependant file node pointer */
	int		doaction = 0;	/* If true do the action */
	static char 	*zero = (char *)0;
	char		**linev = &zero;
	TNODE		*find();
	int		 make(), serr(), system();

#ifdef	DEBUG
	static int	recurlev = 0;	/* Recursion level */
	
	printf("make (lev %d): making <%s> \n", recurlev, what );
#endif

	if( !(snode = find(what, Root)) )
		serr("Don't know how to make <%s>\n", what);
		
	if( !*(linev = snode->depends_on) )	/* If no dependancies */
		doaction++;			/* always do the action. */
	
	for( ; *linev; linev++)		/* Process each dependancy */
	{
#ifdef DEBUG
		recurlev++;
#endif
		make( *linev );
#ifdef DEBUG
		recurlev--;
#endif

		if( !(dnode = find(*linev, Root)) )
			serr("Don't know how to make <%s>\n", *linev);
#ifdef DEBUG
		printf("make (lev %d):source   file ", recurlev);
		ptime( what, snode->time );
		printf("make (lev %d):depenadant file ", recurlev);
		ptime( *linev, dnode->time);
#endif 
		if( snode->time <= dnode->time )
		{
			/* If source node is older than (time is less than)
			   dependant node, do something. If the times are
			   equal, assume that neither file exists but that
			   the action will create them, and do the action.
			*/
			
#ifdef DEBUG
			printf("make (lev %d): %s older than %s\n",
			recurlev, what, *linev);
#endif
			doaction++;
		}
#ifdef DEBUG
		else
			printf("make (lev %d): %s younger than %s\n", recurlev,
			what, *linev );
#endif
	}
	if( doaction )
	{
#ifdef DEBUG
			printf("make (lex, %d): doing action:\n",
			recurlev, *linev, what);
#endif
			for( linev  = snode->do_this; *linev; linev++)
			{
				printf("%s\n", *linev); /* Echo action to screen */
#ifndef DEBUG
			if( system(*linev) )
				serr("Can't process <%s>\n", *linev);

#endif
			/* Change the source file's time to refelect
			   any modification.
			*/
			
			snode->time = gtime( snode->being_made );
		}
	}
#ifdef DEBUG
	printf("make (lev %d): exiting\n", recurlev );
#endif 
	return doaction;
} /* make end */


TNODE	*find( key, root )
char 	*key;
TNODE	*root;
{
	/* If key is in the tree pointed to by root, return a pointer
	   to it, else reutrn 0.
	*/
	
	register int 	notequal ;
	TNODE		*find();
	int		 strcmp();

	if( !root )
		return 0;

	if( !(notequal = strcmp(root->being_made, key)) )
		return (root);
		
	return( find(key, (notequal > 0) ? root->lnode : root->rnode) );
} /* find end */


int	tree(node, rootp)
TNODE *node, **rootp;
{
	/* If node's key is in the tree pointed to by rootp, return 0
	   else put it into the tree and return 1.
	*/
	
	register int 	notequal;
	int		tree(),strcmp();

	if( *rootp == NULL )
	{
		*rootp = node;
		return 1;
	}
	
	if ( !(notequal = strcmp( (*rootp)->being_made, node->being_made)) )
		return 0;
		
	return( tree( node, notequal > 0 ? &((*rootp)->lnode)
					: &((*rootp)->rnode)) );
} /* tree end */



#ifdef DEBUG
 	/* Debugging Routine */
 	
int	 ptime( file, time )
char *file;
long time;
 {
 	/* Print out the time and date field of a TNODE
 	   File is the file name.
 	*/

 	char *ctime();
 	
 	printf("%s; ", file);
 	printf("%s", ctime(&time));
 	printf("\n");
} /* ptime end */

pnode( node )
TNODE *node;
{
	/* Print out the tree node pointed to by "node". */
	
	char **linev;
	printf("+----------------------------------------\n");
	printf("|   node at 0x%x\n", node);
	printf("+----------------------------------------\n");
	printf("|   lnode  = 0x%x, rnode = 0x%x \n ", node->lnode, node->rnode);
	printf("|   time   = 0x%lx, =", node->time);
	printf("|   target = <%s>\n", node->being_made);
	printf("|   dependancies:\n");
	for( linev = node->depends_on; *linev; printf("|\t%s\n", *linev++));
	printf("|   actions:\n");
	for( linev = node->do_this; *linev; printf("|\t%s\n", *linev++));
	printf("+----------------------------------------\n");
} /* pnode end */

trav( root )
TNODE *root;
{
	/* Do an in-order traversal of the tree, printing the
	   node's contents as you go.
	*/
	
	if( root == NULL )
		return;
	trav( root->lnode );
	pnode( root );
	trav( root->rnode );
} /* trav end */
#endif
