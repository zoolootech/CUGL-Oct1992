/* Definitions for data structures callers pass the regex library.
   Copyright (C) 1985 Free Software Foundation, Inc.
 */


#ifndef RE_NREGS
#define RE_NREGS 10
#endif

/* Number of failure points to allocate space for initially,
 when matching.  If this number is exceeded, more space is allocated,
 so it is not a hard limit.  */

#ifndef NFAILURES
#define NFAILURES		    80
#endif /* NFAILURES */

/* width of a byte in bits */

#define BYTEWIDTH		    8
#define FASTMAP_SIZE		    (1 << BYTEWIDTH)

#ifndef SIGN_EXTEND_CHAR
#define SIGN_EXTEND_CHAR(x) (x)
#endif

#ifndef Sword /* must be non-zero in some of the tests below... */
#define Sword 1
#endif

/* JF for syntax stuff */
/* To add more variable-syntax features, just use more bits.  If we go over 16,
   we probably should make obscure_syntax a long.  (JF: Yes, virgina, there
really are 16 bit machines out there) */
#define RE_NO_BK_PARENS	(1<<0)
#define RE_NO_BK_VBAR	(1<<1)

/* This data structure is used to represent a compiled pattern. */

struct re_pattern_buffer
{
    char *buffer;	/* Space holding the compiled pattern commands. */
    int allocated;	/* Size of space that  buffer  points to */
    int used;		/* Length of portion of buffer actually occupied */
    char *fastmap;	/* Pointer to fastmap, if any, or zero if none. */
			/* re_search uses the fastmap, if there is one,
			   to skip quickly over totally implausible characters */
    char fastmap_accurate;
			/* Set to zero when a new pattern is stored,
			   set to one when the fastmap is updated from it. */
    char can_be_null;   /* Set to one by compiling fastmap
			   if this pattern might match the null string.
			   It does not necessarily match the null string
			   in that case, but if this is zero, it cannot.
			   2 as value means can match null string
			   but at end of range or before a character
			   listed in the fastmap.  */
};
typedef struct re_pattern_buffer    REPAT_BUFFER;

/* Structure to store "register" contents data in.

   Pass the address of such a structure as an argument to re_match, etc.,
   if you want this information back.

   start[i] and end[i] record the string matched by \( ... \) grouping i,
   for i from 1 to RE_NREGS - 1.
   start[0] and end[0] record the entire string matched. */

struct re_registers
{
    int start[RE_NREGS];
    int end[RE_NREGS];
};
typedef struct re_registers		REREGS;

/* These are the command codes that appear in compiled regular expressions, one per byte.
  Some command codes are followed by argument bytes.
  A command code can specify any interpretation whatever for its arguments.
  Zero-bytes may appear in the compiled regular expression. */

    /* followed by one byte giving n, and then by n literal bytes	   */
#define RECODE_EXACTN		    1
    /* fails unless at beginning of line				   */
#define RECODE_BEGLINE		    2
    /* fails unless at end of line					   */
#define RECODE_ENDLINE		    3
    /* followed by two bytes giving relative address to jump to 	   */
#define RECODE_JUMP		    4
    /* followed by 2 bytes giving rel addr to resume at in case of failure */
#define RECODE_ON_FAILURE_JUMP	    5
    /* Throw away latest failure point and then jump to address.	   */
#define RECODE_FINALIZE_JUMP	    6
    /* Like jump but finalize if safe to do so.  This is used to jump back */
    /* to the beginning of a repeat.  If the command that follows this	   */
    /* jump is clearly incompatible with the one at the beginning of the   */
    /* repeat, such that we can be sure that there is no use backtracking  */
    /* out of repetitions already completed, then we finalize.		   */
#define RECODE_MAYBE_FINALIZE_JUMP  7
    /* jump, and push a dummy failure point.  This failure point will be   */
    /* thrown away if an attempt is made to use it for a failure.	   */
    /* A + construct makes this before the first repeat.		   */
#define RECODE_DUMMY_FAILURE_JUMP   8
    /* matches any one character					   */
#define RECODE_ANYCHAR		    9
    /* matches any one char belonging to specified set.  First following   */
    /* byte is # bitmap bytes.	Then come bytes for a bit-map saying which */
    /* chars are in.  Bits in each byte are ordered low-bit-first.	   */
    /* A character is in the set if its bit is 1.  A character too large   */
    /* to have a bit in the map is automatically not in the set 	   */
#define RECODE_CHARSET		    10
    /* similar but match any character that is NOT one of those specified  */
#define RECODE_CHARSET_NOT	    11
    /* starts remembering the text that is matched and stores it in a	   */
    /* memory register followed by one byte containing the register number.*/
    /* Register numbers must be in the range 0 through NREGS.		   */
#define RECODE_START_MEMORY	    12
    /* stops remembering the text that is matched and stores it in a	   */
    /* memory register followed by one byte containing the register number.*/
    /* Register numbers must be in the range 0 through NREGS.		   */
#define RECODE_STOP_MEMORY	    13
    /* match a duplicate of something remembered.  Followed by one byte    */
    /* containing the index of the memory register.			   */
#define RECODE_DUPLICATE	    14
    /* Succeeds if before dot						   */
#define RECODE_BEFORE_DOT	    15
    /* Succeeds if at dot						   */
#define RECODE_AT_DOT		    16
    /* Succeeds if after dot						   */
#define RECODE_AFTER_DOT	    17
    /* Succeeds if at beginning of buffer				   */
#define RECODE_BEGBUF		    18
    /* Succeeds if at end of buffer					   */
#define RECODE_ENDBUF		    19
    /* Matches any word-constituent character				   */
#define RECODE_WORDCHAR 	    20
    /* Matches any char that is not a word-constituent			   */
#define RECODE_NOTWORDCHAR	    21
    /* Succeeds if at word beginning					   */
#define RECODE_WORDBEG		    22
    /* Succeeds if at word end						   */
#define RECODE_WORDEND		    23
    /* Succeeds if at a word boundary					   */
#define RECODE_WORDBOUND	    24
    /* Succeeds if not at a word boundary				   */
#define RECODE_NOTWORDBOUND	    25
    /* Matches any character whose syntax is specified followed by a byte  */
    /* which contains a syntax code, Sword or such like 		   */
#define RECODE_SYNTAXSPEC	    26
    /* Matches any character whose syntax differs from the specified.	   */
#define RECODE_NOTSYNTAXSPEC	    27


#define SYNTAX(c)		 re_syntax_table[c]
