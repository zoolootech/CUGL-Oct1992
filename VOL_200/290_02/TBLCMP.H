/*
    TBLCMP.H    - tblcmp.c prototypes, FLEX project 1/89 DrH
*/


extern  void bldtbl(int *state,int statenum,int totaltrans,int comstate,int comfreq);
extern  void cmptmps(void );
extern  void expand_nxt_chk(void );
extern  int find_table_space(int *state,int numtrans);
extern  void genctbl(void );
extern  void gentabs(void );
extern  void genecs(void );
extern  void inittbl(void );
extern  void make_tables(void );
extern  void mkdeftbl(void );
extern  void mkentry(int *state,int numchars,int statenum,int deflink,int totaltrans);
extern  void mk1tbl(int state,int sym,int onenxt,int onedef);
extern  void mkprot(int *state,int statenum,int comstate);
extern  void mktemplate(int *state,int statenum,int comstate);
extern  void mv2front(int qelm);
extern  void ntod(void );
extern  void place_state(int *state,int statenum,int transnum);
extern  void stack1(int statenum,int sym,int nextstate,int deflink);
extern  int tbldiff(int *state,int pr,int *ext);

/* tblcmp.h */
