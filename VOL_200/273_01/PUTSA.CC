put_sa(char *ch, int attr)
/* This will put the character string pointer to by *ch on the screen
   at the current cursor location using attribute attr.
*/
{
        while(*ch) {
                if((*ch != '\x0d') && (*ch !='\x0a')) {
                        put_ca(*ch,attr,1);
                        cur_rt();
                }
                ch++;
        }
}
