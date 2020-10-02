/*****************************************************************************
 *				    test8.c				     *
 *****************************************************************************
 * DESCRIPTION:	Verification routine for the RMAXTask multitasking system.   *
 *									     *
 *		Tests semaphores.					     *
 *									     *
 * REVISIONS:	 3 JUL 90 - RAC - Original code				     *
 *****************************************************************************/

#include	"\rmaxtask\source\rmaxtask.h"
#include	<stdio.h>
#include	<dos.h>

TDESC	*my_semaphore;

int	break_handler() {
    stop_RMAXTask();				/* Do 'about to die' cleanup */
    printf("\nCtrl-Break pressed.  Program terminated.\n");
    return 0;					/* This will abort program */
    }						/* End break_handler() */
    
void alpha() {
    for (;;) {					/* Repeat forever */
	wait_sem(my_semaphore, 0);
	printf("Alpha\n");			/* Print something */
	}    					/* End 'repeat forever' */
    }						/* End alpha() */

void beta() {
    for (;;) {					/* Repeat forever */
	wait_sem(my_semaphore, 0);
	printf("Beta\n");			/* Print something */
	}    					/* End 'repeat forever' */
    }						/* End alpha() */

void main() {

    start_RMAXTask();
    ctrlbrk(break_handler);			/* Establish break handler */
    my_semaphore = create_sem(5);
    create_task("alpha", alpha, STANDARD_PRIORITY, STANDARD_STACK);
    create_task("beta", beta,  STANDARD_PRIORITY, STANDARD_STACK);
    for (;;) {
	wait_key(0);
	signal_sem(my_semaphore);
	}    
    }
