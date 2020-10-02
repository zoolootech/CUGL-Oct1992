/*
 *  program Example3
 *  purpose: test shell for threaded AVL-tree. mainly for profiling.
 *
 *  Usage:  EXAMPLE3 <filename>
 *          file <filename> is a text file which contains a list a words,
 *          one per line and left-adjusted. The list may contain duplicated
 *          words.  Program reads the list, writes the words to the monitor
 *          and inserts them into a threaded AVL tree.  Program pauses briefly,
 *          then writes the list (with duplicates eliminated) to the monitor
 *          in order.  Program pauses briefly again, and then items are
 *          randomly deleted from the tree until it is empty. The tree is
 *          then disposed and the program halts.
 *
 *          EXAMPLE3 is just like EXAMPLE1 except for use allocation &
 *          deallocation routines other than "malloc" & "free".
 *
 *  Copyright (c) 1991  Bert C. Hughes
 *                      200 N. Saratoga
 *                      St.Paul, MN 55104
 *                      Compuserve 71211,577
 */

#if defined NDEBUG
     !*&^%$!!@@@()^H??||~:~:;:=+
     File "Example3.c" MUST be compiled with NDEBUG undefined,
     so that the "assert()" macro/function will be included.
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "tavltree.h"

#if defined __TURBOC__
#include <conio.h>
#else
#define cprintf printf
#endif

void pause(void)
{
    int c;
    cprintf("\r\nPress <ENTER> to continue...");
    while (getchar() != '\n');
    cprintf("\r\n");
}

/*  Here are dead simple memory allocation & deallocation routines,
    whose only purpose is to illustrate use of initialization function
    "tavl_init" with memory allocation routines other than the standard
    "malloc" and "free".  The TAVL library functions will use only these
    routines to allocate/deallocate memory.
*/

#define BUFFER_MAX  30000

void *alloc(size_t size)
{
    static char membuffer[BUFFER_MAX];
    static unsigned next = 0;

    if ((size + next) <= BUFFER_MAX) {
        next += size;
        return (membuffer+next-size);
    }
    else {
        fprintf(stderr,"alloc: Out of memory");
        exit(1);
    }
}

void dealloc(void *p)
{
/*dummy routine does nothing - this memory allocator is a one way street*/
}


void *ident(void *x)    /* function to return identifier of data node */
{                       /* is required by "tavl_init".  In this very */
    return x;           /* simple case, the identifier is simply what */
}                       /* is being inserted into the tree - a string */


void *cpy(const void *p)
/* create a copy of data item p - which must be a string in this example. */
{
    int size = 1 + strlen(p);
    char *s = alloc(size);
    return(memcpy(s,p,size));
}


void *rdstr(void *dest, const void *src)
{
    return((void *)strcpy(dest,src));
}

main (int argc, char *argv[])
{
    TAVL_treeptr tree;
    TAVL_nodeptr p;
    unsigned int n,i,k;
    char **name;
    char s[128];

    int (*cmp)() = strcmp;  /* use C library function "strcmp" to compare */
                            /* identifiers of nodes */
    FILE *fp;

#if defined __TURBOC__
    directvideo = 0;   /* change to 1 if there is "snow" on your monitor */
#endif

    /* open a file which contains a list of words, one per line */

    if (argc < 2)
	strcpy(s,"SHORTLST");
    else
	strcpy(s,argv[1]);

    assert((fp = fopen(s,"r")) != NULL);

    /* initialize the tree */
    assert((tree = tavl_init(cmp,ident,cpy,dealloc,rdstr,alloc,dealloc)));

    cprintf("First, \"%s\" will be read and words found inserted into the tree.\r\n",s);
    pause();

    /* build tree of words */
    while (fscanf(fp,"%s",s) != EOF)
        if (tavl_insert(tree,s,NO_REPLACE) == NULL) {
            cprintf("Out of memory!\r\n");
            exit(0);
        }

   /* check that all identifiers inserted can be found */

    cprintf("Tree is built. Next, word file will be re-read to verify that\r\nall words are in the tree and can be found.\r\n");
    pause();

    fseek(fp,0,SEEK_SET);
    cprintf("Testing 'Find'\r\n");

    while (fscanf(fp,"%s",s) != EOF) {
        cprintf("%-10.8s",s);
        assert(tavl_find(tree,s) != NULL);
    }
    cprintf("\r\n\nFind OK.\r\n");
    cprintf("Next, data items inserted into the tree will be listed in ascending order.\r\n");

    pause();

    fclose(fp);

    cprintf("In order:\r\n");
    p = tavl_reset(tree);
    n = 0;
    while ((p = tavl_succ(p)) != NULL) {
        cprintf("%-10.8s",p->dataptr);
        n++;
    }
    cprintf("\r\n\nNodes = %u\r\n",n);

    k = n;
    p = tavl_reset(tree);

    if ((name = malloc(n * sizeof(char *))) == NULL) {
        cprintf("\r\nOut of memory, can't do random deletions test.\r\n");
        cprintf("Testing count in reverse direction...\r\n");
        while (p = tavl_pred(p)) n--;
        assert(n==0);
        cprintf("Reverse direction tested & OK.\r\n");
        exit(0);
    }

    cprintf("\r\nNext: An array is filled for random deletions while passing through TAVLtree\r\nin reverse direction.  Words will not be written to output.\r\n");
    pause();

    while ((p = tavl_pred(p)) != NULL)
        *(name + --k) = p->dataptr;

    assert(k==0);

    cprintf("\r\nReverse direction tested & OK.\r\n");
    cprintf("\r\nDelete test:  words to delete will be randomly selected from the array just\r\nfilled, written to screen, and then deleted from the TAVLtree.\r\n");
    pause();

    randomize();

    cprintf("\r\nDelete test:\r\n");

    while (n) {
        i = random(n--);            /* select random identifier to delete */
        cprintf("%-10.8s",*(name + i));
        tavl_delete(tree,*(name + i));     /* delete it */
        for (; i < n; i++)                 /* shrink the names list */
            *(name + i) = *(name + i + 1);
    }

    p = tavl_reset(tree);           /* make sure the tree is empty */
    if (tavl_succ(p) != NULL) {
        cprintf("Can't happen: tree still conatains nodes - successor.\r\n");
        exit(0);
    }

    p = tavl_reset(tree);           /* make sure the tree is empty */
    if (tavl_pred(p) != NULL) {
        cprintf("Can't happen: tree still conatains nodes - predecessor.\r\n");
        exit(0);
    }

    cprintf("\r\nDelete OK.\r\n");

    tavl_destroy(tree);

    cprintf("OK - tree destroyed - bye.");
}
