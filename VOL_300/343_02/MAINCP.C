
       /***********************************************
       *
       *       file d:\cips\maincp.c
       *
       *       Functions: This file contains
       *          main
       *
       *       Purpose:
       *          This file contains the main calling
       *          routine for a program which 
       *          cuts pieces from one image and pastes
       *          them into another.
       *
       *       External Calls:
       *          gin.c - get_image_name
       *          numcvrt.c - get_integer
       *                      int_convert
       *          tiff.c - read_tiff_header
       *          cutp.c - cut_image_piece
       *                   paste_image_piece
       *
       *       Modifications:
       *          8 April 1992 - created
       *
       *************************************************/

#include "d:\cips\cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main(argc, argv)
   int argc;
   char *argv[];
{

   char     name[80], name2[80];
   int      count, length, width;
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

   if(argc < 3 || argc > 3){
    printf(
     "\n"
     "\n usage: maincp in-file out_file "
     "\n");
    exit(0);
   }

   strcpy(name, argv[1]);
   strcpy(name2, argv[2]);

   if(does_not_exist(name2)){
      printf("\n\n output file does not exist %s", name2);
      read_tiff_header(name, &image_header);
      round_off_image_size(&image_header,
                           &length, &width);
      image_header.image_length = length*ROWS;
      image_header.image_width  = width*COLS;
      create_allocate_tiff_file(name2, &image_header,
                                out_image);
   }  /* ends if does_not_exist */

       /***********************************************
       *
       *   Cut the three center sections from a
       *   300x300 image and paste them into another 
       *   image.
       *
       ************************************************/

   cut_image_piece(name, the_image, 1, 101, 101, 201);
   paste_image_piece(name2, name, the_image, out_image, 
                     1, 101, 101, 201);

   cut_image_piece(name, the_image, 101, 101, 201, 201);
   paste_image_piece(name2, name, the_image, out_image, 
                     101, 101, 201, 201);

   cut_image_piece(name, the_image, 201, 101, 301, 201);
   paste_image_piece(name2, name, the_image, out_image, 
                     201, 101, 301, 201);

}  /* ends main */
