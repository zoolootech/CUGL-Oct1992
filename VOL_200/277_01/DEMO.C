/* demo for HPGL-Clib */

/* Usage: DEMO COM1, for plot to plotter connected to COM1 Port.
	  DEMO FILE, for output to file named FILE.
	  DEMO COM1 MICRO488, if Micro488 Bus Controller is Present;
*/

#include <stdio.h>
#include "hpgl.c"

main(argc, argv)
int argc;
char *argv[];
{
	FILE *fp;
	char micro488[10];
	char buf[80];
	int micro488add;
	int i;

	fp = fopen(argv[1],"w"); /* open in write mode */
	if ( fp == NULL) {
		printf("Can Not Open Port or FILE: %s\n",argv[1]);
		exit(0);
	}

	for(i=0; i<10; i++) micro488[i] = NULL; /* NULL micro488 */

	if(strcmp(argv[2],"micro488")==0 || strcmp(argv[2],"MICRO488")==0) {
		printf("Micro488 Bus Controller Selected\n");
		printf("Enter Address of Plotter > ");
		gets(buf);
		/* move address from buffer into integer */
		sscanf(buf,"%d",&micro488add);
		if(micro488add > 9) 
			sprintf(micro488,"OUTPUT%d",micro488add);
		else 
			sprintf(micro488,"OUTPUT0%d",micro488add);
	}

	PLOTTER_INIT(fp,micro488);
	hpgl_scale(fp, 0, 10, 0, 7, micro488); /* scale in inches */
	hpgl_pen_sel(fp, 1, micro488);
	hpgl_plt_abs_uu(fp, 2.0, 3.0, micro488);
	hpgl_abs_dir(fp, (float) 0.0, (float) 1.0, micro488);
	hpgl_label(fp, "HPGL-Clib", ETX, micro488);
	fclose(fp);
}
