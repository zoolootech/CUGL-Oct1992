#include <tcutil.h>
#include <string.h>
#include <ctype.h>

get_field(int row, int col, char *str, int sleng, int attr, int dleng, char *mask)
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³Purpose: To provide for formatted field input capability.           ³
³                                                                    ³
³ Inputs: row = row to read field from                               ³
³         col = col to read field from                               ³
³         *str = pointer to char string to store field into.         ³
³         sleng = leng of string pointed to by *str.                 ³
³         attr = attribute used to display field.                    ³
³         dleng = length of display area.                            ³
³         *mask = pointer to mask string.                            ³
³         (THE MASK STRING MUST BE THE SAME LENGTH AS IS SPECIFIED   ³
³          IN THE SLENG PARAMETER!!!!  THE MASK CONSISTS OF THE      ³
³          FOLLOWING SPECIAL CHARACTERS.                             ³
³          A = allow alpha characters only in this position.         ³
³          N = allow numeric characters only in this position.       ³
³      blank = allow any characters in this position.                ³
³          # = force the enter key when the user hits this position. ³
³                                                                    ³
³          ANY OTHER CHARACTER IN THE MASK FIELD WILL BE DISPLAYED   ³
³          ON THE SCREEN AND INPUT WILL NOT BE ALLOWED IN THAT       ³
³          POSITION. NOTE!!!!!!!!!! THE A AND N MASK CHARACTERS      ³
³          MUST BE UPPER CASE IN ORDER TO BE RECOGNIZED!!!!!!!!!!!!!!³
³                                                                    ³
³Outputs: None.                                                      ³
³                                                                    ³
³                                                                    ³
³ Return: An integer value indicating the key that was pressed to    ³
³         terminate the entry of the field.  I.E. the enter key      ³
³         or the ESC key.                                            ³
³                                                                    ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
*/
{
	int ch, ch2, idx=0, tslen=sleng, tdlen=dleng, orow, ocol, ccol;
    int lcol, odx=0, lidx, term_ch=0, x, tidx, fidx;
	int cur_stat, skip=0;
	char mch, backup=0;


	locate(row,col);
	cur_stat = get_cur(&orow,&ocol);
	show_cur(1);
	rcolor(row,col,attr,dleng);
	ccol=ocol;
	lcol=ocol + tdlen -1;
	strcpy(str,mask);
	for(x=0; x <= sleng; x++)  {
		switch (*(str + x)) {
			case 'A':
			case 'X':
			case 'N':
			case '#':
				*(str + x) = ' ';
		}
	}
	writef_n(row,col,attr,str,dleng);

	while(1) {
		mch=*(mask + idx);
		if(!idx) {
			if(mch == 'A' || mch == ' ' || mch == 'N') {
				idx=idx;
			}
			else {
				idx++; ccol++; tslen--; skip=1;
			}
		}
		else skip=0;

		if(!skip) ch=get_xa();
		if(!skip) {
			if(isprint(ch)) ch2=0;
			else ch2=ch;
		}

		if(!skip) {
			switch(ch2) {
				case 0:
                    if(mch == 'A') {
						if(!isalpha(ch)) {
							beep(1600,5);
							break;
						}
					}
                    if(mch == 'N') {
						if(!isdigit(ch)) {
							beep(1600,5);
							break;
						}
					}
					*(str + idx) = ch;
					idx++; ccol++; tslen--;
					break;
				case K_ENTER:
				case K_ESC:
					term_ch=ch;
					goto end_it;
				case K_TAB:
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
					backup=1;
					if(idx) idx--;
					mch = *(mask + idx);
					tslen++;
					ccol--;
					if(mch == 'A' || mch == 'X' || mch == 'N')
						*(str + idx) = ' ';
					break;
				case K_LEFT:
					backup=1;
					idx--; tslen++;
					ccol--;
					break;
				case K_CEND:
					for(x=idx; x < sleng; x++)
						*(str + x) = ' ';
					break;
				default:
					break;
					/*term_ch=ch;*/
					/*goto end_it;*/
			}
		}
		mch = *(mask + idx);
		while(idx && tslen && (mch != 'A' && mch != ' ' && mch != 'N' && mch != '#')) {
			if(backup) {
				idx--; ccol--; tslen++;
			}
			else {
			idx++; ccol++; tslen--;
			}
			mch = *(mask + idx);
		}
		backup=0;
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
		if(mch == '#') {
			term_ch = 0x00;
			goto end_it;
		}
	}
end_it:
	trim_r(str);
	if(!term_ch) term_ch=0x0d;
	if(cur_stat) hide_cur();
	return(term_ch);
}
