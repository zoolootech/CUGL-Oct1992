
/*    ADVENT.H    revised header for BDS c vers 1.43   */

#define MAXLOC 150
#define MAXOBJ 100
#define WORDSIZE 20

#define EOF 0x1a
#define MAXTRAV 20
#define DWARFMAX 7
#define MAXDIE 3
#define MAXTRS 79
#define MAXVFAST 1800
#define MAXVVAL 300

/*
	Object definitions
*/
#define KEYS 1
#define LAMP 2
#define GRATE 3
#define CAGE 4
#define ROD 5
#define ROD2 6
#define STEPS 7
#define BIRD 8
#define DOOR 9
#define PILLOW 10
#define SNAKE 11
#define FISSURE 12
#define TABLET 13
#define CLAM 14
#define OYSTER 15
#define MAGAZINE 16
#define DWARF 17
#define KNIFE 18
#define FOOD 19
#define BOTTLE 20
#define WATER 21
#define OIL 22
#define MIRROR 23
#define PLANT 24
#define PLANT2 25
#define AXE 28
#define DRAGON 31
#define CHASM 32
#define TROLL 33
#define TROLL2 34
#define BEAR 35
#define MESSAGE 36
#define VEND 38
#define BATTERIES 39
#define NUGGET 50
#define COINS 54
#define CHEST 55
#define EGGS 56
#define TRIDENT 57
#define VASE 58
#define EMERALD 59
#define PYRAMID 60
#define PEARL 61
#define RUG 62
#define SPICES 63
#define CHAIN 64

/*
	Verb definitions
*/
#define NULLX 21
#define BACK 8
#define LOOK 57
#define CAVE 67
#define ENTRANCE 64
#define DEPRESSION 63

/*
	Action verb definitions
*/
#define TAKE 1
#define DROP 2
#define SAY 3
#define OPEN 4
#define NOTHING 5
#define LOCK 6
#define ON 7
#define OFF 8
#define WAVE 9
#define CALM 10
#define WALK 11
#define KILL 12
#define POUR 13
#define EAT 14
#define DRINK 15
#define RUB 16
#define THROW 17
#define QUIT 18
#define FIND 19
#define INVENTORY 20
#define FEED 21
#define FILL 22
#define BLAST 23
#define SCORE 24
#define FOO 25
#define BRIEF 26
#define READ 27
#define BREAK 28
#define WAKE 29
#define SUSPEND 30
#define HOURS 31
#define LOG 32

/*
	Bits of array cond
	indicating location status
*/
#define LIGHT 1
#define WATOIL 2
#define LIQUID 4
#define NOPIRAT 8
#define HINTC 16
#define HINTB 32
#define HINTS 64
#define HINTM 128
#define HINT 240

#define SECSIZ 128	/* Sector size for CP/M read/write calls */

/*
   The NSECTS symbol controls the compilation of the buffered
   I/O routines within STDLIB2.C, allowing each user to set the
   buffer size most convenient for his system, while keeping
   the numbers totally invisible to the C source programs using
   buffered I/O (via the BUFSIZ defined symbol.) For larger
   NSECTS, the disk I/O is faster...but more ram is taken up.
   Note that pre-1.4 versions of the library functions
   were not set up to support this customizable buffer size,
   and always compiled as if NSECTS were 1 in this version. To
   change the buffer size allocation, follow these steps:

     1) Alter NSECTS to the desired value here in bdscio.h
     2) Re-compile STDLIB1.C and STDLIB2.C
     3) Use CLIB to combine STDLIB1.CRL and STDLIB2.CRL to make
  	a new DEFF.CRL.

   Make sure you use declare all your I/O buffers with the a
   statement such as:
  		char buf_name[BUFSIZ];
  	 instead of the older and now obsolete:
  		char buf_name[134];
   	(and always #include "bdscio.h" in your programs!)
*/

#define NSECTS 8	/* Number of sectors to buffer up in ram */

#define BUFSIZ (NSECTS * SECSIZ + 6 )	/* Don't touch this */

struct _buf {				/* Or this...	    */
	int _fd;
	int _nleft;
	char *_nextp;
	char _buff[NSECTS * SECSIZ];
};

/*
	Adventure global variables
*/

/*
	Database variables
*/
char dbuff[BUFSIZ];
int fd[7];
struct trav {
	int tdest;
	int tverb;
	int tcond;
} travel[MAXTRAV];
int idx1[MAXLOC/10];
int idx2[MAXLOC/10];
int idx3[MAXLOC/10];
int idx6[30];
char actmsg[32];			/* action messages */
char fastverb[MAXVFAST];
int fastvval[MAXVVAL];
int fastvseek;

/*
	English variables
*/
char verb,object,motion;
char word1[WORDSIZE],word2[WORDSIZE];

/*
	Play variables
*/
int turns;
char loc,oldloc,oldloc2,newloc;	/* location variables */
char cond[MAXLOC];		/* location status    */
int place[MAXOBJ];		/* object location    */
int fixed[MAXOBJ];		/* second object loc  */
char visited[MAXLOC];		/* >0 if has been here*/
int prop[MAXOBJ];		/* status of object   */
int tally,tally2;		/* item counts        */
int limit;			/* time limit         */
int lmwarn;			/* lamp warning flag  */
char wzdark,closing,closed;	/* game state flags   */
char holding;			/* count of held items*/
char detail;			/* LOOK count         */
int knfloc;			/* knife location     */
int clock,clock2,panic;		/* timing variables   */
int dloc[DWARFMAX];		/* dwarf locations    */
char dflag;			/* dwarf flag         */
char dseen[DWARFMAX];		/* dwarf seen flag    */
int odloc[DWARFMAX];		/* dwarf old locations */
int daltloc;			/* alternate appearance */
char dkill;			/* dwarves killed     */
int chloc,chloc2;		/* chest locations    */
char bonus;			/* to pass to end     */
char numdie;			/* number of deaths   */
char object1;			/* to help intrans.   */
char gaveup;			/* 1 if he quit early */
int foobar;			/* fie fie foe foo... */
char saveflg;			/* if game being saved */
char dbgflg;			/* if game is in restart */


char lastglob;			/* to get space req.  */
/*endglobal*/
