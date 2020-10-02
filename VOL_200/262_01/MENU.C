/********************************************************
 *                      M E N U                         *
 *                                                      *
 *                  by Robert Ramey                     *
 *                      May 1987                        *
 ********************************************************/

#include "stdio.h"

struct menu {
    char *question;
    struct mline {
        char *answer;
        int (*routine)();
        int *parameter;
    }
    selection[1];
};

#define MENU    struct menu
#define MLINE   struct mline

#define MAXMDEPTH 8  /* maximum depth of menus */
char hist[MAXMDEPTH],    /* sequence of responses */
    *hptr = hist;   /* pointer to next available spot */

/****************************************************************
menu - a function which manages dialog according to a hierarchy
of menus.  The address of the root menu must be passed as the only
parameter.
*****************************************************************/
int menu(maddress)
MENU maddress[];
{
    char response,*lptr;
    int i, j;

    /* repeat menu until operator response is RETURN or ESCAPE */
    /* or an action routine returns a non-zero value */
    for(;;){
        /* display the menu header */
        fputs(maddress->question,stderr);
        putc('\n', stderr);

        /* display each line in the menu */
        for(i = 0;lptr = maddress->selection[i].answer;++i){
            fputs(lptr,stderr);
            putc('\n',stderr);
        }

        /* loop until a valid response is recieved */
        for(;;){
            putc(':',stderr);   /* display prompt */
            response = getchar();   /* and get response */
            putc('\n',stderr);

            switch(response){
            case EOF:
            case ESC:
                return 99;  /* go back 99 levels */
            case NEWLINE:
                return 0;   /* repeat prev menu */
            }


            /* check response against menu lines */
            /* a valid response matches the first */
            /* character of a menu line */
            response = toupper(response);
            for(j = i ; response !=
            toupper(maddress->selection[j].answer[0])
            && j-- > 0;);
            /* check response against menu entries */

            if(j < 0)
                fputs(
                "invalid response. Try again.\n"
                ,stderr);
            else
                break;
        }

        /* update hist string to record the sequence of */
        /* responses that led us to this point in the menus */
        *hptr++ = response;
        *hptr = NULL;

        /* call the action routine which corresponds to the */
        /* selected menu line */
        response = (*(maddress->selection[j].routine))
            (maddress->selection[j].parameter);

        /* restore hist string */
        *--hptr = NULL;

        /* undo menu calls until returned value is 0 */
        if(response)
            return response - 1;

    }
}


     /* responses that led us to this point in the menus */