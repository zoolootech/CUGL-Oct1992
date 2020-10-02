/*****************************************************************************
 *				    test4.c				     *
 *****************************************************************************
 * DESCRIPTION:	Verification routine for the RMAXTask multitasking system.   *
 *									     *
 *		Check of timeouts on get_mail(), etc.			     *
 *									     *
 * REVISIONS:	 3 JUL 90 - RAC - Original code				     *
 *****************************************************************************/

#include	<dos.h>
#include	<stdio.h>
#include	"\rmaxtask\source\rmaxtask.h"

TDESC	*my_mailbox;
TDESC	*my_semaphore;

int	break_handler() {
    stop_RMAXTask();				/* Do 'about to die' cleanup */
    printf("\nCtrl-Break pressed.  Program terminated.\n");
    return 0;					/* This will abort program */
    }						/* End break_handler() */

void tt() {

    unsigned	i;

    for (;;) {
	wait_sem(my_semaphore, 18);
	printf("TICK ");
	get_mail(&i, &i, my_mailbox, 18, 0);
	printf("TOCK ");
	}
    }    
    
void main() {

    int		i;				/* Generic temp */

    start_RMAXTask();
    ctrlbrk(break_handler);			/* Establish break handler */
    my_mailbox = create_mailbox();
    my_semaphore = create_sem(0);
    create_task("tt", tt, STANDARD_PRIORITY, STANDARD_STACK);
    for (i=1; i<=8; i++) {
	printf("\nSuspending for %d seconds ", i);
	wait_key(18 * i);
	}    
    stop_RMAXTask();				/*  a key is pressed */
    printf("\nDone with main task.\n");
    }
