/*****************************************************************************
 *				    test7.c				     *
 *****************************************************************************
 * DESCRIPTION:	Verification routine for the RMAXTask multitasking system.   *
 *									     *
 *		Keyboard tests						     *
 *									     *
 * REVISIONS:	 3 JUL 90 - RAC - Original code				     *
 *****************************************************************************/

#include	"\rmaxtask\source\rmaxtask.h"
#include	<stdio.h>
#include	<dos.h>

int	break_handler() {
    stop_RMAXTask();				/* Do 'about to die' cleanup */
    printf("\nCtrl-Break pressed.  Program terminated.\n");
    return 0;					/* This will abort program */
    }						/* End break_handler() */

void main() {

    int		i;				/* Generic temp */

    start_RMAXTask();
    ctrlbrk(break_handler);			/* Establish break handler */
    printf("Waiting forever for keystroke ...\n");
    printf("%c hit\n", wait_key(0));
    printf("Repeatedly wait 5 seconds for key.  Press ESC to continue\n");
    for (;;) {
	i = wait_key(91);
	if (i) printf("%c hit\n", i);
	else   printf("TIMEOUT\n");
	if (i == 0x011b) break;
	}
    printf("Waiting forever for keystroke ...\n");
    while (!key_hit()) yield();
    printf("Test complete!\n");    
    stop_RMAXTask();
    }
