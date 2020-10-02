#include <tcutil.h>
#include <string.h>
#include <ctype.h>
get_linex(int row, int col, char *str, int sleng, int attr, int dleng)
/* This will read a string from the screen starting row,col.
   The String that is read will be put into the string pointed to by *str.
   The length of the string is specified by sleng.
   The length of the display area is specified by dleng.
   Attr specifies the attribute to use when displaying the field.
   RETURNED is the character code that terminated the input string.
   I.E. enter key, esc key, function keys, etc.
*/
{
	int ch, ch2, idx=0, tslen=sleng, tdlen=dleng, orow, ocol, ccol;
	int insert=0, lcol, odx=0, lidx, term_ch=0, x, tidx, fidx, flg=0;
	int cur_stat;

    locate(row,col);
	cur_stat=get_cur(&orow,&ocol);
	show_cur(1);
	rcolor(orow,ocol,attr,dleng);
	ccol=ocol;
	lcol=ocol + tdlen -1;
	for(x=0; x <= sleng; x++)  {
		if(*(str + x) == 0x00) flg=1;
		if(flg) *(str + x) = ' ';
		else
		if(!isprint(*(str + x))) *(str + x) = ' ';
	}
	*(str+sleng)=0x00;
	writef_n(orow,ocol,attr,str,dleng);

	while(1) {
		if(insert) show_cur(9);
		else       show_cur(1);
		ch=get_xa();
		if(isprint(ch)) ch2=0; else ch2=ch;
		switch(ch2) {
			case 0:
				if(insert) {
					fidx=sleng - tslen;
					tidx=sleng - tslen +1;
					memmove(str+tidx,str+fidx,tslen-1);
				}
				*(str + idx) = ch;
				idx++; ccol++; tslen--;
				break;
			case K_ENTER:
			case K_ESC:
				term_ch=ch;
				goto end_it;
			case K_RIGHT:
				idx++; ccol++; tslen--;
				break;
			case K_HOME:
				idx=odx=0;
				tslen=sleng;
				ccol=ocol;
				break;
			case K_END:
				for(idx=sleng - 1; idx >=0 ;idx--) {
					if(*(str + idx) != ' ') {
						idx++;
						break;
					}
				}
				tslen = sleng - idx;
				if(idx + 1 <= dleng) {
					odx = 0;
					ccol = ocol + idx;
				}
				else {
					odx = idx - dleng + 1;
					ccol = lcol;
				}
                break;
			case K_BACKSPACE:
				if(idx) idx--;
				tslen++;
				ccol--;
				*(str + idx) = ' ';
				break;
			case K_LEFT:
				idx--; tslen++;
				ccol--;
				break;
			case K_CEND:
				for(x=idx; x < sleng; x++)
					*(str + x) = ' ';
				break;
			case K_DEL:
				if(!tslen)
					*(str + idx) = ' ';
				else {
					tidx=sleng-tslen;
					fidx=sleng-tslen + 1;
					memmove(str+tidx,str+fidx,tslen);
					*(str+sleng -1)=' ';
				}
				break;
			case K_INS:
				insert = insert ^ 0x01;
				break;
			default:
				term_ch=ch;
				goto end_it;
		}
		if(tslen > sleng) tslen=sleng;
		if(tslen < 0) tslen=0;
		if(idx < 0) idx=0;
		if(idx > sleng - 1) idx=sleng-1;
		if(ccol > lcol) {
			ccol=lcol;
			odx++;
		}
		if(ccol < ocol) {
			ccol=ocol;
			odx--;
		}
		if(odx < 0) odx=0;
		if(odx > (sleng-dleng)) odx=sleng-dleng;
		writef_n(orow,ocol,attr,str+odx,dleng);
		locate(orow,ccol);
	}
end_it:
	trim_r(str);
	if(!term_ch) term_ch=0x0d;
	if(cur_stat) hide_cur();
	return(term_ch);
}
