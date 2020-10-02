/*
    CONFLICT.H headers
    [bison]
    DrH 1/4/88
*/
extern  void initialize_conflicts(void );
extern  void set_conflicts(int state);
extern  void resolve_sr_conflict(int state,int lookaheadnum);
extern  void flush_shift(int state,int token);
extern  void log_resolution(int state,int LAno,int token,char *resolution);
extern  void conflict_log(void );
extern  void verbose_conflict_log(void );
extern  void total_conflicts(void );
extern  void count_sr_conflicts(int state);
extern  void count_rr_conflicts(int state);
extern  void print_reductions(int state);
extern  void finalize_conflicts(void );
