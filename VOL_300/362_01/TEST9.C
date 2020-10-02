/*****************************************************************************
 *				    test9.c				     *
 *****************************************************************************
 * DESCRIPTION:	Verification routine for the RMAXTask multitasking system.   *
 *									     *
 *		Simple mailbox check.  Happens to test queueing of message   *
 *		at a mailbox.						     *
 *									     *
 * REVISIONS:	 3 JUL 90 - RAC - Original code				     *
 *****************************************************************************/

#include	<dos.h>
#include	<stdio.h>
#include	"\rmaxtask\source\rmaxtask.h"

TDESC	*mb_1_2;
TDESC	*mb_2_3;
TDESC	*mb_3_4;
TDESC	*mb_4_5;
TDESC	*mb_5_1;

int	break_handler() {
    stop_RMAXTask();				/* Do 'about to die' cleanup */
    printf("\nCtrl-Break pressed.  Program terminated.\n");
    return 0;					/* This will abort program */
    }						/* End break_handler() */

void task1() {
    unsigned	size;
    char	message[50];
    for (;;) {
	get_mail (message, &size, mb_5_1, 0, 0);
	printf("Task 1 got mail\n");
	send_mail(message, size, mb_1_2);
	}    
    }

void task2() {
    unsigned	size;
    char	message[50];
    for (;;) {
	get_mail (message, &size, mb_1_2, 0, 0);
	printf("Task 2 got mail\n");
	send_mail(message, size, mb_2_3);
	}    
    }

void task3() {
    unsigned	size;
    char	message[50];
    for (;;) {
	get_mail (message, &size, mb_2_3, 0, 0);
	printf("Task 3 got mail\n");
	send_mail(message, size, mb_3_4);
	}    
    }

void task4() {
    unsigned	size;
    char	message[50];
    for (;;) {
	get_mail (message, &size, mb_3_4, 0, 0);
	printf("Task 4 got mail\n");
	send_mail(message, size, mb_4_5);
	}    
    }

void task5() {
    unsigned	size;
    char	message[50];
    for (;;) {
	get_mail (message, &size, mb_4_5, 0, 0);
	printf("Task 5 got mail\n");
	send_mail(message, size, mb_5_1);
	}    
    }

void main() {

    int		i;				/* Generic temp */

    start_RMAXTask();
    ctrlbrk(break_handler);			/* Establish break handler */
    mb_1_2 = create_mailbox();
    mb_2_3 = create_mailbox();
    mb_3_4 = create_mailbox();
    mb_4_5 = create_mailbox();
    mb_5_1 = create_mailbox();
    create_task("task1", task1, STANDARD_PRIORITY, STANDARD_STACK);
    create_task("task2", task2, STANDARD_PRIORITY, STANDARD_STACK);
    create_task("task3", task3, STANDARD_PRIORITY, STANDARD_STACK);
    create_task("task4", task4, STANDARD_PRIORITY, STANDARD_STACK);
    create_task("task5", task5, STANDARD_PRIORITY, STANDARD_STACK);
    send_mail("Hello, there!\n", 15, mb_5_1);
    suspend(91);
    stop_RMAXTask();
    }
