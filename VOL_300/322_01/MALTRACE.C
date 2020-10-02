/* Code for dynamic memory allocation leak trace tool.  By Mike
   Schwartz, 3-20-87. */

#include "btree.h"

BTREE MalBtree = NULL;
int MalTraceEnbld = 0; /* Flag to enable/disable tracing of malloc/free
                           calls.  Need to turn it off sometime (e.g.,
                           during printf calls, since printf (indirectly)
                           calls malloc/free, and during
                           uninteresting/debugged initialization code) */

int MallocCallCounter = 0;
int NumPendingMallocs = 0;
int MalBrkNum = -1; /* Malloc call number at which to call MalBreak routine */
int MalBrkSize = -1; /* Malloc size at which to call MalBreak routine */
struct list {
	KEY MalAddr;
	struct list *prev;
	struct list *next;
};
struct list *ListHead = NULL, *ListTail = NULL;

char *
malloc(nbytes)
	unsigned nbytes;
{
	char *mmalloc(), *MalAddr;
	struct list *lp;
	DATUM datum, *dp;

	MalAddr = mmalloc(nbytes);
	if (MalTraceEnbld == 1) {	/* printf calls malloc/free */
		datum.key = MalAddr;
		datum.inf.MalCallNum = ++MallocCallCounter;
		datum.inf.MalSize = nbytes;
		datum.inf.MalAddr = MalAddr;
		MalBtree = Insert(datum, MalBtree);
		NumPendingMallocs++;
		dp = Search(MalAddr, MalBtree);
		/* Insert in ordered doubly linked list of mallocs */
		lp = (struct list *) mmalloc(sizeof(struct list));
		lp->MalAddr = MalAddr;
		(dp->inf).lp = lp;
		if (ListHead == NULL) {
			ListHead = lp;
			lp->prev = NULL;
		} else {
			ListTail->next = lp;
			lp->prev = ListTail;
		}
		lp->next = NULL;
		ListTail = lp;
#ifdef DEBUG_MAL_TRACE
		MalTraceEnbld = 0;	/* printf calls malloc/free */
		printf("\tmalloc(%d)=x%x\n", nbytes, MalAddr);
		fflush(stdout);
		MalTraceEnbld = 1;
#endif DEBUG_MAL_TRACE
		if (MallocCallCounter == MalBrkNum || MalBrkSize == nbytes)
			MalBreak();	/* So we can hit dbx(1) breakpoint */
	}
	return(MalAddr);
}

free(cp)
	char *cp;
{   
	struct list *lp;
	DATUM datum, *dp;

	if (MalTraceEnbld == 1) {
		dp = Search(cp, MalBtree);
		if (dp == NULL) {
			MalTraceEnbld = 0;	/* printf calls malloc/free */
			printf("\tfree(x%x): not malloc'd with MalTraceEnbld\n", cp);
			MalTraceEnbld = 1;
			UntracedFree();
			ffree(lp);
			return;
		}
		NumPendingMallocs--;

		lp = (dp->inf).lp;

		/* Delete from ordered doubly linked list of mallocs */
		if (ListHead == lp) {
			ListHead = ListHead->next;
		} else {
			(lp->prev)->next = lp->next;
		}
		if (ListTail == lp) {
			ListTail = lp->prev;
		} else {
			(lp->next)->prev = lp->prev;
		}
		ffree(lp);
		MalBtree = Delete(cp, MalBtree);

#ifdef DEBUG_MAL_TRACE
		MalTraceEnbld = 0;	/* printf calls malloc/free */
		printf("\tfree(x%x)\n", cp);
		fflush(stdout);
		MalTraceEnbld = 1;
#endif DEBUG_MAL_TRACE
	}
	ffree(cp);
}

/* Don't need to modify btree/linked list structures here, since realloc
   will call malloc/free if it needs to, which will do the right thing. */
char *
realloc(cp, nbytes)
	char *cp; 
	unsigned nbytes;
{   
	char *rrealloc(), *tmp;

	tmp=rrealloc(cp, nbytes);
#ifdef DEBUG_MAL_TRACE
	if (MalTraceEnbld) {
		MalTraceEnbld = 0;	/* printf calls malloc/free */
		printf("\trealloc(x%x, %d)=x%x\n", cp, nbytes, tmp);
		MalTraceEnbld = 1;
	}
#endif DEBUG_MAL_TRACE
	return(tmp);
}

/* If n > 0, print (at most) the first n entries of list of pending info
   (i.e., mallocs which haven't yet been free'd).  If n == 0, print all
   pending entries.  If n < 0, print (at most) the last -n pending entries */
PMal(n)
int n;
{
	struct list *lp, *Start;
	DATUM datum, *dp;
	int i;
	int MalTraceInitialVal = MalTraceEnbld;
	char *OrdSuffix();

	MalTraceEnbld = 0;	/* puts and printf call malloc/free */
	if (ListHead == NULL)
		puts("\tNo pending mallocs");
	else {
		Start = ListHead;
		if (n == 0)
			printf("\tPending mallocs (%d total):\n",
				NumPendingMallocs);
		else if (n == 1)
			printf("\tFirst pending malloc (of %d):\n",
				NumPendingMallocs);
		else if (n > 1)
			printf("\tFirst %d pending mallocs (of %d):\n",
				n, NumPendingMallocs);
		else {
			if (n == -1)
				printf("\tLast pending malloc (of %d):\n",
					NumPendingMallocs);
			else
				printf("\tLast %d pending mallocs (of %d):\n",
					-n, NumPendingMallocs);
			for (Start = ListTail, i = n + 1;
			     Start != NULL && i < 0; Start = Start->prev, i++) {
			}
			if (Start == NULL)
				Start = ListHead;
			n = -n;
		}
		for (lp = Start, i = 0; lp != NULL; lp = lp->next, i++) {
			if (n != 0 && i >= n)
				break;
			dp = Search(lp->MalAddr, MalBtree);
			printf("\t%d%s malloc(%d): x%x\n",
				(dp->inf).MalCallNum,
				OrdSuffix((dp->inf).MalCallNum),
				(dp->inf).MalSize,
				(dp->inf).MalAddr);
		}
	}
	MalTraceEnbld = MalTraceInitialVal;
	return;
}

/* Return a character string suffix for the ordinal number n */
char *
OrdSuffix(n)
int n;
{
	if ( (n % 100) > 10 && (n % 100) < 20)
		return("th");
	switch (n % 10) {
		case 0: case 4: case 5: case 6: case 7: case 8: case 9:
			return("th");
		case 1:
			return("st");
		case 2:
			return("nd");
		case 3:
			return("rd");
	}
}



/* Routines called to allow user to set dbx(1) breakpoints */

/* Breakpoint in suspected leaking malloc call */
MalBreak()
{
}

/* Breakpoint in free call made on data not malloc'd with tracing enabled */
UntracedFree()
{
}

/* Set MalBrkNum to the next malloc call, for the next iteration of tracing */
NextMal()
{
	MalBrkNum = MallocCallCounter + 1;
}
