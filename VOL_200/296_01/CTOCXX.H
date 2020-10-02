
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#define FOREVER for(;;)
#endif

/* Symbol Table Definitions */
#define TABSIZ  353
/* ctocxx.h - header file for C to C++ conversion tool - C.D. Havener */
#define NOARGS  1
#define WITH_ARGS 2
#define SYMSIZ  40 

typedef struct symbol   /* symbol table unit */
        {
        char name[SYMSIZ];
        int type;
        int muldef;
        struct symbol *chain;   /* collision chain */
        union
            {
            int ival;
            long lval;
            } uval;
        }SYM,*SYMPTR;

