
Listing 3 - The Function display_menu_for_display_function (file
display.c)



   /***********************************************
   *
   *   display_menu_for_display_image(
   *
   ************************************************/


display_menu_for_display_image(image_colors, display_colors,
                              invert, color_transform, 
                              monitor_type, 
                              show_hist)
   char color_transform[], monitor_type[];
   int  *invert, *image_colors, *display_colors, *show_hist;
{
   char response[80];
   int  int_response, not_finished, r;

   not_finished = 1;
   while(not_finished){
      printf("\n\nDISPLAY> Enter choice (0 for no change) ");
      printf("\nDISPLAY> 1. Invert is %d (1=on 0=off)", *invert);
      printf("\nDISPLAY> 2. Color Transform-- %s", 
             color_transform);
      printf("\nDISPLAY> 3. Input image has %d colors", 
             *image_colors);
      printf("\nDISPLAY> 4. Display will show %d colors", 
             *display_colors);
      printf("\nDISPLAY> 5. Monitor type is %s",
             monitor_type);
      printf("\nDISPLAY> 6. Histogram is %d", *show_hist);
      printf("  (1=show 0=don't show)");
      printf("\nDISPLAY> _\b");
      get_integer(&r);

      if(r == 0){
         not_finished = 0;
      }

      if(r == 1){
         printf("\nDISPLAY> Enter 1 for invert on");
         printf(" 0 for invert off");
         printf("\nDISPLAY> ___");
         get_integer(&int_response);
         *invert = int_response;
      }  /* ends if r == 1 */

      if(r == 2){
         printf("\nDISPLAY> Enter the new color transform mode ");
         printf("\nDISPLAY> (S) Straight mode");
         printf("   (H) Histogram Equalization");
         printf("\nDISPLAY> _\b");
         read_string(response);
         if((response[0] == 'S') ||
            (response[0] == 's'))
               strcpy(color_transform, "Straight mode");
         else
               strcpy(color_transform,
                "Histogram Equalization mode");
      }  /* ends if r == 2  */

      if(r == 3){
         printf("\nDISPLAY> Enter the number of colors");
         printf(" in the input image");
         printf("\nDISPLAY> ___");
         get_integer(&int_response);
         *image_colors = int_response;
      }  /* ends if r == 3 */

      if(r == 4){
         printf( 
          "\nDISPLAY> Enter the number of colors for the display");
         printf("\nDISPLAY> ___");
         get_integer(&int_response);
         *display_colors = int_response;
      }  /* ends if r == 4 */

      if(r == 5){
         printf("\nDISPLAY> Enter the new monitor type");
         printf("\nDISPLAY> (E) EGA   (V) VGA");
         printf("   (C) CGA   (M) Monochrome");
         printf("\nDISPLAY> _\b");
         read_string(response);
         if((response[0] == 'E') ||
            (response[0] == 'e'))
            strcpy(monitor_type, "EGA");
      if((response[0] == 'V') ||
         (response[0] == 'v'))
            strcpy(monitor_type, "VGA");
      if((response[0] == 'C') ||
         (response[0] == 'c'))
            strcpy(monitor_type, "CGA");
      if((response[0] == 'M') ||
         (response[0] == 'm'))
            strcpy(monitor_type, "Monochrome");
      }  /* ends if r == 5  */

      if(r == 6){
         printf(
            "\nDISPLAY> Enter 1 for show histogram 0 for don't");
         printf("\nDISPLAY> ___");
         get_integer(&int_response);
         *show_hist = int_response;
      }  /* ends if r == 6 */



   }  /* ends while not_finished  */
}  /* ends display_menu  */

