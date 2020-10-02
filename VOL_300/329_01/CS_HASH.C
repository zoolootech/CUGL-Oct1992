/* hash.c */
/* storing, handling and lookup of the substitutions.
 * We use a second hash table, not the same as for symbols.
 */
/*

This programme is shareware.
If you found this programme useful then send me a colourful postcard
with the words "Happy Birthday" (or equivalent) so that it arrives
at my address around the first of January 1991.

Henri de Feraudy
27 rue Chef de Ville
92140 Clamart 
France

 */

#include "csubst.h" 

#define HASHSIZE 103 /* a prime number */

extern int Just_met_EOF; /* in main.c */

char O_string_buffer[SYMBOL_SIZE + 1]; /* used in csubst.l */

/* this structure is used to store a substitution pair */
struct substitution{
			char *original;/* text of original identifier */
			char *replacement;/* text of new identifier */
			struct substitution *next; /* link in "bucket" */
			};
/* this global is the hash table used to store all pairs */
static struct substitution *Hash_table[HASHSIZE];

/* this function is used to find the adresses in the hash table of a 
   substitution 
 */

int hash(str)
char *str;
{
int ret = 0;

while(*str)
     ret += *str++;

return ret % HASHSIZE;
}

/* This routine tells the programme that r_string is going to
   be the string associated with o_string
 */
void install_subst(o_string,r_string)
char *o_string;
char *r_string;
{
#ifdef ANSI_C
extern void *malloc();
#else
extern char *malloc();
#endif

int index;
struct substitution *bucket;

index = hash(o_string);
bucket = Hash_table[index];
while(bucket != NULL && strncmp(o_string, bucket->original,SYMBOL_SIZE))
     bucket = bucket->next;
if(bucket == NULL){
  bucket = (struct substitution *)malloc(sizeof(struct substitution));
  bucket->original = (char *)malloc(strlen(o_string)+1);
  bucket->replacement = (char *)malloc(strlen(r_string)+1);
  strcpy(bucket->original, o_string);
  strcpy(bucket->replacement, r_string);
  bucket->next = Hash_table[index];
  Hash_table[index] = bucket;
  }
}

/* This routine stuffs the substitution pairs into the hash table
 * It supposes that identifiers are at most SYMBOL_SIZE chars long
 * It will only be called if the programme is reading substitution
 * pairs.
 */
void init_csubst()
{
int i;

for(i = 0; i < HASHSIZE; i++)
   {
   Hash_table[i] = NULL;
   }

O_string_buffer[SYMBOL_SIZE] = '\0'; /* to make sure you can print it */
}

/* this function returns null if o_string is not to be replaced
   otherwise it returns the repalcement string.
 */
char *lookup_subst(o_string)
char *o_string;
{
int index;
struct substitution *bucket;
index = hash(o_string);
bucket = Hash_table[index];
while(bucket != NULL && strcmp(o_string, bucket->original))
     {
     bucket = bucket->next;
     }
if(bucket == NULL)
  return NULL;
else
  return bucket->replacement;
}
/* end of file */
