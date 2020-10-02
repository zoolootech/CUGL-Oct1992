right_str(int x, char *str, char *new_str)
/* This will take the right x number of character from the string pointed
   to by *str and place them into the string pointed to by *new_str.
*/
{
        str=str + strlen(str) - x;
        while(*str) {
                *new_str=*str;  
                new_str++; str++;
        }
        *new_str=0x00;
}
