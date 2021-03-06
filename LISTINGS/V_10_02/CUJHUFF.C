





      
       /*****************************************************
       *
       *       file d:\huffman.c
       *
       *
       ******************************************************/


#include "d:\lsu\cujhuff.h"



main(argc, argv)
   int  argc;
   char *argv[];
{
   char  input_file_name[80],
         output_file_name[80],
         r[80];

   int i, j;
   struct item_struct item_array[LENGTH];
   struct header_struct file_header;


   if(argc < 3){
      printf("\nHUFFMAN> You did not enter enough file names.");
      printf("\nHUFFMAN> Try again: huffman in_file_name out_file_name");
      printf("\nHUFFMAN>                     or");
      printf("\nHUFFMAN>            huffman destination_file packed_file d");
      exit(1);
   }

       /*
          If there are four arguments then you are decompressing
          the compressed input file to a full size output file.
       */

   if(argc >= 4){
      strcpy(output_file_name, argv[1]);
      strcpy(input_file_name, argv[2]);
      decode_compressed_file(input_file_name,
                             output_file_name,
                             item_array,
                             &file_header);
   }

   else{   /* else you compress the full size input file and write
              out a compressed output file */

      strcpy(input_file_name, argv[1]);
      strcpy(output_file_name, argv[2]);
      read_input_file_and_create_histogram(input_file_name, item_array);
      sort_item_array(item_array);
      printf("\n\nHUFFMAN> This is the sorted item array:\n");
      print_item_array(item_array);
      create_huffman_code(item_array);
      printf("\n\nHUFFMAN> This is the Huffman coding of the characters:\n");
      print_item_array(item_array);
      printf("\n> Coding the file");
      convert_long_to_short(item_array, &file_header);
      code_and_write_output_file(item_array, 
                                 input_file_name, 
                                 output_file_name,
                                 &file_header);

   }  /* ends else compress input file to output file */


}  /* ends main  */




/*  
         read_input_file_and_create_histogram(...

         Read the input file.  Count up the occurances of each
         character and create a histogram.
*/


read_input_file_and_create_histogram(input_file_name, item_array)
   char  input_file_name[];
   struct item_struct item_array[];
{
   char  buffer[1000];

   int   bytes_read,
         i,
         in_file_desc,
         j;

   clear_item_array(item_array);
   in_file_desc = my_open(input_file_name);
   printf("\n> in file desc = %d", in_file_desc);

   bytes_read = 1000;

   while(bytes_read == 1000){
      bytes_read = my_read(in_file_desc, buffer, 1000);
      for(i=0; i<bytes_read; i++){
         j = buffer[i];
         item_array[j].count = item_array[j].count + 1;
      }  /* ends loop over i  */
   }     /* ends while bytes_read == 1000  */

   close(in_file_desc);


}  /* ends read_input_file_and_create_histogram  */






/* 
        clear_item_array(...

        This function initializes the item_array.
*/


clear_item_array(item_array)
   struct item_struct item_array[];
{
   int i,j, k;

   for(i=0; i<LENGTH; i++){
      item_array[i].indicator = 'E';
      item_array[i].character = i;
      item_array[i].count     = 0;
      for(k=0; k<LLENGTH; k++)
         item_array[i].includes[k] = 256;
      for(j=0; j<CODE_LENGTH; j++)
         item_array[i].coded[j] = OTHER;
   }   /*  ends loop over i  */
}      /*  ends clear_item_array  */





/* 
        print_item_array(item_array)

        This function is for debugging.  It prints
        to the screen the item_array.
*/

print_item_array(item_array)
   struct item_struct item_array[];
{
   char  response[5];
   int   i,
         j,
         k,
         max_i,
         printed;
   float ratio;
   long  max;
   printed = 0;
   max           = 0;

   printf("\n>");
   printed++;

   for(i=0; i<LENGTH; i++){
      if(item_array[i].count > max){
         max = item_array[i].count;
         max_i = i;
      }  /* ends if count > max  */
   }         /* ends loop over i         */

   ratio = 30.0/(float)(item_array[max_i].count);

   for(i=0; i<LENGTH; i++){
      if(item_array[i].count != 0){
         printed++;
         if( (printed%22) == 0){
            printf("\n> Hit return to continue--");
            read_string(response);
         }  /* ends if printed 20 lines */
         printf("\n> [%3d]=%3d=%c=%4d=", i, item_array[i].character,
                                      item_array[i].indicator,
                                      item_array[i].count);
         for(k=0; k<CODE_LENGTH; k++)
            printf("%c", item_array[i].coded[k]);
         for(j=0; j<(ratio*item_array[i].count); j++){
            printf("*");
         }  /* ends loop over j       */
      }     /* ends if count != 0     */
   }            /* ends loop over i       */
}            /* ends print_item_array  */





/*
      convert_long_to_short(...

      This function converts the long item_array into
      a shorter file header.

*/

convert_long_to_short(item_array, file_header)
   struct item_struct item_array[];
   struct header_struct *file_header;
{
   int i, j, k;

   for(i=0; i<LENGTH; i++){
      file_header->items[i].character = item_array[i].character;
      for(j=0; j<CODE_LENGTH; j++)
         file_header->items[i].coded[j] = item_array[i].coded[j];
   }   /*  ends loop over i  */

}  /* ends convert_long_to_short */




/*
      convert_short_to_long(...

      This function converts the short file header into
      a the longer item_array for use in the program.

*/

convert_short_to_long(item_array, file_header)
   struct item_struct item_array[];
   struct header_struct *file_header;
{
   int i, j, k;

   for(i=0; i<LENGTH; i++){
      item_array[i].character =  file_header->items[i].character; 
      for(j=0; j<CODE_LENGTH; j++)
         item_array[i].coded[j] =  file_header->items[i].coded[j]; 
   }   /*  ends loop over i  */

}  /* ends convert_short_to_long */
