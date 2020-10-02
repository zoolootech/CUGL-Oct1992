
/*						 	*
 * HPGL-Clib Version 1.0				*
 * Hewlett Packard Graphics Language 'C' Library 	*
 * (c) Copyright 1988, TerrAqueous Software		*
 */

/* Disclaimer:						*
 * TerrAqueous Software makes no expressed or implied	*
 * warranties of any kind regarding this software. In	*
 * no event will TerrAqueous Software be liable for any	*
 * damages or losses arising out of use of this soft-	*
 * ware.						*
 */

/* Software Developers Rights:				*
 * TerrAqueous Software does not require royalties on	*
 * generated applications using HPGL-Clib       	*
 */

/* HPGL-Clib is SHAREWARE, you are encouraged to copy   * 
 * and share this program with other users, on the      *
 * condition that no fee or consideration is charged,   *
 * and that this notice is not removed.                 *
 *							*
 * COMMERCIAL USERS: A $49 license fee is required if   *
 * used in a place of business or incorporated into     *
 * another product.					*
 *							*
 * PRIVATE USERS: If you are using this program and     *
 * find it of value, your contribution ($49 suggested)  *
 * will be appreciated.					*
 *							*
 * This Software is USER SUPPORTED and your enhance-    *
 * ments sent to us are encouraged. Software sent to us *
 * will become the property of TerrAqueous Software.	*
 *							*
 * Your license fee or contribution will get you the    *
 * most recent version of HPGL-Clib and keep you on our *
 * mailing list of announcements for our latest         *
 * products.
 *							*
 * TerrAqueous Software					*
 * 19515 FM 149, # 40, Houston, Texas 77070		*
 *							*
 * Software that gets you where your going!		*
 */

#include <stdio.h>
#include "hpgl.h"

/* level 1 calls				 	*
 * The following is the lowest level of the hpgl 	*
 * library calls, those which most closely follow	*
 * the HP-GL two letter mnemonic instruction syntax.	*
 * These functions are shown in alphabetical order of   *
 * the two letter mnemonic. A plotter reference manual  *
 * such as the HP7470A Interfacing and Programming      *
 * Manual is recommended.
 *							*
 * micro488 notes the use of a IO_tech MICRO488 Bus     *
 * Controller, a serial to GPIB converter. If this      *
 * device is not in use, make certain that this char    *
 * pointer passes a NULL.
 */

/* AA - arc absolute, plotter units */
void hpgl_arc_abs_pu(fp, x, y, arc_angle, chord_angle, micro488)
FILE *fp;
int x, y, arc_angle, chord_angle;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"AA%d,%d,%d,%d;",x, y, arc_angle, chord_angle);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* AA - arc absolute, user units */
void hpgl_arc_abs_uu(fp, x, y, arc_angle, chord_angle, micro488)
FILE *fp;
int arc_angle, chord_angle;
float x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"AA%f,%f,%d,%d;",x, y, arc_angle, chord_angle);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* AR - arc relative, plotter units */
void hpgl_arc_rel_pu(fp, x, y, arc_angle, chord_angle, micro488)
FILE *fp;
int x, y, arc_angle, chord_angle;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp, "AR%d,%d,%d,%d;",x, y, arc_angle, chord_angle);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* AR - arc relative, user units */
void hpgl_arc_rel_uu(fp, x, y, arc_angle, chord_angle, micro488)
FILE *fp;
int arc_angle, chord_angle;
float x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp, "AR%f,%f,%d,%d;",x, y, arc_angle, chord_angle);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* BP - beep, from hp9111a graphics tablet manual */
void graph_tab_beep(fp, frequency, duration, amplitude, micro488)
FILE *fp;
int frequency, duration, amplitude;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"BP%d,%d,%d;",frequency, duration, amplitude);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* CA - designate alternative character set */
void hpgl_char_set(fp, n, micro488)
FILE *fp;
int n;
char *micro488;
{
	rewind(fp); 
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"CA%d;",n);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* CI - circle */
void hpgl_circle(fp, radius, chord_angle, micro488)
FILE *fp;
int radius, chord_angle;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"CI%d,%d;",radius,chord_angle);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* CP - character plot */
void hpgl_char_plot(fp, spaces, lines, micro488)
FILE *fp;
float spaces, lines;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"CP%f,%f;",spaces,lines);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* CR - cursor rate, from hp9111a graphics tablet manual */
void graph_tab_cursor_rate(fp, rate, micro488)
FILE *fp;
int rate;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"CR%d;",rate);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* CS - designate standard character set */
void hpgl_stand_char(fp, m, micro488)
FILE *fp;
int m;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"CS%d;",m);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* DI - absolute direction */
void hpgl_abs_dir(fp, run, rise, micro488)
FILE *fp;
float run, rise;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"DI%f,%f;",run,rise);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* DR - relative direction */
void hpgl_rel_dir(fp, run, rise, micro488)
FILE *fp;
float run, rise;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"DR%f,%f;",run, rise);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* DT - define terminator */
void hpgl_def_term(fp, t, micro488)
FILE *fp;
int t; /* t must be ASCII character 1 - 127 */
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"DT%d;",t);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* EA - absolute rectangle, plotter units, from Fujitsu FPG-310 manual */
void hpgl_rectangle_abs_pu(fp, x, y, micro488)
FILE *fp;
int x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"EA%d,%d;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* EA - absolute rectangle, user units, from Fujitsu FPG-310 manual */
void hpgl_rectangle_abs_uu(fp, x, y, micro488)
FILE *fp;
float x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"EA%f,%f;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* ER - relative rectangle, plotter units, from Fujitsu FPG-310 manual */
void hpgl_rectangle_rel_pu(fp, x, y, micro488)
FILE *fp;
int x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"ER%d,%d;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* ER - relative rectangle, user units, from Fujitsu FPG-310 manual */
void hpgl_rectangle_rel_uu(fp, x, y, micro488)
FILE *fp;
float x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"ER%f,%f;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* EW - sector form, plotter units, from Fujitsu FPG-310 manual */
void hpgl_sector_form_pu(fp, r, delta_theta, theta, phi, micro488)
FILE *fp;
int r, delta_theta, theta, phi;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"EW%d,%d,%d,%d;",r, delta_theta, theta, phi);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* EW - sector form, user units, from Fujitsu FPG-310 manual */
void hpgl_sector_form_uu(fp, r, delta_theta, theta, phi, micro488)
FILE *fp;
float r; 
int delta_theta, theta, phi;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"EW%f,%d,%d,%d;",r, delta_theta, theta, phi);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* FT - hatching pattern, plotter units, form Fujitsu FPG-310 manual */
void hpgl_hatch_pat_pu(fp, n, d, theta, micro488)
FILE *fp;
int n, d, theta;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"FT%d,%d,%d;",n, d, theta);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* FT - hatching pattern, user units, form Fujitsu FPG-310 manual */
void hpgl_hatch_pat_uu(fp, n, d, theta, micro488)
FILE *fp;
float d;
int n, theta;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"FT%d,%f,%d;",n, d, theta);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* IM - input mask */
void hpgl_in_mask(fp, e_mask, s_mask, p_mask, micro488)
FILE *fp;
int e_mask, s_mask, p_mask; /* default is 233, 0, 0 */
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"IM%d,%d,%d;",e_mask, s_mask, p_mask);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* IP - input p1 and p2 */
void hpgl_in_p1_p2(fp, p1x, p1y, p2x, p2y, micro488)
FILE *fp;
int p1x, p1y, p2x, p2y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"IP%d,%d,%d,%d;",p1x, p1y, p2x, p2y); 
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* IW - input window */
void hpgl_in_wind(fp, x_ll, y_ll, x_ur, y_ur, micro488)
FILE *fp;
int x_ll, y_ll, x_ur, y_ur;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"IW%d,%d,%d,%d;",x_ll, y_ll, x_ur, y_ur);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* LB - label */
void hpgl_label(fp,text, t, micro488)
FILE *fp;
char text[], t; /* default is the defined ETX for terminator */ 
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"LB%s%c",text, t);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* LT - line type */
void hpgl_line_type(fp, pat_num, pat_len, micro488)
FILE *fp;
int pat_num, pat_len;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"LT%d,%d;",pat_num, pat_len);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* OA - output actual position and pen status */
void hpgl_pos_pen
(fp, x, y, p, micro488)
FILE *fp;
int *x, *y, *p;
char *micro488;
{
	char buf[80];
	char enter488[10];
	int i;

	for(i=0; i<80; i++) buf[i] = NULL;
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OA;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c%d%*c%d%*c",x, y, p);
}

/* OC - output commanded position and pen status, plotter units */
void hpgl_out_pos_pen_pu(fp, x, y, p, micro488)
FILE *fp;
int *x, *y, *p;
char *micro488;
{
	char buf[80];
	char enter488[10];

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OC;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c%d%*c%d%*c",x, y, p);
}

/* OC - output commanded position and pen status, user units */
void hpgl_out_pos_pen_uu(fp, x, y, p, micro488)
FILE *fp;
float *x, *y, *p;
char *micro488;
{
	char buf[80];
	char enter488[10];

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OC;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%f%*c%f%*c%f%*c",x, y, p);
}

/* OD - output digitized point and pen status */
void hpgl_out_pt_pen(fp, x, y, p, micro488)
FILE *fp;
int *x, *y, *p;
char *micro488;
{
	char buf[80];
	char enter488[10];

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OD;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c%d%*c%d%*c",x, y, p);
}

/* OE - output error instruction */
int hpgl_out_error(fp, micro488)
FILE *fp;
char *micro488;
{
	char buf[80];
	char enter488[10];
	int error=0;

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OE;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c",&error);
	return(error);
}

/* OF - output factors */
void hpgl_out_fac(fp, x_fac, y_fac, micro488)
FILE *fp;
int *x_fac, *y_fac;
char *micro488;
{
	char buf[80];
	char enter488[10];

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OF;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c%d%*c",x_fac, y_fac);
}

/* OH - output effective drawing range of current paper type, from Fujitsu
   FPG-310 manual */
void hpgl_out_paper(fp, x_ll, y_ll, x_ur, y_ur, micro488)
FILE *fp;
int *x_ll, *y_ll, *x_ur, *y_ur;
char *micro488;
{
	char buf[80];
	char enter488[10];

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OH;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c%d%*c%d%*c%d%*c",x_ll, y_ll, x_ur, y_ur);
}

/* OI - output identification */
void hpgl_out_id(fp, id, micro488)
FILE *fp;
char id[];
char *micro488;
{
	char buf[80];
	char enter488[10];

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OI;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%s%*c",id);
}

/* OK - output key, from hp9111a graphics tablet manual */
int graph_tab_out_key(fp, micro488)
FILE *fp;
char *micro488;
{
	char buf[80];
	char enter488[10];
	int key;

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OK;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c",&key);
	return(key);
}

/* OO - output options */
void hpgl_out_opt(fp, a, pen_sel, c, d, arc_cir, e, f, g, micro488)
FILE *fp;
int *a, *pen_sel, *c, *d, *arc_cir, *e,  *f, *g;
char *micro488;
{
	char buf[80];
	char enter488[10];

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OO;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c%d%*c%d%*c%d%*c%d%*c%d%*c%d%*c%d%*c",
	a, pen_sel, c, d, arc_cir, e, f, g);
}

/* OP - output p1 and p2 */
void hpgl_out_p1_p2(fp, p1x, p1y, p2x, p2y, micro488)
FILE *fp;
int *p1x, *p1y, *p2x, *p2y;
char *micro488;
{
	char buf[80];
	char enter488[10];

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OP;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c%d%*c%d%*c%d%*c",p1x,p1y,p2x,p2y);
}

/* OR - output resolution, from hp9111a graphics tablet manual */
void graph_tab_out_resolution(fp, x, y, micro488)
FILE *fp;
float *x, *y;
char *micro488;
{
	char buf[80];
	char enter488[10];

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OR;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%f%*c%f%*c",x, y);
}

/* OS - output status */
int hpgl_status(fp, micro488)
FILE *fp;
char *micro488;
{
	char buf[80];
	char enter488[10];
	int status=0;

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OS;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c",&status);
	return(status);
}

/* OW - output window */
void hpgl_out_wind(fp, x_ll, y_ll, x_ur, y_ur, micro488)
FILE *fp;
int *x_ll, *y_ll, *x_ur, *y_ur;
char *micro488;
{
	char buf[80];
	char enter488[10];

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"OW;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c%d%*c%d%*c%d%*c",x_ll, y_ll, x_ur, y_ur);
}

/* PA - plot absolute plotter units */
void hpgl_plt_abs_pu(fp, x, y, micro488)
FILE *fp;
int x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"PA%d,%d;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* PA - plot absolute  user units */
void hpgl_plt_abs_uu(fp, x, y, micro488)
FILE *fp;
float x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"PA%.4f,%.4f;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* PD - pen down */
void hpgl_pen_dwn(fp, x, y, micro488)
FILE *fp;
int x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"PD%d,%d;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* PR - plot relative plotter units */
void hpgl_plt_rel_pu(fp, x, y, micro488)
FILE *fp;
int x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"PR%d,%d;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* PR - plot relative user units */
void hpgl_plt_rel_uu(fp, x, y, micro488)
FILE *fp;
float x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"PR%.4f,%.4f;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* PS - paper size, from Fujitsu FPG-310 manual */
void hpgl_paper_size(fp, s, x, y, micro488)
FILE *fp;
int s, x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"PS%d,%d,%d;",s, x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* PT - interval of line for hatching pattern, from Fujitsu FPG-310 manual */
void hpgl_hatch_line_interval(fp, t, micro488)
FILE *fp;
float t;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"PT%f;",t);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* PU - pen up */
void hpgl_pen_up(fp, x, y, micro488)
FILE *fp;
int x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"PU%d,%d;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* RA - absolute rectangle hatching, plotter units, 
   from Fujitsu FPG-310 manual */
void hpgl_rect_hatch_abs_pu(fp, x, y, micro488)
FILE *fp;
int x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"RA%d,%d;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* RA - absolute rectangle hatching, user units, 
   from Fujitsu FPG-310 manual */
void hpgl_rect_hatch_abs_uu(fp, x, y, micro488)
FILE *fp;
float x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"RA%f,%f;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* RC - read cursor, from hp9111a graphics tablet manual */
void graph_tab_read_cursor(fp, x, y, pen, key, status, error, micro488)
FILE *fp;
int *x, *y, *pen, *key, *status, *error;
char *micro488;
{
	char buf[80];
	char enter488[10];

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"RC;");
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c%d%*c%d%*c%d%*c%d%*c%d%*c",x, y, pen, key, status, error);
}

/* RO - rotate either 0 deg. or 90 deg., taken from Fujitsu FPG-310 manual */
void hpgl_rotate(fp, a, micro488)
FILE *fp;
int a;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"RO%d;",a);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* RR - relative rectangle hatching, plotter units, 
   from Fujitsu FPG-310 manual */
void hpgl_rect_hatch_rel_pu(fp, x, y, micro488)
FILE *fp;
int x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"RR%d,%d;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* RR - relative rectangle hatching, user units, 
   from Fujitsu FPG-310 manual */
void hpgl_rect_hatch_rel_uu(fp, x, y, micro488)
FILE *fp;
float x, y;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"RR%f,%f;",x, y);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* RS - read softkey, from hp9111a graphics tablet manual */
int graph_tab_read_softkey(fp, menu_enable, micro488)
FILE *fp;
int menu_enable;
char *micro488;
{
	char buf[80];
	char enter488[10];
	int key;

	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"RS%d;",menu_enable);
	if(*micro488 != NULL) {
		out_enter(micro488, enter488);
		fprintf(fp,"\n%s\n",enter488);
	}
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c",&key);
	return(key);
}
	
/* SC - scale */
void hpgl_scale(fp, xmin, xmax, ymin, ymax, micro488)
FILE *fp;
int xmin, xmax, ymin, ymax;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"SC%d,%d,%d,%d;", xmin, xmax, ymin, ymax);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* SI - absolute character size */
void hpgl_abs_char_sz(fp, width, height, micro488)
FILE *fp;
float width, height;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"SI%f,%f;",width, height);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
} 

/* SL - character slant */
void hpgl_char_sl(fp, tan, micro488)
FILE *fp;
float tan;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"SL%f;",tan);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* SM - symbol mode */
void hpgl_sym(fp, c, micro488)
FILE *fp;
char c;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"SM%c;",c);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* SP - pen select */
void hpgl_pen_sel(fp, pen, micro488)
FILE *fp;
int pen;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"SP%d;",pen);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* SR - relative character size */
void hpgl_char_sz(fp, width, height, micro488)
FILE *fp;
float width, height;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"SR%f,%f;",width, height);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* TL - tick length */
void hpgl_tick_len(fp, tp, tn, micro488)
FILE *fp;
float tp, tn;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"TL%f,%f;",tp, tn);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* UC - user defined character */
void hpgl_usr_char(fp, pen_control, x, y, num, micro488)
FILE *fp;
/* pen_control, x, y are arrays of float, num is the number of arrays */
float *pen_control, *x, *y; 
int num;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"UC");
	if(*micro488 != NULL)
		fprintf(fp,"\n");
	while(num-- > 0) {
		if(*micro488 != NULL)
			fprintf(fp,"%s",micro488);
		fprintf(fp,"%f,%f,%f",*pen_control++, *x++, *y++);
		if(num > 0) 
			fprintf(fp,",");
		else
			fprintf(fp,";");
		if(*micro488 != NULL)
			fprintf(fp,"\n");
	}
}


/* VS - velocity select */
void hpgl_pen_vel(fp, pen_velocity, micro488)
FILE *fp;
float pen_velocity;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"VS%f;",pen_velocity);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* WG - wedge hatching,plotter units, from Fujitsu FPG-310 manual */
void hpgl_wedge_hatch_pu(fp, r, delta_theta, theta, phi, micro488)
FILE *fp;
int r, delta_theta, theta, phi;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"WG%d,%d,%d,%d;",r, delta_theta, theta, phi);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}

/* WG - wedge hatching, user units, from Fujitsu FPG-310 manual */
void hpgl_wedge_hatch_uu(fp, r, delta_theta, theta, phi, micro488)
FILE *fp;
float r; 
int delta_theta, theta, phi;
char *micro488;
{
	rewind(fp);
	if(*micro488 != NULL)
		fprintf(fp,"%s",micro488);
	fprintf(fp,"WG%f,%d,%d,%d;",r, delta_theta, theta, phi);
	if(*micro488 != NULL)
		fprintf(fp,"\n");
}


/* RS-232-C COMMANDS					*/

/* set plotter configuration */
void serial_plot_config(fp, buf_size, handshake)
FILE *fp;
int buf_size, handshake;
{
	rewind(fp);
	fprintf(fp,"\033.@%d;%d:",buf_size, handshake);
}

/* output buffer space */
int serial_out_buffer_space(fp)
FILE *fp;
{
	char buf[80];
	int buf_space;

	rewind(fp);
	fprintf(fp,"\033.B");
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c",&buf_space);
	return(buf_space);
}

/* output extended error */
int serial_out_error(fp)
FILE *fp;
{
	char buf[80];
	int error;

	rewind(fp);
	fprintf(fp,"\033.E");
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c",&error);
	return(error);
}

/* set handshake mode 1 */
void serial_handshake_1(fp, c0, c1, num)
FILE *fp;
/* c1 is an integer array, num is the number of c1 arrays */
int c0, *c1;
int num;
{
	rewind(fp);
	fprintf(fp,"\033.H%d;",c0);
	while(num-- > 0) {
		fprintf(fp,"%d",*c1++);
		if(num > 0)
			fprintf(fp,";");
		else
			fprintf(fp,":");
	}
}

/* set handshake mode 2 */
void serial_handshake_2(fp, c0, c1, num)
FILE *fp;
/* c1 is a character array, num is the number of c1 arrays */
char c0, *c1;
int num;
{
	rewind(fp);
	fprintf(fp,"\033.I%d;",c0);
	while(num-- > 0) {
		fprintf(fp,"%c",*c1++);
		if(num > 0)
			fprintf(fp,";");
		else
			fprintf(fp,":");
	}
}

/* set output mode */
void serial_set_output(fp,t,c1,c2,c3,c4,c5)
FILE *fp;
unsigned int t;
int c1, c2, c3, c4, c5;
{
	rewind(fp);
	fprintf(fp,"\033.M%u;%d;%d;%d;%d;%d:",t, c1, c2, c3, c4, c5);
}

/* output extended status */
int serial_output_status(fp)
FILE *fp;
{
	char buf[80];
	int status;

	rewind(fp);
	fprintf(fp,"\033.O");
	rewind(fp);
	fgets(buf,80,fp);
	sscanf(buf,"%d%*c",&status);
	return(status);
}
	
/* set extended output and handshake mode */
void serial_ext_out_handshake(fp, t, c, num)
FILE *fp;
unsigned int t;
/* c is an array of integers, num is the number if c arrays */
int *c;
int num;
{
	rewind(fp);
	fprintf(fp,"\033.N%u;",t);
	while(num-- > 0) {
		fprintf(fp,"%d",*c);
		if(num > 0)
			fprintf(fp,";");
		else
			fprintf(fp,":");
	}
}

/* parse address from micro488 and follow ENTER in enter488 */
out_enter(micro488,enter488)
char *micro488, *enter488;
{
	int i;

	*enter488++ = 'E';
	*enter488++ = 'N';
	*enter488++ = 'T';
	*enter488++ = 'E';
	*enter488++ = 'R';
	for(i=0; i<6; i++) {
		++micro488; /* point to first digit after OUTPUT */
	}
	/* copy address */
	for(i=0; i<2; i++) {
		*enter488++ = *micro488++;
	}
	*enter488 = NULL;
}
