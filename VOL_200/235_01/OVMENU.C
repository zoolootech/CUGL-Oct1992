/*  007  28-Jan-87  ovmenu.c

        Copyright (c) 1987 by Blue Sky Software.  All rights reserved.
*/

#include <stdio.h>
#include "ov.h"

MENU *save_top;
MENU_STATE curmenu, savmenu;
static int force_selection = FALSE;

extern MENU *top_menu;


/******************************************************************************
                      M E N U _ S A V E / R E S T O R E
 ******************************************************************************/

menu_save() {          /* save the current menu state */

   savmenu = curmenu;
   save_top = top_menu;
}

menu_restore() {       /* restore saved menu state */

   curmenu = savmenu;
   top_menu = save_top;
   force_selection = TRUE;
}


/******************************************************************************
 **                       M E N U _ I N I T                                  **
 *****************************************************************************/

menu_init() {          /* initialize the menu */

   menu_display(curmenu.current_menu = top_menu); /* display the initial menu */
   menu_select(curmenu.current_selection = 0);    /* select the first option */
   menu_prompt(curmenu.current_selection);        /* display selections prompt */
}


/******************************************************************************
 **                      M E N U _ D I S P L A Y                             **
 *****************************************************************************/

menu_display(menu)     /* display a menu */
register MENU *menu;
{
   int col = 0;
   register int i;

   gotorc(MENU_ROW,0);

   /* display each of the menu selections, for each one, remember where it
      is on the menu row and how long it is.  This info is used by the
      menu_select/deselect functions.  The end of the menu is denoted by
      a set of NULL entries,  finished when this entry is found.  Put the
      number of selections for this menu into the global variable
      number_selections for other routines to use */

   for (i = 0; menu->choice != NULL; i++, menu++) {
      curmenu.selection[i].position = col;
      curmenu.selection[i].length = strlen(menu->choice);
      disp_str(menu->choice);
      disp_str("  ");
      col += curmenu.selection[i].length + 2;
   }

   clr_eol();

   curmenu.number_selections = i;      /* remember how many selections in menu */

}


/******************************************************************************
 **                      M E N U _ S E L E C T                               **
 *****************************************************************************/

menu_select(sel)       /* select (highlight) a menu selection */
register int sel;
{
   setvattrib(DIS_HIGH);               /* set selection attribute */

   /* display the selection with attribute set */

   disp_str_at(curmenu.current_menu[sel].choice,MENU_ROW,
               curmenu.selection[sel].position);

   setvattrib(DIS_NORM);               /* return to normal attribute */
}


/******************************************************************************
 **                      M E N U _ P R O M P T                               **
 *****************************************************************************/

menu_prompt(sel)       /* display a selection's prompt string */
int sel;
{
   disp_str_at(curmenu.current_menu[sel].prompt,MENU_ROW+1,0);
   clr_eol();
}


/******************************************************************************
 **                      M E N U _ D E S E L E C T                           **
 *****************************************************************************/

menu_deselect(sel)     /* deselect (unhighlight) a menu selection */
register int sel;
{
   /* display the selection with normal attribute */

   disp_str_at(curmenu.current_menu[sel].choice,MENU_ROW,
               curmenu.selection[sel].position);
}


/******************************************************************************
 **                   M E N U _ A D V A N C E                                **
 *****************************************************************************/

menu_advance() {       /* advance the menu selection pointer */

   menu_deselect(curmenu.current_selection);    /* deselect previous entry */

   /* change the current selection to the next one or wrap around to 0 */

   curmenu.current_selection = (curmenu.current_selection <
                               curmenu.number_selections - 1) ?
                               curmenu.current_selection + 1 : 0;

   menu_select(curmenu.current_selection);      /* highlight it */
   menu_prompt(curmenu.current_selection);      /* display its prompt */
}


/******************************************************************************
 **                   M E N U _ B A C K U P                                  **
 *****************************************************************************/

menu_backup() {        /* backup the menu selection pointer */

   menu_deselect(curmenu.current_selection);    /* deselect previous entry */

   /* change the current selection to the previous or warp around to last */

   curmenu.current_selection = (curmenu.current_selection > 0) ?
                               curmenu.current_selection - 1 :
                               curmenu.number_selections - 1;

   menu_select(curmenu.current_selection);      /* highlight it */
   menu_prompt(curmenu.current_selection);      /* display its prompt */
}


/******************************************************************************
 **                    M E N U _ D O _ C H A R                               **
 *****************************************************************************/

menu_do_char(ch)       /* do the menu selection that starts with ch */
int ch;
{
   register int i;

   for (i = 0; i < curmenu.number_selections; i++)
      if (toupper(*curmenu.current_menu[i].choice) == toupper(ch)) {
         if (i != curmenu.current_selection &&
             curmenu.current_menu[i].func != NULL) {
            menu_deselect(curmenu.current_selection);
            menu_select(curmenu.current_selection = i);
            menu_prompt(curmenu.current_selection);
         }
         do_selection(i);
         return(1);
      }
   return(0);
}

/******************************************************************************
 **                    M E N U _ D O _ C U R R E N T                         **
 *****************************************************************************/

menu_do_current() {    /* do the current menu selection */

   do_selection(curmenu.current_selection);
}


/******************************************************************************
 **                      D O _ S E L E C T I O N                             **
 *****************************************************************************/

static int
do_selection(sel)      /* execute a menu selection */
register int sel;
{
   if (curmenu.current_menu[sel].func != NULL)  /* run function if there is 1 */
      (*curmenu.current_menu[sel].func)();

   if (!force_selection) {     /* function can force menu not to update */

      /* switch to the specified sub menu or top menu if no sub */

      if (curmenu.current_menu[sel].sub_menu != NULL)
         curmenu.current_menu = curmenu.current_menu[sel].sub_menu;
      else
         curmenu.current_menu = top_menu;

      menu_display(curmenu.current_menu);          /* display menu */
      menu_select(curmenu.current_selection = 0);  /* select item */
      menu_prompt(curmenu.current_selection);      /* display prompt */

   } else                              /* menu was forced */

      force_selection = FALSE;         /* only good for one shot */
}
