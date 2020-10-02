/* MDEBUG.C -- Debug routines for MAKE.C.
 *
 * These routines should be linked in if DEBUG is #defined in MAKE.H.
 * Otherwise, they are unnecessary.
 *
 * CREDITS:
 *
 * -- trav() and pnode() functions by Allen Holub (DDJ #106)
 * -- debug() function by James Pritchett
 * -- All code by Allen Holub adapted for BDS C (where necessary) by
 *    James Pritchett.
 *
 * Version 1.0 -- 10/28/85
 * Version 1.1 -- 12/06/85
 */

#include    <bdscio.h>
#include    "make.h"

void debug(s,p)     /* Print a message with a string parameter and
                     * wait for console input to continue.
                     */
char    *s, *p;
{
    printf(s,p);
    getchar();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

trav(root)          /* Print the objectname tree.  This function
                     * is recursive, printing the left branch, then
                     * the root, then the right branch.
                     */
TNODE   *root;
{
    if(root == NULL)    /* NULL node is leaf */
        return;
    trav(root->lnode);  /* Do the left branch */
    pnode(root);        /* Then the node */
    trav(root->rnode);  /* Then the right branch */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

pnode(node)     /* Print a node */
TNODE   *node;
{
    char    **linev;

    printf("+-----------------------------------------\n");
    printf("|   Node at 0x%x\n",node);
    printf("+-----------------------------------------\n");
    printf("|   lnode = 0x%x, rnode = 0x%x\n",node->rnode,node->lnode);
    printf("|   status = %d\n",node->changed);
    printf("|   target = <%s>\n",node->being_made);
    printf("|   dependancies:\n");
    for(linev = node->depends_on; *linev; printf("|\t%s\n",*linev++))
        ;
    printf("|   actions:\n");
    for(linev = node->do_this; *linev; printf("|\t%s\n",*linev++))
        ;
    printf("+-----------------------------------------\n");
    getchar();
}

/* end */

root =