/*
**	file:		bison.h
**	purpose:	Bison's Main header
**	author:		DrH 1/4/88 [bison]
*/

/*
**	Definitions
*/
#define TRUE 1
#define FALSE 0

/*
**	Prototypes
*/
extern  void fatal(char *s);
/* extern  void fatals(char *fmt,int x1,int x2,int x3,int x4,
                        int x5,int x6,int x7,int x8);
*/
extern  void fatals(char *fmt, ... ) ;
extern  void toomany(char *s);
extern  void berror(char *s);
