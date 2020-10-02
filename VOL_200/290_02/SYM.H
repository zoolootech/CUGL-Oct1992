/*
   SYM.H - sym.c prototypes, FLEX project 1/89 DrH
*/

extern  int addsym(char *sym,char *str_def,int int_def,struct hash_entry * *table,int table_size);
extern  void cclinstal(char *ccltxt,int cclnum);
extern  int ccllookup(char *ccltxt);
extern  struct hash_entry *findsym(char *sym,struct hash_entry * *table,int table_size);
extern  int hashfunct(char *str,int hash_size);
extern  void ndinstal(char *nd,char *def);
extern  char *ndlookup(char *nd);
extern  void scinstal(char *str,int xcluflg);
extern  int sclookup(char *str);

