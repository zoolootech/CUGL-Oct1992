
       /***********************************************
       *
       *       file d:\cips\addsub.c
       *
       *       Functions: This file contains
       *          add_image_array
       *          subtract_image_array
       *
       *       Purpose:
       *          These functions implement
       *          image addition and subtraction.
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
       *          1 April 1992 - created
       *
       *************************************************/

#include "d:\cips\cips.h"

     /*******************************************
     *
     *   add_image_array(...
     *
     *   This function adds two ROWSxCOLS image
     *   sections.  The image file named out_name
     *   will receive the sum of the image file
     *   named in1_name and the image file
     *   named in2_name.
     *
     *******************************************/


add_image_array(in1_name, in2_name, out_name, the_image, out_image,
          il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3)
   char   in1_name[], in2_name[], out_name[];
   int    il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];

{
   int    i, j, length, max, width;
   struct tiff_header_struct image_header;


   if(does_not_exist(out_name)){
      printf("\n\n output file does not exist %s", out_name);
      read_tiff_header(in1_name, &image_header);
      round_off_image_size(&image_header,
                           &length, &width);
      image_header.image_length = length*ROWS;
      image_header.image_width  = width*COLS;
      create_allocate_tiff_file(out_name, &image_header,
                                out_image);
   }  /* ends if does_not_exist */

   read_tiff_header(in1_name, &image_header);

   max = 255;
   if(image_header.bits_per_pixel == 4)
      max = 16;

   read_tiff_image(in1_name, the_image, il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image, il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         out_image[i][j] = the_image[i][j] + out_image[i][j];
         if(out_image[i][j] > max)
            out_image[i][j] = max;
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

}  /* ends add_image_array */





     /*******************************************
     *
     *   subtract_image_array(...
     *
     *   This function subtracts two ROWSxCOLS image
     *   sections.  The image file named out_name
     *   will receive the difference of the image file
     *   named in1_name and the image file
     *   named in2_name.
     *
     *   out_name = in1_name - in2_name
     *
     *******************************************/


subtract_image_array(in1_name, in2_name, out_name, the_image, out_image,
          il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3)
   char   in1_name[], in2_name[], out_name[];
   int    il1, ie1, ll1, le1,
          il2, ie2, ll2, le2,
          il3, ie3, ll3, le3;
   short  the_image[ROWS][COLS],
          out_image[ROWS][COLS];

{
   int    i, j, length, width;
   struct tiff_header_struct image_header;


   if(does_not_exist(out_name)){
      printf("\n\n output file does not exist %s", out_name);
      read_tiff_header(in1_name, &image_header);
      round_off_image_size(&image_header,
                           &length, &width);
      image_header.image_length = length*ROWS;
      image_header.image_width  = width*COLS;
      create_allocate_tiff_file(out_name, &image_header,
                                out_image);
   }  /* ends if does_not_exist */

   read_tiff_header(in1_name, &image_header);

   read_tiff_image(in1_name, the_image, il1, ie1, ll1, le1);
   read_tiff_image(in2_name, out_image, il2, ie2, ll2, le2);

   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
         out_image[i][j] = the_image[i][j] - out_image[i][j];
         if(out_image[i][j] < 0)
            out_image[i][j] = 0;
      }  /* ends loop over j */
   }  /* ends loop over i */

   write_array_into_tiff_image(out_name, out_image,
                               il3, ie3, ll3, le3);

}  /* ends subtract_image_array */

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



       /***********************************************
       *
       *       file d:\cips\rotate.c
       *
       *       Functions: This file contains
       *          rotate_flip_image_array
       *
       *       Purpose:
       *          This function rotates or flips an image
       *          array in one of five ways.
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
       *          1 April 1992 - created
       *
       *************************************************/


     /*******************************************
     *
     *   rotate_flip_image_array(...
     *
     *   This function rotates an image array
     *   in one of three ways or flips an image
     *   array either vertically or horizontally.
     *   The rotation_type parameter specifies
     *   the operation.  When rotation_type is
     *   1, 2, or 3 you rotate.  When it is
     *   4 or 5 you flip.
     *
     *   I define rotation as this:  Pin down the
     *   lower left hand corner of the image array
     *   and rotate the image 90 degrees clockwise.
     *   1 rotation is 90 degrees, 2 rotations are
     *   180 degrees, and 3 rotations are 270 degrees.
     *   4 rotations bring you back to where you
     *   started.
     *
     *   The cases are:
     *
     *   If the input image array is:
     *        1 2 3
     *        4 5 6
     *        7 8 9
     *
     *   Rotate # 1 - the result is:
     *        7 4 1
     *        8 5 2
     *        9 6 3
     *
     *   Rotate # 2 - the result is:
     *        9 8 7
     *        6 5 4
     *        3 2 1
     *
     *   Rotate # 3 - the result is:
     *        3 6 9
     *        2 5 8
     *        1 4 7
     *
     *   Flip # 4 - horizontal the result is:
     *        3 2 1
     *        6 5 4
     *        9 8 7
     *
     *   Flip # 5 - vertical the result is:
     *        7 8 9
     *        4 5 6
     *        1 2 3
     *
     *
     *   The in_file is the source image with
     *   parameters given by il1, ie1, ll1, le1.
     *
     *   The out_file is the destination image with
     *   parameters given by il2, ie2, ll2, le2.
     *
     *******************************************/


rotate_flip_image_array(in_name, out_name, the_image,
                   out_image, il1, ie1, ll1, le1,
                   il2, ie2, ll2, le2, rotation_type)
   char  in_name[], out_name[];
   int   il1, ie1, ll1, le1,
         il2, ie2, ll2, le2, rotation_type;
   short the_image[ROWS][COLS],
         out_image[ROWS][COLS];
{
   int    cd2, i, j, length, rd2, type, width;
   struct tiff_header_struct image_header;

   if(does_not_exist(out_name)){
      printf("\n\n output file does not exist %s", out_name);
      read_tiff_header(in_name, &image_header);
      round_off_image_size(&image_header,
                           &length, &width);
      image_header.image_length = length*ROWS;
      image_header.image_width  = width*COLS;
      create_allocate_tiff_file(out_name, &image_header,
                                out_image);
   }  /* ends if does_not_exist */

     /*******************************************
     *
     *   Check the rotation_type.  If it is not
     *   a valid value, set it to 1.
     *
     *******************************************/

   type = rotation_type;
   if(type != 1  &&
      type != 2  &&
      type != 3  &&
      type != 4  &&
      type != 5) type = 1;

   read_tiff_image(in_name, the_image, il1, ie1, ll1, le1);

     /*******************************************
     *
     *   Rotate the image array as desired.
     *
     *******************************************/

     /*******************************************
     *
     *   1 90 degree rotation
     *
     *******************************************/

   if(type == 1  ||  type == 2  ||  type == 3){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++)
            out_image[j][COLS-1-i] = the_image[i][j];
      }  /* ends loop over i */
   }  /* ends if type == 1 or 2 or 3 */


     /*******************************************
     *
     *   a second 90 degree rotation
     *
     *******************************************/

   if(type == 2  ||  type == 3){
      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++)
            out_image[j][COLS-1-i] = the_image[i][j];
      }  /* ends loop over i */
   }  /* ends if type == 2 or 3 */


     /*******************************************
     *
     *   a third 90 degree rotation
     *
     *******************************************/

   if(type == 3){
      for(i=0; i<ROWS; i++)
         for(j=0; j<COLS; j++)
            the_image[i][j] = out_image[i][j];
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++)
            out_image[j][COLS-1-i] = the_image[i][j];
      }  /* ends loop over i */
   }  /* ends if type == 3 */


     /*******************************************
     *
     *   Flip the image array horizontally
     *   about the center vertical axis.
     *
     *******************************************/

   if(type == 4){
      cd2 = COLS/2;
      for(j=0; j<cd2; j++){
         for(i=0; i<ROWS; i++){
            out_image[i][COLS-1-j] = the_image[i][j];
         }  /* ends loop over i */
      }  /* ends loop over j */

      for(j=cd2; j<COLS; j++){
         for(i=0; i<ROWS; i++){
            out_image[i][COLS-1-j] = the_image[i][j];
         }  /* ends loop over i */
      }  /* ends loop over j */
   }  /* ends if type == 4 */


     /*******************************************
     *
     *   Flip the image array vertically
     *   about the center horizontal axis.
     *
     *******************************************/

   if(type == 5){
      rd2 = ROWS/2;
      for(i=0; i<rd2; i++){
         for(j=0; j<COLS; j++){
            out_image[ROWS-1-i][j] = the_image[i][j];
         }  /* ends loop over j */
      }  /* ends loop over i */

      for(i=rd2; i<ROWS; i++){
         for(j=0; j<COLS; j++){
            out_image[ROWS-1-i][j] = the_image[i][j];
         }  /* ends loop over j */
      }  /* ends loop over i */
   }  /* ends if type == 5 */

   write_array_into_tiff_image(out_name, out_image,
                               il2, ie2, ll2, le2);

}  /* ends rotate_flip_image_array */
