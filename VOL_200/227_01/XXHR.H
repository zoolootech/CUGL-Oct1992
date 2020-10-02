/*
 * 	x x h r . h
 *	-----------
 *	Low-level driver for the Elsa PC XHR - DXHR and XXHR vector
 *	graphics board.
 *	Only very little features of this boards are supported, so this
 *	low-level driver is in fact only useful for tests. Expand it, if
 *	you like to use this boards.
 */

/*
 * definiton of hardware-dependent constants:
 */
#define GREEN    1			/* color green */
#define MAX_X    1280			/* Num. x-pixel */
#define MAX_Y    1024			/* Num. y-pixel */
#define X_LOW    0			/* lowest x valid coordinate */
#define X_HIGH   1279			/* highest x valid coordinate */
#define Y_LOW    0			/* lowest y valid coordinate */
#define Y_HIGH   1023			/* highest y valid coordinate */
#define XXHRGRAF 1			/* XXHR library */

#define	XXHRINT  0x64			/* driver interrupt		*/

#define	exitgraf(x)
