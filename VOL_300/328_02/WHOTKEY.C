/* whotkey.c
 * 		This routine installs HOTKEY event handlers.
 *		Method: for each new hotkey, allocate a structure in a linked list
 *			the list element describes the hotkey, the function and the 
 *				flags to eliminate reentrant calls to the functions.
 *		For each keystroke, run through the list to test for hotkeys.
 *
 */
#include "wsys.h"


struct WHOTKEY_st
	{
	void (*whk_func)(void);
	int whk_val;
	int whk_reentrant;
	struct WHOTKEY_st *whk_next;	
	};

typedef struct WHOTKEY_st WHOTKEY;



/*pointer to hot key traps not installed using this routine, ie, menus.
 */
static int (*prior_handler)(int) = NULL;

static char first_call =1;

static int handler (int);

/*end of chain of WHOTKEYS
 */
static WHOTKEY *anchor = NULL;




void whotkey_install ( int key, void (*func)(void) )
	{
	WHOTKEY *hk;
	
	if ( first_call )
		{
		first_call = 0;
		prior_handler = wkeytrap;
		wkeytrap = handler;
		} 
		
	hk = wmalloc ( sizeof (WHOTKEY), "HOTKEY");
		
	/*construct linked list
	 */
	hk -> whk_next = anchor;
	anchor = hk;
	
	hk -> whk_reentrant = 0;
	hk -> whk_func       = func;
	hk -> whk_val        = key;
		
	return;		/* end whotkey_install */
		
	}
	
	
static int handler (int key)
	{
	WHOTKEY *hk;
	
	
	for (hk = anchor; hk != NULL; hk = hk->whk_next )
		{
		if ( key == hk->whk_val   &&  ! (hk-> whk_reentrant) )
			{
			hk-> whk_reentrant = 1;
			(*(hk->whk_func))();
			hk-> whk_reentrant = 0;
			key = 0;			/* swallow keystroke (its a hotkey)*/
			break;				/* exit for() loop early */
			}
		}
	
	if ( prior_handler != NULL )
			{
			key = (*prior_handler)(key);	
			}
	
	return (key);
	}
