#define NOCCARGC  /* no argument count passing */
extern char *zzmem;
/*
** free(ptr) - Free previously allocated memory block.
** Memory must be freed in the reverse order from which
** it was allocated.
** ptr    = Value returned by calloc() or malloc().
** Returns ptr if successful or NULL otherwise.
*/
free(ptr) char *ptr; {
   return (zzmem = ptr);
   }
#asm
cfree  equ    free
       entry  cfree
#endasm
