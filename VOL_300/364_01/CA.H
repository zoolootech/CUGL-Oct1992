
#if !defined(FUNC_STRUC)

struct calls_from
 {
  struct func *caller;
  struct calls_from *next;
 };

struct func

 {
  char name[32];
  char module[9];
  unsigned line_no;           /* first line of function:  declaration */
  unsigned endfunc_line;     /* last line of function */
  int fnptr;
  struct func *next;         /* alphabetized linked list  */
  struct func *next_function;  /* list in order of appearance in file */
  struct calls_from  *first_call;
 } ;
#define FUNC_STRUC
#endif

#if !defined(_GLOBALS_LIST)
#define _MAX_MODS 6
#define FIRST_MODS_CHECK 0
#define FINAL_MODS_CHECK 1
/* used in: GLOBL_CA,  INTF_CA, LINKL_CA, UTILG_CA */
struct globals_list
 {
  char *variable;
  char *module;
  int type;
  char *type_ident;    /* ptr to name of typedef type if type == TYPEDEF */
  char *struct_ident; /* point to name of structure or union */
  char *complex_decl;
  char storage;       /* type of storage */
  int modifiers[_MAX_MODS];     /* list of modifiers & qualfiers */
  struct globals_list *next;
 };
#define _GLOBALS_LIST
#endif

struct user_typeStack
{
   char * type;
   struct user_typeStack *next;
};

struct filespec
 {
 char *path;
 struct filespec *next;
 };

#define COMPLEX_INI  -1
#define NO_COMPLEX   0
#define COMPLEX_FN   0x10
#define COMPLEX_VAR  0x20
#define STD_DECL     0x30

#if defined (TYPEDEF_DEFINE)
  #define __TYPEDEFS 60
  #define __TYPEDEFSIZE 32
  #define _MOD_     0x1
  #define _TYPE_    0x2
  #define _FNPTR_  ( COMPLEX_FN  | _TYPE_ )
  #define _CMPLV_  ( COMPLEX_VAR | _TYPE_ )
 #if defined (TDEF_CA)
   char __type_def[__TYPEDEFS][__TYPEDEFSIZE];
   int  __typ_mod[__TYPEDEFS];
   int __typedef_count;
   int __typenum;
 #elif defined (TDEF_EXTERNAL)
   extern char __type_def[__TYPEDEFS][__TYPEDEFSIZE];
   extern int  __typ_mod[__TYPEDEFS];
 #if defined (TDEF_NUM )
   extern int __typedef_count;
 #endif
   extern int __typenum;
 #endif
#endif

#if defined (INCLUDEFILES_DEFINE)
#define INCLF_LIMIT 8
struct includeFILE {
  FILE *address[INCLF_LIMIT];
  char file[INCLF_LIMIT][80];
  int level;
  };
#endif

#define BIGBUFSIZE  350 + 261
#define LAST_FUNCTION 0xFFFF
#define NO_TOKEN       0X0
#define IS_TOKEN       0X1
#define PTR_ASSIGNED   0X2


#if !defined (IFDEFS_DEFINED)

/* temporary storage of information on if/def defines for testing */
/* & analysis of truth values in ifdef_true() */
typedef struct if_def {
    char *def1;              /* definition name */
    char *def2;
    unsigned int boolean1;   /* truth value of def */
    unsigned int boolean2;
    int op;                 /*  relational operator in if/def expresion: */
			    /*  either && or || */
} IF_DEF;
#define IFDEFS_DEFINED
#endif

#if defined(EXIT_CODES)
enum {
	NO_IFDEF_STK, BAD_IFDEF_STK,         /* IFDEF_CA */   /* memory */

	NO_USER_TYP_STK, BAD_USER_TYP_STK,   /* UTLG_CA */    /* memory */

	BAD_FILE_BUF, NO_FILE_STR,           /* INTF_CA */    /* memory */
	BAD_ARGV,  BAD_FILE, BAD_PROJ,                        /* files  */

	NO_BSEARCH_ARRAY,                     /* XRF_CA  */    /* memory */
	NULL_POINTER
	};

#endif

/* defines for splitting up report file */
#define START_RPT     -1
#define FIRST_SPLIT    0
#define SECOND_SPLIT   1
#define THIRD_SPLIT    2

