/*
%CC1 $1.C -X -E4300
%CLINK $1 DIO  -S
%DELETE	$1.CRL 
*/
/*********************************************************************
*				SORTM				     *
**********************************************************************
*		   COPYRIGHT 1983 EUGENE H. MALLORY		     *
*********************************************************************/

#include "bdscio.h"
#include "DIO.H"


struct tnode
    BEGIN
	struct tnode *left;
	struct tnode *right;
	char text[MAXLINE];
    END;
	    
int nocase,reverse,col_pos,len;
int bot,top,dif,try,next;
int rctr,lctr;
#ifdef C80
FILE *fcb, fcb1;
#else
FILE fcb;
#endif
    
main(argc,argv)
char **argv;
int argc;

BEGIN
	
int i,j,linectr;
struct tnode *root,*space;
char line_buf[MAXLINE],fname[MAXLINE];
int ii,jj,optionerr;
char *ss;

#ifdef C80
fcb = &fcb1;
#endif

    dioinit(&argc,argv);
    
/*********************************************************************
***		  ARGUMENT PROCESSING				   ***
*********************************************************************/

	nocase = reverse = col_pos = len = 0;

	optionerr = 0;
	
	for (ii=argc-1;ii>0;ii--)
	    if (argv[ii][0] == '-')
		BEGIN
		    for	(ss = &argv[ii][1]; *ss	!= '\0';)
			BEGIN
			    switch (toupper(*ss++))
				BEGIN
				    case 'L':
					len = atoi(ss);
					break;
				    case 'U':
					nocase = 1;
					break;
				    case 'R':
					reverse	= 1;
					break;
				    case 'C':
					col_pos	= atoi(ss);
					break;
				    case 'H':
					optionerr = TRUE;
					break;
				    default:
					typef("SORTM: Illegal option %c.\n"
						,*--ss);
					ss++;
					optionerr = TRUE;
					break;
				END
			    while (isdigit(*ss)) ss++;
			END
		    for	(jj=ii;jj<(argc-1);jj++) argv[jj] = argv[jj+1];
		    argc--;
		END
	
	    
	if (optionerr) 
	    BEGIN
		typef("SORTM: Sorts files replacing original with result.\n");
		typef("Ex. B:SORTM <filelist\nLegal options are:\n");
		typef("-R   Reverse order sort.\n-U   Upper case compare.\n");
		typef("-Cn  Column to begin compare.\n");
		typef("-Ln  Length of string to	compare.\n");
		exit(0);
	    END
	    
    if (len == 0) len =	MAXLINE;
    if (col_pos	> 0) col_pos--;
    
/*********************************************************************
***		     END OF ARGUMENT PROCESSING			   ***
*********************************************************************/

    rsvstk(2000);
#ifndef	C80
    _allocp = NULL;
#endif
    
    while (!getstring(fname))
	BEGIN
	    root = NULL;
	    linectr = 0;
	    
	    fname[strlen(fname)-1] = 0;
	    if (fopen(fname,fcb) == ERROR)
		THEN
		    typef("SORTM: Unable to open file %s.",fname);
		    bios(1);
		END
	    
	    while (TRUE)
		BEGIN
		    if (!fgets(line_buf,fcb)) break;
		    linectr++;
		    space = alloc(strlen(line_buf) + 1+2+2);
		    if (space == -1)
			THEN
			    typef("SORTM: Out of space at line %d.",linectr);
			    exit(0);
			END
		    strcpy(space->text,line_buf);
		    space->left	= NULL;
		    space->right = NULL;
		    rctr = lctr	= 0;
		    install(&root,&space,&root);
		END
	    
	    fclose(fcb);
	    if (fcreat(fname,fcb) == ERROR)
		THEN
		    typef("SORTM: Unable to create file	%s.",fname);
		    bios(1);
		END
	    
	    if (root !=	NULL)
		treeprint(root);
	    
	    putc(0x1A,fcb);
	    fflush(fcb);
	    fclose(fcb);
	    printf("%s\n",fname);
	END
	
    dioflush();	
END
	    
install(nodeptr,spaceptr,rootptr)
struct tnode **nodeptr,**spaceptr,**rootptr;
BEGIN
int state;
    if (*nodeptr == NULL)
	BEGIN
	    if (rctr ==	0 && lctr > 30)
		BEGIN
		    (*spaceptr)->right = (*rootptr);
		    *rootptr = *spaceptr;
		END
	    else if (lctr == 0 && rctr > 30)
		BEGIN
		    (*spaceptr)->left =	(*rootptr);
		    *rootptr = *spaceptr;
		END
	    else
		BEGIN
		    *nodeptr = *spaceptr;
		END
	    return;
	END
    else
	BEGIN
	    state = str_comp((*nodeptr)->text,(*spaceptr)->text);
	    if (state >	0)
		BEGIN
left:			 
		    lctr++;
		    install(&((*nodeptr)->left),spaceptr,rootptr);
		    return;
		END
	    else
		BEGIN
right:			  
		    rctr++;
		    install(&((*nodeptr)->right),spaceptr,rootptr);
		    return;
		END
	END
END
		    
treeprint(nodeptr)
struct tnode *nodeptr;
BEGIN
    if (nodeptr->left != NULL)
	treeprint(nodeptr->left);

    fputs(nodeptr->text,fcb);

    if (nodeptr->right != NULL)
	treeprint(nodeptr->right);
    free(nodeptr);
END

int str_comp(s1,t1)
char *s1,*t1;
BEGIN
int i,j;

    if ((strlen(s1) <= col_pos)	&& (strlen(t1) <= col_pos)) return 0;
    if (strlen(s1)<=col_pos) 
	if (!reverse) return -1;
	else return 1;
    if (strlen(t1)<=col_pos) 
	if (!reverse) return 1;
	else return -1;
    if (nocase)
    BEGIN
	for (i=col_pos;i<col_pos+len;i++)
	    BEGIN
		if (toupper(s1[i]) != toupper(t1[i])) 
		    if (!reverse) return toupper(s1[i])-toupper(t1[i]);
			else return toupper(t1[i])-toupper(s1[i]);
		if (s1[i] == '\0')  return 0;
	    END
	return 0;
    END
    else
    BEGIN
	for (i=col_pos;i<col_pos+len;i++)
	    BEGIN
		if (s1[i] != t1[i]) 
		    if (!reverse) return s1[i] - t1[i];
			else return t1[i]-s1[i];
		if (s1[i] == '\0')  return 0;
	    END
	return 0;
    END
END
