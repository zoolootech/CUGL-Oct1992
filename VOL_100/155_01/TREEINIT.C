#include <stdio.h>
#include "btree.h"


/* This program initializes a file as a valid key file.  This
 *  program must be used to set-up key files properly.
 */


main()
{
    extern char instr[];

    FILE *fopen(), *fd;
    char filename[20];
    struct keyinfo fileinfo;

    printf("\n\nName of key file to initialize: ");
    gets(filename);
    printf("\n\nLength of Key: ");
    gets(instr);
    fileinfo.keylength = atoi(instr);
    if ((fd = fopen(filename, "w")) != NULL) {
        fprintf(fd,"%c%2d%5ld%5ld%5ld%c",
               '~',fileinfo.keylength,1L,0L,0L,'~');
        fclose(fd);
    }
    else {
        BELL
        printf("\nCan't open file %s\n", filename);
    }
    exit(0);
}

