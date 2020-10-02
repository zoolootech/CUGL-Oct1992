/*
 *  program: Example2
 *  purpose: Test shell for threaded avl-tree. Program reads a file
 *           which contains a list of words, one per line.  As words
 *           are read, they are written to monitor screen and inserted
 *           into the threaded AVL tree.  After file is read, a prompt
 *           line is written to the bottom of the screen, and user is
 *           presented with choices. Additional words may be inserted,
 *           tree can be searched for a word, the tree can listed in
 *           forward or reverse order, words may be deleted from the
 *           tree, the predecessor or successor of the last found word
 *           may be listed.
 *
 *  Usage:   Example2 <filename>    where filename is the name of a
 *                                  text file which contains left-adjusted
 *                                  words, one per line.
 *
 *  Copyright (c) 1990, 1991   Bert C. Hughes
 *                             200 N. Saratoga
 *                             St.Paul, MN 55104
 *                             Compuserve 71211,577
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tavltree.h"

#if defined __TURBOC__
#include <conio.h>
#else
#define cprintf printf

int getche(void)
{
    char s[128];
    gets(s);
    return (strlen(s) ? s[0] : '\n');
}
#endif


void list(TAVL_treeptr , TAVL_nodeptr (*)(TAVL_nodeptr));

main (int argc, char *argv[])
{
    char s[128];
    FILE *fp;
    TAVL_treeptr tree;
    TAVL_nodeptr p;
    int cmpfunc();
    void *id();
    void *cpy();
    void *rdstr();

    TAVL_nodeptr (*dirfunc)(TAVL_nodeptr);

#if defined __TURBOC__
    directvideo = 0;  /* change to 1 if there is "snow" on your screen */
#endif

    /* open a file which contains a list of words, one per line */

    if (argc < 2)
        strcpy(s,"WORDLIST");
    else
        strcpy(s,argv[1]);

    if ((fp = fopen(s,"r")) == NULL) {
        cprintf("Unable to open file: %s\r\n",s);
        exit(1);
    }

    /* initialize the tree */
    if ((tree = tavl_init(cmpfunc,id,cpy,free,rdstr,malloc,free)) == NULL) {
      cprintf("\r\nError initializing tree.");
      exit(1);
    }

    while (!(fscanf(fp,"%s",s) == EOF)) {
	cprintf("%-16.14s",s);
	if (tavl_insert(tree,s,0) == NULL) {
	    cprintf("\r\nError inserting into tree.");
	    exit(1);
	}
    }

    fclose(fp);

/* Several cases of direct use of a field of TAVL_NODE below. */
/* All TAVL fields are READONLY! Only tavl-library routines */
/* alter tree & node field values. */

    p = tavl_reset(tree);

    do  {
#if !defined __TURBOC__
	printf("\n ...press selection key, then <ENTER> ...");
#endif
	cprintf("\r\n\:select:   <I>nsert   <F>ind   <D>elete   <L>ist   <S>ucc   <P>red   <Q>uit  ");
	switch (getche()) {
            case 'I': case 'i':
                        cprintf("\r\nInsert: ");
                        gets(s);
                        tavl_insert(tree,s,1);
                        break;

            case 'F': case 'f':
                        cprintf("\r\nFind: ");
                        p = tavl_find(tree,gets(s));
                        if (p != NULL)
                            cprintf("\r\nfind OK: %s\r\n",p->dataptr);
                        else
                            cprintf("\r\nNot found.\r\n");
                        break;

            case 'D': case 'd':
                        cprintf("\r\nDelete: ");
                        gets(s);
                        tavl_delete(tree,s);
                        break;

            case 'L': case 'l':
                        cprintf("\r\n<F>orward or <R>everse?  ");
                        if (getche() != 'r')
                            dirfunc = tavl_succ;
                        else
                            dirfunc = tavl_pred;
                        cprintf("\r\n");
                        list(tree,dirfunc);
                        break;

            case 'S': case 's':
                        if ((p = tavl_succ(p)) != NULL)
                            cprintf("\r\n\nSuccessor: %s\r\n",p->dataptr);
                        else
                            cprintf("\r\n\nNo more.\r\n");
                        break;

            case 'P': case 'p':
                        if ((p = tavl_pred(p)) != NULL)
                            cprintf("\r\n\nPredecessor: %s\r\n",p->dataptr);
                        else
                            cprintf("\r\n\nNo more.\r\n");
                        break;

            case 'Q': case 'q':
                        tavl_destroy(tree);
                        exit(0);

            default:
                        cprintf("\r\n");
        }
    }   while (1);
}


int cmpfunc(void *p, void *q)
/* function compares two identifiers. Required parameter of tavl_init */
{
    return strcmp(p,q);
}

void *id(void *s)
/* function returns identifier of items inserted into tree,
   which in this case are simply strings.
*/
{
    return s;
}

void *cpy(void *p)
/* create a copy of data item p - which must be a string in this example. */
{
    int size = 1 + strlen(p);
    char *s = malloc(size);
    return(memcpy(s,p,size));
}

void *rdstr(char *dest, const char *src)
{
    return((void *)strcpy(dest,src));
}

void list(TAVL_treeptr tree, TAVL_nodeptr (*f)(TAVL_nodeptr))
{
    TAVL_nodeptr p = tavl_reset(tree);
    long i = 0;
    while ((p = (*f)(p)) != NULL) {

#if defined __TURBOC__
/* TurboC users can stop the output... */

        if (kbhit()) {
            while (kbhit())
                getch(); /* remove it */
            cprintf("\r\n\nHalt?  <y/n> ");
            if (getche() == 'y') {
                cprintf("\r\n\n");
                i = -1;
                break;
            }
            cprintf("\r\n\n");
        }

#endif
	cprintf("%-16.14s",p->dataptr);
        i++;
    }
    cprintf("\r\n         >> %ld items in TAVLtree <<\r\n",i);
}

