/*
 *  address     Address a standard letter using the file 'address' (created
 *              by the letter program.
 */
#include <stdio.h>

#define HEAD 4
#define LENGTH 24
#define PO 5;

#define WIDTH 95

#define ERROR -1
/*
 *  read address into this array
 */
#define MAXLINES 10
char *lines[MAXLINES];

FILE *fd, *printer;

main()
{
    char buff[80];
    int max, j, offset, len, count;
    char *malloc();

    /*
     *  open the address file
     */
    if ( (fd = fopen("address", "r")) == 0) {
        printf("address file does not exist.\n");
        exit(ERROR);
    }
    printer = fopen("LPT1", "w");

    for(count=0, max=0; count<MAXLINES; count++) {
        if ( fgets(buff, 80, fd) == 0)
            break;
        else {
            len = strlen(buff);
            lines[count] = malloc(80);
            if (len > max)
                max = len;
            strcpy(lines[count], buff);
        }
    }           
    offset = ( (WIDTH - max) / 2) + PO
    for(j=0; j<offset; j++)
        buff[j]=' ';
    buff[j] = '\0';

    offset = ( LENGTH - count ) / 2 - HEAD;

    for(j=0; j<offset; j++)
        fputs("\n", printer);

    for(j=0; j<count; j++) {
        fprintf(printer, "%s%s", buff, lines[j]);
    }
    
    fputs("\014", printer);
    fclose(fd);
    fclose(printer);
}
