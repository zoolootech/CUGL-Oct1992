

       /***********************************************
       *
       *       file d:\cips\half.c
       *
       *       Functions: This file contains
       *          main
       *
       *       Purpose:
       *          This file contains the main calling
       *          routine for a program which shrinks
       *          an image in half (600x600 to a   
       *          300x300).  The output image will 
       *          always be an even multiple of
       *          ROWS and COLS.
       *
       *       External Calls:
       *          gin.c - get_image_name
       *          numcvrt.c - get_integer
       *                      int_convert
       *          tiff.c - read_tiff_header
       *          scale.c - shrink_image_array
       *
       *       Modifications:
       *          25 April 1992 - created
       *
       *************************************************/

#include "d:\cips\cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{

   char     method[80], name[80], name2[80];
   int      count, i, j, length, width,
            il, ie, ll, le;
   struct   tiff_header_struct image_header;

   _setvideomode(_TEXTC80); /* MSC 6.0 statements */
   _setbkcolor(1);
   _settextcolor(7);
   _clearscreen(_GCLEARSCREEN);

       /***********************************************
       *
       *       Interpret the command line parameters.
       *
       ************************************************/

   if(argc < 4 || argc > 4){
    printf(
     "\n"
     "\n usage: half in-file out-file method"
     "\n        method can be Average, Median, Corner"
     "\n");
    exit(0);
   }

   strcpy(name,   argv[1]);
   strcpy(name2,  argv[2]);
   strcpy(method, argv[3]);

   if(method[0] != 'A' &&
      method[0] != 'a' &&
      method[0] != 'M' &&
      method[0] != 'm' &&
      method[0] != 'C' &&
      method[0] != 'c'){
      printf("\nERROR: Did not enter a valid method"
             "\n       The valid methods are:"
             "\n       Average, Median, Corner");
       printf(
        "\n"
        "\n usage: half in-file out-file method"
        "\n        method can be Average, Median, Corner"
        "\n");
      exit(-2);
   }

   il = 1;
   ie = 1;
   ll = ROWS+1;
   le = COLS+1;

       /***********************************************
       *
       *       Read the input image header and setup
       *       the looping counters.
       *
       *       Halve the looping counters.
       *
       *       Create the output image.
       *
       ************************************************/

   read_tiff_header(name, &image_header);

   length = (90 + image_header.image_length)/ROWS;
   width  = (90 + image_header.image_width)/COLS;
   if( (length % 2) != 0) length++;
   if( (width  % 2) != 0) width++;
   length = length/2;
   width  = width/2;
   count  = 1;

   image_header.image_length = length*ROWS;
   image_header.image_width  = width*COLS;
   create_allocate_tiff_file(name2, &image_header,
                             out_image);

       /***********************************************
       *
       *   Read and shrink each 200x200 area of the
       *   input image and write them to the output
       *   image.
       *
       ************************************************/

   count = 1;
   for(i=0; i<length; i++){
      for(j=0; j<width; j++){
         printf("\nrunning %d of %d", count++, length*width);
         shrink_image_array(name, name2, 
                            the_image, out_image,
                            il+i*ROWS*2, ie+j*COLS*2,
                            ll+i*ROWS*2, le+j*COLS*2,
                            il+i*ROWS, ie+j*COLS,
                            ll+i*ROWS, le+j*COLS,
                             2, method);
      }  /* ends loop over j */
   }  /* ends loop over i */
}  /* ends main  */
