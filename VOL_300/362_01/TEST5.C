/*****************************************************************************
 *				    test5.c				     *
 *****************************************************************************
 * DESCRIPTION:	Verification routine for the RMAXTask multitasking system.   *
 *									     *
 *		Simple mailbox check.  Happens to test queueing of message   *
 *		at a mailbox.						     *
 *									     *
 * REVISIONS:	 3 JUL 90 - RAC - Original code				     *
 *****************************************************************************/

#include	<dos.h>
#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	"\rmaxtask\source\rmaxtask.h"

TDESC	*my_mailbox;

int	break_handler() {
    stop_RMAXTask();				/* Do 'about to die' cleanup */
    printf("\nCtrl-Break pressed.  Program terminated.\n");
    return 0;					/* This will abort program */
    }						/* End break_handler() */

void printer() {

    char	message[50];
    unsigned	size;
    
    for(;;) {
	get_mail(message, &size, my_mailbox, 0, 0);
	printf("Message: %s\n",   message);
	printf("Size:    %d\n\n", size);
	}    
    }

void sender() {

    int		i;
    char	message[50];

    send_mail("1", 1 + strlen("1"), my_mailbox);
    send_mail("22", 1 + strlen("22"), my_mailbox);
    send_mail("333", 1 + strlen("333"), my_mailbox);
    send_mail("22", 1 + strlen("22"), my_mailbox);
    send_mail("1", 1 + strlen("1"), my_mailbox);
    for (i=1; i<10000; i *= 10) {
	sprintf(message, "i = %d", i);
	send_mail(message, 1 + strlen(message), my_mailbox);
	}
    yield();
    stop_RMAXTask();				/*  a key is pressed */
    exit(0);
    }

void main() {

    int		i;				/* Generic temp */

    start_RMAXTask();
    ctrlbrk(break_handler);			/* Establish break handler */
    my_mailbox = create_mailbox();
    create_task("sender", sender, STANDARD_PRIORITY, STANDARD_STACK);
    create_task("printer", printer, STANDARD_PRIORITY, STANDARD_STACK);
    wait_key(0);
    }
