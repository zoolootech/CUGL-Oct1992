
Listing 6 - Changes to the function print_graphics_image (file
djet.c)





print_graphics_image(image1, image2, image_name,
                     il, ie, ll, le, image_colors,
                     invert, caption, show_hist,
                     color_transform)
   char  caption[], image_name[], color_transform[];
   int   image_colors, invert,
         il, ie, ll, le, show_hist;
   short image1[ROWS][COLS], image2[ROWS][COLS];
{
   char c[80],
        page[80];

   FILE *printer;

   int  i,
        j;

   unsigned long histogram[256], final_hist[256];
   printer = fopen("prn", "w");


      /**********************************************
      *
      *   Print a few blank lines on the page.
      *
      ***********************************************/

   strcpy(page, "                             ");
   my_fwriteln(printer, page);
   my_fwriteln(printer, page);


      /*****************************************************
      *
      *   Read in two image arrays.
      *
      ******************************************************/

   printf("\nReading image");
   read_tiff_image(image_name, image1, il, ie, ll, le);


   printf("\nReading image");
   read_tiff_image(image_name, image2, il, ie+100, ll, le+100);

      /*****************************************************
      *
      *   If show_hist is 1 OR do hist equalization
      *   then zero the histogram and
      *   calculate it for the two image arrays.
      *
      ******************************************************/

   if( (show_hist == 1)  ||
       (color_transform[0] == 'H')){
      zero_histogram(histogram);
      zero_histogram(final_hist);
      printf("\nDJET> Calculating histograms");
      calculate_histogram(image1, histogram);
      calculate_histogram(image2, histogram);
   }

        /**************************************************
        *
        *   Alter the images to 64 gray shades.
        *   Either do it with straight multiply
        *   and divide or use hist equalization.
        *
        *   If using hist equalization then you must
        *   also read and calculate the hist for
        *   the other two image arrays that will be
        *   printed.
        *
        ***************************************************/

   if(color_transform[0] == 'S'){
   if(image_colors == 256){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
            image1[i][j] = image1[i][j]/4;
            image2[i][j] = image2[i][j]/4;
        }
      }
   }  /* ends if image_colors == 256 */


   if(image_colors == 16){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
            image1[i][j] = image1[i][j]*4;
            image2[i][j] = image2[i][j]*4;
        }
      }
   }  /* ends if image_colors == 16 */
   }  /* ends if color_transform == S */

   if(color_transform[0] == 'H'){
      printf("\nReading image");
      read_tiff_image(image_name, image1, il+100, ie, ll+100, le);
      printf("\nReading image");
      read_tiff_image(image_name, image2,
                   il+100, ie+100, ll+100, le+100);
      printf("\nDJET> Calculating histograms");
      calculate_histogram(image1, histogram);
      calculate_histogram(image2, histogram);

      printf("\nReading image");
      read_tiff_image(image_name, image1, il, ie, ll, le);

      printf("\nReading image");
      read_tiff_image(image_name, image2, il, ie+100, ll, le+100);

      printf("\nDJET> Equalizing histogram");
      perform_histogram_equalization(image1, histogram,
                                     64.0, 40000.0);
      printf("\nDJET> Equalizing histogram");
      perform_histogram_equalization(image2, histogram,
                                     64.0, 40000.0);

      printf("\nDJET> Calculating histograms");
      calculate_histogram(image1, final_hist);
      calculate_histogram(image2, final_hist);


   }  /* ends if color_transform == H */



      /************************************************
      *
      *   If invert is set them invert the transformed
      *   image arrays (they now only have 64 shades
      *   of gray).
      *
      *************************************************/

   if(invert == 1){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
               image1[i][j] = 63 - image1[i][j];
               image2[i][j] = 63 - image2[i][j];
         }
      }
   }



        /**************************************************
        *
        *   Now set the graphics mode on the printer
        *
        ***************************************************/

   printf("\nBegin");
   end_graphics_mode(printer);
   select_300_dpi_resolution(printer);
   set_raster_width(printer);
   start_raster_graphics(printer);
   select_full_graphics_mode(printer);

        /**************************************************
        *
        *   Print the two arrays to make a 100x200 output.
        *   To do this you loop over 100 rows, set the 
        *   r buffer to the image values, set the
        *   graphics, and print the row via function
        *   print_original_200_row.
        *
        ***************************************************/

   for(i=0; i<100; i++){
      for(j=0; j<100; j++){
         r[j]     = image1[i][j];
         r[j+100] = image2[i][j];
      }  /* ends loop over j */

      end_graphics_mode(printer);
      select_300_dpi_resolution(printer);
      set_raster_width(printer);
      start_raster_graphics(printer);
      select_full_graphics_mode(printer);

      print_original_200_row(printer, r);

      printf("\n\tPrinting row %d", i);
   }  /* ends loop over i */

           /* ends first half */



      /*****************************************************
      *
      *   In order to print 200x200 repeat
      *   the above steps for 2 more 100x100 arrays
      *
      ******************************************************/


   printf("\nReading image");
   read_tiff_image(image_name, image1, il+100, ie, ll+100, le);
   printf("\nReading image");
   read_tiff_image(image_name, image2,
                   il+100, ie+100, ll+100, le+100);


        /**************************************************
        *
        *   Alter the images to 64 shades of gray.
        *
        *   Either do it with straight multiply
        *   and divide or use hist equalization.
        *
        ***************************************************/


   if(color_transform[0] == 'S'){
   if(image_colors == 256){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
            image1[i][j] = image1[i][j]/4;
            image2[i][j] = image2[i][j]/4;
        }
      }
   }  /* ends if image_colors == 256 */


   if(image_colors == 16){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
            image1[i][j] = image1[i][j]*4;
            image2[i][j] = image2[i][j]*4;
        }
      }
   }  /* ends if image_colors == 16 */
   }  /* ends if color_transform == S */



   if(color_transform[0] == 'H'){

      printf("\nDJET> Equalizing histogram");
      perform_histogram_equalization(image1, histogram,
                                     64.0, 40000.0);
      printf("\nDJET> Equalizing histogram");
      perform_histogram_equalization(image2, histogram,
                                     64.0, 40000.0);

      printf("\nDJET> Calculating histograms");
      calculate_histogram(image1, final_hist);
      calculate_histogram(image2, final_hist);

   }  /* ends if color_transform == S */



      /************************************************
      *
      *   If invert is set them invert the transformed
      *   image arrays (they now only have 64 shades
      *   of gray).
      *
      *************************************************/

   if(invert == 1){
      for(i=0; i<ROWS; i++){
         for(j=0; j<COLS; j++){
               image1[i][j] = 63 - image1[i][j];
               image2[i][j] = 63 - image2[i][j];
         }
      }
   }



   printf("\nBegin");
   end_graphics_mode(printer);
   select_300_dpi_resolution(printer);
   set_raster_width(printer);
   start_raster_graphics(printer);
   select_full_graphics_mode(printer);


        /**************************************************
        *
        *   Print the two arrays to make a 100x200 output.
        *   To do this you loop over 100 rows, set the 
        *   r buffer to the image values, set the
        *   graphics, and print the row via function
        *   print_original_200_row.
        *
        ***************************************************/

   for(i=0; i<100; i++){
      for(j=0; j<100; j++){
         r[j]     = image1[i][j];
         r[j+100] = image2[i][j];
      }  /* ends loop over j */

      end_graphics_mode(printer);
      select_300_dpi_resolution(printer);
      set_raster_width(printer);
      start_raster_graphics(printer);
      select_full_graphics_mode(printer);

      print_original_200_row(printer, r);

      printf("\n\tPrinting row %d", i);
   }  /* ends loop over i */


      /*****************************************************
      *
      *   If show_hist is 1 then calculate the histogram
      *   for the two image arrays and print the histogram.
      *
      ******************************************************/

   if(show_hist == 1){
      if(color_transform[0] == 'S'){
         calculate_histogram(image1, histogram);
         calculate_histogram(image2, histogram);
         print_hist_image(printer, histogram);
      }
      if(color_transform[0] == 'H'){
         print_hist_image(printer, final_hist);
      }
   }

        /**************************************************
        *
        *   Print a couple of blank lines then print      
        *   the caption. 
        *
        ***************************************************/

   end_graphics_mode(printer);
   strcpy(page, "      ");
   my_fwriteln(printer, page);
   my_fwriteln(printer, page);

   sprintf(page, "                      %s", caption);
   my_fwriteln(printer, page);

   putc(FORMFEED, printer);

   fclose(printer);

   printf("\nEnd");

}  /* ends print_graphics_image */


