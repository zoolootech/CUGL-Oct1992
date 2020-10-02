/*--------------------------------------------------------------------------*\
| ADV-DEF.H : defines the adventure world types and externals                |
\*--------------------------------------------------------------------------*/

/*----------------------------*/
/* Location
*/
#define MaxDirs 6       /* Maximum 6 directions used (N E S W U D) */

enum { north, east, south, west, up, down };   /* named directions */

typedef struct   /* room information */
{
  char  *name;             /* description of the room */
  int   link [ MaxDirs ];  /* room connections: N E S W U D */
}
room_type;


/*----------------------------*/
/* objects
*/
typedef struct     /* object information */
{
  char
    *name;         /* name of object */

  int
    init_loc,      /* initial location */
    attr,          /* attribute:  b0=get  (b1=check in get)  b2=look  */
    loc;           /* current location */
}
object_type;


/*----------------------------*/
/* vocabulary
*/
typedef struct          /* vocabulary word */
{
  char *name;           /* words used */
  int num;              /* which object it corresponds to */
}
vocab_type;


/*----------------------------*/
/* externals to be provided by the specific adventure-def file
*/
extern room_type room[];

extern object_type obj [];

/*
  start w/blank string, end w/null string
  {
  "    " , 0 ,
 ...
  "",0
  }
*/
extern vocab_type v_noun [];
extern vocab_type v_verb[];

extern int
  num_verbs, num_nouns,         /* # of verbs and nouns */
  NumLocs, NumObjs, NumDirs;    /* # locations, objects, directions */

