/* GNUPLOT - misc.c */
/*
 * Copyright (C) 1986, 1987, 1990   Thomas Williams, Colin Kelley
 *
 * Permission to use, copy, and distribute this software and its
 * documentation for any purpose with or without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and 
 * that both that copyright notice and this permission notice appear 
 * in supporting documentation.
 *
 * Permission to modify the software is granted, but not the right to
 * distribute the modified code.  Modifications are to be distributed 
 * as patches to released version.
 *  
 * This software  is provided "as is" without express or implied warranty.
 * 
 *
 * AUTHORS
 * 
 *   Original Software:
 *     Thomas Williams,  Colin Kelley.
 * 
 *   Gnuplot 2.0 additions:
 *       Russell Lang, Dave Kotz, John Campbell.
 * 
 * send your comments or suggestions to (pixar!info-gnuplot@sun.com).
 * 
 */

#include <stdio.h>
#include <math.h>
#include "plot.h"
#include "setshow.h"
#include "help.h"
#ifdef __TURBOC__
#include <graphics.h>
#endif

#ifndef _IBMR2
extern char *malloc();
#endif

extern int c_token;
extern char replot_line[];
extern struct at_type at;
extern struct ft_entry ft[];
extern struct udft_entry *first_udf;
extern struct udvt_entry *first_udv;

extern struct at_type *temp_at();

extern BOOLEAN interactive;
extern char *infile_name;
extern int inline_num;

/* State information for load_file(), to recover from errors
 * and properly handle recursive load_file calls
 */
typedef struct lf_state_struct LFS;
struct lf_state_struct {
    FILE *fp;				/* file pointer for load file */
    char *name;			/* name of file */
    BOOLEAN interactive;		/* value of interactive flag on entry */
    int inline_num;			/* inline_num on entry */
    LFS *prev;				/* defines a stack */
} *lf_head = NULL;			/* NULL if not in load_file */

static BOOLEAN lf_pop();
static void lf_push();

/*
 * instead of <strings.h>
 */
extern int strcmp();


/*
 * cp_free() releases any memory which was previously malloc()'d to hold
 *   curve points.
 */
cp_free(cp)
struct curve_points *cp;
{
	if (cp) {
		cp_free(cp->next_cp);
		if (cp->title)
			free((char *)cp->title);
		free((char *)cp);
	}
}



save_functions(fp)
FILE *fp;
{
register struct udft_entry *udf = first_udf;
	
	if (fp) {
		while (udf) {
			if (udf->definition)
				fprintf(fp,"%s\n",udf->definition);
			udf = udf->next_udf;
		}
		(void) fclose(fp);
	} else
		os_error("Cannot open save file",c_token);			
}


save_variables(fp)
FILE *fp;
{
register struct udvt_entry *udv = first_udv->next_udv;	/* skip pi */

	if (fp) {
		while (udv) {
			if (!udv->udv_undef) {
				fprintf(fp,"%s = ",udv->udv_name);
				disp_value(fp,&(udv->udv_value));
				(void) putc('\n',fp);
			}
			udv = udv->next_udv;
		}
		(void) fclose(fp);
	} else
		os_error("Cannot open save file",c_token);			
}


save_all(fp)
FILE *fp;
{
register struct udft_entry *udf = first_udf;
register struct udvt_entry *udv = first_udv->next_udv;	/* skip pi */

	if (fp) {
		save_set_all(fp);
		while (udf) {
			if (udf->definition)
				fprintf(fp,"%s\n",udf->definition);
			udf = udf->next_udf;
		}
		while (udv) {
			if (!udv->udv_undef) {
				fprintf(fp,"%s = ",udv->udv_name);
				disp_value(fp,&(udv->udv_value));
				(void) putc('\n',fp);
			}
			udv = udv->next_udv;
		}
		fprintf(fp,"%s\n",replot_line);
		(void) fclose(fp);
	} else
		os_error("Cannot open save file",c_token);			
}


save_set(fp)
FILE *fp;
{
	if (fp) {
		save_set_all(fp);
		(void) fclose(fp);
	} else
		os_error("Cannot open save file",c_token);			
}


save_set_all(fp)
FILE *fp;
{
struct text_label *this_label;
struct arrow_def *this_arrow;
	fprintf(fp,"set terminal %s\n", term_tbl[term].name);
	fprintf(fp,"set output %s\n",strcmp(outstr,"STDOUT")? outstr : "" );
	fprintf(fp,"set %sclip points\n", (clip_points)? "" : "no");
	fprintf(fp,"set %sclip one\n", (clip_lines1)? "" : "no");
	fprintf(fp,"set %sclip two\n", (clip_lines2)? "" : "no");
	fprintf(fp,"set dummy %s\n",dummy_var);
	fprintf(fp,"set format x \"%s\"\n", xformat);
	fprintf(fp,"set format y \"%s\"\n", yformat);
	fprintf(fp,"set %sgrid\n", (grid)? "" : "no");
	switch (key) {
		case -1 : 
			fprintf(fp,"set key\n");
			break;
		case 0 :
			fprintf(fp,"set nokey\n");
			break;
		case 1 :
			fprintf(fp,"set key %g,%g\n",key_x,key_y);
			break;
	}
	fprintf(fp,"set nolabel\n");
	for (this_label = first_label; this_label != NULL;
			this_label = this_label->next) {
		fprintf(fp,"set label %d \"%s\" at %g,%g ",
			   this_label->tag,
			   this_label->text, this_label->x, this_label->y);
		switch(this_label->pos) {
			case LEFT : 
				fprintf(fp,"left");
				break;
			case CENTRE :
				fprintf(fp,"centre");
				break;
			case RIGHT :
				fprintf(fp,"right");
				break;
		}
		fputc('\n',fp);
	}
	fprintf(fp,"set noarrow\n");
	for (this_arrow = first_arrow; this_arrow != NULL;
			this_arrow = this_arrow->next) {
		fprintf(fp,"set arrow %d from %g,%g to %g,%g\n",
			   this_arrow->tag,
			   this_arrow->sx, this_arrow->sy,
			   this_arrow->ex, this_arrow->ey);
	}
	if ((!log_x)||(!log_y))
		fprintf(fp,"set nologscale xy\n");
	if (log_x||log_y)
		fprintf(fp,"set logscale %c%c\n", 
			log_x ? 'x' : ' ', log_y ? 'y' : ' ');
	fprintf(fp,"set offsets %g, %g, %g, %g\n",loff,roff,toff,boff);
	fprintf(fp,"set %spolar\n", (polar)? "" : "no");
	fprintf(fp,"set %sparametric\n", (parametric)? "" : "no");
	fprintf(fp,"set samples %d\n",samples);
	fprintf(fp,"set size %g,%g\n",xsize,ysize);
	fprintf(fp,"set data style ");
	switch (data_style) {
		case LINES: fprintf(fp,"lines\n"); break;
		case POINTS: fprintf(fp,"points\n"); break;
		case IMPULSES: fprintf(fp,"impulses\n"); break;
		case LINESPOINTS: fprintf(fp,"linespoints\n"); break;
		case DOTS: fprintf(fp,"dots\n"); break;
	}
	fprintf(fp,"set function style ");
	switch (func_style) {
		case LINES: fprintf(fp,"lines\n"); break;
		case POINTS: fprintf(fp,"points\n"); break;
		case IMPULSES: fprintf(fp,"impulses\n"); break;
		case LINESPOINTS: fprintf(fp,"linespoints\n"); break;
		case DOTS: fprintf(fp,"dots\n"); break;
	}
	fprintf(fp,"set tics %s\n", (tic_in)? "in" : "out");
     save_tics(fp, xtics, 'x', &xticdef);
     save_tics(fp, ytics, 'y', &yticdef);
	fprintf(fp,"set title \"%s\"\n",title);
	fprintf(fp,"set trange [%g : %g]\n",tmin,tmax);
	fprintf(fp,"set xlabel \"%s\"\n",xlabel);
	fprintf(fp,"set xrange [%g : %g]\n",xmin,xmax);
	fprintf(fp,"set ylabel \"%s\"\n",ylabel);
	fprintf(fp,"set yrange [%g : %g]\n",ymin,ymax);
	fprintf(fp,"set %s %c\n", 
		autoscale_t ? "autoscale" : "noautoscale", 't');
	fprintf(fp,"set %s %c%c\n", 
		(autoscale_y||autoscale_x) ? "autoscale" : "noautoscale", 
		autoscale_x ? 'x' : ' ', autoscale_y ? 'y' : ' ');
	fprintf(fp,"set zero %g\n",zero);
}

save_tics(fp, onoff, axis, tdef)
	FILE *fp;
	BOOLEAN onoff;
	char axis;
	struct ticdef *tdef;
{
    if (onoff) {
	   fprintf(fp,"set %ctics", axis);
	   switch(tdef->type) {
		  case TIC_COMPUTED: {
			 break;
		  }
		  case TIC_SERIES: {
			 fprintf(fp, " %g,%g,%g", tdef->def.series.start,
				    tdef->def.series.incr, tdef->def.series.end);
			 break;
		  }
		  case TIC_USER: {
			 register struct ticmark *t;
			 fprintf(fp, " (");
			 for (t = tdef->def.user; t != NULL; t=t->next) {
				if (t->label)
				  fprintf(fp, "\"%s\" ", t->label);
				if (t->next)
				  fprintf(fp, "%g, ", t->position);
				else
				  fprintf(fp, "%g", t->position);
			 }
			 fprintf(fp, ")");
			 break;
		  } 
	   }
	   fprintf(fp, "\n");
    } else {
	   fprintf(fp,"set no%ctics\n", axis);
    }
}

load_file(fp, name)
	FILE *fp;
	char *name;
{
    register int len;
    extern char input_line[];

    int start, left;
    int more;
    int stop = FALSE;

    lf_push(fp);			/* save state for errors and recursion */

    if (fp == (FILE *)NULL) {
	   char errbuf[BUFSIZ];
	   (void) sprintf(errbuf, "Cannot open load file '%s'", name);
	   os_error(errbuf, c_token);
    } else {
	   /* go into non-interactive mode during load */
	   /* will be undone below, or in load_file_error */
	   interactive = FALSE;
	   inline_num = 0;
	   infile_name = name;

	   while (!stop) {		/* read all commands in file */
		  /* read one command */
		  left = MAX_LINE_LEN;
		  start = 0;
		  more = TRUE;

		  while (more) {
			 if (fgets(&(input_line[start]), left, fp) == NULL) {
				stop = TRUE; /* EOF in file */
				input_line[start] = '\0';
				more = FALSE;	
			 } else {
				inline_num++;
				len = strlen(input_line) - 1;
				if (input_line[len] == '\n') { /* remove any newline */
				    input_line[len] = '\0';
				    /* Look, len was 1-1 = 0 before, take care here! */
				    if (len > 0) --len;
				} else if (len+1 >= left)
				  int_error("Input line too long",NO_CARET);
				 
				if (input_line[len] == '\\') { /* line continuation */
				    start = len;
				    left  = MAX_LINE_LEN - start; /* left -=len;*/
				} else
				  more = FALSE;
			 }
		  }

		  if (strlen(input_line) > 0) {
			 screen_ok = FALSE;	/* make sure command line is
							   echoed on error */
			 do_line();
		  }
	   }
    }

    /* pop state */
    (void) lf_pop();		/* also closes file fp */
}

/* pop from load_file state stack */
static BOOLEAN				/* FALSE if stack was empty */
lf_pop()					/* called by load_file and load_file_error */
{
    LFS *lf;

    if (lf_head == NULL)
	 return(FALSE);
    else {
	   lf = lf_head;
	   if (lf->fp != (FILE *)NULL)
		(void) fclose(lf->fp);
	   interactive = lf->interactive;
	   inline_num = lf->inline_num;
	   infile_name = lf->name;
	   lf_head = lf->prev;
	   free((char *)lf);
	   return(TRUE);
    }
}

/* push onto load_file state stack */
/* essentially, we save information needed to undo the load_file changes */
static void
lf_push(fp)			/* called by load_file */
	FILE *fp;
{
    LFS *lf;
    
    lf = (LFS *)alloc(sizeof(LFS), (char *)NULL);
    if (lf == (LFS *)NULL) {
	   if (fp != (FILE *)NULL)
		(void) fclose(fp);		/* it won't be otherwise */
	   int_error("not enough memory to load file", c_token);
    }
	 
    lf->fp = fp;			/* save this file pointer */
    lf->name = infile_name;	/* save current name */
    lf->interactive = interactive;	/* save current state */
    lf->inline_num = inline_num; /* save current line number */
    lf->prev = lf_head;		/* link to stack */
    lf_head = lf;
}

load_file_error()			/* called from main */
{
    /* clean up from error in load_file */
    /* pop off everything on stack */
    while(lf_pop())
	 ;
}

/* find char c in string str; return p such that str[p]==c;
 * if c not in str then p=strlen(str)
 */
int
instring(str, c)
	char *str;
	char c;
{
    int pos = 0;

    while (str != NULL && *str != '\0' && c != *str) {
	   str++; 
	   pos++;
    }
    return (pos);
}

show_functions()
{
register struct udft_entry *udf = first_udf;

	fprintf(stderr,"\n\tUser-Defined Functions:\n");

	while (udf) {
		if (udf->definition)
			fprintf(stderr,"\t%s\n",udf->definition);
		else
			fprintf(stderr,"\t%s is undefined\n",udf->udf_name);
		udf = udf->next_udf;
	}
}


show_at()
{
	(void) putc('\n',stderr);
	disp_at(temp_at(),0);
}


disp_at(curr_at, level)
struct at_type *curr_at;
int level;
{
register int i, j;
register union argument *arg;

	for (i = 0; i < curr_at->a_count; i++) {
		(void) putc('\t',stderr);
		for (j = 0; j < level; j++)
			(void) putc(' ',stderr);	/* indent */

			/* print name of instruction */

		fputs(ft[(int)(curr_at->actions[i].index)].f_name,stderr);
		arg = &(curr_at->actions[i].arg);

			/* now print optional argument */

		switch(curr_at->actions[i].index) {
		  case PUSH:	fprintf(stderr," %s\n", arg->udv_arg->udv_name);
					break;
		  case PUSHC:	(void) putc(' ',stderr);
					disp_value(stderr,&(arg->v_arg));
					(void) putc('\n',stderr);
					break;
		  case PUSHD:	fprintf(stderr," %s dummy\n",
					  arg->udf_arg->udf_name);
					break;
		  case CALL:	fprintf(stderr," %s", arg->udf_arg->udf_name);
					if (arg->udf_arg->at) {
						(void) putc('\n',stderr);
						disp_at(arg->udf_arg->at,level+2); /* recurse! */
					} else
						fputs(" (undefined)\n",stderr);
					break;
		  case JUMP:
		  case JUMPZ:
		  case JUMPNZ:
		  case JTERN:
					fprintf(stderr," +%d\n",arg->j_arg);
					break;
		  default:
					(void) putc('\n',stderr);
		}
	}
}


/* alloc:
 * allocate memory 
 * This is a protected version of malloc. It causes an int_error 
 * if there is not enough memory, but first it tries FreeHelp() 
 * to make some room, and tries again. If message is NULL, we 
 * allow NULL return. Otherwise, we handle the error, using the
 * message to create the int_error string.
 */

char *
alloc(size, message)
	unsigned int size;				/* # of bytes */
	char *message;			/* description of what is being allocated */
{
    char *p;				/* the new allocation */
    char errbuf[100];		/* error message string */

    p = malloc(size);
    if (p == (char *)NULL) {
#ifndef VMS
	   FreeHelp();			/* out of memory, try to make some room */
#endif
	   
	   p = malloc(size);		/* try again */
	   if (p == (char *)NULL) {
		  /* really out of memory */
		  if (message != NULL) {
			 (void) sprintf(errbuf, "out of memory for %s", message);
			 int_error(errbuf, NO_CARET);
			 /* NOTREACHED */
		  }
		  /* else we return NULL */
	   }
    }

    return(p);
}
