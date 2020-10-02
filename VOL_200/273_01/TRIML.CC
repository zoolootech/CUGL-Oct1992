trim_l(char *str)
/* This will remove all blanks from the left side of the string
   pointed to by *str.
*/
{
        int non_blank_found = 0;
        char *newstr;
        newstr=str;
        while(*str) {
                if(*str==' ' && !non_blank_found)
                        str++;
                else {
                        non_blank_found=1;
                        *newstr=*str;
                        newstr++; str++;
                }
        }
        *newstr=0x00;
}
