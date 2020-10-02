/*
 *  program Example4
 *  purpose: Test shell for threaded AVL-tree. Demonstrates alternate
 *           identifier functions, alternate compare functions.
 *
 *  Usage:  EXAMPLE4 <filename>
 *
 *  Copyright (c) 1991  Bert C. Hughes
 *                      200 N. Saratoga
 *                      St.Paul, MN 55104
 *                      Compuserve 71211,577
 */

#if defined NDEBUG
     !*&^%$!!@@@()^H??||~:~:;:=+
     File "Example4.c" MUST be compiled with NDEBUG undefined,
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

typedef struct datarecord {
          long  EmpID;
          long  salary;
          char  Name[20];
        } DataRecord, *DRptr;


void *NameID(void *x)
{
    DRptr rec = x;
    return (rec->Name);
}

void *Employee(void *x)
{
    DRptr rec = x;
    return (&rec->EmpID);
}

int IDcmp(void *x1, void *x2)
{
    DRptr x = x1;
    DRptr y = x2;
    return (x->EmpID > y->EmpID ? -1 : (x->EmpID < y->EmpID ? +1 : 0));
}


void *copy_rec(const void *x)
{
    DRptr p = malloc(sizeof(DataRecord));
    if (p)
        memcpy(p,x,sizeof(DataRecord));
    return(p);
}

void *read_rec(void *dest, const void *src)
{
    return(memcpy(dest,src,sizeof(DataRecord)));
}

main (int argc, char *argv[])
{
    TAVL_treeptr tree1, *tree2;
    TAVL_nodeptr p;
    DataRecord InpR, *Dta;

    int (*cmp)() = strcmp;  /* use C library function "strcmp" to compare */
                            /* DataRecord.Name identifiers */

    int (*numcmp)() = IDcmp; /* Employee Idenitifier */

    FILE *fp;

#if defined __TURBOC__
    directvideo = 0;   /* change to 1 if there is "snow" on your monitor */
#endif

    if (argc < 2)
	assert(fp = fopen("EMPLDATA","r"));
    else
	assert(fp = fopen(argv[1],"r"));

    assert((tree1 = tavl_init(cmp,NameID,copy_rec,free,read_rec,malloc,free)));
    assert((tree2 = tavl_init(numcmp,Employee,copy_rec,free,read_rec,malloc,free)));

    cprintf("Input will be echoed...\r\n");

    while (fscanf(fp,"%ld %ld %s",&InpR.EmpID,&InpR.salary,InpR.Name) != EOF) {
	/* echo input */
	cprintf("%-10.ld %-10ld %+20s\r\n",InpR.EmpID,InpR.salary,InpR.Name);
	/* insert into trees */
	if (tavl_insert(tree1,&InpR,0) == NULL) {
	    cprintf("\r\nOut of memory!");
	    exit(1);
	}
	if (tavl_insert(tree2,&InpR,0) == NULL) {
	    cprintf("\r\nOut of memory!");
	    exit(1);
	}
    } /* end while - input loop */

    fclose(fp);

    pause();

    cprintf("Listed by NAME\r\n");

    p = tavl_reset(tree1);

    while (p = tavl_succ(p)) {
	Dta = p->dataptr;
	cprintf("%ld    %-23.20s   %ld\r\n",Dta->EmpID,Dta->Name,Dta->salary);
    }

    pause();

    cprintf("Listed by Employee_ID (descending)\r\n");

    p = tavl_reset(tree2);

    while (p = tavl_succ(p)) {
	Dta = p->dataptr;
	cprintf("%ld    %-23.20s   %ld\r\n",Dta->EmpID,Dta->Name,Dta->salary);
    }
}
