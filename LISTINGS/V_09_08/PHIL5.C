
Listing 5 - Changes to the Function display_using_halftoning (file
ht.c)



display_using_halftoning(in_image, file_name,
         il, ie, ll, le, threshold, invert,
         image_colors, image_header, monitor_type,
         print, show_hist, color_transform)

   char  color_transform[], file_name[], monitor_type[];
   int   image_colors, invert,
         il, ie, ll, le, threshold,
         print, show_hist;
   short in_image[ROWS][COLS];
   struct tiff_header_struct *image_header;

{
   char response[80];

   int  a,
        b,
        channel,
        color,
        count,
        data_color,
        display_mode,
        horizontal,
        i,
        j,
        k,
        l,
        line_color,
        max_horizontal,
        max_vertical,
        not_finished,
        one,
        vertical,
        x,
        x_offset,
        y,
        y_offset,
        zero;

   float area, new_grays;

   unsigned long histogram[256], new_hist[256];


       if(  (show_hist == 1)  &&
            (color_transform[0] != 'H'))
          zero_histogram(histogram);
         /*******************************************
         *
         *   Use the monitor type to set the vertical
         *   horizontal and display_mode parameters.
         *   Also set the values for one and zero.
         *   one and zero will vary depending on the
         *   monitor type.
         *
         ********************************************/


      if(  (monitor_type[0] == 'M')  ||
           (monitor_type[0] == 'm')){
         vertical     = 3;
         horizontal   = 2;
         display_mode = _HRESBW;
         one          = 1;
         zero         = 0;
      }

      if(  (monitor_type[0] == 'C')  ||
           (monitor_type[0] == 'c')){
         vertical     = 3;
         horizontal   = 2;
         display_mode = _MRES4COLOR;
         one          = 3;
         zero         = 1;
      }

      if(  (monitor_type[0] == 'V')  ||
           (monitor_type[0] == 'v')){
         vertical     = 6;
         horizontal   = 4;
         display_mode = _VRES16COLOR;
         one          = 5;
         zero         = 1;
      }

      if(  (monitor_type[0] == 'E')  ||
           (monitor_type[0] == 'e')){
         vertical     = 6;
         horizontal   = 3;
         display_mode = _ERESCOLOR;
         one          = 5;
         zero         = 1;
      }

      max_horizontal = (image_header->image_length+50)/100;
      max_vertical   = (image_header->image_width+50)/100;

      if(horizontal > max_horizontal) horizontal = max_horizontal;
      if(vertical > max_vertical) vertical = max_vertical;
      if(print == 1){
         vertical   = 1;
         horizontal = 1;
      }



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

               printf("\nHT> Calculating histogram");
               printf(" %d of %d",count,vertical*horizontal);
               count++;

               read_tiff_image(file_name, in_image, il+y,
                            ie+x, ll+y, le+x);
               calculate_histogram(in_image, histogram);

            }  /* ends loop over b */
         }  /* ends loop over a */
      }  /* ends if display_mode == H */




           /* set graphics mode */
      if(print == 0)
         _setvideomode(display_mode); /* MSC 6.0 */
      else{
         printf("\n\nHT> Calculating for printing ");
         printf("\nHT> Counting from 0 to 99\n");
      }

        /*********************************************
        *
        *   Loop over horizontal and vertical. Read
        *   the image array and display it after
        *   calculating the half tone values.
        *
        *
        *   If you want to show the histogram AND
        *   do not want to do hist equalization
        *   then calculate the hist from the 
        *   original image array.
        *
        *   If you want to do hist equalization
        *   then calculate the new_hist AFTER
        *   the image has been equalized.
        *
        *   We will equalize the histogram down
        *   to half the original shades of gray
        *   and will cut the threshold in half.
        *
        *****************************************/



      for(i=0; i<horizontal; i++){
         for(j=0; j<vertical; j++){
            read_tiff_image(file_name, in_image, il+i*100,
                           ie+j*100, ll+i*100, le+j*100);

            if(   (show_hist == 1)  &&
                  (color_transform[0] != 'H'))
               calculate_histogram(in_image, histogram);

            if(color_transform[0] == 'H'){

               area = ((long)(vertical))*((long)(horizontal));
               area = area*10000.0;
               new_grays = image_colors/2;

               perform_histogram_equalization(in_image,
                        histogram, new_grays, area);

               calculate_histogram(in_image, new_hist);

            }  /* ends if color_transform == S */

            if(color_transform[0] == 'H')
               half_tone(in_image, threshold/2, eg, ep, i, j,
                      one, zero, invert, print);

            else
               half_tone(in_image, threshold, eg, ep, i, j,
                      one, zero, invert, print);

         }  /* ends loop over j */
      }  /*  ends loop over i */



         /***************************
         *
         *   if show_hist == 1 then
         *   display the histogram
         *   in the lower right hand
         *   corner of screen
         *
         ****************************/

      if(  (show_hist == 1)   &&
           (print == 0)){

         if(monitor_type[0] == 'V'){
            y_offset   = 470;
            x_offset   = 380;
            line_color = 3;
            data_color = 8;
         }

         if(monitor_type[0] == 'E'){
            y_offset   = 310;
            x_offset   = 380;
            line_color = 3;
            data_color = 8;
         }

         if(monitor_type[0] == 'M'){
            y_offset   = 190;
            x_offset   = 60;
            line_color = 1;
            data_color = 1;
         }

         if(monitor_type[0] == 'C'){
            y_offset   = 190;
            x_offset   = 60;
            line_color = 1;
            data_color = 3;
         }

         if(color_transform[0] == 'S')
            display_histogram(histogram, x_offset,
                   y_offset, line_color, data_color);

         if(color_transform[0] == 'H')
            display_histogram(new_hist, x_offset,
                   y_offset, line_color, data_color);

      }  /* ends if show_hist == 1 and print == 0 */



      if(print == 1) printf("\n\nHT> Hit ENTER to continue");
      read_string(response);
      clear_text_screen();

}

