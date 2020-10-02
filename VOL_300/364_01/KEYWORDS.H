
#if !defined(TYPES_DEFINED)
enum  data_types
#endif
 {
  CHARACTER,      /*  0  */
  INTEGER,
  UNSIGNED,
  SIGNED,
  SHORT,
  LONG,            /*  5  */
  FLOAT,
  DOUBLE,
  VOID,
  file,
  STRUCTURE,         /*   10   */
  TYPEDEF,
  INCLUDE_,
  DEFINE_,
  UNION,
  HUGE,                          /*   15    */
  SEGMENT,
  ENUM,
  END_TYPES                        /*  17   */
 };

#define PREPROCESSOR (END_TYPES + 0x10)
#define NO_TYPE 0x80
#define TYPEDEF_OPEN TYPEDEF + 101
#define TYPEDEF_ONELINE TYPEDEF + 100

#if defined (EXTERNAL)
  extern char *TYPES[];
  extern char *MODIFIERS[];
#else
char *TYPES[] = { "char", "int", "unsigned", "signed", "short", "long",
		   "float", "double", "void", "FILE", "struct", "typedef",
		   "include", "define", "union",  "huge", "segment",
		   "enum"
		 };

char *MODIFIERS[] = { "cdecl", "fortran", "pascal", "based", "far", "near",
		       "interrupt", "extern", "static" , "segname", "loadds",
		       "volatile", "const"
		       };

#define TYPES_DEFINED
#endif

enum data_modifiers {  CDECL,   FORTRAN,   PASCAL,   BASED,   FAR,   NEAR,
		       INTERRUPT, EXTERN, STATIC, SEGNAME, LOADDS, VOLATILE,
		       CONSTANT, END_MODS
		       };


#define USER_MOD  END_MODS + 1

#define SIGNED_VAR  0x7300
#define UNSIGNED_VAR  0x7500
#define LONG_DOUBLE 0x6c00

#define nKEY_WORDS 16
#if defined (EXTERNAL)
  extern char *KEYWORDS[nKEY_WORDS] ;
#else
char *KEYWORDS[nKEY_WORDS] =
{
  "_asm",
  "auto",
  "break",
  "case",
  "continue",
  "default",
  "do",
  "else",
  "enum",
  "for",
  "goto",
  "if",
  "sizeof",
  "switch",
  "return",
  "while"

};
#endif

