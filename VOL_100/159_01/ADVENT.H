
/*    ADVENT.H    revised header for BDS c vers 1.43   */

/*                Revised for Eco-C88 V2.72 by Bob Withers
                  Defined all variables for driver routines and
                  altered header to declare them external for
                  all sub-modules.    BW - 09/14/85
*/

#define NUL			    '\0'

#define MAXDIM(a)		    (sizeof(a) / sizeof(a[0]))

#define VOID			    void
#define NEAR			    near
#define PASCAL			    pascal

#ifndef TRUE
#define TRUE			    1
#define FALSE			    0
#endif

typedef short			    SHORT;
typedef unsigned short		    USHORT;
typedef long			    LONG;
typedef unsigned long		    ULONG;
typedef short			    BOOL;

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAXLOC			    150
#define MAXOBJ			    100
#define WORDSIZE		    20

#define DWARFMAX		    7
#define MAXDIE			    3
#define MAXTRS			    79

#define READ_BIN		    "rb"
#define WRITE_BIN		    "wb"


#define VOCAB_OBJECT		    1000
#define VOCAB_VERB		    2000
#define VOCAB_MSG		    3000

/*
        Object definitions
*/
#define KEYS			    1
#define LAMP			    2
#define GRATE			    3
#define CAGE			    4
#define ROD			    5
#define ROD2			    6
#define STEPS			    7
#define BIRD			    8
#define DOOR			    9
#define PILLOW			    10
#define SNAKE			    11
#define FISSURE 		    12
#define TABLET			    13
#define CLAM			    14
#define OYSTER			    15
#define MAGAZINE		    16
#define DWARF			    17
#define KNIFE			    18
#define FOOD			    19
#define BOTTLE			    20
#define WATER			    21
#define OIL			    22
#define MIRROR			    23
#define PLANT			    24
#define PLANT2			    25
#define STALACTITE		    26
#define FIGURE			    27
#define AXE			    28
#define DRAWING 		    29
#define PIRATE			    30
#define DRAGON			    31
#define CHASM			    32
#define TROLL			    33
#define TROLL2			    34
#define BEAR			    35
#define MESSAGE 		    36
#define VOLCANO 		    37
#define VEND			    38
#define BATTERIES		    39
#define CARPET			    40
#define NUGGET			    50
#define DIAMONDS		    51
#define SILVER			    52
#define JEWELS			    53
#define COINS			    54
#define CHEST			    55
#define EGGS			    56
#define TRIDENT 		    57
#define VASE			    58
#define EMERALD 		    59
#define PYRAMID 		    60
#define PEARL			    61
#define RUG			    62
#define SPICES			    63
#define CHAIN			    64

/*
        Verb definitions
*/
#define NULLX			    21
#define BACK			    8
#define LOOK			    57
#define CAVE			    67
#define ENTRANCE		    64
#define DEPRESSION		    63

/*
        Action verb definitions
*/
#define TAKE			    1
#define DROP			    2
#define SAY			    3
#define OPEN			    4
#define NOTHING 		    5
#define LOCK			    6
#define ON			    7
#define OFF			    8
#define WAVE			    9
#define CALM			    10
#define WALK			    11
#define KILL			    12
#define POUR			    13
#define EAT			    14
#define DRINK			    15
#define RUB			    16
#define THROW			    17
#define QUIT			    18
#define FIND			    19
#define INVENTORY		    20
#define FEED			    21
#define FILL			    22
#define BLAST			    23
#define SCORE			    24
#define FOO			    25
#define BRIEF			    26
#define READ			    27
#define BREAK			    28
#define WAKE			    29
#define SUSPEND 		    30
#define HOURS			    31
#define LOG			    32
#define SAVE			    33
#define RESTORE 		    34
#define VERBOSE 		    35

/*
        Bits of array cond
        indicating location status
*/
#define LIGHT			    1
#define WATOIL			    2
#define LIQUID			    4
#define NOPIRAT 		    8
#define HINTC			    16
#define HINTB			    32
#define HINTS			    64
#define HINTM			    128
#define HINT			    240

/*
        Adventure global variables
*/

struct S_VocabTab
{
    char	*pWord;
    SHORT	 sWord;
};
typedef struct S_VocabTab	    VOCABTAB;

struct trav
{
    SHORT	 tdest;
    SHORT	 tverb;
    SHORT	 tcond;
};
typedef struct trav		    TRAV;

struct travtab
{
    TRAV	*pTrav; 			// trav array for location
    SHORT	 sTrav; 			// # entries for location
};
typedef struct travtab		    TRAVTAB;


#ifdef DRIVER
#define CLASS
#define INIT(x) 		= x
#else
#define CLASS			extern
#define INIT(x)
#endif

#define NEARDATA		NEAR

CLASS SHORT  brief_sw;
/*
        Database variables
*/
CLASS TRAV *	 NEARDATA pTravel;		/* travel array & count for */
CLASS SHORT	 NEARDATA sTravCnt;		/* the current location     */
CLASS SHORT	 NEARDATA actmsg[32]			  /* action messages */
#ifdef DRIVER
      = {
	      0,  24,  29,   0,  33,   0,  33,	38,  38,  42,  /*  0 -	9 */
	     14,  43, 110,  29, 110,  73,  75,	29,  13,  59,  /* 10 - 19 */
	     59, 174, 109,  67,  13, 147, 155, 195, 146, 110,  /* 20 - 29 */
	     13,  13					       /* 30 - 31 */
	};
#else
	 ;
#endif

/*
        English variables
*/
CLASS SHORT	 NEARDATA verb;
CLASS SHORT	 NEARDATA object;
CLASS SHORT	 NEARDATA motion;
CLASS char	 NEARDATA word1[WORDSIZE];
CLASS char	 NEARDATA word2[WORDSIZE];

/*
        Play variables
*/
CLASS SHORT	 NEARDATA turns 	   INIT(0);
CLASS SHORT	 NEARDATA loc		   INIT(1);
CLASS SHORT	 NEARDATA oldloc	   INIT(1);
CLASS SHORT	 NEARDATA oldloc2	   INIT(1);
CLASS SHORT	 NEARDATA newloc	   INIT(3);
CLASS SHORT	 NEARDATA cond[MAXLOC]		     /* location status    */
#ifdef DRIVER
      = {
	      0,   5,	1,   5,   5,   1,   1,	 5,  17,   1,  /*   0 -   9 */
	      1,   0,	0,  32,   0,   0,   2,	 0,   0,  64,  /*  10 -  19 */
	      2,   2,	2,   0,   6,   0,   2,	 0,   0,   0,  /*  20 -  29 */
	      0,   2,	2,   0,   0,   0,   0,	 0,   4,   0,  /*  30 -  39 */
	      2,   0, 128, 128, 128, 128, 136, 136, 136, 128,  /*  40 -  49 */
	    128, 128, 128, 136, 128, 136,   0,	 8,   0,   2,  /*  50 -  59 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /*  60 -  69 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   2,  /*  70 -  79 */
	    128, 128, 136,   0,   0,   8, 136, 128,   0,   2,  /*  80 -  89 */
	      2,   0,	0,   0,   0,   4,   0,	 0,   0,   0,  /*  90 -  99 */
	      1,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 100 - 109 */
	      0,   0,	0,   4,   0,   1,   1,	 0,   0,   0,  /* 110 - 119 */
	      0,   0,	8,   8,   8,   8,   8,	 8,   8,   8,  /* 120 - 129 */
	      8,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 130 - 139 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 140 - 149 */
	};
#else
	 ;
#endif

CLASS SHORT	 NEARDATA place[MAXOBJ] 	      /* object location    */
#ifdef DRIVER
      = {
	      0,   3,	3,   8,  10,  11,   0,	14,  13,  94,  /*  0 -	9 */
	     96,  19,  17, 101, 103,   0, 106,	 0,   0,   3,  /* 10 - 19 */
	      3,   0,	0, 109,  25,  23, 111,	35,   0,  97,  /* 20 - 29 */
	      0, 119, 117, 117,   0, 130,   0, 126, 140,   0,  /* 30 - 39 */
	     96,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 40 - 49 */
	     18,  27,  28,  29,  30,   0,  92,	95,  97, 100,  /* 50 - 59 */
	    101,   0, 119, 127, 130,   0,   0,	 0,   0,   0,  /* 60 - 69 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 70 - 79 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 80 - 89 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 90 - 99 */
	};
#else
	 ;
#endif

CLASS SHORT	 NEARDATA fixed[MAXOBJ] 	      /* second object loc  */
#ifdef DRIVER
      = {
	      0,   0,	0,   9,   0,   0,   0,	15,   0,  -1,  /*  0 -	9 */
	      0,  -1,  27,  -1,   0,   0,   0,	-1,   0,   0,  /* 10 - 19 */
	      0,   0,	0,  -1,  -1,  67,  -1, 110,   0,  -1,  /* 20 - 29 */
	     -1, 121, 122, 122,   0,  -1,  -1,	-1,  -1,   0,  /* 30 - 39 */
	     -1,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 40 - 49 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 50 - 59 */
	      0,   0, 121,   0,  -1,   0,   0,	 0,   0,   0,  /* 60 - 69 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 70 - 79 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 80 - 89 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 90 - 99 */
	};
#else
	 ;
#endif

CLASS SHORT	 NEARDATA visited[MAXLOC];	     /* >0 if has been here*/
CLASS SHORT	 NEARDATA prop[MAXOBJ]		      /* status of object   */
#ifdef DRIVER
      = {
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /*  0 -	9 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 10 - 19 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 20 - 29 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 30 - 39 */
	      0,   0,	0,   0,   0,   0,   0,	 0,   0,   0,  /* 40 - 49 */
	     -1,  -1,  -1,  -1,  -1,  -1,  -1,	-1,  -1,  -1,  /* 50 - 59 */
	     -1,  -1,  -1,  -1,  -1,  -1,  -1,	-1,  -1,  -1,  /* 60 - 69 */
	     -1,  -1,  -1,  -1,  -1,  -1,  -1,	-1,  -1,  -1,  /* 70 - 79 */
	     -1,  -1,  -1,  -1,  -1,  -1,  -1,	-1,  -1,  -1,  /* 80 - 89 */
	     -1,  -1,  -1,  -1,  -1,  -1,  -1,	-1,  -1,  -1,  /* 90 - 99 */
	};
#else
	 ;
#endif

CLASS SHORT	 NEARDATA tally 	   INIT(15);   /* item counts	     */
CLASS SHORT	 NEARDATA tally2	   INIT(0);
CLASS SHORT	 NEARDATA limit 	   INIT(100);  /* time limit	     */
CLASS SHORT	 NEARDATA lmwarn	   INIT(0);    /* lamp warning flag  */
CLASS BOOL	 NEARDATA wzdark	   INIT(FALSE);
CLASS BOOL	 NEARDATA closing	   INIT(FALSE);
CLASS BOOL	 NEARDATA closed	   INIT(FALSE);
CLASS SHORT	 NEARDATA holding	   INIT(0);    /* count of held items*/
CLASS SHORT	 NEARDATA detail	   INIT(0);    /* LOOK count	     */
CLASS SHORT	 NEARDATA knfloc	   INIT(0);    /* knife location     */
CLASS SHORT	 NEARDATA clock1	   INIT(30);   /* timing variables   */
CLASS SHORT	 NEARDATA clock2	   INIT(50);
CLASS SHORT	 NEARDATA panic 	   INIT(0);
CLASS SHORT	 NEARDATA dloc[DWARFMAX]	      /* dwarf locations    */
#ifdef DRIVER
      = {
	      0,  19,  27,  33,  44,  64,   0		       /*  0 - 6  */
	};
#else
	 ;
#endif

CLASS SHORT	 NEARDATA dflag 	   INIT(0);    /* dwarf flag	     */
CLASS SHORT	 NEARDATA dseen[DWARFMAX];	       /* dwarf seen flag    */
CLASS SHORT	 NEARDATA odloc[DWARFMAX];	       /* dwarf old locs     */
CLASS SHORT	 NEARDATA daltloc	   INIT(18);   /* alt appearance     */
CLASS SHORT	 NEARDATA dkill 	   INIT(0);    /* dwarves killed     */
CLASS SHORT	 NEARDATA chloc 	   INIT(114);  /* chest locations    */
CLASS SHORT	 NEARDATA chloc2	   INIT(140);
CLASS SHORT	 NEARDATA bonus 	   INIT(0);    /* to pass to end     */
CLASS SHORT	 NEARDATA numdie	   INIT(0);    /* number of deaths   */
CLASS SHORT	 NEARDATA object1;		       /* to help intrans.   */
CLASS BOOL	 NEARDATA gaveup	   INIT(FALSE);
CLASS SHORT	 NEARDATA foobar	   INIT(0);    /* fie fie foe foo... */
CLASS BOOL	 NEARDATA saveflg	   INIT(FALSE);/* game being saved?  */
CLASS SHORT	 NEARDATA dbgflg;		       /* game in restart?   */


CLASS char	 NEARDATA lastglob;		       /* to get space req.  */

/*  endglobal  */

/*  function prototypes 						    */

/*  advent.c  */
int		       main(int argc, char **argv);
VOID		PASCAL restore(VOID);

/*  database.c	*/
VOID		PASCAL gettrav(SHORT loc);
SHORT		PASCAL yes(SHORT msg1, SHORT msg2, SHORT msg3);
VOID		PASCAL rspeak(SHORT msg);
VOID		PASCAL pspeak(SHORT item, SHORT state);
VOID		PASCAL desclg(SHORT loc);
VOID		PASCAL descsh(SHORT loc);
SHORT		PASCAL vocab(char *word, SHORT val);
BOOL		PASCAL dark(VOID);
BOOL		PASCAL here(SHORT item);
BOOL		PASCAL toting(SHORT item);
BOOL		PASCAL forced(SHORT atloc);
BOOL		PASCAL pct(SHORT x);
BOOL		PASCAL at(SHORT item);
VOID		PASCAL dstroy(SHORT obj);
VOID		PASCAL move(SHORT obj, SHORT where);
VOID		PASCAL juggle(SHORT loc);
VOID		PASCAL carry(SHORT obj, SHORT where);
VOID		PASCAL drop(SHORT obj, SHORT where);
SHORT		PASCAL put(SHORT obj, SHORT where, SHORT pval);
SHORT		PASCAL dcheck(VOID);
SHORT		PASCAL liq(VOID);
SHORT		PASCAL liqloc(SHORT loc);
SHORT		PASCAL liq2(SHORT pbottle);
VOID		PASCAL bug(SHORT n);

/*  verb.c  */
VOID		PASCAL trverb(VOID);
VOID		PASCAL vtake(VOID);
VOID		PASCAL vdrop(VOID);
VOID		PASCAL vopen(VOID);
VOID		PASCAL vsay(VOID);
VOID		PASCAL von(VOID);
VOID		PASCAL voff(VOID);
VOID		PASCAL vwave(VOID);
VOID		PASCAL vkill(VOID);
VOID		PASCAL vpour(VOID);
VOID		PASCAL veat(VOID);
VOID		PASCAL vdrink(VOID);
VOID		PASCAL vthrow(VOID);
VOID		PASCAL vfind(VOID);
VOID		PASCAL vfill(VOID);
VOID		PASCAL vfeed(VOID);
VOID		PASCAL vread(VOID);
VOID		PASCAL vblast(VOID);
VOID		PASCAL vbreak(VOID);
VOID		PASCAL vwake(VOID);
VOID		PASCAL actspk(SHORT verb);
VOID		PASCAL needobj(VOID);

/*  english.c  */
BOOL		PASCAL english(VOID);
BOOL		PASCAL analyze(char *word, SHORT *type, SHORT *value);
VOID		PASCAL getin(VOID);
VOID		PASCAL getword(char **buff, char *word);
VOID		PASCAL skipspc(char **buff);

/*  turn.c  */
VOID		PASCAL turn(VOID);
VOID		PASCAL describe(VOID);
VOID		PASCAL descitem(VOID);
VOID		PASCAL domove(VOID);
VOID		PASCAL goback(VOID);
VOID		PASCAL dotrav(VOID);
VOID		PASCAL badmove(VOID);
VOID		PASCAL spcmove(SHORT rdest);
VOID		PASCAL dwarfend(VOID);
VOID		PASCAL normend(VOID);
VOID		PASCAL score(VOID);
VOID		PASCAL death(VOID);
VOID		PASCAL doobj(VOID);
VOID		PASCAL trobj(VOID);
char *		PASCAL probj(SHORT object);
VOID		PASCAL dwarves(VOID);
VOID		PASCAL dopirate(VOID);
BOOL		PASCAL stimer(VOID);

/*  itverb.c  */
VOID		PASCAL itverb(VOID);
VOID		PASCAL ivtake(VOID);
VOID		PASCAL ivopen(VOID);
VOID		PASCAL ivkill(VOID);
VOID		PASCAL iveat(VOID);
VOID		PASCAL ivdrink(VOID);
VOID		PASCAL ivquit(VOID);
VOID		PASCAL ivfill(VOID);
VOID		PASCAL ivfoo(VOID);
VOID		PASCAL ivread(VOID);
VOID		PASCAL inventory(VOID);
VOID		PASCAL addobj(SHORT obj);

/*  saveadv.c  */
VOID		PASCAL saveadv(VOID);
