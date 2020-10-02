/* ------------------------------------------- */
/*     DETAB - Convert tabs to blanks          */
/*             Adapted from Software Tools     */
/*             By Kernighan and Plauger        */
/*                                             */
/*             Written by Terry Davis          */
/*             Last Update: 25 July 1984       */
/*             CIS 70040,1162 STC BCA881       */
/*             Tele 1-612-253-0923             */
/*                                             */
/* ------------------------------------------- */
/*     USAGE:                                  */
/*             DETAB FROMFILE TOFILE N         */
/*               N is the number of columns    */
/*                 between tab stops           */
/* ------------------------------------------- */

#include <stdio.h>
#include <ctype.h>
#define EOS 0
#define DEFAULT 8

double  VERS = 1.00;

main(argc,argv)
int     argc;
char    *argv[];

{
        FILE    *fd, *td, *fopen(;
        int     n = DEFAULT, c, col = 0;
        char    *sp;

        if ((argc < 3) || (argc > 4))   {
                fprintf(stderr,"\n");
                fprintf(stderr,"DETAB Version %.2f (C)Copyright T Davis, 1984\n",VERS);
                fprintf(stderr,"Usage: DETAB source desination [n] n");
                fprintf(stderr,"        sorce        - input file\n");
                fprintf(stderr,"        destination  - output file\n");
                fprintf(stderr,"        n (optional) - tab increment\n");
                exit(1);
        }
        sp = argv[1];
        while ((*sp = toupper(*sp)) != EOS) sp++;
        if ((fd = fopen(argv[1],"r")) == NULL){
                fprintf(stderr,"DETAB: File %1 not found.\n",argv[1]);
                exit(2);
        }
        sp = argv[2];
        while ((*sp = toupper(*sp)) != EOS) sp++;
        if ((td = fopen(argv[2],"w")) == NULL){
                fprintf(stderr,"DETAB: Unable to open %s .\n",argv[2]);
                exit(2);
        }
        if (argc == 4){
                (n = atoi(argv[3]));
                if ((n > 32) || (n < 1)){
                        fprintf(stderr,"DETAB: Value for N out of range (%d).",argv[3]);
                exit(2);
                }
        }
        while ((c=fgetc(fd)) != EOF){
                switch (c){
                        case '\t':
                                for (;(((col % n) != 0) || (col == 0)); col++)
                                        fputc(' ',td);
                                break;
                        case '\n':
                                fputc(c,td);
                                col = 0;
                                break;
                        default:
                                fputc(c,td);
                                col ++;
                                break;
                }
        }
        fclose(td);
        fflush(fd);
        fclose(fd);
        return;
}
