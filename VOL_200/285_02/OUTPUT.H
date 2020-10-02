/*
    OUTPUT.H header
    [bison]
    DrH 1/4/88
*/
extern  void output_headers(void );
extern  void output_trailers(void );
extern  void output(void );
extern  void output_token_translations(void );
extern  void output_gram(void );
extern  void output_stos(void );
extern  void output_rule_data(void );
extern  void output_defines(void );
extern  void output_actions(void );
extern  void token_actions(void );
extern  int action_row(int state);
extern  void save_row(int state);
extern  void goto_actions(void );
extern  int default_goto(int symbol);
extern  void save_column(int symbol,int default_state);
extern  void sort_actions(void );
extern  void pack_table(void );
extern  int matching_state(int vector);
extern  int pack_vector(int vector);
extern  void output_base(void );
extern  void output_table(void );
extern  void output_check(void );
extern  void output_parser(void );
extern  void output_program(void );
extern  void free_itemsets(void );
extern  void free_shifts(void );
extern  void free_reductions(void );

