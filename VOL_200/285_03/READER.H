/*
    READER.H header
    [bison]
    DrH 1/4/88
*/
extern  void reader(void );
extern  void read_declarations(void );
extern  void copy_definition(void );
extern  void parse_token_decl(int what_is,int what_is_not);
extern  void parse_start_decl(void );
extern  void parse_type_decl(void );
extern  void parse_assoc_decl(int assoc);
extern  void parse_union_decl(void );
extern  void parse_expect_decl(void );
extern  void output_ltype(void );
extern  char *get_type_name(int n,struct symbol_list *rule);
extern  void copy_guard(struct symbol_list *rule,int stack_offset);
extern  void copy_action(struct symbol_list *rule,int stack_offset);
extern  struct bucket *gensym(void );
extern  void readgram(void );
extern  void record_rule_line(void );
extern  int get_type(void );
extern  void packsymbols(void );
extern  void output_token_defines(FILE *file);
extern  void packgram(void );
extern  int read_signed_integer(FILE *stream);
