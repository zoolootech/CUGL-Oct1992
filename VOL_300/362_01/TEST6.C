/*****************************************************************************
 *				    test6.c				     *
 *****************************************************************************
 * DESCRIPTION:	Verification routine for the RMAXTask multitasking system.   *
 *									     *
 *		Test of several tasks queueing up at the same mailbox.	     *
 *									     *
 * REVISIONS:	 3 JUL 90 - RAC - Original code				     *
 *****************************************************************************/

#include	<stdio.h>
#include	<dos.h>
#include	<string.h>
#include	"\rmaxtask\source\rmaxtask.h"

TDESC	*my_mailbox;

int	break_handler() {
    stop_RMAXTask();				/* Do 'about to die' cleanup */
    printf("\nCtrl-Break pressed.  Program terminated.\n");
    return 0;					/* This will abort program */
    }						/* End break_handler() */

void A() {
    char	message[50];
    unsigned	size;
    for(;;) {
	get_mail(message, &size, my_mailbox, 0, 0);
	printf("Task A got message %s\n",   message);
	}    
    }

void B() {
    char	message[50];
    unsigned	size;
    for(;;) {
	get_mail(message, &size, my_mailbox, 0, 0);
	printf("Task B got message %s\n",   message);
	}    
    }

void C() {
    char	message[50];
    unsigned	size;
    for(;;) {
	get_mail(message, &size, my_mailbox, 0, 0);
	printf("Task C got message %s\n",   message);
	}    
    }

void sender() {

    int		i;
    char	message[50];

    for (i=1; i<20; i++) {
	sprintf(message, "%d", i);
	send_mail(message, 1 + strlen(message), my_mailbox);
	}
    }

void main() {

    int		i;				/* Generic temp */

    start_RMAXTask();
    ctrlbrk(break_handler);			/* Establish break handler */
    my_mailbox = create_mailbox();
    create_task("sender", sender, STANDARD_PRIORITY-1, STANDARD_STACK);
    create_task("A", A, STANDARD_PRIORITY, STANDARD_STACK);
    create_task("B", B, STANDARD_PRIORITY, STANDARD_STACK);
    create_task("C", C, STANDARD_PRIORITY, STANDARD_STACK);
    suspend(38);
    stop_RMAXTask();
    }
