


       /***********************************************
       *
       *       file d:\cips\roundoff.c
       *
       *       Functions: This file contains
       *          main
       *
       *       Purpose:
       *          This program takes an image file and
       *          rounds if off by writing it to an new
       *          file that has an even multiple of
       *          rows and cols.
       *
       *       External Calls:
       *          tiff.c - read_tiff_header
       *          rtiff.c - read_tiff_image
       *          gin.c - get_image_name
       *          gpcips - get_parameters
       *          numcvrt.c - get_integer
       *          wtiff.c - create_allocate_tiff_file
       *                    write_array_into_tiff_image
       *
       *       Modifications:
       *          31 March 1991 - created
       *
       *************************************************/

#include "d:\cips\cips.h"



short the_image[ROWS][COLS];
short out_image[ROWS][COLS];

main()
{
   char     caption[80], name[80], name2[80], rep[80];
   int      count, i, ie, il, j, le, length, ll, 
            new_ie, new_il, vertical, width;
   struct   tiff_header_struct image_header;


   _setvideomode(_TEXTC80);      /* MSC 6.0 statements */
   _setbkcolor(1);
   _settextcolor(7);
   _clearscreen(_GCLEARSCREEN);


   new_ie = 1;
   new_il = 1;
   il     = 1;
   ie     = 1;
   ll     = ROWS+1;
   le     = COLS+1;

   strcpy(name,  "d:/pix/adam256.tif");
   strcpy(name2, "d:/pix/output.tif");

   printf("\n\nThis is the PATTERN program");
   printf("\nIt copies a portion of one image file");
   printf("\nto another image file.  The output image");
   printf("\nfile will be a multiple of ROWS and COLS.\n");


   printf("\nCIPS> Enter input image name\n");
   get_image_name(name);
   printf("\nCIPS> Enter output image name\n");
   get_image_name(name2);
   printf("\nEnter the il and ie where you want to");
   printf("\nstart copying from input image");
   get_parameters(&il, &ie, &ll, &le);

   printf("\nEnter number of vertical blocks (100 rows)");
   printf(" __\b\b");
   get_integer(&length);

   printf("\nEnter number of horizontal blocks (100 cols)");
   printf(" __\b\b");
   get_integer(&width);

   printf("\nCreating the output image file %s", name2);
   read_tiff_header(name, &image_header);
   image_header.image_length = length*ROWS;
   image_header.image_width  = width*COLS;
   create_allocate_tiff_file(name2, &image_header,
                             out_image);

/*********
   for(i=0; i<ROWS; i++){
      for(j=0; j<COLS; j++){
	     if( ((i/10) % 7) == 1)
		    out_image[i][j] = 150;
	     else
		    out_image[i][j] = 1;
	 }
   }
*********/
   for(i=0; i<ROWS; i++)
      for(j=0; j<COLS; j++)
	     out_image[i][j] = j;

   count = 1;

   for(i=0; i<length; i++){
      for(j=0; j<width; j++){
         printf("\nRunning %d of %d", count, length*width);
         count++;
         write_array_into_tiff_image(name2, out_image,
                         new_il + i*ROWS,
                         new_ie + j*COLS,
                         new_il + i*ROWS + ROWS,
                         new_ie + j*COLS + COLS);
      }
   }

}  /* ends main */
