#include <stdio.h>
#ifdef	__TURBOC__
#include <conio.h>
#endif
#include "graphics.h"

void main()  /* turtle grafik demo */
{
initgraf(-1);
gt_init();
gt_show();
gt_dir(180);
gt_forwd(200);
gt_dir(90);
gt_forwd(200);
gt_dir(0);
gt_forwd(200);
gt_dir(270);
gt_forwd(200);
gt_dir(315);
gt_forwd(200);
gt_dir(180);
gt_forwd(200);
gt_dir(135);
gt_forwd(200);
gt_dir (90);
gt_forwd(200);
gt_dir(315);
gt_forwd(200);
gt_dir(360);
gt_forwd(200);
gt_dir(270);
gt_forwd(200);
gt_dir(180);
gt_forwd(200);
gt_dir(90);
gt_forwd(200);
gt_dir(360);
gt_forwd(200);
gt_dir(135);
gt_forwd(200);
if(getch() == 'P')
	prtgrf();
exitgraf(-1);
}
