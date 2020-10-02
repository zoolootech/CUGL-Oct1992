
/* burlap.h : 11/27/88, d.c.oshel */

#include "ciao.h"
#include "keys.h"
#include "fafnir.h"
#include "ctree.h"

#define MLOC 62  /* horizontal coordinate of scrollbar menus */

#define SCRNFILE "BURLAP.S"
#define PARMFILE "BURLAP.P"
#define ISAMFILE "BURLAP.DT"
#define INDXFILE "BURLAP.IX"

#define datno 0                  /* ISAMFILE's datafile number  */
#define ixnam 1                  /* INDXFILE's 1st index number */
#define ixzip 2                  /*            2nd              */
#define ixcmp 3                  /*            3rd              */

#define ASCIIFILE "BURLAP.ASC"  /* old-style comma-delimited data */

#define bomb bomb0
#define select select0

extern int delimiter;
extern char nyi[];
extern char far *burlap_screen;  /* in burlap.c */

extern int ISAM_field_range; /* NUMBER_OF_REAL_FIELDS, an important GLOBAL */

/* maximum ISAM record size is 80 chars per field times 12 fields */
#define BUFSIZE (8+(MAXVFLDLEN * 12)) /* happens to be NUMBER_OF_REAL_FIELDS */

/* record selection macro functions */

#define MACROSIZE 512
extern char macro_buffer[];       
extern int select_flag;

extern int evaluate_macro( char *p );
extern int choose_records( void );
extern int selected( void );

/* forward */

extern void strip_blanks( char *q );  
extern void load_form( void );
extern void show_form( void );
extern void put_screen( void );

extern void print_labels( int index );  /* index determines sort order */
extern void ISAM_crash( char *p );
extern int ask( char *msg );

