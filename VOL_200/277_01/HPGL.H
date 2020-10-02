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

#define ETX	'\003' /* end of text */

/* CN - continuous sample, from hp9111a graphics tablet manual */
#define CONTINUOUS_SAMPLE(fp,micro488)   { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"CN;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* DC */
#define DIGITIZE_CLEAR(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"DC;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* DF */
#define PLOTTER_DEFAULT(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488);\
fprintf(fp,"DF;"); if(*micro488 != NULL) fprintf(fp,"\n"); }

/* DP */
#define DIGITIZE_POINT(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"DP;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* IN */
#define PLOTTER_INIT(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"IN;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* IW */
#define DEFAULT_WINDOW(fp,micro488) { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"IW;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* PA */
#define PLOT_ABSOLUTE(fp,micro488) { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"PA;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* PU */
#define PENUP(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"PU;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* PR */
#define PLOT_RELATIVE(fp,micro488) { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"PR;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* PD */
#define PENDOWN(fp,micro488) { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"PD;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* SA */
#define SELECT_ALT_CHAR(fp,micro488) { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"SA;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* SC */
#define PLOTTER_UNITS_ON(fp,micro488) { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"SC;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* SF - switch follow, from hp9111a graphics tablet manual */
#define SWITCH_FOLLOW(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"SF;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* SG - single sample, from hp9111a graphics tablet manual */
#define SINGLE_SAMPLE(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"SG;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* SI - aboslute character size default, .19 w, .27 h */
#define DEFAULT_ABS_CHAR(fp,micro488) { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"SI;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* SK - set key, from hp9111a graphics tablet manual */
#define SET_KEY(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"SK;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* SN - switch normal, from hp9111a graphics tablet manual */
#define SWITCH_NORMAL(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"SN;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* SS */
#define SELECT_STD_CHAR(fp,micro488) { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"SS;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* TD - test digitizer, from hp9111a graphics tablet manual */
#define TEST_DIGITIZER(fp,micro488) { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"TD;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* TL */
#define DEFAULT_TICK_LEN(fp,micro488) { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"TL;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* TP - take point, from hp9111a graphics tablet manual */
#define TAKE_POINT(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"TP;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* XT */
#define X_TICK(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"XT;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* YT */
#define Y_TICK(fp,micro488)  { \
rewind(fp); if(*micro488 != NULL) fprintf(fp,"%s",micro488); fprintf(fp,"YT;");\
if(*micro488 != NULL) fprintf(fp,"\n"); }

/* RS-232-C ESCAPE COMMANDS				*/
#define PLOTTER_ON_SERIAL(fp) { rewind(fp); fprintf(fp,"\033.("); }
#define PLOTTER_OFF_SERIAL(fp) { rewind(fp); fprintf(fp,"\033.)"); }
#define RESET_HANDSHAKE_SERIAL(fp) { rewind(fp); \
	fprintf(fp,"\033.R"); }
#define ABORT_GRAPHICS_SERIAL(fp) { rewind(fp); \
	fprintf(fp,"\033.K"); }
#define ABORT_DEVICE_CTL_SERIAL(fp) { rewind(fp); \
	fprintf(fp,"\033.J"); }

/* enables hardwire handshake and disables monitor mode */
#define PLOTTER_CONFIG_SERIAL(fp) { rewind(fp); \
	fprintf(fp,"\033.@:"); }

/* carriage return as terminator, no turnaround delay, no output
   trigger, no echo terminate, no output initiator character */
#define DEFAULT_OUTPUT_SERIAL(fp) { rewind(fp); \
	fprintf(fp,"\033.M:"); }

/* no intercharacter delay, no xoff trigger character or immediate
   response string */
#define DEFAULT_EXT_OUT_HAND_SERIAL(fp) { rewind(fp); \
	fprintf(fp,"\033.N:");


/* level 1 library calls */
void hpgl_arc_abs_pu();
void hpgl_arc_abs_uu();
void hpgl_arc_rel_pu();
void hpgl_arc_rel_uu();
void graph_tab_beep();
void hpgl_char_set();
void hpgl_circle();
void hpgl_char_plot();
void graph_tab_cursor_rate();
void hpgl_stand_char();
void hpgl_abs_dir();
void hpgl_rel_dir();
void hpgl_def_term();
void hpgl_rectangle_abs_pu();
void hpgl_rectangle_abs_uu();
void hpgl_rectangle_rel_pu();
void hpgl_rectangle_rel_uu();
void hpgl_sector_form_pu();
void hpgl_sector_form_uu();
void hpgl_hatch_pat_pu();
void hpgl_hatch_pat_uu();
void hpgl_in_mask();
void hpgl_in_p1_p2();
void hpgl_in_wind();
void hpgl_label();
void hpgl_line_type();
void hpgl_pos_pen();
void hpgl_out_pos_pen_pu();
void hpgl_out_pos_pen_uu();
void hpgl_out_pt_pen();
int hpgl_out_error();
void hpgl_out_fac();
void hpgl_out_paper();
void hpgl_out_id();
int graph_tab_out_key();
void hpgl_out_opt();
void hpgl_out_p1_p2();
void graph_tab_out_resolution();
int hpgl_status();
void hpgl_out_wind();
void hpgl_plt_abs_pu();
void hpgl_plt_abs_uu();
void hpgl_pen_dwn();
void hpgl_plt_rel_pu();
void hpgl_plt_rel_uu();
void hpgl_paper_size();
void hpgl_hatch_line_interval();
void hpgl_pen_up();
void hpgl_rect_hatch_abs_pu();
void hpgl_rect_hatch_abs_uu();
void graph_tab_read_cursor();
void hpgl_rotate();
void hpgl_rect_hatch_rel_pu();
void hpgl_rect_hatch_rel_uu();
int graph_tab_read_softkey();
void hpgl_scale();
void hpgl_abs_char_sz();
void hpgl_char_sl();
void hpgl_sym();
void hpgl_pen_sel();
void hpgl_char_sz();
void hpgl_tick_len();
void hpgl_usr_char();
void hpgl_pen_vel();
void wedge_hatch_pu();
void wedge_hatch_uu();
void serial_plot_config();
int serial_out_buffer_space();
int serial_out_error();
void serial_handshake_1();
void serial_handshake_2();
void serial_set_output();
int serial_output_status();
void serial_ext_out_handshake();




