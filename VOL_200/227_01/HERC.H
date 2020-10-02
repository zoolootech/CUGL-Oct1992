/*
 * h e r c . h
 * -----------
 * Low-level header file for the hercules monochrome graphics card.
 * Defines all hardware-specific parameters. Does a #define HERCGRAF 1
 * to identify hercules mode.
 *
 * update history
 * --------------
 * May, 28. 1987     compatibility macro "setmode()" added.
 *
 * Written by        Rainer Gerhards
 *                   Petronellastr. 6
 *                   D-5112 Baesweiler
 *                   West Germany
 *                   Phone (49) 2401 - 1601
 */

/*
 * definiton of hardware-dependent constants:
 */
#define GREEN    1                          /* color green */
#define MLXPORT  0x03bf                     /* Crt. Port */
#define CRTPORT  0x03b8                     /* Enable/Disable Board Port */
#define HERCGRAF 1                          /* Hercules-library */
#define MAX_X    720                        /* Pixelanzahl in x-Richtung */
#define MAX_Y    348                        /* Pixelanzahl in y-Richtung */
#define X_LOW    0                          /* lowest x coordinate */
#define X_HIGH   719                        /* highest x coordinate */
#define Y_LOW    0                          /* lowest y coordinate */
#define Y_HIGH   347                        /* highest y coordinate */
#define BYGRAFP  0x8000                     /* Num. bytes for graphics page */
#define BYTEXTP  80 * 25                    /* Num. bytes for text page */
#define TEXTMODE 0                          /* textmode id */
#define GRAFMODE 6                          /* graphics mode id */

/*
 * for compatibility with the other graphics devices
 */
#define setmode(x) if((((x) >= 0x04) && ((x) <= 0x06)) || \
                      (((x) >= 0x0d) && ((x) <= 0x10))    \
                     )                                    \
                     initgraf(-1);                        \
                   else                                   \
                     exitgraf(-1);
#ifdef	LIB_MODE
	extern void	setbyte(int, int, int);
#endif