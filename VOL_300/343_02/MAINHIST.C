



       /***********************************************
       *
       *       file d:\cips\mainhist.c
       *
       *       Functions: This file contains
       *          main
       *
       *       Purpose:
       *          This file contains the main calling
       *          routine which will perform histogram
       *          equalization on an input image to
       *          produce an output image.
       *
       *       External Calls:
       *          gin.c - get_image_name
       *          numcvrt.c - get_integer
       *                      int_convert
       *          tiff.c - read_tiff_header
       *            rtiff.c - read_tiff_image
       *            wtiff.c - write_array_into_tiff_image
       *            hist.c - calculate_histogram
       *                     zero_histogram
       *                     perform_histogram_equalization
       *
       *       Modifications:
       *          17 March 1992 - created  
       *
       *************************************************/

#include "d:\cips\cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{

   char name[80], name2[80];
   float new_grays, area;
   int  count, i, ie, il, j, le, length, ll, size,
        t, type, v, width, x, y;

   unsigned long histogram[256], new_hist[256];


   struct   tiff_header_struct image_header;

   _setvideomode(_TEXTC80); /* MSC 6.0 statements */
   _setbkcolor(1);
   _settextcolor(7);
   _clearscreen(_GCLEARSCREEN);

   if(argc < 3){
    printf("\n\nNot enough parameters:");
    printf("\n");
    printf("\n   usage: mainhist in-file out-file  ");
    exit(0);
   }

   strcpy(name, argv[1]);
   strcpy(name2, argv[2]);

   if(does_not_exist(name2)){
      read_tiff_header(name, &image_header);
      round_off_image_size(&image_header,
                           &length, &width);
      image_header.image_length = length*ROWS;
      image_header.image_width  = width*COLS;
      create_allocate_tiff_file(name2, &image_header,
                                the_image);
   }  /* ends if does_not_exist */


   il = 1;
   ie = 1;
   ll = ROWS+1;
   le = COLS+1;

   read_tiff_header(name, &image_header);

   length = (90 + image_header.image_length)/ROWS;
   width  = (90 +image_header.image_width)/COLS;
   count  = 1;
   printf("\nlength=%d  width=%d", length, width);

   zero_histogram(histogram);
   zero_histogram(new_hist);


   for(i=0; i<length; i++){
      for(j=0; j<width; j++){
        printf("\nCalculating Histogram %d of %d", count, length*width);
        count++;

         read_tiff_image(name, the_image, il+i*ROWS,
                        ie+j*COLS, ll+i*ROWS, le+j*COLS);
         calculate_histogram(the_image, histogram);


      } /* ends loop over j */
   }  /* ends loop over i */


   area      = ((long)(length))*((long)(width));
   area      = area*10000.0;
   new_grays = 250;

   count = 1;

   for(i=0; i<length; i++){
      for(j=0; j<width; j++){
        printf("\nDoing equalization %d of %d", count, length*width);
        count++;

         read_tiff_image(name, the_image, il+i*ROWS,
                        ie+j*COLS, ll+i*ROWS, le+j*COLS);
         perform_histogram_equalization(the_image, histogram,
                                          new_grays, area);
         write_array_into_tiff_image(name2, the_image,
                                     il+i*ROWS, ie+j*COLS, 
                                    ll+i*ROWS, le+j*COLS);
      } /* ends loop over j */
   }  /* ends loop over i */

}  /* ends main  */
