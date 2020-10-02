


       /***********************************************
       *
       *       file d:\cips\cutp.c
       *
       *       Functions: This file contains
       *          cut_image_piece
       *          paste_image_piece
       *          check_cut_and_paste_limits
       *
       *       Purpose:
       *          These functions cut pieces out
       *          of images and paste them back into
       *          images.
       *
       *       External Calls:
       *          wtiff.c - does_not_exist
       *                    round_off_image_size
       *                    create_allocate_tiff_file
       *                    write_array_into_tiff_image
       *          tiff.c - read_tiff_header
       *          rtiff.c - read_tiff_image
       *
       *
       *       Modifications:
       *          3 April 1992 - created
       *
       *************************************************/

#include "d:\cips\cips.h"

     /*******************************************
     *
     *   cut_image_piece(...
     *
     *   This function cuts out a rectangular
     *   piece of an image.  This rectangle can
     *   be any shape so long as no dimension
     *   is greater than ROWS or COLS.
     *
     *******************************************/


cut_image_piece(name, the_image, il, ie, ll, le)
   char   name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS];

{
   if(does_not_exist(name)){
      printf("\n\ncut_image_piece>> ERROR "
             "image file does not exist %s", name);
      return(-1);
   }  /* ends if does_not_exist */

   read_tiff_image(name, the_image, il, ie, ll, le);

}  /* ends cut_image_piece */




     /*******************************************
     *
     *   paste_image_piece(...
     *
     *   This function pastes a rectangular
     *   piece of an image into another image.
     *   This rectangle can be any shape so long
     *   as no dimension is greater than ROWS or COLS.
     *   The rectangle to be pasted into the image
     *   is described by the il, ie, ll, le
     *   parameters.
     *
     *   You pass is the out_image array just in
     *   case you need to allocate the destination
     *   image.
     *
     *******************************************/


paste_image_piece(dest_name, source_name, the_image,
                  out_image, il, ie, ll, le)
   char   dest_name[], source_name[];
   int    il, ie, ll, le;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];

{
   struct tiff_header_struct image_header;

   if(does_not_exist(dest_name)){
      printf("\n\ncut_image_piece>> "
             "image file does not exist %s", dest_name);
      read_tiff_header(source_name, &image_header);
      create_allocate_tiff_file(dest_name, &image_header,
                                out_image);
   }  /* ends if does_not_exist */

   write_array_into_tiff_image(dest_name, the_image,
                               il, ie, ll, le);

}  /* ends paste_image_piece */



     /*******************************************
     *
     *   check_cut_and_paste_limits(...
     *
     *   This function looks at the line and
     *   element parameters and ensures that they
     *   are not bigger than ROWS and COLS.  If
     *   they are bigger, the last element or
     *   last line parameters are reduced.
     *
     *******************************************/

check_cut_and_paste_limits(il, ie, ll, le)
   int *il, *ie, *ll, *le;
{
   if((*ll - *il) > ROWS)
      *ll = *il + ROWS;
   if((*le - *ie) > COLS)
      *le = *ie + COLS;
}  /* ends check_cut_and_paste_limits */

