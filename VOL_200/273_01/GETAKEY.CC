get_akey(char *ch, char *list)
/* Wait until one of the charaacters in the list is pressed. ch will be set
   to the upper case value of the key pressed.
*/
{
        char scan;
        strupr(list);
        while(1) {
			get_ca(ch,&scan);
			if(*ch=='\x00') continue;
			*ch=toupper(*ch);
			if(strchr(list,*ch)) return;
		}
}
