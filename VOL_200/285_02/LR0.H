/*
    LR0.H header
    [bison]
    DrH 1/4/88
*/
extern  void allocate_itemsets(void );
extern  void allocate_storage(void );
extern  void free_storage(void );
extern  void generate_states(void );
extern  void new_itemsets(void );
extern  void append_states(void );
extern  int get_state(int );
extern  struct core *new_state(int );
extern  void initialize_states(void );
extern  void save_shifts(void );
extern  void save_reductions(void );
extern  void augment_automaton(void );
extern  void insert_start_shift(void );
