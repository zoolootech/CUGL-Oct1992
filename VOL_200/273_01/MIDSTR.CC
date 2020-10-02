mid_str(int begin, int leng, char *o_str, char *n_str)
/* Extract the characters from *o_str starting at position begin for leng
   number of characters and place them into *n_str.
*/
{
        if(begin > strlen(o_str)) return(1);
        o_str = o_str + begin -1;
        
        while(*o_str && leng) {
                *n_str=*o_str;
                n_str++;
                o_str++;
                leng--;
        }
        *n_str=0x00;
        return(0);
}
