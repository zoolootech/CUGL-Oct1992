/**
*                           b i o s . h
*                           -----------
* Hardware-specific definitions for the standrad BIOS driver.
* Note, that the listed limits are not common to all video-modes!
**/

/**
* definiton of hardware-dependent constants:
**/
#define GREEN    1                          /* color green */
#define MAX_X    640                        /* Num. x-pixel */
#define MAX_Y    200                        /* Num. y-pixel */
#define X_LOW    0                          /* lowest x valid coordinate */
#define X_HIGH   639                        /* highest x valid coordinate */
#define Y_LOW    0                          /* lowest y valid coordinate */
#define Y_HIGH   199                        /* highest y valid coordinate */
#define BYGRAFP  16 * 1024L                 /* Num. bytes for graphics page */
#define BYTEXTP  80 * 25  * 2               /* Num. bytes for text page */

#define initgraf(x) setmode(0xf)            /* for compatibility with the */
#define exitgraf(x) setmode(7)              /* previous hercules library. */
