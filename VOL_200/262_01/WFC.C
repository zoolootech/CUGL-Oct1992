/****************************************************************
 *               Word Frequency Analyzer Program                *
 *          to demonstrate use of symbol table library          *
 *                                                              *
 *             Robert Ramey    25 September 1986                *
 ****************************************************************/

/****************************************************************
This program processes one or more text file.   When all files
have been processed,  a table is produced on the standard output
which shows how many times each word in the text occurred
*****************************************************************/
#include <stdio.h>

typedef char SYMBOLTABLE;
main(argc,argv)
int argc;
char *argv[];
{
    FILE *file_pointer;
    SYMBOLTABLE *symbol_table_pointer;
    char *symbol_pointer;
    int i;

    /* initialize symbol table.  Data is one integer per symbol
    to contain the count of the occurrences of that symbol in
    the text.  Hash is set to 256 as it seems to be on the order
    of the number of different words in a short text. */
    symbol_table_pointer = symmk(sizeof(int),256);

    if(argc == 1){
        /* if called without arguments assume standard input*/
        file_pointer = stdin;
        ldwords(symbol_table_pointer,stdin);
    }
    else
        /* for each argument process corresponding file */
        for(i = 1;i < argc;++i)
            if(file_pointer = fopen(argv[i],"r"))
            ldwords(symbol_table_pointer,file_pointer);

    /* dump results to output */
    i = 0;
    while(symbol_pointer = symdmp(symbol_table_pointer, i++))
        printf("%d\t%s\n",
        *(int *)symdat(symbol_table_pointer,symbol_pointer),
        symbol_pointer);
    exit(0);
}
/*************************************************************
ldwords - reads one file and loads words to symbol table
***********************************************************/
void
ldwords(symbol_table_pointer,file_pointer)
FILE *file_pointer;
SYMBOLTABLE *symbol_table_pointer;
{
    char *word_pointer, *symbol_pointer, *getwrd(),word_array[20];
    while(word_pointer = getwrd(file_pointer,word_array))
        if(symbol_pointer =
        symlkup(symbol_table_pointer,word_pointer))
            ++*((int *)symdat
            (symbol_table_pointer,symbol_pointer));
        else
            if(symbol_pointer =
            symadd(symbol_table_pointer,word_pointer) )
                *(int *)symdat
                (symbol_table_pointer,symbol_pointer)
                = 1;
            else{
                printf("Sorry - out of memory\n");
                exit(1);
            }
    return;
}
/****************************************************************
getwrd - returns pointer to character string equal to next word
in file.  Punctuation is considered white space.  Returned words
contain no white space.
****************************************************************/
char *
getwrd(file_pointer,word_pointer)
FILE *file_pointer;
char *word_pointer;
{
    char *wp, character;

    wp = word_pointer;
    /* skip to next letter */
    while(!isalpha(character))
        if((character = getc(file_pointer)) == EOF)
            return NULL;

    /* gather characters of word */
    while(isalnum(character)){
        *wp++ = character;
        character = getc(file_pointer);
    }

    *wp = '\0';
    return word_pointer;
}
*****************************