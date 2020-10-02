

   /********************************************************
   *
   *   file d:\cips\djet.c
   *
   *   Functions: This file contains
   *      end_graphics_mode
   *      get_graphics_caption
   *      print_bytes
   *      print_graphics_image
   *      print_original_200_row
   *      select_300_dpi_resolution
   *      select_full_graphics_mode
   *      set_horizontal_offset
   *      set_raster_width
   *      start_raster_graphics
   *
   *   Purpose:
   *      These functions print a 200x200 image using
   *      dithering to an HP DeskJet or compatable (Laserjet).  
   *      This uses an 8x8 matrix which gives 64 shades of 
   *      gray.
   *
   *   External Calls:
   *          rtiff.c - read_tiff_image
   *           hist.c - zero_histogram
   *                    calculate_histogram
   *                    perform_histogram_equalization
   *
   *
   *   Modifications:
   *      January 1991 - created
   *      25 August 1991 - modified for use in the 
   *         C Image Processing System.
   *
    ÚÄÄÄÄÄ¿   ÚÄÄÄÄÄ¿
    ³     ³   ³     ³   The function print_graphics_image
    ³     ³   ³     ³   begins with 2 100x100 image arrays
    ³     ³   ³     ³
    ³     ³   ³     ³
    ÀÄÄÄÄÄÙ   ÀÄÄÄÄÄÙ          
              
    ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
    ³               ³   It joins them into 
    ³               ³   1 100x200 image array
    ³               ³
    ³               ³
    ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
        
    ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
    ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
    ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
    ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ 
           .            It loops and creates
           .            100 200 element image arrays
           .
    ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
    ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
    
    
          The function print_original_200_row receives a 200 
          element array
    ÚÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂ¿
    ÀÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÙ

          This array is transformed into a 8x200 array of 
          characters called 'row'
    ÚÄÄÄÄÄÄÄÄÄ ... þÄÄÄÄÄÄÄ¿
    ÃÄÄÄÄÄÄÄÄÄ ... þÄÄÄÄÄÄÄ´
    ÃÄÄÄÄÄÄÄÄÄ ... þÄÄÄÄÄÄÄ´
    ÃÄÄÄÄÄÄÄÄÄ ... þÄÄÄÄÄÄÄ´
    ÃÄÄÄÄÄÄÄÄÄ ... þÄÄÄÄÄÄÄ´
    ÃÄÄÄÄÄÄÄÄÄ ... þÄÄÄÄÄÄÄ´
    ÃÄÄÄÄÄÄÄÄÄ ... þÄÄÄÄÄÄÄ´
    ÃÄÄÄÄÄÄÄÄÄ ... þÄÄÄÄÄÄÄ´
    ÀÄÄÄÄÄÄÄÄÄ ... þÄÄÄÄÄÄÄÙ

          Each column of this array is a 1x8 character array which 
          is an 8-bit x 8-bit array
    ÉÍÍ»
    º  º
    ÈÍÍ¼
          Each row of 'row' is passed to the funnction print_bytes 
          for graphics printing 
    
    




   ********************************************************/


#include "d:\cips\cips.h"

#define ESCAPE 27
#define FORMFEED  '\014'

short r[200];



      /*******************************************
      *
      *   The patterns array holds the rows to the
      *   8x8 matrices used for printing 
      *   shades of gray.
      *
      ********************************************/

char patterns[64][8] =
   { {255, 255, 255, 255, 255, 255, 255, 255},
     {255, 255, 255, 255, 255, 255, 255, 127},
     {255, 255, 255, 255, 255, 255, 255,  63},
     {255, 255, 255, 255, 255, 255, 255,  31},
     {255, 255, 255, 255, 255, 255, 255,  15},
     {255, 255, 255, 255, 255, 255, 255,   7},
     {255, 255, 255, 255, 255, 255, 255,   3},
     {255, 255, 255, 255, 255, 255, 255,   1},
     {255, 255, 255, 255, 255, 255, 255,   0},
     {255, 255, 255, 255, 255, 255, 127,   0},
     {255, 255, 255, 255, 255, 255,  63,   0},
     {255, 255, 255, 255, 255, 255,  31,   0},
     {255, 255, 255, 255, 255, 255,  15,   0},
     {255, 255, 255, 255, 255, 255,   7,   0},
     {255, 255, 255, 255, 255, 255,   3,   0},
     {255, 255, 255, 255, 255, 255,   1,   0},
     {255, 255, 255, 255, 255, 255,   0,   0},
     {255, 255, 255, 255, 255, 127,   0,   0},
     {255, 255, 255, 255, 255,  63,   0,   0},
     {255, 255, 255, 255, 255,  31,   0,   0},
     {255, 255, 255, 255, 255,  15,   0,   0},
     {255, 255, 255, 255, 255,   7,   0,   0},
     {255, 255, 255, 255, 255,   3,   0,   0},
     {255, 255, 255, 255, 255,   1,   0,   0},
     {255, 255, 255, 255, 255,   0,   0,   0},
     {255, 255, 255, 255, 127,   0,   0,   0},
     {255, 255, 255, 255,  63,   0,   0,   0},
     {255, 255, 255, 255,  31,   0,   0,   0},
     {255, 255, 255, 255,  15,   0,   0,   0},
     {255, 255, 255, 255,   7,   0,   0,   0},
     {255, 255, 255, 255,   3,   0,   0,   0},
     {255, 255, 255, 255,   1,   0,   0,   0},
     {255, 255, 255, 255,   0,   0,   0,   0},
     {255, 255, 255, 127,   0,   0,   0,   0},
     {255, 255, 255,  63,   0,   0,   0,   0},
     {255, 255, 255,  31,   0,   0,   0,   0},
     {255, 255, 255,  15,   0,   0,   0,   0},
     {255, 255, 255,   7,   0,   0,   0,   0},
     {255, 255, 255,   3,   0,   0,   0,   0},
     {255, 255, 255,   1,   0,   0,   0,   0},
     {255, 255, 255,   0,   0,   0,   0,   0},
     {255, 255, 127,   0,   0,   0,   0,   0},
     {255, 255,  63,   0,   0,   0,   0,   0},
     {255, 255,  31,   0,   0,   0,   0,   0},
     {255, 255,  15,   0,   0,   0,   0,   0},
     {255, 255,   7,   0,   0,   0,   0,   0},
     {255, 255,   3,   0,   0,   0,   0,   0},
     {255, 255,   1,   0,   0,   0,   0,   0},
     {255, 255,   0,   0,   0,   0,   0,   0},
     {255, 127,   0,   0,   0,   0,   0,   0},
     {255,  63,   0,   0,   0,   0,   0,   0},
     {255,  31,   0,   0,   0,   0,   0,   0},
     {255,  15,   0,   0,   0,   0,   0,   0},
     {255,   7,   0,   0,   0,   0,   0,   0},
     {255,   3,   0,   0,   0,   0,   0,   0},
     {255,   1,   0,   0,   0,   0,   0,   0},
     {255,   0,   0,   0,   0,   0,   0,   0},
     {127,   0,   0,   0,   0,   0,   0,   0},
     { 63,   0,   0,   0,   0,   0,   0,   0},
     { 31,   0,   0,   0,   0,   0,   0,   0},
     { 15,   0,   0,   0,   0,   0,   0,   0},
     {  7,   0,   0,   0,   0,   0,   0,   0},
     {  3,   0,   0,   0,   0,   0,   0,   0},
     {  1,   0,   0,   0,   0,   0,   0,   0}};





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



   /****************************************************
   *
   *   get_graphics_caption(...
   *
   ****************************************************/

get_graphics_caption(caption)
   char caption[];
{
   printf("\nEnter the caption to be printed\n---");
   read_string(caption);

}  /* ends get_graphics_caption */





select_full_graphics_mode(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('b', printer);
   putc('0', printer);
   putc('M', printer);

}


set_horizontal_offset(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('b', printer);
   putc('4', printer);
   putc('9', printer);
   putc('6', printer);
   putc('X', printer);

}






set_shorter_horizontal_offset(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('b', printer);
   putc('4', printer);
   putc('8', printer);
   putc('0', printer);
   putc('X', printer);

}






end_graphics_mode(printer)
   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('r', printer);
   putc('B', printer);
}


set_raster_width(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('r', printer);
   putc('2', printer);
   putc('2', printer);
   putc('0', printer);
   putc('0', printer);
   putc('S', printer);

}



start_raster_graphics(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('r', printer);
   putc('0', printer);
   putc('A', printer);

}



select_300_dpi_resolution(printer)

   FILE *printer;
{
   putc(ESCAPE, printer);
   putc('*', printer);
   putc('t', printer);
   putc('3', printer);
   putc('0', printer);
   putc('0', printer);
   putc('R', printer);

}




print_bytes(printer, buffer)
   FILE *printer;
   char buffer[];
{
   int        i;

   putc(ESCAPE, printer);
   putc('*', printer);
   putc('b', printer);
   putc('2', printer);
   putc('0', printer);
   putc('0', printer);
   putc('W', printer);


   for(i=0; i<200; i++){
      putc(buffer[i], printer);
   }
}  /* ends print_bytes */




   /**************************************************
   *
   *   print_original_200_row(...
   *
   ***************************************************/

print_original_200_row(printer, short_row)
   FILE  *printer;
   short short_row[200];
{
   char  row[8][200];
   char  c[200], response[80];
   int         i, j, k;
   short value;
   for(i=0; i<200; i++){
      value = short_row[i];
      if(value > 63) value = 63;
      if(value < 0)  value =  0;

      for(j=0; j<8; j++)
         row[j][i] = patterns[value][j];

   }  /* ends loop over i */

   for(i=0; i<8; i++){
      for(j=0; j<200; j++)
         c[j] = row[i][j];
      set_horizontal_offset(printer);
      print_bytes(printer, c);
   }  /* ends loop over i */

}  /* ends print_original_200_row */






   /***********************************
   *
   *   print_hist_image(...
   *
   ************************************/



print_hist_image(printer, hist)
   FILE *printer;
   unsigned long hist[];
{
   char   c, d;
   int          i, j, k;
   unsigned long limit, max;

   d = 0;
   c = 255;

      /********************************
      *
      *   First scale the histogram
      *
      *********************************/

   max = 0;
   for(i=0; i<256; i++)
      if(hist[i] > max) max = hist[i];

   if(max > 200){
      for(i=0; i<256; i++){
        hist[i] = (hist[i]*200)/max;
      }
   }


      /********************************
      *
      *   Second print it
      *
      *   Print a space between the image
      *   and the histogram.
      *
      *********************************/


   for(i=0; i<20; i++){
         end_graphics_mode(printer);
         select_300_dpi_resolution(printer);
         set_raster_width(printer);
         start_raster_graphics(printer);
         select_full_graphics_mode(printer);
         set_horizontal_offset(printer);
         putc(ESCAPE, printer);
         putc('*', printer);
         putc('b', printer);
         putc('2', printer);
         putc('0', printer);
         putc('0', printer);
         putc('W', printer);

         for(j=0; j<200; j++)
            putc(d, printer);
   }


   printf("\n\nHIST> Now printing the histogram");
   for(i=0; i<256; i++){
      printf("\n\tHIST> Histogram[%d]=%ld", i, hist[i]);

            /* print the line 2 times */
      for(k=0; k<2; k++){

         end_graphics_mode(printer);
         select_300_dpi_resolution(printer);
         set_raster_width(printer);
         start_raster_graphics(printer);
         select_full_graphics_mode(printer);


            /***************************
            *
            *        Print grid marks every
            *        50 pixels.  Do this by
            *        setting a shorter margin
            *        then printing 2 marks then
            *        the data.
            *
            ****************************/

         if( (i ==   0)        ||
             (i ==  50) ||
             (i == 100) ||
             (i == 150) ||
             (i == 200) ||
             (i == 255)){

            set_shorter_horizontal_offset(printer);
            putc(ESCAPE, printer);
            putc('*', printer);
            putc('b', printer);
            putc('2', printer);
            putc('0', printer);
            putc('2', printer);
            putc('W', printer);

            putc(c, printer);
            putc(c, printer);


            if(hist[i] >= 200)
               hist[i] = 200;

            limit = 200 - hist[i];

            if(hist[i] == 0)
               putc(c, printer);

            for(j=0; j<hist[i]; j++)
               putc(c, printer);

            for(j=0; j<limit; j++)
              putc(d, printer);

         }  /* ends print grid marks */


            /***************************
            *
            *        If you do not print
            *        grid marks, set the normal
            *        margin and then print the
            *        data.
            *
            ****************************/

         else{
            set_horizontal_offset(printer);
            /* this prints 200 bytes so print 200 */
            putc(ESCAPE, printer);
            putc('*', printer);
            putc('b', printer);
            putc('2', printer);
            putc('0', printer);
            putc('0', printer);
            putc('W', printer);

            if(hist[i] >= 200)
               hist[i] = 200;

            limit = 200 - hist[i];

            if(hist[i] == 0)
               putc(c, printer);

            for(j=0; j<hist[i]; j++)
               putc(c, printer);

            for(j=0; j<limit; j++)
              putc(d, printer);

         }  /* ends else no grid marks */

      }  /* ends loop over k */

   }  /* ends loop over i */

}  /* ends print_hist_image */
