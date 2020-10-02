/*************************** dumppp.c ******************************

Utility program to dump a data file on the console screen to
permit inspection of its contents. This program was intended for
use with image data files for evaluation of their contents.

Daniel Geist
Michael Vannier

Mallinckrodt Institute of Radiology
Washiington University School of Medicine
510 S. Kingshighway Blvd.
St. Louis, Mo. 63110

******************************************************************/

#include <stdio.h>
/* #include <stdlib.h> */
#include <ctype.h>

unsigned char buffer[256];


main(argc,argv)

int argc;
char *argv[];

{ 
    int i,j,k,read=256;
    FILE *fn;

    if (argc<2) error_exit("no file given");
    fn=fopen(argv[1],"rb");
    if(fn==0) error_exit("could not open file");
    if(argc>2) 
    { 
        j=atoi(argv[2]);
        if (j>0) for(i=0;i<j;i++) read=fread(buffer,1,256,fn);
        else j=0;
    }
    else j=0;

    while(read==256)
    {
        printf(" \n\n               block no. %d\n\n",j++);
        read=fread(buffer,1,256,fn);
        for (i=read;i<256;i++) buffer[i]=0;
        for (i=0;i<16;i++)
        {
            for(k=0;k<16;k++) printf(" %.2X",buffer[i*16+k]);
            printf("   ");
            for (k=0;k<16;k++) 
                putchar(isprint(buffer[i*16+k])?buffer[i*16+k]:'.');    
            printf("  %.3d\n",i*16);
        }
    } 
}

error_exit(message)
char *message;
{ 
    printf("DUMP -- %s\n",message);
    exit(0);
}
