/* fafnir.h -- general purpose forms dragon, with Elysian Fields
               5/10/88, 8/20/88, 9/10/88, 9/24/88, by d.c.oshel
               */

/*========================================================================* 
**  A FORM_RULES array defines a form.                                    **
**                                                                        **
**  All relevant information about a form and its fields is collected in  **
**  the field elements of a FORM_RULES array.                             **
 *========================================================================*/

typedef char * FPTR; /* pointer to a character array used for data entry */

#define FIELDS FPTR  /* for convenience in declaring field names */

typedef int (cdecl * VALIDATER)(); /* pointer to C function returning int */

#define VNOP ((VALIDATER)0)     /* "null function", validate not required */

/* NOTE: character validaters:  unsigned (*cv)( unsigned ch, int position );
         field validaters:      int (*fv)( char *p, int x, int y, int len );
         field initializers:    int (*fi)( char *p, int x, int y, int len );
         */

typedef struct field_element {
    FPTR *fptr;   /* the ADDRESS of a char * to this field's storage area */
    FPTR dflt;    /* pointer to a default data entry mask for this field */
    int x;        /* screen column of field, 0..79 */
    int y;        /* screen line of field, 0..24 */
    int len;      /* field width; if negative, width of window & field = 80 */
    VALIDATER fi; /* pointer to each-time-arrived field initializer */
                  /* if fi == VNOP, the field is not touched */
    VALIDATER cv; /* pointer to character-by-character validation function */
                  /* if cv == VNOP, the cursor does not stop in the field */
    VALIDATER fv; /* pointer to each-time-exited field validation function */
                  /* if fv == VNOP, the field is considered valid */

} FORM_RULES;

/* see EditField concerning variable length fields (len < 0), below */


/*======================================================================*/
/*             FORM_RULES macros for standard field types               */
/*======================================================================*/

#define MAXVFLDLEN 80     /* longest variable length field */
#define SLIDE(L) (-L)     /* marks window width for variable length field */

#define DEFINE_FIELDS(Q) FORM_RULES Q[] = {
#define END_FIELDS };
#define GetPage(SCREEN,NAME,OFFSET,NUMFLZ) SCREEN,&NAME[OFFSET],NUMFLZ 
#define FormSize(F) (sizeof(F)/sizeof(FORM_RULES))

/* default edit masks */

#define TX_MASK NULL      /* text */
#define DL_MASK zdol      /* dollar */
#define PH_MASK zfone     /* phone */
#define DT_MASK zdate     /* date */
#define SN_MASK zssn      /* social security number */
#define ZR_MASK zeroes    /* all zeroes */
#define YN_MASK "N"       /* Yes or No */

#define DL_SIZE 15
#define PH_SIZE 14
#define SN_SIZE 11
#define TZ_SIZE 10
#define DT_SIZE 8
#define ZP_SIZE 5
#define YN_SIZE 1
#define TF_SIZE YN_SIZE
#define ST_SIZE 2
#define CO_SIZE 2

#define TOP_OF_FORM 0 

#define UPDATE 1   /* see Example, below */
#define ADD 0      /* see Example, below */

/* the following macros reduce the field definition task to simple lists */

#define FIELD(F,M,X,Y,L,I,C,V)  {&F,M,X,Y,L,I,C,V},  /* generic macro */

            /*------------------------------------------*/
            /*  F  field name                           */
            /*  M  default field mask, i.e., contents   */
            /*  X  screen column                        */
            /*  Y  screen row                           */
            /*  L  field length, i.e., width            */
            /*  I  field initialization, VALIDATER name */
            /*  C  character validation, VALIDATER name */
            /*  V  field validation, VALIDATER name     */
            /*------------------------------------------*/

            /*     Example of editing a prior record:
            *
            *      FIELDS lastname, firstnam, fullname, homephon;
            *
            *      DEFINE_FIELDS( edits_record )
            *         TEXT(      lastname, 10, 2, 15               )
            *         TEXT(      firstnam, 10, 3, 15               )
            *         VIRTUAL(   fullname, 10, 5, 30, concatnames  )
            *         PHONE_SET( homephon, "(319) 555-    ", 10, 7 )
            *      END_FIELDS;
            *
            *      #define ALL 4
            *
            *      AllocateFields( edits_record, ALL );
            *      <--find previous record and load fields because UPDATE-->
            *      action = OnePageForm( 
            *               GetPage( edits_record, TOPF, ALL_FIELDS ),
            *               UPDATE
            *               );
            *      <--save/ignore edits, etc., based on action-->
            *      ReleaseFields( edits_record, ALL );
            */

            /* Note macro syntax: use "NAME( ", not "NAME ("
            *  do not put trailing commas after ) in field type macros.
            */

            /* WARNING: the NumFields dimension is ** CRITICAL **
            *           in AllocateFields!  Potential system crash.
            */

            /* WARNING: routines to load fields must pad right
            *           with blanks, and honor field width!
            */


/* standard text field definitions */

#define TEXT(F,X,Y,L)         {&F,TX_MASK,X,Y,L,VNOP,cgood,VNOP},
#define UPPER(F,X,Y,L)        {&F,TX_MASK,X,Y,L,VNOP,c2upr,VNOP},

#define TEXT_SET(F,M,X,Y,L)   {&F,M,X,Y,L,VNOP,cgood,VNOP},
#define UPPER_SET(F,M,X,Y,L)  {&F,M,X,Y,L,VNOP,c2upr,VNOP},


/* masked numeric input, L <= 10, R is a VALIDATER for range checking */

#define ZERO(F,X,Y,L,R)       {&F,ZR_MASK,X,Y,L,VNOP,cnmrc,R},
#define ZERO_SET(F,M,X,Y,L,R) {&F,M,X,Y,L,VNOP,cnmrc,R},


/* standard fields with pre-determined widths */

#define DOLLAR(F,X,Y)         {&F,DL_MASK,X,Y,DL_SIZE,lj_dol,cdoll,rj_dol},
#define PHONE(F,X,Y)          {&F,PH_MASK,X,Y,PH_SIZE,VNOP,cfone,VNOP},
#define SSN(F,X,Y)            {&F,SN_MASK,X,Y,SN_SIZE,VNOP,cnmrc,VNOP},
#define DATE(F,X,Y)           {&F,DT_MASK,X,Y,DT_SIZE,VNOP,cnmrc,vdate},
#define ZIP(F,X,Y)            {&F,ZP_MASK,X,Y,YN_SIZE,VNOP,cnmrc,VNOP},
#define TENZIP(F,X,Y)         {&F,TZ_MASK,X,Y,TZ_SIZE,VNOP,cnzip,VNOP},

#define YESNO(F,X,Y)          {&F,"Y",X,Y,TF_SIZE,VNOP,ctfyn,vtrufal},
#define NOYES(F,X,Y)          {&F,"N",X,Y,TF_SIZE,VNOP,ctfyn,vtrufal},

#define DOLLAR_SET(F,M,X,Y)   {&F,M,X,Y,DL_SIZE,lj_dol,cdoll,rj_dol},
#define PHONE_SET(F,M,X,Y)    {&F,M,X,Y,PH_SIZE,VNOP,cfone,VNOP},
#define SSN_SET(F,M,X,Y)      {&F,M,X,Y,SN_SIZE,VNOP,cnmrc,VNOP},
#define DATE_SET(F,M,X,Y)     {&F,M,X,Y,DT_SIZE,VNOP,cnmrc,vdate},
#define ZIP_SET(F,M,X,Y)      {&F,M,X,Y,YN_SIZE,VNOP,cnmrc,VNOP},
#define TENZIP_SET(F,M,X,Y)   {&F,M,X,Y,TZ_SIZE,VNOP,cnzip,VNOP},


/* standard two-letter U.S. state and territory abbreviations */

#define STATE_SET(F,M,X,Y)    {&F,M,X,Y,ST_SIZE,VNOP,c2upr,vstate},


/* Iowa Dept. of Transportation numeric county codes */

#define COUNTY_SET(F,M,X,Y)   {&F,M,X,Y,CO_SIZE,VNOP,cnmrc,vcounty},


/* information-only field types */

#define TODAY(F,X,Y)             {&F,NULL,X,Y,DT_SIZE,vtoday,VNOP,VNOP},

#define DISPLAY(F,X,Y,L)         {&F,NULL,X,Y,L,VNOP,VNOP,VNOP},
#define VIRTUAL(F,X,Y,L,A)       {&F,NULL,X,Y,L,A,VNOP,VNOP}, /* (*fi)() does it! */
#define COMPUTE(F,X,Y,L,B)       {&F,NULL,X,Y,L,VNOP,VNOP,B}, /* (*fv)() does it! */

#define DISPLAY_SET(F,M,X,Y,L)   {&F,M,X,Y,L,VNOP,VNOP,VNOP},
#define VIRTUAL_SET(F,M,X,Y,L,A) {&F,M,X,Y,L,A,VNOP,VNOP}, /* (*fi)() does it! */
#define COMPUTE_SET(F,M,X,Y,L,B) {&F,M,X,Y,L,VNOP,VNOP,B}, /* (*fv)() does it! */


/* INITIALIZE is a dummy (and costly!) secondary reference to a field.

   The idea is to get a field which is not immediately used into the range 
   of fields allocated and/or initialized by AllocateFields or InitForm,
   even though EditForm's range of fields deliberately excludes or includes
   the field elsewhere, where it might be defined as TEXT(F,M,X,Y,L), etc.,
   depending on circumstances.

   There is some overhead for including the INITIALIZE macro, so it should
   be used sparingly, if at all.  FORM_RULES arrays can generally be
   designed (or rearranged!) so that INITIALIZE is not necessary.
   */

#define INITIALIZE(F,M,L)     {&F,M,0,0,L,VNOP,VNOP,VNOP},

/*=========================== end of field macros ======================*/


/*------------------------*/
/* EditForm Return Values */
/*------------------------*/

/* this constant is defined, but it is not returned by anything
*/

#define EDIT_FORM 0   

/* these constants are returned by EditForm, OnePageForm, TwoPageForm */

#define SAVE_FORM 1
#define SKIP_FORM 2
#define DELETE_FORM 3
#define STOP_SEARCH 4

/* note that EditForm can also return three keys: PGUP, PGDN, END */
/* note that EditField returns the values of keys; see below */



/* skip flag used by EditField, NO_SKIP needs Enter to exit from a field */

#define SKIP_OK 1
#define NO_SKIP 0



/* form skip flag used by EditForm, SINGLEPAGE needs Esc to exit from form */

#define SINGLEPAGE 0
#define MULTIPAGE 1




/* =====  Common Default Field Masks  ===== */

extern char zssn[];     /* "000-00-0000" */
extern char zdate[];    /* "00/00/00" */
extern char zfone[];    /* "(   )    -    " */
extern char zeroes[];   /* "0000000000", ten zeroes */
extern char zdol[];     /* "0.00" */

extern char *Fafnir_form_exit_actions[];  /* 3 is max! GLOBAL, 12/4/88, dco */

/* two useful lists */

extern char *states[];   /* in vstate.c  */
extern char *counties[]; /* in vcounty.c */




/* ====== bomb0(): A good global system error handler ======= */

typedef void (cdecl * PTR_TO_CRASH_FN)(void);

#define NO_CRASH_FN ((PTR_TO_CRASH_FN)0)

/* Fafnir's suggested system error handler will call the user's crash 
   function if one has been set, then call vid_exit(), then vprintf(msg,...) 
   at the bottom of the screen, then unconditionally exit(255)

   when called, bomb0() TERMINATES the program with errorlevel 255

   -----------------------------------------
   NOTICE: user may #define bomb(X) bomb0(X)  <-- harmless, upward compatible
   -----------------------------------------
   */
extern void bomb0( char *msg, ... );


/* sample call:  set_crash_function( abnormal_shutdown )
                 bomb0("out of memory"); 

   if bomb0() is called, user's abnormal_shutdown() function will be called
   first, before bomb0() does anything else (such as printing the diagnostic)

   if set_crash_function() has not been called, no (*call_on_crash)() will
   be executed; i.e., if not set the crash function is defined as a NOOP
   */
extern void set_crash_function( PTR_TO_CRASH_FN call_on_crash );



/* user should call this function on program exit to ensure field integrity;
   it is possible to overwrite field buffers, since field char *'s are not
   hidden from the user; calls bomb0() if programmer error can be detected
   */
extern void exit_fafnir( void );




/* ========== Common Character Validater Functions =========== */

extern unsigned cgood( unsigned c ); /* any printable ASCII char */
extern unsigned c2upr( unsigned c ); /* uppercase alpha, else like cgood */
extern unsigned cnmrc( unsigned c ); /* digits 0..9 only */

/* two "free-form numeric" character validaters, allow blank field */
extern unsigned cnzip( unsigned c ); /* space, hyphen (or minus), 0..9 only */
extern unsigned cdoll( unsigned c ); /* dollar, i.e., $12,345,678.90- and space */

/* fills "(   )    -    " zfone mask with "(123) 678-ABCD", alphanumerics */
extern unsigned cfone( unsigned c, int pos );

/* returns 'Y' or 'N' on T,t,Y,y,1 or N,n,F,f,0 */
unsigned ctfyn( unsigned c );


/* ========== Common Field Validater Functions =========== */


/* the FORM_RULE "{ &fptr, zblank, X, Y, 15, lj_dol, cdoll, rj_doll }"
** makes a very nice description of a DOLLAR type data entry field!
*/
extern int rj_dol ( char *p, int x, int y, int len ); /* rightjustify $0.00 */
extern int lj_dol ( char *p, int x, int y, int len ); /* leftjustify  $0.00 */

/* dates
*/
extern int vdate( char *p, int x, int y, int len ); /* valid date string? */
extern int vtoday( char *p, int x, int y, int len ); /* set p <- today's date */

/* state and county lists
*/
extern int vstate( char *p, int x, int y, int len );  /* U.S. state abbrvs. */
extern int vcounty( char *p, int x, int y, int len ); /* Iowa DOT county codes */

/* boolean
*/
extern int vtrufal( char *p, int x, int y, int len ); /* Y or N */


/* ======= Global Utility Functions ========= */


/* returns the value of len (or fewer) digits in p as a signed short int
   */
extern int atoin( char *p, int len );



/* returns the value of len (or fewer) digits in p as a signed long int
   */
extern long atoln( char *p, int len );



/* atodoln() returns the value of len (or fewer) digits in p as a signed 
   "dollar long" int; i.e., the string "$1.00" returns 100L, normalized
   as n dollars * (100 cents/dollar).  Value may be negative!  A hyphen
   anywhere in the string (normally leading or trailing) yields negative.

   atodoln() allows free-form user cents fields (!), i.e., 
   $100 is $100. is $100.0 is $100.00 is $100.000 (mills digit truncated!)
   all these cases return (long) 10000 cents, equivalent to $100.00
   */
extern long atodoln( char *p, int len );




/* a useful date macro, needs char * to "MM/DD/YY" string,
** evaluates to a ((long) julian_date), must #include "ciao.h" to use it
*/

#define str2jul(D) (mdy2jul((atoin((D),2)),(atoin((D)+3,2)),(atoin((D)+6,2)+1900)))




/* display a timed popout message
*/
extern void boxmsg( char *msg, ... ); 



/* like boxmsg(), but not timed and overwrites current screen (!)
*/
extern void nboxmsg( char *msg, ... ); 



/* a macro for menus */
#define SIZE(CANARY) (sizeof(CANARY)/sizeof(char *))


/* present a scrollbar menu of choices to the user,
** saves and restores screen and cursor,
** returns an int representing menu offset from 0 among the selections
*/
extern int select( int topx, int topy, char *menu[], int menu_size ); 


/* same as select, but does not save screen or cursor 
*/
extern int select0( int topx, int topy, char *menu[], int menu_size );



/* saves screen, 
   sprintf's up to 127-byte string from msg, ... 
   executes (*fn)( string ), 
   restores screen, 
   returns the (*fn)() return value 
   */
extern unsigned screenroutine( unsigned (*fn)(char *), char *msg,... );


/* =================== Prepare Screens, Display Forms ==================== */


/* PrepScreen() reads a 4000-byte screen file from disk to a buffer, and 
   returns a char far * to the buffer which contains the screen.

   Screen files must be created using ES.EXE, or any similar program which
   writes video text page 0 to a disk file.
   */

extern char far * PrepScreen( char *screen );


/* PopScreen() copies the buffer set by PrepScreen() onto the screen
   */
extern void PopScreen( char far *p );


/* DisplayForm returns nothing, assumes PrepScreen has been called to
** load a screen file, and that screen points to a buffer with screen image
*/

extern void DisplayForm( char far *screen, FORM_RULES form[], int NumFields );




/* ======== *** MUST CALL *** Functions to Initialize a Form ============ */


/* Dynamically allocates the indicated range of fields in a form,
   and initializes each to a null-terminated string of len blanks;
   if the dflt pointer is not NULL, installs the field's DATA ENTRY MASK;
   if the field cannot be allocated, calls bomb() and gives up;

   This is a *** MUST CALL *** if strings have not already been
   assigned to the form's fptrs!  See also InitForm().
   */
extern void AllocateFields( FORM_RULES form[], int NumFields );



/* Copies up to len bytes of each field's default string into its 
   actual field buffer, in the range of fields indicated; if the
   default string is shorter than the field itself, the remainder of
   the field is padded to len bytes with blanks.

   Used to install a form's DATA ENTRY MASKS into each of its fields,
   so this is a *** MUST CALL *** if AllocateFields() was not called.
   */
extern void InitForm( FORM_RULES form[], int NumFields );



/* releases a form's dynamic memory allocation as set by AllocateFields;
   does nothing if AllocateFields was not called
   */
extern void ReleaseFields( FORM_RULES form[], int NumFields );




/* ================== Edit Functions ===================== */


/* EditField returns KEY CONSTANTS as exit codes (defined in "keys.h"):
**
**                 ESC or CR
**                 F2, END, PGDN, PGUP, UP, DN
**
** exit keys F2..DN do NOT validate the field, and do NOT change its contents!
** exit keys Esc or Enter, however, DO validate, and DO update IFF valid!
** if the field is not valid, according to (*fvalid)(), does not exit
** 
** Note: if ((*cvalid)( SPC, i )) at all i, INS and DEL keys are enabled!
** Shift_F1 always displays current version information, without exiting
**
** New, 9/24/88, d.c.oshel -- if (len < 0), the field is treated like a
**               variable length field padded right with blanks in a
**               buffer MAXVFLDLEN chars long.  The edit field becomes a
**               window len wide which "slides" over the longer edit buffer.
**
**               CAUTION:  fldptr  * M U S T *  point to a string which
**               contains MAXVFLDLEN characters or blanks!  If INS and DEL
**               keys can't be enabled (space not allowed in all positions),
**               the field is treated like an ordinary field len chars wide.
*/

extern unsigned EditField (char *fldptr,      /* data address */
				           int x, int y,      /* screen coordinates */
				           int len,           /* field length */ 
				           int skip_to_next,  /* end-of-field handling flag */
				           VALIDATER cvalid,  /* individual char validation */
				           VALIDATER fvalid); /* whole field validation */



/*  EditForm returns these exit codes:       SAVE_FORM, SKIP_FORM,
**  plus, IFF MultiPage:                     PGDN, PGUP,
**  plus, IFF cursor did not stop anywhere:  END
*/
extern unsigned EditForm ( FORM_RULES form[], int NumFields, int MultiPage );



/* ========== Screen Managers =========== */


/*  _onepageform returns:  SAVE_FORM, SKIP_FORM
**                         accepts a pointer to a screen buffer
*/

extern int _onepageform( char far *screen, FORM_RULES Page[], int NumFields );



/*  _twopageform returns:  SAVE_FORM, SKIP_FORM
**                         accepts pointers to two screen buffers
*/

extern int _twopageform( char far *screen1, FORM_RULES Page1[], int NumFields1,
                         char far *screen2, FORM_RULES Page2[], int NumFields2 );


/*  OnePageForm returns:  SAVE_FORM, SKIP_FORM,
**  plus, IFF OldRecord:  DELETE_FORM, STOP_SEARCH
**
**  accepts the name of a screen file, gives opening dialogue if the
**  OldRecord flag is true
**
**  assumes fields are Allocated, and either cleared or pre-loaded (OldRecord)
*/
extern int OnePageForm( char *ScreenFileName, FORM_RULES Page[], 
                        int NumFields, 
                        int OldRecord );


/*  TwoPageForm returns:  SAVE_FORM, STOP_SEARCH,
**  plus, IFF OldRecord:  SKIP_FORM, DELETE_FORM
**
**  accepts the names of two screen files, gives opening dialogue if the
**  OldRecord flag is true
**
**  assumes fields are Allocated, and either cleared or pre-loaded (OldRecord)
*/
extern int TwoPageForm( char *ScreenFileName1, FORM_RULES Page1[], 
                        int NumFields1,
                        char *ScreenFileName2, FORM_RULES Page2[], 
                        int NumFields2, 
                        int OldRecord );


