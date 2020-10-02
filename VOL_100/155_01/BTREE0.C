#include <stdio.h>
#include <stdlib.h>
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
 *
 *
 * keyfiles are created with a tilde (~) as the first character
 *  in the file, a 2 byte integer (keylength), a 5 byte integer
 *  (next available node), a 5 byte integer (head of the list),
 *  a 5 byte integer (number in the list), and another tilde.
 */

/*
 *	Modifier: Honma Michimasa
 */
 
/**** function prot. ***********/
FILE *open_keyfile(char *filename, struct keyinfo *fileinfo);
			   /* used to open keyfile */
void close_keyfile(struct keyinfo *fileinfo);
			   /* write out header information and close file */
int write_node(long nbr, struct node *nodeinfo, struct keyinfo *fileinfo);
			   /* write a node's info to file */
void print_node(struct node *node1);
			  /* display contents of a node on screen (debug) */
void push_left_stack(long nbr);
			  /* moved left in tree - record this in stack */
void push_right_stack(long nbr);
			  /* moved right in tree - record this in stack */
long pop_right_stack();
long pop_left_stack();
void get_node(long nbr, struct node *nodeinfo, struct keyinfo *fileinfo);
			  /* read the info stored in node NBR */



FILE *open_keyfile(char *filename, struct keyinfo *fileinfo)   /* used to open keyfile */
/*
char *filename;
struct keyinfo *fileinfo;
*/
{
    extern char instr[];

    FILE *fopen(), *fd;
    
    if ((fd = fopen(filename,"r+")) != NULL) {  /* file exists */
        fgets(instr, 2, fd);
        if (*instr != 126) {
            BELL
            printf("%s is not a valid key file\n", filename);
            fclose(fd);
            exit(0);
        } 
        fgets(instr, 3, fd);
        fileinfo->keylength = atoi(instr);
        fgets(instr, 6, fd);
        fileinfo->next_avail = atol(instr); 
        fgets(instr, 6, fd);
        fileinfo->list_head = atol(instr); 
        fgets(instr, 6, fd);
        fileinfo->nbr_in_list = atol(instr); 
        fgets(instr, 2, fd);
        if (*instr != 126) {
            BELL
            printf("%s is not a valid key file\n", filename);
            fclose(fd);
            exit(0);
        } 
    }
    else {    /* file doesn't exist - report error */
        BELL
        printf("\n%s not a valid key file", filename);
        exit(0);
    }
    return(fd);
}



void close_keyfile(struct keyinfo *fileinfo)   /* write out header information and close file */
/*
struct keyinfo *fileinfo;
*/
{
    fseek(fileinfo->file, 0L, 0);
    fprintf(fileinfo->file,"%c%2d%5ld%5ld%5ld%c",
                '~',                  /* keyfile token */
                fileinfo->keylength,
                fileinfo->next_avail,
                fileinfo->list_head,
                fileinfo->nbr_in_list,
                '~');
    fclose(fileinfo->file);
    return;
}

int write_node(long nbr, struct node *nodeinfo, struct keyinfo *fileinfo)   /* write a node's info to file */
/* long nbr;    node to write at */
/* struct node *nodeinfo;   node info to write */
/* struct keyinfo *fileinfo; */
{
    long spot;
    int count;

    spot = nbr * (fileinfo->keylength + DATA_LENGTH);
    if (fseek(fileinfo->file, spot, 0) == -1) {  /* error */
        printf("WRITE_NODE: fseek error on node %ld\n", nbr);
        return(NO);
    }
    if (fprintf(fileinfo->file, "%2d%2d%5ld%5ld%5ld",
            nodeinfo->delete_flag,
            nodeinfo->balance,
            nodeinfo->left_link,
            nodeinfo->right_link,
            nodeinfo->rec_nbr) == EOF)
        return(NO);
    for(count = 0; nodeinfo->key[count] != '\0'; count++) /* write out key */
        if (putc(nodeinfo->key[count], fileinfo->file) == EOF)
            return(NO);
    for(; count < fileinfo->keylength; count++)  /* blank fill if necessary */
        if (putc(' ',fileinfo->file) == EOF)
            return(NO);
    return(YES);
}

void print_node(struct node *node1)  /* display contents of a node on screen (debug) */
/*
struct node *node1;
*/
{

    printf("left link: %ld,  right link: %ld\n",
            node1->left_link, node1->right_link);
    printf("key: %s, record nbr: %ld\n",node1->key, node1->rec_nbr);
    printf("balance index: %d\n", node1->balance);
    return;
}


void push_left_stack(long nbr)  /* moved left in tree - record this in stack */
/*
long nbr;
*/
{
    extern int stack_level;

    if (++left_history.stack_cntr >= STACK_LENGTH) {
        printf("PUSH_LEFT_STACK: overflow node number: %ld\n", nbr);
        exit(0);
    }
    left_history.element[left_history.stack_cntr] = nbr;
    left_history.level[left_history.stack_cntr] = ++stack_level;
    return;
}


void push_right_stack(long nbr)  /* moved right in tree - record this in stack */
/*
long nbr;
*/
{
    extern int stack_level;

    if (++right_history.stack_cntr >= STACK_LENGTH) {
        printf("PUSH_RIGHT_STACK: overflow node number: %ld\n", nbr);
        exit(0);
    }
    right_history.element[right_history.stack_cntr] = nbr;
    right_history.level[right_history.stack_cntr] = ++stack_level;
    return;
}

  /* return the next one on the stack and keep left stack at proper level */
long pop_right_stack() 
{
    extern int stack_level;

/* pop both to stay at same tree level */
    stack_level = right_history.level[right_history.stack_cntr];
    while(left_history.level[left_history.stack_cntr] > stack_level)
        left_history.stack_cntr--;
    if (right_history.stack_cntr == END)
        return(END);
    stack_level--;
    return(right_history.element[right_history.stack_cntr--]);
}


  /* return the next one on the stack and keep right stack at proper level */
long pop_left_stack()
{
    extern int stack_level;

/* pop both to stay at same tree level */
    stack_level = left_history.level[left_history.stack_cntr];
    while(right_history.level[right_history.stack_cntr] > stack_level)
        right_history.stack_cntr--;
    if (left_history.stack_cntr == END)
        return(END);    
    stack_level--;
    return(left_history.element[left_history.stack_cntr--]);
}


void get_node(long nbr, struct node *nodeinfo, struct keyinfo *fileinfo)  /* read the info stored in node NBR */
/* long nbr;      node to retrieve */
/* struct node *nodeinfo;    where to store the node's information */
/* struct keyinfo *fileinfo; */
{
    extern char instr[];

    long spot;

    spot = nbr * (fileinfo->keylength + DATA_LENGTH);
    fseek(fileinfo->file, spot, 0);
    fgets(instr, 3, fileinfo->file);  /* get delete flag */
    nodeinfo->delete_flag = atoi(instr);
    fgets(instr, 3, fileinfo->file);  /* get balance factor */
    nodeinfo->balance = atoi(instr);
    fgets(instr, 6, fileinfo->file);  /* left_link */
    nodeinfo->left_link = atol(instr);
    fgets(instr, 6, fileinfo->file);  /* right_link */
    nodeinfo->right_link = atol(instr);
    fgets(instr, 6, fileinfo->file);
    nodeinfo->rec_nbr = atol(instr);  /* data record number */
    fgets(nodeinfo->key, fileinfo->keylength + 1, fileinfo->file);
    return;
}
