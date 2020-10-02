
/* Author: Ray Swartz
 *         P.O. Box 2528
 *         Santa Cruz, Calif. 95063
 * Last Modified: 4/28/85
 *
 * ANY USE OF THESE LIBRARY ROUTINES EITHER PERSONAL OR COMMERCIAL
 * IS ALLOWED UPON THE CONDITION THAT THE USER IDENTIFY THEM
 * AS BEING USED IN THE PROGRAM.  IDENTIFYING INFORMATION MUST
 * APPEAR IN THE PROGRAM DOCUMENTATION OR ON THE TERMINAL SCREEN.
 *
 *         #################################    
 *         # UNATTRIBUTED USE IS FORBIDDEN #
 *         #################################
 *
 */

/* Modifier: Honma Michimasa
 *	Higashi Kamagaya 2-6-54
 *	Kamagayashi, Chiba, Japan, 273-01
*/

#define NOT_FOUND   -1
#define AT_END      -2
#define YES          1
#define NO           0
#define TOP         -1   /* flag to show if top of list = rotate node */
#define END          0   /* end pointer in a node */
#define QUIT         0   /* menu options */
#define FIND         1
#define INSERT       2
#define NEXT         3
#define PREVIOUS     4
#define DELETE       5
#define FIRST        6
#define LAST         7
#define CLEAR_LINE           printf("\033[K")   /* for MS-DOS machine */
#define CLS                  printf("\033[2J")  /* for MS-DOS machine */
#define BELL                 putchar(0x07);
#define DATA_LENGTH 19  /* characters in first record of key file */
#define MAX_NODES 100000 /* maximum nodes allowed in a keyfile */

struct keyinfo {   /* Header information on each open keyfile */
    FILE *file;          /* file pointer to keyfile */
    int keylength;       /* Length of file key */
    long next_avail;     /* Next free node in tree (nbr_in_list + 1) */
    long list_head;      /* Node number at the head of the list */
    long nbr_in_list;    /* Number of (active) nodes in the tree */
};

struct node {      /* The composition of a tree-node */
    long rec_nbr;        /* The pointer to the data file record */
    long left_link;      /* The node to this one's immediate left */
    long right_link;     /* The node to this one's immediate right */
    char *key;           /* Pointer to this record's key */
    int delete_flag;     /* 1 if deleted, 0 if live */
    int balance;         /* -1 left subtree longer, 0 even, +1 right bigger */
};


#define STACK_LENGTH 50    /* length of history stacks */  

typedef struct stack {    /* tree traversal stack */
    long element[STACK_LENGTH];   /* Node number pushed onto history stack */
    int level[STACK_LENGTH];      /* Stack level of this element */
    int stack_cntr;               /* Top of stack */
} STACK;
