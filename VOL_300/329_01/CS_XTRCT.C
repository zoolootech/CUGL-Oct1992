/* cs_xtrct.c */
/* These routines take care of a "symbol table" which contains
 * all things one might want to extract from a source file
 * when mode is EXTRACT_SYMBOLS
 */
#include "csubst.h"

#define HASHSIZE 103 /* a prime number */
#define MAXTOTAL 32767  /* the maximum value of symbols */


struct symbol{
			char *original;/* text of original symbol */
			symbol_type_t type;	/* flag indicates origin */
			struct symbol *next; /* link in "bucket" */
			};
typedef int count_t; /* The type we use to count symbols */
static struct symbol *Symbol_table[HASHSIZE];
extern int hash();

#ifdef ANSI_C
extern void *malloc();
#else
extern char *malloc();
#endif

void init_extract()
{
int index;
for(index = 0; index < HASHSIZE; index++)
   Symbol_table[index] = NULL;

}

/* this routine returns 1 iff the symbol is in the table */
int symbol_lookup(string)
char *string;
{
int index;
struct symbol *bucket;

index = hash(string);
bucket = Symbol_table[index];
while(bucket != NULL && strncmp(string, bucket->original,SYMBOL_SIZE))
     bucket = bucket->next;
return bucket != NULL;
}

/* This routine tells the hash table that o_string is an symbol
 */

void symbol_install(o_string, type_flag)
char *o_string;
symbol_type_t type_flag;
{
int index;
struct symbol *bucket;

index = hash(o_string);
bucket = Symbol_table[index];
while(bucket != NULL && strncmp(o_string, bucket->original,SYMBOL_SIZE))
     bucket = bucket->next;
if(bucket == NULL){
  bucket = (struct symbol *)malloc(sizeof(struct symbol));
  bucket->original = (char *)malloc(strlen(o_string)+1);
  strcpy(bucket->original, o_string);
  bucket->next = Symbol_table[index];
  bucket->type = type_flag;
  Symbol_table[index] = bucket;
  }
}

/* this routine counts the number of symbols 
   stored in the hash table
 */
count_t count_symbols()
{
int index;
struct symbol *bucket;
count_t total = 0;

for(index = 0; index < HASHSIZE; index++)
   {
   bucket = Symbol_table[index];
   while(bucket != NULL)
	{
 	bucket = bucket->next;
	if(total == MAXTOTAL )
	  {
	  fatal("maximum number of symbols exceeded");
          }
	 else
         total++;
        }
   }
return total;
}

/* this routine is needed by qsort to compare symbols */
compar(psymb1, psymb2)
struct symbol **psymb1, **psymb2;
{
if((**psymb1).type != (**psymb2).type)
  return((**psymb1).type - (**psymb2).type);
else
return(strcmp((*psymb1)->original, (*psymb2)->original));
}

/* for display purposes */
char *type_name(type)
symbol_type_t type;
{
switch(type)
	{
	case TYPE_MACRO:
		return("MACRO");
	case TYPE_IDENTIFIER:
		return("IDENTIFIER");
	case TYPE_STRING:
		return("STRING");
	}
return NULL;
}

/* this routine makes a sorted table of 
   the symbols and prints them
 */

void sort_and_print()
{
struct symbol *bucket;
int index;
count_t total;
struct symbol **ptable;
struct symbol **table;
symbol_type_t next_type;

total  = count_symbols();
if(total == 0)
  return;
table = (struct symbol **)malloc(total*(sizeof(struct symbol *)));
if(table == NULL)
  {
  fatal1("cant allocate %d ", total);
  }
else
ptable = table;
for(index = 0; index < HASHSIZE; index++)
    {
   bucket = Symbol_table[index];
   while(bucket != NULL)
	{
	*ptable++ = bucket;
 	bucket = bucket->next;
        }
    }
qsort(table, total, sizeof(struct symbol *), compar);
ptable = table;
printf("\n/* %sS */\n\n", type_name(table[0]->type));
   
while(total)
     {
     struct symbol *p_next_symbol;
     total--;
     if(total != 0){
        p_next_symbol = *(ptable + 1);
        next_type = p_next_symbol->type;
        }
     printf("%s\n",(*ptable)->original);
     if( next_type != (*ptable)->type && total != 0)
	printf("\n/* %sS */\n\n", type_name(next_type));
     ptable++;
     }
}
/* end of file */
