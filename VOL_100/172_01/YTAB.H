
typedef union  {
    char   *buff;
    struct nlist *list;
    struct des   *des_ptr;
    struct nfa   *elem_ptr;
} YYSTYPE;
extern YYSTYPE yylval;
#define NAME 257
#define CCLASS 258
#define STRING 259
#define CONCAT 260
