/*
   NFA.H - nfa.c prototypes, FLEX project 1/89 DrH
*/

extern  void add_accept( int, int, int ) ;
extern  int copysingl( int, int ) ;
extern  void dumpnfa( int) ;
extern  int dupmachine( int ) ;
extern  int link_machines( int, int ) ;
extern  int mkbranch( int, int) ;
extern  int mkclos( int state ) ;
extern  int mkopt( int mach ) ;
extern  int mkor( int first, int second ) ;
extern  int mkposcl( int state ) ;
extern  int mkrep( int mach, int lb, int ub ) ;
extern  int mkstate( int sym ) ;
extern  void mkxtion( int statefrom, int stateto ) ;

