			 /* dump.c */
/*************************************************************

	  3-D Reconstruction of Medical Images

	Three Dimensional Reconstruction Of Medical
	Images from Serial Slices - CT, MRI, Ultrasound


   These programs process a set of slices images (scans) for one
   patient. It outputs two sets of files containing nine predefined
   views of bony surfaces. One set contains distance values and
   the other gradient values.

   The distance values are used as 3-D spatial topographic surface
   coordinate maps for geometrical analysis of the scanned object.

   The gradient values are used for rendering the surface maps on
   CRT displays for subjective viewing where perception of small
   surface details is important.

	Daniel Geist, B.S.
	Michael W. Vannier, M.D.

	Mallinckrodt Institute of Radiology
	Washington University School of Medicine
	510 S. Kingshighway Blvd.
	St. Louis, Mo. 63110

	These programs may be copied and used freely for non-commercial
	purposes by developers with inclusion of this notice.


********************************************************************/

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
