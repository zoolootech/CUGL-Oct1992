
        /*******************************************************
        *
        *       file d:\cips\gin.c
        *
        *       Functions: This file contains
        *           get_image_name
        *
        *       Purpose - This function prompts the user to
  		  *            enter the name of an image.
        *
        *       External Calls:
        *           rstring.c - read_string
        *                       clear_buffer
        *
        *       Modifications:
        *           26 September 86 - now uses vision3.h
		  *               instead of vision2.h and the read_string
		  *               and get_integer instead of  scanf.
        *           11 March 1987 - this function was
		  *               removed from the file ip.c and put
		  *               in file gin.c.
        *
        ******************************************************/


#include "d:\cips\cips.h"




	/*********************************************
	 * get_image_name(...
	 *
	 * This function reads in the desired image
	 * file name.
	 *
	 *********************************************/

get_image_name(name)
   char name[];
{
   char base_name[80],
	     dir_name[80],
		  new_name[80],
		  response[80];
	int  l;

   printf("\n\nImage name is--%s\n", name);
   printf("\nDo you want to change:");
   printf("\n (f) file name");
   printf("\n (d) directory name");
   printf("\n (n) no change");
   printf("\n     _\b");
   clear_buffer(response);
   read_string(response);

   if((response[0] == 'F') ||
      (response[0] == 'f')){
      printf("\n\nEnter file name (name only no extension)");
		printf("\n--");
      read_string(new_name);
      extract_directory_name(name, dir_name);
		sprintf(name, "%s%s.tif", dir_name, new_name);
   }

   if((response[0] == 'D') ||
      (response[0] == 'd')){
      printf("\n\nEnter directory name\n--");
      read_string(dir_name);
		l = strlen(dir_name);
		if(dir_name[l-1] != 47){
			dir_name[l]   = '/';
			dir_name[l+1] = '\0';
		}
      printf("\n\nEnter file name (name only no extension)");
		printf("\n--");
      read_string(new_name);
		sprintf(name, "%s%s.tif", dir_name, new_name);
   }

}       /* ends get_image_name  */




	/*********************************************
	 * extract_directory_name(...
	 *
	 * This function extracts the sub-directory
	 * name out of a file name.
	 *
	 *********************************************/

extract_directory_name(file_name, dir_name)
	char file_name[], dir_name[];
{
	int i, j, k;

	i = 1;
	j = 0;
	k = 0;
	while(i){
		if(file_name[k] == 47  ||
		   file_name[k] == 92)     j = k;
		if(file_name[k] == '\0')   i = 0;
		k++;
	}
	j++;
	strncpy(dir_name, file_name, j);
	dir_name[j] = '\0';

}  /* ends extract_directory_name */





	/*********************************************
	 *   extract_base_image_name(...
	 *
	 *   This function looks at a full file name
	 *   and pulls off the sub-directory name and
	 *   the file extension and returns the base
	 *   file name.
	 *
	 *********************************************/

extract_base_file_name(file_name, base_name)
   char base_name[], file_name[];
{
	int i, j, k;
	i = 1;
	j = 0;
	k = 0;
	while(i){
		if(file_name[k] == 47  ||
		   file_name[k] == 92)     j = k;
		if(file_name[k] == '\0')   i = 0;
		k++;
	}

	i = 1;
	k = 0;
	j++;
	while(i){
		if(file_name[j] == '.')
		   i = 0;
		else
		   base_name[k] = file_name[j];
		j++;
		k++;
	}
	k--;
    base_name[k] = '\0';
printf("\nEBN> base is %s", base_name);
}  /* ends extract_base_file_name */
