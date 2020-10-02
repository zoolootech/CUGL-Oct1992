


/*  Video.c maps boxes and borders to the screen : determines video hardware */

/*  Uses memrite : an assembler routine that writes directly to video ram  */

#include <conio.h>
#include <dos.h>
#include <string.h>
#include <mem.h>
#include "video.h"
#include "crt.h"


int box(int left, int top, int right, int bot,int vpage, int style, int atr)
{
register int i;
int x,v_bar,h_bar,ul_corner,ur_corner,ll_corner,lr_corner;
unsigned char top_line[81], mid_line[81], bot_line[81];

       if(right < (left + 2) || bot < (top + 2))
               return(0);

    if(style == 0)
       ul_corner = ur_corner = ll_corner = lr_corner = h_bar = v_bar = 32;
    else if(style == 1 ) {
       ul_corner = 218;
       ur_corner = 191;
       h_bar = 196;
       v_bar = 179;
       ll_corner = 192;
       lr_corner = 217;
    }
    else if(style == 2) {
       ul_corner = 201;
       ur_corner = 187;
       h_bar = 205;
       v_bar = 186;
       ll_corner = 200;
       lr_corner = 188;
    }
    else
	return(0);

    x = right - left;
    top_line[0] = ul_corner;
    mid_line[0] = v_bar;
    bot_line[0] = ll_corner;

    for(i = 1;i < x; i++) {
	top_line[i] = bot_line[i] = h_bar;
	mid_line[i] = 32;
    }

    top_line[i] = ur_corner;
    mid_line[i] = v_bar;
    bot_line[i] = lr_corner;
    i++;
    top_line[i] = mid_line[i] = bot_line[i] = '\0';

    x = bot - top;

    memrite(atr,top,left,vpage,HORIZ,top_line);
    for(i = 1; i < x; i++)
	memrite(atr,(top+i),left,vpage,HORIZ,mid_line);

    memrite(atr,(top+i),left,vpage,HORIZ,bot_line);

    return(1);
}

int border(int left,int top,int right,int bot,int vpage,int style,int atr)
{
register int i;
int x,v_bar,h_bar,ul_corner,ur_corner,ll_corner,lr_corner;
unsigned char top_line[81], mid_line[52], bot_line[81];

    if(right < (left + 2) || bot < (top + 2))
	return(0);

    if(style == 0)
	ul_corner = ur_corner = ll_corner = lr_corner = h_bar = v_bar = 32;
    else if(style == 1 ) {
	ul_corner = 218;  /* Single line border */
	ur_corner = 191;
	h_bar = 196;
	v_bar = 179;
	ll_corner = 192;
	lr_corner = 217;
    }
    else if(style == 2) {
	ul_corner = 201;
	ur_corner = 187;
	h_bar = 205;
	v_bar = 186;
	ll_corner = 200;
	lr_corner = 188;
    }
    else
	return(0);

    x = right - left;
    top_line[0] = ul_corner;
    bot_line[0] = ll_corner;

    for(i = 1;i < x; i++)
	top_line[i] = bot_line[i] = h_bar;

    top_line[i] = ur_corner;
    bot_line[i] = lr_corner;
    i++;
    top_line[i] = bot_line[i] = '\0';
    x = bot - top;
    memset(mid_line,v_bar,x);
    mid_line[x] = '\0';

    memrite(atr,top,left,vpage,HORIZ,top_line);
    memrite(atr,(top+1),left,vpage,VERT,mid_line);
    memrite(atr,(top+1),right,vpage,VERT,mid_line);
    memrite(atr,bot,left,vpage,HORIZ,bot_line);

    return(1);
}

unsigned char getmode()
{
    _AH = 15;
    _BH = 0;
    geninterrupt(0x10);
    return(_AL);
}

enum VIDEO_CARD get_adaptor()
{
unsigned char equip_code = 0;

    _AX = 0x1A00;
    geninterrupt(0x10);
    if(_AL == 0x1A) {
	switch(_BL) {
		case 0: return(NONE);
		case 1: return(MDA);
		case 2: return(CGA);
		case 4: return(EGACOLOR);
		case 5: return(EGAMONO);
		case 7: return(VGAMONO);
		case 8: return(VGACOLOR);
		case 10:
		case 12:return(MCGACOLOR);
		case 11:return(MCGAMONO);

	default:        return(CGA);
	}
    }
    else {
	_AH = 0x12;
	_BX = 0x0010;
	geninterrupt(0x10);
	if(_BX != 0x10)
	    if(_BH == 0)
		return(EGACOLOR);
	    else
		return(EGAMONO);
	else {
	    geninterrupt(0x11);
	    equip_code = (_AL & 0x30) >> 4;
	    switch(equip_code) {
		case 1:
		case 2: return(CGA);
		case 3: return(MDA);

		default:    return(NONE);
	    }
	}
    }
}





