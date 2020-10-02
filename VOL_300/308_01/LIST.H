

/*
	HEADER:         CUG308;
	TITLE:          Generic linked list header file;
	DESCRIPTION:    "Header file for linked list module";
	VERSION:        2.01;
	DATE:           5/6/90;
	COMPILERS:      STANDARD C;
	KEYWORDS:       list header;
	FILENAME:       LIST.H;
	SEE-ALSO:       LIST.C, LIST.DOC;
	AUTHOR:         Michael Kelly
			254 Gold St. Boston, Ma. 02127;
	COPYRIGHT:      May be used freely if authorship is acknowledged;
*/


/*
 *  prevent multiple includes
 */
#if !defined(LIST_H)
#define LIST_H

#define LIST_SLOT_INC 2     /* list table expanded by this increment */

/*
 *  global List error indicator (like errno)
 */
enum Lerror  {
    OK,                 /* no error 					 */
    NO_MEM,             /* not enough memory 				 */
    NULL_PTR,           /* NULL pointer - (usually a function argument)  */
    EMPTY_LIST,         /* empty List 					 */
    INV_SIZE,           /* invalid size - (usually itemsize == 0) 	 */
    NO_LISTS,           /* no Lists - (no Lists are active) 		 */
    RE_INIT,            /* reinitialize - (init an already active List)  */
    INV_ID,             /* invalid List id - (using a destroyed List)    */
    FATAL_ERROR         /* fatal error - (program termination suggested) */
    };

extern enum Lerror lerror;

enum Place  {
    FIRST, LAST, CURRENT };

typedef struct ENTRY  {
    size_t itemsize;
    void   *item;
}
Entry;

typedef struct LINK  {
    struct LINK	*next;
    struct LINK *prev;
    Entry 	*entry;
}
Link;

typedef struct LIST  {
    int (*add)(int id, void *item, size_t itemsize, enum Place place);
    int (*delete)(int id);
    int (*remitem)(int id, void *itembuf);
    int (*chgcompare)(int id, int (*newcompare)());
    int (*find)(int id, void *item1);
    int (*replitem)(int id, void *newitem, size_t newsize);
    int (*cmpitem)(int id, void *item1);
    int (*sort)(int id);
    int (*getitem)(int id, void *itembuf);
    size_t (*getsize)(int id);
    void *(*getptr)(int id);
    int (*first)(int id);
    int (*last)(int id);
    int (*next)(int id);
    int (*prev)(int id);
    int (*destroy)(int id);
    Link  *First, *Last, *Current;              /* Private */
    int (*compare)();   			/* Private */
    int id;                                 	/* List id : Private */
    size_t entries;                             /* items in list -- */
}						/* treat as -- */
List;                                           /* "read only" variable */

extern int initlist(List *newlist, int (*cmpfunc)());
extern int compact_list_table(void);

#endif

