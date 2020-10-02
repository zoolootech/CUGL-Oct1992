/*
    LALR.H header
    [bison]
    DrH 1/4/88
*/

extern  void lalr(void );
extern  void set_state_table(void );
extern  void set_accessing_symbol(void );
extern  void set_shift_table(void );
extern  void set_reduction_table(void );
extern  void set_maxrhs(void );
extern  void initialize_LA(void );
extern  void set_goto_map(void );
extern  int map_goto(int state,int symbol);
extern  void initialize_F(void );
extern  void build_relations(void );
extern  void add_lookback_edge(int stateno,int ruleno,int gotono);
extern  short * *transpose(short * *R,int n);
extern  void compute_FOLLOWS(void );
extern  void compute_lookaheads(void );
extern  void digraph(short * *relation);
extern  void traverse(int i);
