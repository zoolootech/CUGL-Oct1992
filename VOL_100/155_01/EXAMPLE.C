/* **** EXAMPLE.C ****
 *  This program is an example of how to use Ray Swartz's btree library.
 *  To use this program, a key file must be initialized first.
 *  This program maintains no data entries besides the keys entered
 *  into the key file.
 *
 *
 *
 *
 * Author: Ray Swartz
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



#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include "btree.h"
#include "beeprot.h"

extern void cursor(int row, int col);  /* cursor movement on a MS-DOS machine */
extern int main_prompt(char *prompt);   /* prompt used as database interface */
extern void get_key(char *prompt, char *key, struct keyinfo *fileinfo);
extern void print_msg(char *prompt);  /* go to line 20, clear window, ring bell, prompt */
extern void t_delay(); /* display message delay */
extern int yes_or_no(char *prompt);  /* print prompt and return YES or NO */

void show_rec(struct node *current_node);
void enter_data(struct node *current_node, struct keyinfo *fileinfo);

extern char instr[];       /* general input strings */

void main()
{
    struct keyinfo header;     /* keyfile being used */
    struct node current_node;  /* stores current node's information */
    char *key1;                /* key to search for */
    int choice;                /* user's menu choice */
    long node_nbr;             /* number of current_ node */
    int sentinel;
    
    printf("Key file to use: ");
    gets(instr);
    header.file = open_keyfile(instr, &header);
        /* must allocate space for the key pointers */
    key1 = malloc(header.keylength + 1);
    current_node.key = malloc(header.keylength + 1);
    cursor(1,1);
    CLS;
      while ((choice = main_prompt("Choice: ")) != QUIT) {
        if (choice == FIND) {
            cursor(1,1);
            CLS;
            get_key("Key to Find: ", key1, &header);
            if(find_key(key1, &node_nbr, &current_node, &header)==NOT_FOUND) {
                print_msg("Key not found");
                show_rec(&current_node);  /* show key node "found" */
            }
            else if(node_nbr == END)  /* no undeleted node found */
                printf("node_nbr == END\n");  /* tree "empty" */
            else
                show_rec(&current_node);  /* exact match found */
        }
        else if (choice == NEXT) {
            cursor(1,1);
            CLS;
            ym1:
            sentinel=0;
            switch (get_next(&node_nbr, &current_node, &header)) {
                case(NO):            
                    sentinel=1;    /* no current node */
                    break;  
                case(YES):
                    show_rec(&current_node); /* found one */
                    break;    
                case(AT_END):
                    print_msg("At end of file");
                    show_rec(&current_node);
            }
            if (sentinel == 1) goto ym1;
        }
        else if (choice == PREVIOUS) {
            cursor(1,1);
            CLS;
            ym2:
	    sentinel = 0;
            switch (get_previous(&node_nbr, &current_node, &header)) {
                case(NO):            
                    sentinel=1;    /* no current node */
                    break;  
                case(YES):
                    show_rec(&current_node); /* found one */
                    break;    
                case(AT_END):
                    print_msg("At end of file");
                    show_rec(&current_node);
            }
            if (sentinel == 1) goto ym2;
        }
        else if (choice == FIRST) {
            cursor(1,1);
            CLS;
            switch (get_first(&node_nbr, &current_node, &header)) {
                case(YES):
                    show_rec(&current_node); /* found first */
                    break;    
                case(NO):
                    print_msg("Must FIND a record first");
                    show_rec(&current_node);
            }
        }
        else if (choice == LAST) {
            cursor(1,1);
            CLS;
            switch (get_last(&node_nbr, &current_node, &header)) {
                case(YES):
                    show_rec(&current_node); /* found last */
                    break;    
                case(NO):
                    print_msg("Must FIND a record first");
                    show_rec(&current_node);
            }
        }
        else if (choice == INSERT) {
            cursor(1,1);
            CLS;
            enter_data(&current_node, &header);  /* get key to insert */
            insert(current_node.key, header.next_avail, &header);
        }
        else if (choice == DELETE) {
            if (yes_or_no("Delete this record? (y/n) ") == YES)
                delete_key(node_nbr, &current_node, &header);
        }
    }  /* QUIT chosen by user */
    close_keyfile(&header);
    exit(0);
}


void show_rec(struct node *current_node)
/*
struct node *current_node;
*/
{
    cursor(1,1);
    CLS;
    print_node(current_node);
}

void enter_data(struct node *current_node, struct keyinfo *fileinfo)
/*
struct keyinfo *fileinfo;
struct node *current_node;
*/
{
    printf("Enter a key: ");
    gets(instr);
    strncpy(current_node->key, instr, fileinfo->keylength);
    current_node->key[fileinfo->keylength] = '\0';
}
