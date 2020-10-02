
#ifndef HPGL_H
#define HPGL_H

#define STRING_TERM_CHAR '\003'    /* default char for string termination */
				   /* on hp plotter */
#define INIT_STRING "\033.(;IN;PU;PU;SP1;LT;PU;PU;"   /* initialization string */
						/* for hp plotter */

void hpgl_init(), hpgl_startline(), hpgl_line(), hpgl_endline();
void hpgl_label(), hpgl_moveto(), hpgl_selectpen(), hpgl_write();

#endif
