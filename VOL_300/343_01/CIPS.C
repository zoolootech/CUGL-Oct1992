



       /***********************************************
       *
       *       file d:\cips\cips.c
       *
       *       Functions: This file contains
       *          main
       *          clear_text_screen
       *          show_menu
       *          show_image
       *
       *       Purpose:
       *          This file contains the main calling
       *          routine in the C Image Processing System.
       *
       *       External Calls:
       *          numcvrt.c - get_integer
       *          gin.c - get_image_name
       *          rtiff.c - read_tiff_image
       *          tiff.c - read_tiff_header
       *          rstring.c - read_string
       *          display.c - display_image
       *                      display_menu_for_display_image
       *          pi.c - print_image
       *          ht.c - display_using_halftoning
       *                 get_threshold_value
       *          djet.c - print_graphics_image
       *                   get_graphics_caption
       *          hist.c - display_menu_for_histogram
       *                   calculate_area_histogram
       *                   print_histogram
       *                   show_histogram
       *          edge.c - detect_edges
       *                   get_edge_options
       *                   quick_edge
       *          edge2.c - homogeneity
       *                    difference_edge
       *                    contrast_edge
       *          edge3.c - gaussian_edge
       *                    enhance_edges
       *          filter.c - filter_image
       *                     median_filter
       *                     get_filter_options
       *          addsub.c - add_image_array
       *                     subtract_image_array
       *          cutp.c - cut_image_piece
       *                   paste_image_piece
       *                   check_cut_and_paste_limits
       *          rotate.c - rotate_flip_image_array
       *          scale.c - zoom_image_array
       *                    shrink_image_array
       *
       *       Modifications:
       *          26 June 1990 - created
       *
       *************************************************/

#include "d:\cips\cips.h"


short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main()
{

   char caption[80],
        color_transform[80],
        low_high[80],
        method[80],
        monitor_type[80],
        name[80],
        name2[80],
        name3[80],
        rep[80],
        zoom_shrink[80];


   int  color                = 99,
        detect_threshold     = 1,
        detect_type          = 1,
        display_colors       = 16,
        filter_type          = 1,
        high                 = 100,
        horizontal           = 3,
        i                    = 0,
        ie                   = 1,
        ie2                  = 1,
        ie3                  = 1,
        il                   = 1,
        il2                  = 1,
        il3                  = 1,
        image_colors         = 256,
        invert               = 0,
        j                    = 0,
        length               = 3,
        le                   = COLS+1,
        le2                  = COLS+1,
        le3                  = COLS+1,
        ll                   = ROWS+1,
        ll2                  = ROWS+1,
        ll3                  = ROWS+1,
        not_finished         = 1,
        print                = 0,
        response             = 99,
        rotation_type        = 1,
        scale                = 2,
        show_hist            = 0,
        size                 = 7,
        threshold            = 128,
        vertical             = 3,
        width                = 3;

   long     mean_of_pixels;
   unsigned long histogram[256];
   short    low_hi_filter[3][3];
   struct   tiff_header_struct image_header;


   clear_text_screen();

   strcpy(color_transform, "Straight mode");
   strcpy(monitor_type, "VGA");
   strcpy(low_high, "l");
   strcpy(zoom_shrink, "z");
   strcpy(method, "r");

   strcpy(name,  "d:/pix/adam256.tif");
   strcpy(name2, "d:/pix/output.tif");
   strcpy(name3, "d:/pix/output.tif");

     while(not_finished){

        show_menu();

        get_integer(&response);

        switch (response){

        case 1:/* display image header */
         get_image_name(name);
         read_tiff_header(name, &image_header);
         printf("\n\nCIPS> The image header is:");
         printf("\n\t\twidth=%ld length=%ld  start=%ld  bits=%ld",
              image_header.image_width,
              image_header.image_length,
              image_header.strip_offset,
              image_header.bits_per_pixel);
         printf("\nCIPS> Hit Enter to continue");
         read_string(rep);
        break;

        case 2:/* display image numbers */
         get_image_name(name);
         get_parameters(&il, &ie, &ll, &le);
         read_tiff_image(name, the_image, il, ie, ll, le);
         show_image(the_image, il, ie);
         break;

        case 3:   /* print image numbers */
         get_image_name(name);
         get_parameters(&il, &ie, &ll, &le);
         read_tiff_image(name, the_image, il, ie, ll, le);
         print_image(the_image, name, 1, 1, 1, 100, 18,
                     il, ie);
        break;

        case 4:   /* display image */
         get_image_name(name);
         read_tiff_header(name, &image_header);
         get_parameters(&il, &ie, &ll, &le);
         display_menu_for_display_image(&image_colors,
                   &display_colors, &invert,
                   color_transform, monitor_type,
                   &show_hist);
         display_image(name, the_image, il, ie,
                   ll, le, &image_header, monitor_type,
                   color_transform, invert,
                   image_colors, display_colors, show_hist);
        break;

        case 5:   /* display image using halftoning */
         get_image_name(name);
         read_tiff_header(name, &image_header);
         get_parameters(&il, &ie, &ll, &le);
         display_menu_for_display_image(&image_colors,
                   &display_colors, &invert,
                   color_transform, monitor_type,
                   &show_hist);
         get_threshold_value(&threshold, &print);
         display_using_halftoning(the_image, name,
                   il, ie, ll, le, threshold,
                   invert, image_colors, &image_header,
                   monitor_type, print, show_hist,
                   color_transform);
        break;

        case 6:   /* print graphics image */
         get_image_name(name);
         read_tiff_header(name, &image_header);
         get_parameters(&il, &ie, &ll, &le);
         display_menu_for_display_image(&image_colors,
                   &display_colors, &invert,
                   color_transform, monitor_type,
                   &show_hist);
         get_graphics_caption(caption);
         print_graphics_image(the_image, out_image, name,
                   il, ie, ll, le, image_colors,
                   invert, caption, show_hist,
                   color_transform);
        break;

        case 7:   /* print or display histogram numbers */
         get_image_name(name);
         read_tiff_header(name, &image_header);
         get_parameters(&il, &ie, &ll, &le);
         display_menu_for_histogram(&print, &vertical,
                   &horizontal);
         calculate_area_histogram(histogram, vertical,
                   horizontal, the_image, name,
                   il, ie, ll, le);
         if(print == 0)
            show_histogram(histogram);
         if(print == 1)
            print_histogram(histogram, name);
        break;

        case 8:  /* perform edge detection */
           printf("\nCIPS> Enter input image name\n");
           get_image_name(name);
           printf("\nCIPS> Enter output image name\n");
           get_image_name(name2);
           get_parameters(&il, &ie, &ll, &le);
           get_edge_options(&detect_type, &detect_threshold,
                            &high, &size);
           if(detect_type == 1  ||
              detect_type == 2  ||
              detect_type == 3)
              detect_edges(name, name2, the_image, out_image,
                           il, ie, ll, le, detect_type,
                           detect_threshold, high);
           if(detect_type == 4)
              quick_edge(name, name2, the_image, out_image,
                         il, ie, ll, le, detect_threshold,
                         high);
           if(detect_type == 5)
                homogeneity(name, name2, the_image, out_image,
                         il, ie, ll, le, detect_threshold,
                         high);
           if(detect_type == 6)
                difference_edge(name, name2, the_image, out_image,
                              il, ie, ll, le, detect_threshold,
                              high);
           if(detect_type == 7)
                contrast_edge(name, name2, the_image, out_image,
                            il, ie, ll, le, detect_threshold,
                            high);
           if(detect_type == 8)
                gaussian_edge(name, name2, the_image, out_image,
                             il, ie, ll, le, size,
                            detect_threshold, high);
         break;

        case 9: /* perform edge enhancement */
           printf("\nCIPS> Enter input image name\n");
           get_image_name(name);
           printf("\nCIPS> Enter output image name\n");
           get_image_name(name2);
           get_parameters(&il, &ie, &ll, &le);
           printf("\nCIPS> Enter high threshold parameter");
           printf(" \n\t___\b\b\b");
           get_integer(&high);
           enhance_edges(name, name2, the_image, out_image,
                  il, ie, ll, le, high);
         break;

        case 10: /* perform image filtering */
           printf("\nCIPS> Enter input image name\n");
           get_image_name(name);
           printf("\nCIPS> Enter output image name\n");
           get_image_name(name2);
           get_parameters(&il, &ie, &ll, &le);
           get_filter_options(&filter_type, low_high);
           if(low_high[0] == 'l' ||
               low_high[0] == 'L' ||
               low_high[0] == 'h' ||
               low_high[0] == 'H'){
              setup_filters(filter_type, low_high, low_hi_filter);
              filter_image(name, name2, the_image, out_image,
                           il, ie, ll, le,
                           low_hi_filter, filter_type);
           }

           if(low_high[0] == 'm' ||
               low_high[0] == 'M')
                 median_filter(name, name2, the_image, out_image,
                             il, ie, ll, le, filter_type);

        break;

        case 11: /* perform image addition and subtraction */
           printf("\nCIPS> Image Addition:"
                "\n         output = first + second"
                "\n      Image Subtractions:"
                "\n         output = first - second");
           printf("\nCIPS> Enter first image name\n");
           get_image_name(name);
           printf("\nCIPS> Enter second image name\n");
           get_image_name(name2);
           printf("\nCIPS> Enter output image name\n");
           get_image_name(name3);
           printf("\nCIPS> Enter parameters for first image");
           get_parameters(&il, &ie, &ll, &le);
           printf("\nCIPS> Enter parameters for second image");
           get_parameters(&il2, &ie2, &ll2, &le2);
           printf("\nCIPS> Enter parameters for output image");
           get_parameters(&il3, &ie3, &ll3, &le3);
           printf("\n\nCIPS> Enter a=addition    s=subtraction\n");
           printf("\nCIPS> _\b");
           read_string(low_high);
           if(low_high[0] == 'a' || low_high[0] == 'A')
              add_image_array(name, name2, name3,
                  the_image, out_image,
                  il, ie, ll, le,
                  il2, ie2, ll2, le2,
                  il3, ie3, ll3, le3);
           if(low_high[0] == 's' || low_high[0] == 'S')
              subtract_image_array(name, name2, name3,
                  the_image, out_image,
                  il, ie, ll, le,
                  il2, ie2, ll2, le2,
                  il3, ie3, ll3, le3);
        break;

        case 12: /* image cutting and pasting */
           printf("\n\nCIPS> Cut from source image and paste to"
                  " destination image");
           printf("\nCIPS> Enter source image name");
           get_image_name(name);
           get_parameters(&il, &ie, &ll, &le);
           check_cut_and_paste_limits(&il, &ie, &ll, &le);
           cut_image_piece(name, the_image,
                           il, ie, ll, le);
           printf("\nCIPS> Enter destination image name");
           get_image_name(name2);
           printf("\nCIPS> Enter destination image parameters");
           get_parameters(&il, &ie, &ll, &le);
           check_cut_and_paste_limits(&il, &ie, &ll, &le);
           paste_image_piece(name2, name, the_image,
                             out_image, il, ie, ll, le);
        break;

        case 13: /* image rotation and flipping */
           printf("\nCIPS> Enter source image name");
           get_image_name(name);
           get_parameters(&il, &ie, &ll, &le);
           printf("\nCIPS> Enter destination image name");
           printf("\nCIPS> (source can equal destination)");
           get_image_name(name2);
           printf("\nCIPS> Enter destination image parameters");
           get_parameters(&il2, &ie2, &ll2, &le2);
           printf("\nCIPS> Enter number of Rotations (1, 2, 3)");
           printf("\nCIPS> or type of Flip (4=horizontal 5=vertical)");
           printf("\nCIPS> __\b");
           get_integer(&rotation_type);
           rotate_flip_image_array(name, name2, the_image,
                               out_image, il, ie, ll, le,
                               il2, ie2, ll2, le2,
                               rotation_type);
        break;

        case 14: /* image scaling */
           printf("\nCIPS> Enter input image name");
           get_image_name(name);
           get_parameters(&il, &ie, &ll, &le);
           printf("\nCIPS> Enter output image name");
           get_image_name(name2);
           get_scaling_options(zoom_shrink, &scale, method);
           if(zoom_shrink[0] == 'z' || zoom_shrink[0] == 'Z')
              zoom_image_array(name, name2, the_image, out_image,
                               il, ie, ll, le, scale, method);
           if(zoom_shrink[0] == 's' || zoom_shrink[0] == 'S'){
               printf("\nCIPS> Enter output image parameters");
                 get_parameters(&il2, &ie2, &ll2, &le2);
               shrink_image_array(name, name2, the_image, out_image,
                                  il, ie, ll, le, il2, ie2, ll2, le2,
                                  scale, method);
           }
        break;

        case 15: /* create image */
           printf("\nCIPS> Enter input name of image to create");
           get_image_name(name);
           printf("\nCIPS> Enter number of %d blocks wide",COLS);
           printf("\n      ___\b\b");
           get_integer(&width);
           printf("\nCIPS> Enter number of %d blocks tall",ROWS);
           printf("\n      ___\b\b");
           get_integer(&length);
           image_header.lsb            = 1;
           image_header.bits_per_pixel = 8;
           image_header.image_length   = length*COLS;
           image_header.image_width    = width*ROWS;
           image_header.strip_offset   = 1000;
           for(i=0; i<ROWS; i++)
              for(j=0; j<COLS; j++)
                 the_image[i][j] = 0;
           create_allocate_tiff_file(name,&image_header,the_image);
        break;

        case 20:  /* exit system */
         not_finished = 0;
        break;

        default:
         printf("\nCould not understand response, try again");
        break;

     }               /* ends switch response          */
  }               /* ends while not finished */
}               /* ends main                  */




   /******************************************************
   *
   *   clear_text_screen()
   *
   *   This calls Microsoft C functions to clear the text
   *   screen and set a blue background with gray text.
   *
   *******************************************************/


clear_text_screen()
{
   _setvideomode(_TEXTC80);      /* MSC 6.0 statements */
   _setbkcolor(1);
   _settextcolor(7);
   _clearscreen(_GCLEARSCREEN);
}  /* ends clear_text_screen */




   /******************************************************
   *
   *   show_image(...
   *
   *   This function displays the image numbers on the
   *   screen as text.  It displays 20 rows  with 18
   *   columns each.
   *
   *******************************************************/

show_image(image, il, ie)
   int   il, ie;
   short image[ROWS][COLS];
{
   int i, j;
   printf("\n   ");
   for(i=0; i<18; i++)
      printf("-%3d", i+ie);

   for(i=0; i<20; i++){
      printf("\n%2d>", i+il);
      for(j=0; j<18; j++)
         printf("-%3d", image[i][j]);
   }

   printf("\nPress enter to continue");
   get_integer(&i);

}  /* ends show_image  */





   /******************************************************
   *
   *   show_menu(..
   *
   *   This function displays the CIPS main menu.
   *
   *******************************************************/
show_menu()
{

        printf("\n\nWelcome to CIPS");
        printf("\nThe C Image Processing System");
        printf("\nThese are you choices:");
        printf("\n\t1.  Display image header");
        printf("\n\t2.  Show image numbers");
        printf("\n\t3.  Print image numbers");
        printf("\n\t4.  Display image (VGA & EGA only)");
        printf("\n\t5.  Display or print image using halftoning");
        printf("\n\t6.  Print graphics image using dithering");
        printf("\n\t7.  Print or display histogram numbers");
        printf("\n\t8.  Perform edge detection");
        printf("\n\t9.  Perform edge enhancement");
        printf("\n\t10. Perform image filtering");
        printf("\n\t11. Perform image addition and subtraction");
        printf("\n\t12. Perform image cutting and pasting");
        printf("\n\t13. Perform image rotation and flipping");
        printf("\n\t14. Perform image scaling");
        printf("\n\t15. Create a blank image");
        printf("\n\t20. Exit system");
        printf("\n\nEnter choice _\b");

}    /* ends show_menu */
