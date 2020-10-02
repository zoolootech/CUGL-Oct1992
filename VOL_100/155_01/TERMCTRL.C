#include <stdio.h>
#include <string.h>
#include "btree.h"

/* Author: Ray Swartz
 *         Berkeley Decision/Systems, Inc.
 *         P.O. Box 2528
 *         Santa Cruz, Calif. 95063
 * Last Modified: 4/28/85
 *
 * ANY USE OF THESE LIBRARY ROUTINES EITHER PERSONAL OR COMMERCIAL
 * IS ALLOWED UPON THE CONDITION THAT THE USER IDENTIFY THEM
 * AS BEING USED IN THE PROGRAM.  IDENTIFYING INFORMATION MUST
 * APPEAR IN THE PROGRAM DOCUMENTATION OR ON THE TERMINAL SCREEN.
 *
 *         #################################    
 *         # UNATTRIBUTED USE IS FORBIDDEN #
 *         #################################
 *
 */

/* Modifier: Honma Michimasa
 *	Higashi Kamagaya 2-6-54
 *	Kamagayashi, Chiba, Japan, 273-01
 *
 *
 *
 */
 
 /***** function prot. *************/
void cursor(int row, int col);  /* cursor movement on a MS-DOS machine */
int main_prompt(char *prompt);   /* prompt used as database interface */
void get_key(char *prompt, char *key, struct keyinfo *fileinfo);
 		   /* prompt for and read the key to FIND (from keyboard) */
void print_msg(char *prompt);  /* go to line 20, clear window, ring bell, prompt */
void t_delay(void); /* display message delay */
 /**********************************/
 
void cursor(int row, int col)  /* cursor movement on a MS-DOS machine */
{
    printf("\033[%d;%dH", row, col);
}


int main_prompt(char *prompt)   /* prompt used as database interface */
{
    extern char instr[];

    int count;

    cursor(20,1);
    CLEAR_LINE;
    printf("Find   Next   Previous   Insert   Delete   Quit   Top   Last");
    while(1) {
        cursor(22,1);
        CLEAR_LINE;
        printf(prompt);
        gets(instr);
        switch(*instr) {
            case 'F':
            case 'f':
                return(FIND);
            case 'I':
            case 'i':
                return(INSERT);
            case 'N':
            case 'n':
                return(NEXT);
            case 'P':
            case 'p':
                return(PREVIOUS);
            case 'D':
            case 'd':
                return(DELETE);
            case 'Q':
            case 'q':
                return(QUIT);
            case 'T':
            case 't':
                return(FIRST);
            case 'L':
            case 'l':
                return(LAST);
        }
    }
}


   /* prompt for and read the key to FIND (from keyboard) */
void get_key(char *prompt, char *key, struct keyinfo *fileinfo)
{
    extern char instr[];

    cursor(20,1);
    CLEAR_LINE;
    printf(prompt);
    gets(instr);
    strncpy(key, instr, fileinfo->keylength);
    key[fileinfo->keylength]='\0';
    return;
}


void print_msg(char *prompt)  /* go to line 20, clear window, ring bell, prompt */
{
    cursor(20,1);
    CLEAR_LINE;
    BELL
    printf(prompt);
    t_delay();
}


void t_delay() /* display message delay */
{
    long  count = 0;
    
    while(count++ < 60000)
      ;
}


int yes_or_no(char *prompt)  /* print prompt and return YES or NO */
{
    extern char instr[];

    while(1) {
        cursor(20,1);
        CLEAR_LINE;
        printf(prompt);
        gets(instr);
        if (*instr == 'Y' || *instr == 'y')
            return(YES);
        else if(*instr == 'N' || *instr == 'n')
            return(NO);
        else {
            BELL
            printf("\nPlease enter Y or N only\n");
        }
    }
}
