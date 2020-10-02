/*
   DFA.H - dfa.c prototypes, FLEX project 1/89 DrH
*/

extern  int *epsclosure( int *, int *, int [], int *, int *) ;
extern  void increase_max_dfas( void) ;
extern  int snstods( int [], int, int [], int, int, int * ) ;
extern  int symfollowset( int [], int, int, int[] ) ;
extern  void sympartition( int [], int, int [], int [] ) ;

