/**
*                           e g a . h
*                           ---------
**/

/**
* definiton of hardware-dependent constants:
**/
#define GREEN    1                          /* color green */
#define MAX_X    640                        /* Num. x-pixel */
#define MAX_Y    350                        /* Num. y-pixel */
#define X_LOW    0                          /* lowest x valid coordinate */
#define X_HIGH   639                        /* highest x valid coordinate */
#define Y_LOW    0                          /* lowest y valid coordinate */
#define Y_HIGH   349                        /* highest y valid coordinate */
#define BYGRAFP  128 * 1024L                /* Num. bytes for graphics page */
#define BYTEXTP  80 * 25 * 2                /* Num. bytes for text page */
#define EGAGRAF  1                          /* EGA library */

#define initgraf(x) setmode(0xf)            /* for compatibility with the */
#define exitgraf(x) setmode(7)              /* previous hercules library. */

#ifdef	USEPROTT
extern void	setewm2(void);
extern void	rsestdwm(void);
extern void	setbyte(int, int, int, int);
extern int	qgetpix(int, int);
extern int	qsetpix(int, int, int);
#endif
