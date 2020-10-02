/****************************************************************************/
/* file name plot.h                                                         */
/*  (c) by Ronald Michaels. This program may be freely copied, modified,    */
/*  transmitted, or used for any non-commercial purpose.                    */
/* include this header in plot.c and in file which calls plot functions     */
/****************************************************************************/

void init_graph       (void);
void close_graph      (void);
void set_scales       (double,int,double *,double *);
void point            (double,int,double,double);

