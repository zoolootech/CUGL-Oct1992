/************************** dep.c ****************************

							    */
/*                       Distances.C                        */
/*                 distance shading program                 */

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

#define LINE_SIZE 256
#define HEADER_SIZE 512
#define CTLINE_SIZE 512

int FIRSTSLICE,LASTSLICE,THRESHOLD; /* some global variables */
int  buffer[LINE_SIZE];
char ybuf[LINE_SIZE];

/* input and output files */
char *fnamein="ctbild.000",*fnd="dis.out";

/* Set input file name - add number to file extension name*/
setfilename(filenum)
int filenum;
{
    fnamein[7]=filenum/100+'0';
    fnamein[8]=(filenum%100)/10+'0';
    fnamein[9]='0'+filenum%10;
}
create_distance_shades()
{
    int z,x,y;
    FILE *fd,*fn;
    fd=fopen(fnd,"wb");                  /*open output file */
    for(z=0;z<(LASTSLICE-FIRSTSLICE+1);z++){  /* for each slice*/
        setfilename(FIRSTSLICE+z);       /*open slice file */
        fn=fopen(fnamein,"rb");
        fseek(fn,(long)HEADER_SIZE,SEEK_SET);    /* read header    */
        for(y=0;y<LINE_SIZE;y++){           /*for each line */
            fread(buffer,1,CTLINE_SIZE,fn);   /*read line */
            /* find Threshold transition */
            for(x=1;(x<LINE_SIZE) && (buffer[x]<THRESHOLD);x++);
            if(x<LINE_SIZE) ybuf[y]=LINE_SIZE-x;  /* if transition then distance shade */
            else ybuf[y]=0;           /*else empty */
        }
        fclose(fn);              /*close scan file */
        fwrite(ybuf,1,LINE_SIZE,fd);
        printf("did %d \n",z);
    }
    fclose(fd);                /* close output file */
}


/**********************************************************/
/**** MAIN ***** MAIN ***** MAIN ***** MAIN ***** MAIN ****/
/**********************************************************/
main()
{
    /* first get some parameters from user */
    printf("Enter Starting scan number: ");
    scanf("%d",&FIRSTSLICE);
    printf("Enter ending scan number: ");
    scanf("%d",&LASTSLICE);
    printf("Enter threshold number: ");
    scanf("%d",&THRESHOLD);
    THRESHOLD+=1024;

    create_distance_shades();
}
