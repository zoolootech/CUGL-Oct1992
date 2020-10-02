/*
	PAGE:   Currently setup for TRS-80 Model II operation.

    Paging routine - outputs 'pagelength' lines (of ASCII text)
    to standard output then waits for KB input to continue with
    the next page of 'pagelength' lines.
    If the KB input is the F1 key the next ('pagelength' / 2) lines
    (half page) will be output, any other key outputs a full page.
    
    Notes: The BREAK key will cause an orderly exit back to CP/M.
           If 'pagelength' is not specified it defaults to 23 lines.

    Usage: pg [-pagelength] filename

    Bugs: None that I know of.

    Environment: CP/M 2.2 (P&T)
	 	 BDS C compiler Rev. 1.42

    Author: Jack S. Bakeman, Jr.
            1222 Inverrary Ln.
            Deerfield, IL 60015
	    (312) 459-0565
*/

#include "bdscio.h"
#define BRK  0x03	/* BREAK key on the TRS-80 Model II	  */
#define F1   0x01	/* Function key F1 on the TRS-80 Model II */

main(argc, argv)
char **argv;
{
    char ibuf[BUFSIZ], linbuf[MAXLINE];
    char length[6];
    int c,i;
    int pglen;
    
    if (argc < 2)        /* User has given no arguments to process */
        {
        printf("Usage: pg [-pagelength] filename\n");
        exit();
        }
    if (argc == 2)        /* User has not specified a pagelength */
        {
        pglen = 23;        /* Default pagelength */
        if (fopen(argv[1], ibuf) == ERROR)
            {
            printf("Can't open %s\n", argv[1]);
            exit();
            }
        }
    if (argc >= 3)        /* Both arguments have been supplied */
        {
        if(*(argv[1]) == '-')    /* Check pagelength for possible typos */
            {
            strcpy(length,argv[1]);
            for (i = 1; length[i] != NULL; i++)
                if (isalpha(length[i]))
                    length[i] = NULL;
            pglen = ((i = abs(atoi(length))) > 0) ? i : 23;    /* NEVER = 0 */
            if (fopen(argv[2], ibuf) == ERROR)
                {
                printf("Can't open %s\n", argv[2]);
                exit();
                }
            }
        else        /* Garbage 1st. argument */
            {
            printf("Unknown flag\n");
            exit();
            }
        }
    while (1)        /* Work gets done here, above is just a parser */
        {
        for (i = 0; i < ((c == F1) ? pglen/2 : pglen); i++)
            {
            if (fgets(linbuf, ibuf) != 0)
                 printf("%s", linbuf);
            else        /* EOF or CNTL Z reached */
                 exit();
            }
	if ((c = bios(3)) == BRK) /* Raw character input, no console echo */
	    break;
        }
    fclose(ibuf);
}
