
Listing 4 - Changes to the function display_image (file display.c)



display_image(file_name, image, il, ie, ll, le,
              image_header, monitor_type, color_transform,
              invert, image_colors, display_colors,
              show_hist)
   char    color_transform[],
           file_name[],
           monitor_type[];
   int     display_colors,
           image_colors,
           invert,
           il,
           ie,
           ll,
           le,
           show_hist;
   short   image[ROWS][COLS];
   struct  tiff_header_struct *image_header;
{
   char  channels[80],
         response[80];

   int   a,
         b,
         c,
         channel,
         count,
         display_mode,
         key,
         horizontal,
         max_horizontal,
         max_vertical,
         not_finished,
         r,
         vertical,
         x_offset,
         y_offset;

   unsigned int block,
                color,
                i,
                j,
                x,
                y;

   unsigned long histogram[256], new_hist[256];



     /*************************************************
     *
     *   If you want to display the histogram and do not
     *   want to perform hist equalization, then
     *   zero the histogram for calculations.
     *
     *************************************************/

   if(  (show_hist == 1)   &&
        (color_transform[0] != 'H'))
      zero_histogram(histogram);

   not_finished = 1;
   while(not_finished){


      if(display_colors == 16){
         if(monitor_type[0] == 'V'){
            horizontal   = 4;
            vertical     = 6;
            display_mode = _VRES16COLOR; /* MSC 6.0 */
         }  /* ends if V */
         if(monitor_type[0] == 'E'){
            horizontal   = 3;
            vertical     = 6;
            display_mode = _ERESCOLOR; /* MSC 6.0 */
         }  /* ends if E */

      }  /* ends if colors == 16 */

      else{
         horizontal   = 2;
         vertical     = 2;
         display_mode = _MAXCOLORMODE; /* MSC 6.0 */
      }


      max_horizontal = (image_header->image_length+50)/100;
      max_vertical   = (image_header->image_width+50)/100;

      if(horizontal > max_horizontal) horizontal = max_horizontal;
      if(vertical > max_vertical) vertical = max_vertical;



        /****************************************
        *
        *   If color transform wants histogram
        *   equalization, then read in the
        *   image arrays and calculate the
        *   histogram.   Zero both the histogram
        *   and the new_hist.  You will need the
        *   new_hist if you want to display the
        *   equalized hist.
        *
        *****************************************/

      if(color_transform[0] == 'H'){
         count = 1;
         zero_histogram(histogram);
         zero_histogram(new_hist);
         for(a=0; a<vertical; a++){
            for(b=0; b<horizontal; b++){

               x = a*100;
               y = b*100;

               printf("\nDISPLAY> Calculating histogram");
               printf(" %d of %d",count,vertical*horizontal);
               count++;
               read_tiff_image(file_name, image, il+y,
                            ie+x, ll+y, le+x);
               calculate_histogram(image, histogram);

            }  /* ends loop over b */
         }  /* ends loop over a */
      }  /* ends if display_mode == H */



        /* set graphics mode */

   _setvideomode(display_mode); /* MSC 6.0 */
   if(display_colors == 16) map_16_shades_of_gray(display_mode);



        /****************************************
        *
        *   Loop over this size and
        *   read and display ROWSxCOLS arrays.
        *
        *   If you want to show the histogram AND
        *   do not want to do hist equalization
        *   then calculate the hist from the 
        *   original image array.
        *
        *   If you want to do hist equalization
        *   then calculate the new_hist AFTER
        *   the image has been equalized by the
        *   the transform_the_colors function.
        *
        *   NOTE: Remember that the function
        *   transform_the_colors changes the
        *   gray shade values in image array.
        *
        *****************************************/

      for(a=0; a<vertical; a++){
         for(b=0; b<horizontal; b++){

            x = a*100;
            y = b*100;
            read_tiff_image(file_name, image, il+y, 
                            ie+x, ll+y, le+x);
            if(  (show_hist == 1)  &&
                 (color_transform[0] != 'H'))
               calculate_histogram(image, histogram);

            transform_the_colors(image, color_transform,
                                 display_colors,
                                 image_colors, histogram,
                                 horizontal, vertical);

            if(color_transform[0] == 'H')
               calculate_histogram(image, new_hist);
            display_image_portion(image, x, y, display_colors, 
                                  image_colors, invert);
         }        /* ends loop over b */
      }        /* ends loop over a */


         /***************************
         *
         *   if show_hist == 1 then
         *   display the histogram
         *   in the lower right hand
         *   corner of screen
         *
         *   If hist equalization was
         *   performed then show the
         *   new_hist.  If it wasn't
         *   done then show the original
         *   histogram.
         *
         ****************************/

      if(show_hist == 1){
         if(monitor_type[0] == 'V')
            y_offset = 470;
         if(monitor_type[0] == 'E')
            y_offset = 310;
         x_offset = 380;
         if(color_transform[0] == 'H')
            display_histogram(new_hist, x_offset,
                   y_offset, 10, 15);
         else
            display_histogram(histogram, x_offset,
                   y_offset, 10, 15);
      }

      read_string(response);
      printf("\nEnter 0 to quit 1 to do again");
      get_integer(&not_finished);

          /* set display back to text mode */
      clear_text_screen();


   }  /* ends while not_finished  */

}

