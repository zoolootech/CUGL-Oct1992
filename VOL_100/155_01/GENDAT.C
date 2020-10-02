/*
 *	gendat.c
 	btree用のテストデータ（INDEX）を作成する。
 	制作：本間
 	1990/10/01
 */
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include "btree.h"
#include "beeprot.h"

#define  FILENAME   "test.idx"

extern char instr[];       /* general input strings */

void main()
{
    int i;
    struct keyinfo header;     /* keyfile being used */
    struct node current_node;  /* stores current node's information */
    long node_nbr;             /* number of current_ node */
    
    header.file = open_keyfile(FILENAME, &header);
        /* must allocate space for the key pointers */
    current_node.key = malloc(header.keylength + 1);

    for(i=0; i<1000; i++){
       if(gets(instr) == NULL) break;
       strncpy(current_node.key, instr, header.keylength);
       current_node.key[header.keylength] = '\0';
       insert(current_node.key, header.next_avail, &header);
    }
    close_keyfile(&header);
    free(current_node.key);
    exit(0);
}
