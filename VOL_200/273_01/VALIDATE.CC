validate(char *goods, char val_field)
{
/* This function will validate a character field for valid values.

   *goods points to a character string that contains all valid character
   values for the character field.

   val_field is the character to validate.

   return = 1 if val_field is valid.
   return = 0 if val_fiels in not valid.
*/

	int x;
	for(x=0;x<strlen(goods);x++) {
		if(val_field==*(goods + x)) return(1);
	}
	return(0);
}
