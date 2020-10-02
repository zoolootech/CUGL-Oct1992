left_str(int x, char *str, char *new_str)
/* Please the left x number of character from *str into *new_str */
{
        while(x && *str) {
                *new_str=*str;
                new_str++; str++; x--;
        }
        *new_str=0x00;
}
