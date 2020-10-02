#include <ctype.h>
melt(int attr, char type)
/* Melt will clear the screen in various ways and will set the attributes to
   attr.
   type = t,b,l,r,a
          t=clear from top to bottom
          b=clear from bottom to top
          l=clear from left to right
          r=clear from right to left
          a=implode screen (clear from outside to inside)
*/
{
  extern int color, mono, cga, ega, scrseg, bios;
  int   row, col, orow, ocol, row2, col2;
  type = toupper(type);
  delay(1);
  if(bios) get_cur(&orow,&ocol);

	if(type == 'T') {
 	   for(row=0; row < 25; row++) {
			for(col=0; col < 80; col++) {
				if(bios) {
					locate(row,col);
					put_ca(' ',attr,1);
				}
                else
					writefc(row,col,attr,' ');
			}
			delay(10);
		}
	}



	if(type == 'B') {
 	   for(row=24; row >=0; row--) {
			for(col=0; col < 80; col++) {
				if(bios) {
					locate(row,col);
					put_ca(' ',attr,1);
				}
				else
					writefc(row,col,attr,' ');
			}
			delay(10);
		}
	}


	if(type == 'L') {
 	   for(col=0; col < 80; col++) {
			for(row=0; row < 25; row++) {
				if(bios) {
					locate(row,col);
					put_ca(' ',attr,1);
				}
				else
					writefc(row,col,attr,' ');
			}
			delay(5);
		}
	}


	if(type == 'R') {
 	   for(col=79; col >=0; col--) {
			for(row=0; row < 25; row++) {
				if(bios) {
					locate(row,col);
					put_ca(' ',attr,1);
				}
				else
					writefc(row,col,attr,' ');
			}
			delay(5);
		}
	}
	if(type == 'A') {
		for(row2=0,col2=0; col2 < 15;row2++,col2++) {
			for(col=col2; col <= (79-col2); col++) {
				if(bios) {
					locate(row2,col);
					put_ca(' ',attr,1);
				}
				else
					writefc(row2,col,attr,' ');
			}
			for(row=row2; row <= (24-row2); row++) {
				if(bios) {
					locate(row,79- col2);
					put_ca(' ',attr,1);
				}
				else
					writefc(row,79 - col2,attr,' ');
			}
			for(col=(79-col2); col >= col2; col--) {
				if(bios) {
					locate(24 - row2,col);
					put_ca(' ',attr,1);
				}
				else
					writefc(24 - row2,col,attr,' ');
			}
			for(row=(24 - row2); row >=row2; row--) {
				if(bios) {
					locate(row,col2);
					put_ca(' ',attr,1);
				}
				else
					writefc(row,col2,attr,' ');
			}
			delay(10);
		}
	}
	if(bios) locate(orow,ocol);
	return(0);
}
