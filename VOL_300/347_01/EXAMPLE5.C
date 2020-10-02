/*
 *  program Example5
 *  purpose: Test shell for threaded AVL-tree. Demonstrates alternate
 *           identifier functions, alternate compare functions.
 *
 *  Usage:  EXAMPLE5 <filename>
 *
 *  Copyright (c) 1991  Bert C. Hughes
 *                      200 N. Saratoga
 *                      St.Paul, MN 55104
 *                      Compuserve 71211,577
 */

#if defined NDEBUG
     !*&^%$!!@@@()^H??||~:~:;:=+
     File "Example5.c" MUST be compiled with NDEBUG undefined,
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

int getche(void)
{
    char s[128];
    gets(s);
    return (*s ? *s : '\n');
}

#endif

typedef struct {
          long  EmpID;
          long  salary;
          char  *Name;
        } DataRecord, *PDataRecord;
/*
typedef struct {
          long  EmpID;
          long  salary;
          char  Name[128];
        } TempDatarecord;
*/

void *NameID(void *x)
{
    PDataRecord rec = x;
    return(rec->Name);
}

void *make_rec(const void *x)
{
    PDataRecord q = x;
    PDataRecord p = malloc(sizeof(DataRecord));
    if (p) {
        p->Name = malloc(1+strlen(q->Name));
        if (p->Name) {
           strcpy(p->Name,q->Name);
           p->salary = q->salary;
           p->EmpID  = q->EmpID;
        }
        else {
           free(p);
           p = NULL;
        }
    }
    return(p);
}

void *copy_rec(void *buffer, const void *x)
{
    PDataRecord dest = buffer;
    PDataRecord src  = x;

    dest->EmpID = src->EmpID;
    dest->salary= src->salary;
    strcpy(dest->Name,src->Name);
    return(dest);
}

void free_rec(void *rec)
{
    free(((PDataRecord)rec)->Name);
    free(rec);
}

void putrec(PDataRecord rec)
{
    cprintf("%-23.20s %ld %12.ld\r\n",rec->Name,rec->EmpID,rec->salary);
}

void list(TAVL_treeptr , TAVL_nodeptr (*)(TAVL_nodeptr));

main (int argc, char *argv[])
{
    TAVL_treeptr tree;
    TAVL_nodeptr p;
    char s[128];
    TAVL_nodeptr (*dirfunc)(TAVL_nodeptr);
    int (*cmp)() = strcmp;  /* use C library function "strcmp" to compare */
                            /* DataRecord.Name identifiers */
    FILE *fp;

    DataRecord TempRec;
    if ((TempRec.Name = malloc(128)) == NULL) {
        cprintf("Out of memory.\r\n");
        exit(1);
    }


#if defined __TURBOC__
    directvideo = 0;   /* change to 1 if there is "snow" on your monitor */
#endif

    if (argc < 2)
        assert(fp = fopen("EMPLDATA","r"));
    else
        assert(fp = fopen(argv[1],"r"));

    assert((tree = tavl_init(cmp,NameID,make_rec,free_rec,copy_rec,malloc,free)));

    cprintf("Input will be echoed...\r\n");

    while (fscanf(fp,"%ld %ld %s",&TempRec.EmpID,&TempRec.salary,TempRec.Name) != EOF) {
	/* echo input */
	putrec(&TempRec);
	/* insert into trees */
	if ((p = tavl_insert(tree,&TempRec,NO_REPLACE)) == NULL) {
	    cprintf("\r\nOut of memory!");
	    exit(1);
	}
    } /* end while - input loop */

    fclose(fp);

    do {
#if !defined __TURBOC__
	printf("\n ...press selection key, then <ENTER> ...");
#endif
	cprintf("\r\n\:Select:   <I>nsert   <F>ind   <D>elete   <L>ist   <S>ucc   <P>red   <Q>uit  ");
	switch (getche()) {
	    case 'I': case 'i':
			cprintf("\r\nInsert: Name: ");
                        gets(TempRec.Name);
                        cprintf("  Employee ID: ");
                        gets(s);
                        TempRec.EmpID = atol(s);
                        cprintf("       Salary: ");
                        gets(s);
                        TempRec.salary = atol(s);
                        tavl_insert(tree,&TempRec,1);
                        break;

            case 'F': case 'f':
                        cprintf("\r\nFind: ");
                        if (p = tavl_find(tree,gets(s))) {
                            cprintf("\r\n ... OK:  ");
                            putrec(tavl_getdata(tree,p,&TempRec));
                        }
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
                        if ((p = tavl_succ(p)) != NULL) {
                            cprintf("\r\n\nSuccessor:\r\n");
                            putrec(tavl_getdata(tree,p,&TempRec));
                        }
                        else
                            cprintf("\r\n\nNo more.\r\n");
                        break;

            case 'P': case 'p':
                        if ((p = tavl_pred(p)) != NULL) {
                            cprintf("\r\n\nPredecessor:\r\n");
                            putrec(tavl_getdata(tree,p,&TempRec));
                        }
                        else
                            cprintf("\r\n\nNo more.\r\n");
                        break;

            case 'Q': case 'q':
                        tavl_destroy(tree);
                        exit(0);

            default:
                        cprintf("\r\n");
        }
    } while (1);
}


void list(TAVL_treeptr tree, TAVL_nodeptr (*f)(TAVL_nodeptr))
{
    TAVL_nodeptr p = tavl_reset(tree);
    long i = 0;
    DataRecord TempRec;
    TempRec.Name = malloc(128);
    if (TempRec.Name == NULL) {
        cprintf("Out of memory.\r\n");
        return;
    }

    cprintf("    Name            Employee ID       Salary     \r\n");
    cprintf("-------------------------------------------------\r\n");

    while ((p = (*f)(p)) != NULL) {
#if defined __TURBOC__
       /* TurboC users can stop the output */
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
        putrec(tavl_getdata(tree,p,&TempRec));
        i++;
    }
    if (i >= 0) cprintf("\r\n         >> %ld items in TAVLtree <<\r\n",i);
    free(TempRec.Name);
}
