/*
** include sgtty.h
**
** data structure for _ioctl()
*/
 
/*
** sgttyb structure
*/
#define SG_SIZE        6
#define SG_ISPEED      0       /* input speed */
#define SG_OSPEED      1       /* output speed */
#define SG_ERASE       2       /* character erase character */
#define SG_KILL        3       /* line kill character */
#define SG_FLGS        4       /* control flags (two bytes) */
 
/*
** _ioctl() request codes
*/
#define TIOCGETP       0       /* materialize sgttyb structure */
#define TIOCSETP       1       /* modify sgttyb structure */
