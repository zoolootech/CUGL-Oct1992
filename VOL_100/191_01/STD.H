/*#title STD.H	03/15/86 */

#define FALSE    0
#define TRUE     !FALSE
#define NUL      0
#define FOREVER	 while (TRUE)
#define XX       0
#define CR       0x0D
#define LF       0x0A

typedef char     BOOLEAN;
typedef unsigned char BYTE;
typedef unsigned WORD;
typedef int      (*FUNPTR)();
