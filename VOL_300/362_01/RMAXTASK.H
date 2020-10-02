/*****************************************************************************
 *				  rmaxtask.h				     *
 *****************************************************************************
 * DESCRIPTION: This file contains data structure definitions and function   *
 *		prototypes for the RMAXTask multitasking package.	     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original code				     *
 *		10 DEC 91 - RAC - Added kill_task() and changed create_task  *
 *				  to return a pointer to the created task    *
 *				  instead of a status code.		     *
 *****************************************************************************/

/*****************************************************************************
 *			  Data Structure Definitions			     *
 *****************************************************************************/

#ifndef _RMAXTASK
#define _RMAXTASK

#define TDESC	struct task_descriptor

TDESC {					/* The Task Descriptor */
    char	task_name[20];		/* Text task name for debugging */
    TDESC	*prev_wait;		/* Links to other tasks waiting at a */
    TDESC	*next_wait;		/*  mailbox or semaphore */
    TDESC	*prev_time;		/* Links to other tasks waiting on */
    TDESC	*next_time;		/*  the timer */
    long	timeout;		/* Timer expiration time */
    unsigned	priority;		/* 0 = lowest, 0xFFFF = highest */
    char	*stack_bottom;		/* Beginning of memory for stack */
    char	far *my_stack;		/* Far pointer to this task's stack */
    unsigned	my_BP;			/* Saved BP register */
    unsigned	my_SI;			/* Saved SI register */
    unsigned	my_DI;			/* Saved DI register */
    unsigned	wait_status;		/* Status of last wait operation, or */
					/*  semaphore count */
    void	*msg;			/* Pointer to place to put mail, or */
					/*  to oldest queued mailbox message */
    unsigned	*mail_size;		/* Pointer to place to put mail size */
    };

#define MQE struct msg_queue_entry

MQE {
    MQE		*next_msg;		/* Link to next message in mailbox */
    unsigned	msg_size;		/* Size of this message */
    char	*msg_text;		/* Pointer to message text */
    };    

/*****************************************************************************
 *			      Manifest Constants			     *
 *****************************************************************************/

#define TIMER_PRIORITY		0xF000	/* Timer task is very high priority */
#define STANDARD_PRIORITY	100	/* Priority of Joe Average task */
#define STANDARD_STACK		2000	/* Stack size for Joe Average task */

enum { OKAY, TIMEOUT };			/* Wait return status values */

/*****************************************************************************
 *			      Function Prototypes			     *
 *****************************************************************************/

/*****************************************************************************
 *			       start_RMAXTask()				     *
 *****************************************************************************
 * DESCRIPTION:	Initializes the RMAXTask software.  This function must be    *
 *		called before any other RMAXTask functions are used.	     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

int start_RMAXTask(void);		/* 1 if all is well, 0 if error */

/*****************************************************************************
 *			        stop_RMAXTask()				     *
 *****************************************************************************
 * DESCRIPTION:	Cleans up after the RMAXTask software.  Must be called	     *
 *		before the application returns to DOS.			     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

void stop_RMAXTask(void);

/*****************************************************************************
 *				 create_task()				     *
 *****************************************************************************
 * DESCRIPTION: Establishes a specified C function as an RMAXTask task and   *
 *		makes it READY.						     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *		10 DEC 91 - RAC - Changed return value from simple error     *
 *				   code to a pointer to the created task.    *
 *****************************************************************************/

TDESC *create_task(			/* Pointer to task, or NULL if error */
    char	*name,			/* Pointer to text name for task */
    void	(*fn)(),		/* Ptr to the code for the task */
    unsigned	priority,		/* Desired task priority */
    unsigned	ssize			/* # of bytes for task's stack */
    ); 

/*****************************************************************************
 *				  kill_task()				     *
 *****************************************************************************
 * DESCRIPTION:	Allows one task to murder another.			     *
 *									     *
 * NOTES:	This function must be used with extreme care to make sure    *
 *		that the task being killed has his affairs in order.  He     *
 *		should not "own" any dynamic memory, he should not have any  *
 *		open files, etc.					     *
 *									     *
 *		A task should NOT use this function to kill itself.  Tasks   *
 *		wishing to commit suicide should do so by simply returning   *
 *		from themselves, either by an explicit return or by simply   *
 *		running off the end of themselves.  Tasks contemplating      *
 *		suicide should also make sure their affairs are in order     *
 *		as noted above.						     *
 *									     *
 * REVISIONS:	10 DEC 91 - RAC - Adapted from auto_kill()		     *
 *****************************************************************************/

void kill_task(TDESC *victim);			/* Kill victim */

/*****************************************************************************
 *				    yield()				     *
 *****************************************************************************
 * DESCRIPTION: Makes the calling task READY and then invokes the scheduler. *
 *		If a task plans to run for a long time without either	     *
 *		waiting for an event or going to sleep, it should call this  *
 *		routine periodically to allow higher-priority tasks the      *
 *		chance to run.						     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

void yield(void);

/*****************************************************************************
 *				   suspend()				     *
 *****************************************************************************
 * DESCRIPTION: Suspends a task for a given period of time.  The time is     *
 *		given in BIOS clock ticks, which occur at about 18.2 Hz.     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

void suspend(
    long	ticks			/* Sleep for <ticks> clock ticks */
    );

/*****************************************************************************
 *			       create_mailbox()				     *
 *****************************************************************************
 * DESCRIPTION: Creates a mailbox.					     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

TDESC *create_mailbox(void);		/* Ptr to new mailbox, NULL if error */

/*****************************************************************************
 *				  send_mail()				     *
 *****************************************************************************
 * DESCRIPTION: Sends a message to a mailbox.				     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

int send_mail(				/* 1 if all is well, 0 if error */
    void	*msg,			/* Pointer to message to send */
    unsigned	size,			/* Size of message to send */
    TDESC	*mailbox		/* Pointer to destination mailbox */
    );

/*****************************************************************************
 *				  get_mail()				     *
 *****************************************************************************
 * DESCRIPTION:	Waits at a mailbox for mail or a timeout.		     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *		29 NOV 90 - RAC - Added 'reverse' parameter.		     *
 *****************************************************************************/

void get_mail(
    void	*msg,			/* Where to put result */
    unsigned	*size,			/* Where to put result size */
    TDESC	*mailbox,		/* Pointer to mailbox to use */
    long	timeout,		/* Give up this many ticks from now */
    int		reverse			/* Reverse normal queueing order */
    );

/*****************************************************************************
 *				 check_mail()				     *
 *****************************************************************************
 * DESCRIPTION: Tells if a particular mailbox has mail or not.		     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original code				     *
 *****************************************************************************/

int check_mail(				/* 1 if there is mail, else 0 */
    TDESC	*mailbox		/* Ptr to mailbox of interest */
    );

/*****************************************************************************
 *				 get_status()				     *
 *****************************************************************************
 * DESCRIPTION:	For the current task, returns the status of the most recent  *
 *		call to suspend(), get_mail(), wait_key(), or wait_sem().    *
 *									     *
 * NOTE:	This routine will always return TIMEOUT following a call to  *
 *		suspend().						     *
 *									     *
 * REVISIONS:	 3 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

int get_status(void);			/* TIMEOUT or OKAY */

/*****************************************************************************
 *				 create_sem()				     *
 *****************************************************************************
 * DESCRIPTION:	Creates a semaphore.					     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

TDESC *create_sem(			/* Ptr to semphore, or NULL if error */
    unsigned	count			/* Desired initial count */
    );

/*****************************************************************************
 *				 signal_sem()				     *
 *****************************************************************************
 * DESCRIPTION: Signals a semaphore.					     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

void signal_sem(
    TDESC	*semaphore		/* Ptr to semaphore of interest */
    );

/*****************************************************************************
 *				 wait_sem()				     *
 *****************************************************************************
 * DESCRIPTION: Waits for a semaphore.					     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

void wait_sem(
    TDESC	*semaphore,		/* Ptr to semaphore of interest */
    long	timeout			/* Give up this many ticks from now */
    );

/*****************************************************************************
 *				  check_sem()				     *
 *****************************************************************************
 * DESCRIPTION: Returns a semaphore count.				     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

unsigned check_sem(			/* The semaphore count */
    TDESC	*semaphore		/* Ptr to semaphore of interest */
    );

/*****************************************************************************
 *				  wait_key()				     *
 *****************************************************************************
 * DESCRIPTION:	Waits for a keystroke or a timeout.			     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original definition.			     *
 *****************************************************************************/

int wait_key(				/* The keystroke, or '\0' if timeout */
    long	timeout			/* Give up this many ticks from now */
    );

/*****************************************************************************
 *				   key_hit()				     *
 *****************************************************************************
 * DESCRIPTION: Tells if keystroke is available or not.			     *
 *									     *
 * REVISIONS:	 1 JUL 90 - RAC - Original code				     *
 *****************************************************************************/

int key_hit(void);			/* 1 if there is a key, else 0 */

///////////////////////////////////////////////////////////////////////////////
//				  fake_key()				     //
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:	Allows a task to simulate a keystroke from the keyboard.     //
//									     //
// REVISIONS:	 8 OCT 90 - RAC - Original code				     //
///////////////////////////////////////////////////////////////////////////////

void fake_key(int key);

/*****************************************************************************
 *				  RMAX_time()				     *
 *****************************************************************************
 * DESCRIPTION: Returns the value of rmax_tick, which starts at zero and is  *
 *		bumped at about 18.2 Hz forever thereafter.  (It's a long    *
 *		integer and won't wrap around for years (literally)).  Use   *
 *		this function instead of the BIOS ticks to avoid the dreaded *
 *		"midnight death" syndrome.				     *
 *									     *
 * REVISIONS:	11 OCT 91 - RAC - Original code				     *
 *****************************************************************************/

long RMAX_time();

#endif
