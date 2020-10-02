/* 
HEADER: 	CUG
TITLE:		MTP.C - Macro Text Processor;
VERSION:	0.05;
DATE:		09/13/86;
DESCRIPTION:	"MTP is a Macro Text Processor that performs
		macro expansions of specified text patterns in
		a file. Based on the deterministic finite state
		automaton of UNIX's fgrep utility, it has the
		distinction of not requiring a symbol table for
		the list of macro expansions. The text patterns
		are identified without backtracking, and need not
		consist of one alphanumeric 'word' as with most
		other macro processors.";
KEYWORDS:	macro,text,fgrep,filter;
SYSTEM:		ANY;
FILENAME:	MTP.C;
WARNINGS:	"As you can see from the version number, MTP is
		very much in the developmental stage. This is not
		to say it is bug-ridden. What is presented here
		should be free of bugs. It does however have very
		few options at present. These will be added in
		future versions, based on user (yes - YOU!)
		feedback. It is being released as an example of
		what you can do with the Aho-Corasick algorithm
		other than FGREP (see FGREP.DOC)";
CRC:		xxxx
SEE-ALSO:	FGREP.DOC,FGREP.C;
AUTHORS:	Ian Ashdown - byHeart Software;
COMPILERS:	ANY;
REFERENCES:	;
ENDREF
*/

/*-------------------------------------------------------------*/

/* MTP.C - Macro Text Processor
 *
 * Version 0.05	      September 13th, 1986
 *
 * Copyright 1986:    Ian Ashdown
 *		      byHeart Software
 *		      620 Ballantree Road
 *		      West Vancouver, B.C.
 *		      Canada V7S 1W3
 *		      (604) 922-6148
 *
 * This program may be copied for personal, non-commercial use
 * only, provided that the above copyright notice is included in
 * all copies of the source code. Copying for any other use
 * without previously obtaining the written permission of the
 * author is prohibited.
 *
 * Notes:
 *
 * The algorithm used in this program constructs a deterministic
 * finite state automaton (FSA) for pattern matching from the sub-
 * strings, then uses the FSA to process the text string in one
 * pass. The time taken to construct the FSA is proportional to
 * the sum of the lengths of the the substrings. The number of
 * state transitions made by the FSA in processing the text
 * string is independent of the number of substrings.
 *
 * Algorithm Source:
 *
 * "Efficient String Matching: An Aid to Bibliographic Search"
 * Alfred V. Aho & Margaret J. Corasick
 * Communications of the ACM
 * pp. 333 - 340, Vol. 18 No. 6 (June '75)
 *
 * USAGE: mtp [-n] string_file <files>
 *
 * where:
 *
 *	-n	Substitutions are not nested. That is, once a
 *		translation string has been substituted for a
 *		source string, the substitution itself is not
 *		scanned for another string to substitute.
 *
 *	string_file
 *
 *		is a text file of newline-separated
 *		source-translation strings. Each line of the file
 *		comprises one string, and consists of a source
 *		string, followed by one or more '\t' (tab)
 *		characters, and a translation string. Source
 *		strings may not contain '\t' or '\0' (NULL)
 *		characters. Translation strings may not contain
 *		'\0' characters. Empty lines consisting of only a
 *		newline ('\n') are permitted. Example:
 *
 *		Replace me\twith me!
 *
 *	file	is any text file consisting of newline-separated
 *		strings.
 *
 * DIAGNOSTICS:
 *
 * Exit status is 0 unless error was encountered, in which case
 * an exit status of 2 is returned.
 *
 * BUGS:  Lines are limited to 256 characters.
 */

/*** Include Files ***/

#include <stdio.h>
#include <ctype.h>

/*** Definitions ***/

#define TRUE		   1
#define FALSE		   0
#define MAX_LINE	 257  /* Maximum number of characters */
			      /* per line plus NULL delimiter */

/* The following definition applies to the Lattice C compiler
 * (Version 2.15) for MS-DOS. The Lattice library does not have
 * the Unix function "index()". However, it does have an
 * equivalent function called "stpchr()".
 */

/* #define index stpchr */    /* Remove comment delimiters for */
			      /* Lattice C version 2.15 */

#define CMD_ERR	   0	/* Error codes */
#define OPT_ERR	   1
#define INP_ERR	   2
#define STR_ERR	   3
#define MEM_ERR	   4

/*** Typedefs ***/

typedef int BOOL;	/* Boolean flag */

/* Some C compilers, including Lattice C, do not support the
 * "void" data type. Remove the following comment delimiters for
 * such compilers.
 */

/* typedef int void; */

/*** Data Structures ***/

/* Queue element */

typedef struct queue
{
  struct state_el *st_ptr;
  struct queue *next_el;
} QUEUE;

/* Transition element */

typedef struct transition
{
  char lchar;			/* Transition character */
  struct state_el *nextst_ptr;	/* Transition state pointer */
  struct transition *next_el;
} TRANSITION;

/* FSA state element */

typedef struct state_el
{
  TRANSITION *go_ls;	/* Pointer to head of "go" list */
  TRANSITION *mv_ls;	/* Pointer to head of "move" list */
  struct state_el *fail_state;	/* "failure" transition state */
  char *out_str;	/* Terminal state message (if any) */
  int src_length;	/* Source substring length */
  int trn_length;	/* Translation substring length */
} FSA;

/*** Global Variables and Structures ***/

/* Dummy "failure" state */

FSA FAIL_STATE;

/* Define a separate data structure for State 0 of the FSA to
 * speed processing of the input while the FSA is in that state.
 * Since the Aho-Corasick algorithm only defines "go" transitions
 * for this state (one for each valid input character) and no
 * "failure" transitions or output messages, only an array of
 * "go" transition state numbers is needed. The array is accessed
 * directly, using the input character as the index.
 */

FSA *MZ[128];	/* State 0 of FSA */

/* Command-line option flags */

BOOL nflag = FALSE;  /* Substitution nesting disabled if TRUE */

/*** Main Body of Program ***/

int main(argc,argv)
int argc;
char **argv;
{
  char *temp;
  void proc_file(),
       bd_go(),
       bd_move(),
       error();

  /* Check for minimum number of command line arguments */

  if(argc < 2)
    error(CMD_ERR,NULL);

  /* Parse the command line for user-selected options */

  while(--argc && (*++argv)[0] == '-')
    for(temp = argv[0]+1; *temp != '\0'; temp++)    
      switch(toupper(*temp))
      {
	case 'N':
	  nflag = TRUE;
	  break;
	default:
	  error(OPT_ERR,NULL);
      }

  /* Check for string file argument */

  if(!argc)
    error(CMD_ERR,NULL);

  /* Build the "go" transitions. */

  bd_go(*argv++);
  argc--;

  /* Build the "failure" and "move" transitions */

  bd_move();

  /* Process each of the input files if not "stdin". */

  if(!argc)
    proc_file(NULL);
  else
    while(argc--)
      proc_file(*argv++);

  /* Return 0 to the parent process to indicate no errors. */

  exit(0);
}

/*** Functions and Procedures ***/

/* PROC_FILE() - Run the FSA on the input file "in_file" */

void proc_file(in_file)
char *in_file;
{
  char buffer[2*MAX_LINE],	/* Input string buffer */
       *nl,
       *index(),
       *fgets();
  FILE *in_fd,
       *fopen();
  void run_fsa(),
       error();

  if(in_file != NULL)	/* A file was specified as the input */
  {
    if(!(in_fd = fopen(in_file,"r")))
      error(INP_ERR,in_file);
  }
  else
    in_fd = stdin;

  /* Read in a line at a time for processing */

  while(fgets(buffer,MAX_LINE,in_fd))
  {
    if(nl = index(buffer,'\n'))  /* Remove newline */
      *nl = '\0';
    run_fsa(buffer);	/* Run the FSA on string "buffer" */
    puts(buffer);	/* Output the translated string */
  }
  if(in_file != NULL)
    fclose(in_fd);
}

/* RUN_FSA() - Run the finite state automaton with string
 *	       "target" as input.
 */

void run_fsa(target)
register char *target;
{
  register FSA *st_ptr;
  char tail[MAX_LINE];
  FSA *go(),
      *move();

  st_ptr = NULL;	/* Initialize FSA */

  /* Process the next input character in the target string */

  while(*target)
  {
    st_ptr = move(st_ptr,*target);

    /* Substitute translation string for source string */

    if(st_ptr && st_ptr->out_str)  /* Terminal state? */
    {
      /* Longest possible substring match? */

      if(!move(st_ptr,*(target+1)))
      {
	/* Substitute translation for source in target string */

	strcpy(tail,st_ptr->out_str);
	strcat(tail,target+1);
	target -= (st_ptr->src_length - 1);
	strcpy(target,tail);
	if(nflag == TRUE)  /* Nesting disabled? */
	  target += st_ptr->trn_length;
      }
    }
    target++;
  }
}

/* GO() - Process "litchar" and return a pointer to the FSA's
 *	  corresponding "go" transition state. If the character
 *	  is not in the FSA state's "go" transition list, then
 *	  return a pointer to FAIL_STATE.
 */

FSA *go(st_ptr,litchar)
FSA *st_ptr;
register char litchar;
{
  register TRANSITION *current;

  /* If State 0, then access separate State 0 data structure of
   * the FSA. Note that there are no failure states defined for
   * any input to FSA State 0.
   */

  if(!st_ptr)
    return MZ[litchar];
  else
  {
    /* Point to the head of the linked list of "go" transitions
     * associated with the state.
     */

    current = st_ptr->go_ls;

    /* Transverse the list looking for a match to the input
     * character.
     */

    while(current)
    {
      if(current->lchar == litchar)
	break;
      current = current->next_el;
    }

    /* Null value for "current" indicates end of list was reached
     * without having found match to input character.
     */

    return current ? current->nextst_ptr : &FAIL_STATE;
  }
}

/* MOVE() - Process "litchar" and return a pointer to the FSA's
 *	    corresponding "move" transition state.
 */

FSA *move(st_ptr,litchar)
FSA *st_ptr;
register char litchar;
{
  register TRANSITION *current;

  /* If State 0, then access separate State 0 data structure of
   * the FSA.
   */

  if(!st_ptr)
    return MZ[litchar];
  else
  {
    /* Point to the head of the linked list of "move" transitions
     * associated with the state.
     */

    current = st_ptr->mv_ls;

    /* Transverse the list looking for a match to the input
     * character.
     */

    while(current)
    {
      if(current->lchar == litchar)
	break;
      current = current->next_el;
    }

    /* Null value for "current" indicates end of list was reached
     * without having found match to input character. The
     * returned pointer is then to State 0.
     */

    return current ? current->nextst_ptr : NULL;
  }
}

/* BD_GO() - Build the "go" transitions for each state from the
 *	     string "str".
 */ 

void bd_go(str)
char *str;
{
  register char litchar;
  char *nl,
       inp_buffer[MAX_LINE],	/* Input string buffer */
       src_buffer[MAX_LINE],	/* Source string buffer */
       trn_buffer[MAX_LINE],	/* Translation string buffer */
       *inp_ptr,		/* Input buffer pointer */
       *src_ptr,		/* Source buffer pointer */
       *trn_ptr,		/* Translation buffer pointer */
       *fgets();
  FILE *str_fd,
       *fopen();
  void error(),
       enter();

  /* Initialize FSA State 0 "go" transition array so that every
   * invocation of "go()" with "state" = 0 initially returns a
   * pointer to FAIL_STATE.
   */

  for(litchar = 1; litchar & 127; litchar++)
    MZ[litchar] = &FAIL_STATE;

  /* Get the newline-terminated strings from the file "str" one
   * at a time and enter them into the FSA.
   */

  if(!(str_fd = fopen(str,"r")))
    error(STR_ERR,str);

  while(fgets(inp_buffer,MAX_LINE,str_fd))
  {
    /* Skip blank lines */

    if(*inp_buffer == '\n')
      continue;

    /* Initialize buffer pointers */

    inp_ptr = inp_buffer;
    src_ptr = src_buffer;
    trn_ptr = trn_buffer;

    /* Copy source string to source buffer */

    while(*inp_ptr != '\t')
      *src_ptr++ = *inp_ptr++;
    *src_ptr = '\0';	/* Terminate source string */

    /* Skip tabs */

    while(*inp_ptr == '\t')
      inp_ptr++;

    /* Copy translation string to translation buffer */

    while(*inp_ptr != '\n')
      *trn_ptr++ = *inp_ptr++;
    *trn_ptr = '\0';	/* Terminate translation string */

    enter(src_buffer,trn_buffer);
  }
  fclose(str_fd);

  /* For every input character that does not lead to a defined
   * "go" transition from FSA State 0, set the corresponding
   * element in the State 0 "go" transition array to indicate
   * a "go" transition to State 0.
   */

  for(litchar = 1; litchar & 127; litchar++)
    if(MZ[litchar] == &FAIL_STATE)
      MZ[litchar] = NULL;
}

/* ENTER() - Enter a string into the FSA by running each
 *	     character in turn through the current partially-
 *	     built FSA. When a failure occurs, add the remainder
 *	     of the string to the FSA as one new state per
 *	     character. (Note that '\0' can never be a valid
 *	     character - C uses it to terminate a string.)
 */

void enter(src_str,trn_str)
char *src_str,
     *trn_str;
{
  FSA *s,
      *go(),
      *create();
  TRANSITION *current,
	     *insert();
  char *strsave();
  register char *temp;
  register FSA *st_ptr = NULL;	/* Start in FSA State 0 */
  register FSA *nextst_ptr;

  /* Run each character in turn through partially-built FSA until
   * a failure occurs.
   */  

  temp = src_str;
  while((s = go(st_ptr,*temp)) != &FAIL_STATE)
  {
    temp++;
    st_ptr = s;
  }

  /* Process the remainder of the string */

  while(*temp)
  {
    /* If a new state, then create a new state and insert
     * transition character and "go" transition in current
     * state. (Note special case of FSA State 0.)
     */

    if(!st_ptr)
      nextst_ptr = MZ[*temp++] = create();
    else if(!(current = st_ptr->go_ls))
    {
      nextst_ptr = create();
      st_ptr->go_ls = insert(nextst_ptr,*temp++);
    }
    else
    {
      /* ... or it was the character that the FSA returned a
       * "failure" for. Find the tail of the current state's list
       * of "go" transitions, create a new state and append it
       * to the current state's "go" list.
       */

      while(current->next_el)
	current = current->next_el;
      nextst_ptr = create();
      current->next_el = insert(nextst_ptr,*temp++);
    }
    st_ptr = nextst_ptr;
  }

  /* Store lengths of source and translation strings */

  st_ptr->src_length = strlen(src_str);
  st_ptr->trn_length = strlen(trn_str);

  /* Make translation string terminal state's output message */

  st_ptr->out_str = strsave(trn_str);
}

/* INSERT() - Create a new "go" transition and return a pointer
 *	      to it.
 */

TRANSITION *insert(st_ptr,litchar)
FSA *st_ptr;
char litchar;
{
  TRANSITION *current;
  char *malloc();
  void error();
    
  if(!(current = (TRANSITION *)malloc(sizeof(TRANSITION))))
    error(MEM_ERR,NULL);
  current->lchar = litchar;
  current->nextst_ptr = st_ptr;
  current->next_el = NULL;
  return current;
}

/* CREATE() - Create an FSA state and return a pointer to it. */

FSA *create()
{
  FSA *st_ptr;
  char *malloc();
  void error();

  if(!(st_ptr = (FSA *)malloc(sizeof(FSA))))
    error(MEM_ERR,NULL);
  st_ptr->go_ls = st_ptr->mv_ls = NULL;
  st_ptr->fail_state = NULL;
  st_ptr->out_str = NULL;
  return st_ptr;
}

/* BD_MOVE() - Build the "failure" and "move" transitions for
 *	       each state from the "go" transitions.
 */

void bd_move()
{
  register char litchar;
  register FSA *r,	/* Temporary FSA state pointers */
	       *s,
	       *t;
  FSA *go(),
      *move();
  TRANSITION *current,
	     *insert();
  QUEUE *first,		/* Pointer to head of queue */
	*last;		/* Pointer to tail of queue */
  void add_queue(),
       delete_queue();

  last = first = NULL;	/* Initialize the queue of FSA states */

  /* For each input character with a "go" transition out of FSA
   * State 0, add a pointer to the "go" state to the queue. Note
   * that this will also serve as the "move" transition list for
   * State 0.
   */

  for(litchar = 1; litchar & 127; litchar++)
    if(s = go(NULL,litchar))
      add_queue(&first,&last,s);

  /* While there are still state pointers in the queue, do ... */

  while(first)
  {
    /* Remove State "r" pointer from the head of the queue. */

    r = first->st_ptr;
    delete_queue(&first);

    /* For every input to State "r" that has a "go" transition to
     * State "s", do ...
     */

    for(litchar = 1; litchar & 127; litchar++)
    {
      if((s = go(r,litchar)) != &FAIL_STATE)
      {
	/* If a defined "go" transition exists for State "r" on
	 * input "litchar", add a pointer to State "s" to the end
	 * of the queue.
	 */

	add_queue(&first,&last,s);

	/* Calculate the "failure" transition of State "s" using
	 * the following algorithm.
	 */

	t = r->fail_state;
	while(go(t,litchar) == &FAIL_STATE)
	  t = t->fail_state;
	s->fail_state = go(t,litchar);
      }
      else
      {
	/* ... otherwise set the pointer to State "s" to a
	 * pointer to the precalculated "move" transition of
	 * State "r"'s failure state on input "litchar".
	 */

	s = move(r->fail_state,litchar);
      }

      /* Add State "s" as the "move" transition for State "r" on
       * input "litchar" only if it is not State 0.
       */

      if(s)
	if(!r->mv_ls)	/* First instance of the list? */
	  current = r->mv_ls = insert(s,litchar);
	else		/* No, just another one ... */
	  current = current->next_el = insert(s,litchar);
    }
  }
}

/* ADD_QUEUE() - Add an instance to the tail of a queue */

void add_queue(head_ptr,tail_ptr,st_ptr)
QUEUE **head_ptr,
      **tail_ptr;
FSA *st_ptr;
{
  QUEUE *pq;
  char *malloc();
  void error();

  /* Allocate the necessary memory and set the variables. */

  if(!(pq = (QUEUE *)malloc(sizeof(QUEUE))))
    error(MEM_ERR,NULL);

  pq->st_ptr = st_ptr;
  pq->next_el = NULL;

  if(!*head_ptr)	/* First instance of the queue? */
    *tail_ptr = *head_ptr = pq;
  else			/* No, just another one ... */
    *tail_ptr = (*tail_ptr)->next_el = pq;
}

/* DELETE_QUEUE() - Delete an instance from the head of queue */

void delete_queue(head_ptr)
QUEUE **head_ptr;
{
  *head_ptr = (*head_ptr)->next_el;
}

/* STRSAVE() - Save a string somewhere in memory */

char *strsave(str)
char *str;
{
  int strlen();
  char *p,
       *malloc();
  void error();

  if(p = malloc(strlen(str) + 1))
    strcpy(p,str);
  else
    error(MEM_ERR,NULL);
  return p;
}

/* ERROR() - Error reporting. Returns an exit status of 2 to the
 *	     parent process.
 */

void error(n,str)
int n;
char *str;
{
  fprintf(stderr,"\007\n*** ERROR - ");
  switch(n)
  {
    case CMD_ERR:
      fprintf(stderr,"Illegal command line");
      break;
    case OPT_ERR:
      fprintf(stderr,"Illegal command line option");
      break;
    case INP_ERR:
      fprintf(stderr,"Can't open input file %s",str);
      break;
    case STR_ERR:
      fprintf(stderr,"Can't open string file %s",str);
      break;
    case MEM_ERR:
      fprintf(stderr,"Out of memory");
      break;
    default:
      break;
  }
  fprintf(stderr," ***\n\nUsage: mtp [-n]");
  fprintf(stderr," string_file <files>\n");
  exit(2);
}

/*** End of MTP.C ***/
string somewhere in memory */

char *strsave(str)
char *str;
{
  int strlen();
  char *p,
       *malloc();