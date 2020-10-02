/*
    PRINT.H header
    [bison]
    DrH 1/4/88
*/
extern  void terse(void );
extern  void verbose(void );
extern  void print_token(int extnum,int token);
extern  void print_state(int state);
extern  void print_core(int state);
extern  void print_actions(int state);
