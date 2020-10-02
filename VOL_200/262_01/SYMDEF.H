/****************************************************************
 *      Definitions for Symbol Table Manipulation Package       *
 *                                                              *
 *                    by Robert Ramey                           *
 *                    8  August  1986                           *
 ****************************************************************/

typedef struct _symtbl {    /*structure for symbol table data */
    int _element_size; /*size of data not includeing links nor*/
              /*symbol itself */
    int _hash_factor; /*number of chains of symbols */
    int *_hash_address[1]; /*address of start of chain */
} SYMBOLTABLE;

typedef char SYMBOLENTRY;
   by Robert Ramey                  