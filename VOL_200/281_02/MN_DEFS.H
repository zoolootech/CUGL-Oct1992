/*  mn_defs.h
 *
 *  Menu definitions Header
 *
 *  Copyright (c) 1988 by David A. Miller - Unicorn Software
 *
 */


typedef struct m_item
{
   int		    row;
   int		    col;
   int		    att_inactive;
   int		    att_active;
   char 	   *text;
   char 	    letter;
   int		    att_letter;
   struct m_item   *next;
   struct m_item   *prev;
   char 	   *user1;
   char 	   *user2;
   PFI		    item_function;
} MNU_ITEM, *MNU_ITEMPTR;


typedef struct mnu
{
   MNU_ITEMPTR	    mnu_item;
   WINDOWPTR	    mnu_window;
   int		    nbr_choices;
} MNU, *MNUPTR;



#define   BDR_110LNP	&bdr_110ln
#define   BDR_220LNP	&bdr_220ln
