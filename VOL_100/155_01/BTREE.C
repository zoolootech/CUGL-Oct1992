#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include "btree.h"
#include "beeprot.h"

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
 * The insert routine was directly translated from the algorithm
 *  in D. Knuth's Volume 3 of The Art of Computer Programming
 *  (Sorting and Searching) pages 455 - 457.  Single letter
 *  variables (p, q, r, s) are used to make the algorithm steps
 *  more obvious.
 */
/*
 *	Modifier: Honma Michimasa
 */
 
STACK right_history, left_history;
int stack_level;   /* tree level at present */
char instr[256];   /* general string input */


int insert(char *argkey, long recnbr, struct keyinfo *fileinfo) /* insert key (argkey) into tree */
/* struct keyinfo *fileinfo;  file to insert key into */
/* char *argkey;     key to insert */
/* long recnbr;      record number of corresponding data record */
{
    static long top;  /* where balancing will have to take place, if at all */
    static long p_rec;
    static long s_rec;
    static long q_rec;
    static long r_rec;
    static struct node q_node, s_node, r_node, p_node;
    static int alloc_flag = NO;  /* flag for pointer space allocation */
    int compare;   /* holds key comparison values */
    
    if (fileinfo->next_avail >= MAX_NODES) {
        printf("Only %d nodes allowed in a keyfile\n");
        exit(1);
    }
    if(alloc_flag == NO) {   /* set up key pointers in node structures  */
        q_node.key = malloc(fileinfo->keylength + 1); /* first call to */
        s_node.key = malloc(fileinfo->keylength + 1); /* insert function */
        p_node.key = malloc(fileinfo->keylength + 1);
        r_node.key = malloc(fileinfo->keylength + 1);
        alloc_flag = YES;
    }
    if (fileinfo->list_head == 0) {   /* no records in list */
        fileinfo->list_head = 1;  /* insert key as head of list */
        p_node.balance = p_node.right_link = p_node.left_link = 0;
        p_node.delete_flag = NO;
        p_node.rec_nbr = recnbr;
        strcpy(p_node.key, argkey);
        write_node(1L, &p_node, fileinfo);
        fileinfo->next_avail++;
        fileinfo->nbr_in_list++;
        return(YES);
    }
    top = TOP;    /* initialize to see if list head changes */
    p_rec = fileinfo->list_head;   /* Step A1 (find spot to insert) */
    s_rec = fileinfo->list_head;
    while(1) {
        get_node(p_rec, &p_node, fileinfo);
        if ((compare = strcmp(argkey, p_node.key)) < 0) {  /* Step A2 */
            q_rec = p_node.left_link;  /* Step A3 (move left) */
            if (q_rec == END) {  /* insert here */
                q_rec = fileinfo->next_avail++;
                p_node.left_link = q_rec;
                break;  /* loop exit */
            }
            else {  /* look again from this node */
                get_node(q_rec, &q_node, fileinfo);
                if (q_node.balance != 0) {
                    top = p_rec;
                    s_rec = q_rec;
                }
            }
            p_rec = q_rec;
        }
        else  {  /* Step A4 (move right) */
            q_rec = p_node.right_link;
            if (q_rec == END) {  /* insert here */
                q_rec = fileinfo->next_avail++;
                p_node.right_link = q_rec;
                break;  /* loop exit */
            }
            else {  /* look again from this node */
                get_node(q_rec, &q_node, fileinfo);
                if (q_node.balance != 0) {
                    top = p_rec;
                    s_rec = q_rec;
                }
                p_rec = q_rec;
            }
        }
    } /* end of while loop */
/* Step 5 (insert key at q_node) */      
    q_node.left_link = q_node.right_link = END;
    q_node.balance = 0;
    q_node.delete_flag = NO;
    q_node.rec_nbr = recnbr;
    strcpy(q_node.key, argkey);
    if (write_node(q_rec, &q_node, fileinfo) == NO)
        return(NO);  /* write failed */
    write_node(p_rec, &p_node, fileinfo);
    get_node(s_rec, &s_node, fileinfo); /* Step A6 (adjust balance factors) */
    if (strcmp(argkey, s_node.key) < 0)
        r_rec = p_rec = s_node.left_link;
    else
        r_rec = p_rec = s_node.right_link;
    while (p_rec != q_rec) {
        get_node(p_rec, &p_node, fileinfo);
        if(strcmp(argkey, p_node.key) < 0) {
            p_node.balance = -1;
            write_node(p_rec, &p_node, fileinfo);
            p_rec = p_node.left_link;
        }
        else {
            p_node.balance = 1;
            write_node(p_rec, &p_node, fileinfo);
            p_rec = p_node.right_link;
        }
    }
    compare = (strcmp(argkey, s_node.key) < 0) ? -1 : 1; /* Step A7 */
    if (s_node.balance == 0) {  /* Step A7i */
        fileinfo->nbr_in_list++;
        s_node.balance = compare;
        write_node(s_rec, &s_node, fileinfo);
        return(YES);
    }
    else if(s_node.balance == -compare) {  /* Step A7ii */
        fileinfo->nbr_in_list++;
        s_node.balance = 0;
        write_node(s_rec, &s_node, fileinfo);
        return(YES);
    }
    else { /* Step A7iii (tree out of balance) */
        fileinfo->nbr_in_list++;
        get_node(s_rec, &s_node, fileinfo);
        get_node(r_rec, &r_node, fileinfo);
        if (r_node.balance == compare) { /* Step A8 (single rotate) */
            p_rec = r_rec;
            link(compare, &s_node, -compare, &r_node);
            link_nbr(-compare, &r_node, s_rec);
            s_node.balance = r_node.balance = 0;
        }
        else {   /* Step A9 (double rotate) */
            nbr_link(&p_rec, -compare, &r_node);
            get_node(p_rec, &p_node, fileinfo);
            link(-compare, &r_node, compare, &p_node);
            link_nbr(compare, &p_node, r_rec);
            link(compare, &s_node, -compare, &p_node);
            link_nbr(-compare, &p_node, s_rec);
            node_bal(compare, &p_node, &s_node, &r_node);
            p_node.balance = 0;
            write_node(p_rec, &p_node, fileinfo);
        }
        if (top == TOP)  /* Step A10 */ 
            fileinfo->list_head = p_rec; /* balanced at list head */ 
        else {   /* balanced at top of a sub-tree */
            get_node(top, &q_node, fileinfo); 
            if (s_rec == q_node.right_link)
                q_node.right_link = p_rec; 
            else
                q_node.left_link = p_rec;
            write_node(top, &q_node, fileinfo);
        }
        write_node(s_rec, &s_node, fileinfo);
        write_node(r_rec, &r_node, fileinfo);
        return(YES);
    }
}


void link(int alpha1, struct node *node1, int alpha2, struct node *node2)
/*
int alpha1, alpha2;
struct node *node1, *node2;
*/
{
    /* see definition of LINK(a, P) on Knuth pg. 455 (Vol. 3) */
      
    if (alpha1 == -1 && alpha2 == -1)
        node1->left_link = node2->left_link;
    else if(alpha1 == -1 && alpha2 == 1)
        node1->left_link = node2->right_link;
    else if(alpha1 == 1 && alpha2 == -1)
        node1->right_link = node2->left_link;
    else
        node1->right_link = node2->right_link;
}


void nbr_link(long *nbr, int alpha, struct node *node1) /* set a record number according to alpha */
/*
long *nbr;
int alpha;
struct node *node1;
*/
{
    *nbr = (alpha == 1) ? node1->right_link : node1->left_link;
}

void link_nbr(int alpha, struct node *node1, long nbr) /* set a link according to alpha */
/*
int alpha;
struct node *node1;
long nbr;
*/
{

    if (alpha == 1)
        node1->right_link = nbr;
    else
        node1->left_link = nbr;
}

void node_bal(int alpha, struct node *node1, struct node *node2, struct node *node3)  /* node balancing in Step A9 */
/*
int alpha;
struct node *node1, *node2, *node3;
*/
{

    if (node1->balance == alpha) {
        node2->balance = -alpha;
        node3->balance = 0;
    } 
    else if(node1->balance == 0)
        node2->balance = node3->balance = 0;
    else {
        node2->balance = 0;
        node3->balance = alpha;
    }
}


void delete_key(long node_nbr, struct node *current_node, struct keyinfo *fileinfo)
/* long node_nbr;   node to delete */
/* struct node *current_node;   deleted node's information */
/* struct keyinfo *fileinfo;   keyfile */
{
    if (node_nbr == END)
        printf("DELETE: can't delete node 0\n");
    else {
        current_node->delete_flag = YES;
        if (write_node(node_nbr, current_node, fileinfo) == NO)
            print_msg("DELETE: write_node returned NO");
        else
            fileinfo->nbr_in_list--;
    }
}



int get_next(long *node_nbr, struct node *current_node, struct keyinfo *fileinfo)  /* retrieve next higher node */
/* struct keyfile(keyinfo ?) *fileinfo; */
/* struct node *current_node;   where to store node's info */
/* long *node_nbr;     node to retrieve */
{
    long search_node;
   
    search_node = *node_nbr;
    if (*node_nbr == END) {   /* undefined current node */
         print_msg("Must FIND a record first\n");
         return(NO);
    }
    do {
        if(current_node->right_link == END) {  /* can't move right */
            if(left_history.stack_cntr == END) { /* none in stack */
                /* node_nbr at end of non-deleted elements - retreive it */
                get_node(*node_nbr, current_node, fileinfo);
                return(AT_END);
            }
            else {    /* can't go right & stack full (pop stack) */
                search_node = pop_left_stack();
                get_node(search_node, current_node, fileinfo);
            }  
        }
        else {  /* move right */
            push_right_stack(search_node);
            search_node = current_node->right_link;
            get_node(search_node, current_node, fileinfo);
            while(current_node->left_link != END) { /* bottom left */
                push_left_stack(search_node);
                search_node = current_node->left_link; /* of this sub-tree */
                get_node(search_node, current_node, fileinfo);
            }
        }
    } while (current_node->delete_flag == YES);
    *node_nbr = search_node;
    return(YES);
}



int find_key(char *key1, long *node_nbr, struct node *current_node, struct keyinfo *fileinfo) /* locate a key */
/* char *key1;   key to find */
/* struct keyinfo *fileinfo;  */
/* long *node_nbr;  number of "found" node */
/* struct node *current_node;   where "found" node is stored */
{
    int direction;   /* flag to determine moving right or left */
    long search_node; /* node presently being searched */

/* initalize stacks to empty */
    right_history.stack_cntr = left_history.stack_cntr = END;
    right_history.element[END] = left_history.element[END] = END;
    stack_level = 0;   /* tree level at start of search */
    search_node = fileinfo->list_head;  /* begin at list head */
    get_node(search_node, current_node, fileinfo);
    while((direction = strcmp(key1, current_node->key)) != 0 ||
           current_node->delete_flag == YES) {
        if (direction > 0) { /* search to right */
            if (current_node->right_link != END) { 
                push_right_stack(search_node); /* not found, more to see */
                search_node = current_node->right_link;
                get_node(search_node, current_node, fileinfo);
            }
            else if(current_node->delete_flag == YES) { /* node deleted */
              /* skip all deleted nodes */
                while(current_node->delete_flag == YES) { 
                   if(get_next(&search_node,current_node,fileinfo) == AT_END){
                        get_previous(&search_node, current_node, fileinfo);
                        *node_nbr = search_node;      
                        return(NOT_FOUND);
                   }
                }
                *node_nbr = search_node;
                return(NOT_FOUND);
            }
            else {  /* at end of a branch */
                *node_nbr = search_node;
                return(NOT_FOUND);
            }
        }   
        else {  /* search to left */
            if (current_node->left_link != END) {
                push_left_stack(search_node); 
                search_node = current_node->left_link;
                get_node(search_node, current_node, fileinfo);
            }                         
            else if(current_node->delete_flag == YES) { /* left = END */
                while(current_node->delete_flag == YES) {
                 if (get_next(&search_node,current_node,fileinfo)==AT_END) {
                       get_previous(&search_node, current_node, fileinfo);
                       *node_nbr = search_node;     
                       return(NOT_FOUND);
                   }
                }
                *node_nbr = search_node;
                return(NOT_FOUND);
            }   
            else {
                *node_nbr = search_node;
                return(NOT_FOUND);
            }
        }
    }  /* end of search while loop */
    return(YES);  /* exact match and not deleted only */
}

int get_first(long *node_nbr, struct node *current_node, struct keyinfo *fileinfo)
/*** added function  ****/
{
    int direction;   /* flag to determine moving right or left */
    long search_node; /* node presently being searched */

/* initalize stacks to empty */
    right_history.stack_cntr = left_history.stack_cntr = END;
    right_history.element[END] = left_history.element[END] = END;
    stack_level = 0;   /* tree level at start of search */
    search_node = fileinfo->list_head;  /* begin at list head */
    get_node(search_node, current_node, fileinfo);
    while(current_node->left_link != END) {
         push_left_stack(search_node); /* not found, more to see */
         search_node = current_node->left_link;
         get_node(search_node, current_node, fileinfo);
    }
    if(current_node->delete_flag == YES) { /* left = END */
        /* skip all deleted nodes */
         while(current_node->delete_flag == YES) {
             if (get_next(&search_node,current_node,fileinfo)==AT_END) {
                    get_previous(&search_node, current_node, fileinfo);
                    *node_nbr = search_node;     
                    return(YES);
             }
         }
         *node_nbr = search_node;
         return(YES);
    }
    *node_nbr = search_node;
    return(YES);
}

int get_last(long *node_nbr, struct node *current_node, struct keyinfo *fileinfo)
/*** added function  ****/
{
    int direction;   /* flag to determine moving right or left */
    long search_node; /* node presently being searched */

/* initalize stacks to empty */
    right_history.stack_cntr = left_history.stack_cntr = END;
    right_history.element[END] = left_history.element[END] = END;
    stack_level = 0;   /* tree level at start of search */
    search_node = fileinfo->list_head;  /* begin at list head */
    get_node(search_node, current_node, fileinfo);
    while(current_node->right_link != END) {
        push_right_stack(search_node); /* not found, more to see */
        search_node = current_node->right_link;
        get_node(search_node, current_node, fileinfo);
    }
    if(current_node->delete_flag == YES) { /* node deleted */
        /* skip all deleted nodes */
         while(current_node->delete_flag == YES) { 
             if(get_next(&search_node,current_node,fileinfo) == AT_END){
                   get_previous(&search_node, current_node, fileinfo);
                   *node_nbr = search_node;      
                   return(YES);
              }
         }
         *node_nbr = search_node;
         return(YES);
    }
    *node_nbr = search_node;
    return(YES); 
}

int get_previous(long *node_nbr, struct node *current_node, struct keyinfo *fileinfo)
/*
long *node_nbr;
struct node *current_node;
struct keyinfo *fileinfo;
*/
{
    long search_node;

    search_node = *node_nbr;
    if (*node_nbr == END) {   /* undefined current node */
        print_msg("Must FIND a record first\n");
        return(NO);
    }
    do {
        if(current_node->left_link == END) {  /* can't move left */
            if(right_history.stack_cntr == END) { /* none in stack */
                /* node_nbr at end of non-deleted elements - retreive it */
                get_node(*node_nbr, current_node, fileinfo);
                return(AT_END);  /* don't reset node_nbr */
            }
            else {    /* can't go left & stack full (pop stack) */
                search_node = pop_right_stack();
                get_node(search_node, current_node, fileinfo);
            }  
        }
        else {  /* move left then to bottom right - this is previous one */
            push_left_stack(search_node);
            search_node = current_node->left_link;
            get_node(search_node, current_node, fileinfo);
            while(current_node->right_link != END) { /* bottom right */
                push_right_stack(search_node);       /* of this sub-tree */
                search_node = current_node->right_link;
                get_node(search_node, current_node, fileinfo);
            }
        }
    } while (current_node->delete_flag == YES);
    *node_nbr = search_node;   /* set node_nbr to previous node found */    
    return(YES);
}


FILE *open_keyfile(char *filename, struct keyinfo *fileinfo)   /* used to open keyfile */
/*
char *filename;
struct keyinfo *fileinfo;
*/
{

    FILE  *fd;
    
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
}


void push_left_stack(long nbr)  /* moved left in tree - record this in stack */
/*
long nbr;
*/
{

    if (++left_history.stack_cntr >= STACK_LENGTH) {
        printf("PUSH_LEFT_STACK: overflow node number: %ld\n", nbr);
        exit(0);
    }
    left_history.element[left_history.stack_cntr] = nbr;
    left_history.level[left_history.stack_cntr] = ++stack_level;
}


void push_right_stack(long nbr)  /* moved right in tree - record this in stack */
/*
long nbr;
*/
{
    if (++right_history.stack_cntr >= STACK_LENGTH) {
        printf("PUSH_RIGHT_STACK: overflow node number: %ld\n", nbr);
        exit(0);
    }
    right_history.element[right_history.stack_cntr] = nbr;
    right_history.level[right_history.stack_cntr] = ++stack_level;
}

  /* return the next one on the stack and keep left stack at proper level */
long pop_right_stack() 
{
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
}
