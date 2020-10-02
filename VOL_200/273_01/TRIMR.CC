trim_r(char *str)
/* This will trim all blanks characters from the rigth side of the
   string pointed to by *str.
*/
{
        char *s_str;
        int str_len;
        str_len=strlen(str);
        if(!str_len) return(0);
        s_str=str;
        str=str + str_len - 1;
        while((*str==' ') && str_len) {
                str--; str_len--;
        }
        *(str+1)=0x00;
        str_len=strlen(s_str);
        return(str_len);
}
