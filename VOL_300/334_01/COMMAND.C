/* GNUPLOT - command.c */
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

#ifdef MSDOS
#include <process.h>

#ifdef __ZTC__
#define P_WAIT 0
#include <time.h>	/* usleep() */
#else

#ifdef __TURBOC__
#include <dos.h>	/* sleep() */
#include <conio.h>
extern unsigned _stklen = 8192;	/* increase stack size */

#else	/* must be MSC */
#include <time.h>	/* kludge to provide sleep() */
void sleep();		/* defined later */
#endif /* TURBOC */
#endif /* ZTC */

#endif /* MSDOS */


#include "plot.h"
#include "setshow.h"
#include "help.h"

#ifndef STDOUT
#define STDOUT 1
#endif

#ifndef HELPFILE
#define HELPFILE "docs/gnuplot.gih" /* changed by makefile */
#endif

#define inrange(z,min,max) ((min<max) ? ((z>=min)&&(z<=max)) : ((z>=max)&&(z<=min)) )

/*
 * instead of <strings.h>
 */

extern char *gets(),*getenv();
extern char *strcpy(),*strncpy(),*strcat();
extern int strlen(), strcmp();

#ifdef unix
#ifdef GETCWD
extern char *getcwd();	/* some Unix's use getcwd */
#else
extern char *getwd();	/* most Unix's use getwd */
#endif
#else
extern char *getcwd();	/* Turbo C, MSC and VMS use getcwd */
#endif
extern int chdir();

extern double magnitude(),angle(),real(),imag();
extern struct value *const_express(), *pop(), *complex();
extern struct at_type *temp_at(), *perm_at();
extern struct udft_entry *add_udf();
extern struct udvt_entry *add_udv();
extern void squash_spaces();
extern void lower_case();

extern BOOLEAN interactive;	/* from plot.c */

/* input data, parsing variables */
struct lexical_unit token[MAX_TOKENS];
char input_line[MAX_LINE_LEN+1] = "";
int num_tokens, c_token;
int inline_num = 0;			/* input line number */

char c_dummy_var[MAX_ID_LEN+1]; 		/* current dummy var */

/* the curves of the plot */
struct curve_points *first_plot = NULL;
static struct udft_entry plot_func;
struct udft_entry *dummy_func;

/* support for replot command */
char replot_line[MAX_LINE_LEN+1] = "";
static int plot_token;					/* start of 'plot' command */

com_line()
{
	read_line(PROMPT);

	/* So we can flag any new output: if false at time of error, */
	/* we reprint the command line before printing caret. */
	/* TRUE for interactive terminals, since the command line is typed. */
	/* FALSE for non-terminal stdin, so command line is printed anyway. */
	/* (DFK 11/89) */
	screen_ok = interactive; 

	do_line();
}


do_line()	  /* also used in load_file */
{
	if (is_system(input_line[0])) {
		do_system();
		(void) fputs("!\n",stderr);
		return;
	}
	num_tokens = scanner(input_line);
	c_token = 0;
	while(c_token < num_tokens) {
		command();
		if (c_token < num_tokens)	/* something after command */
			if (equals(c_token,";"))
				c_token++;
			else
					int_error("';' expected",c_token);
	}
}



command()
{
    char sv_file[MAX_LINE_LEN+1];
    /* string holding name of save or load file */

	c_dummy_var[0] = '\0';		/* no dummy variable */

	if (is_definition(c_token))
		define();
	else if (equals(c_token,"help") || equals(c_token,"?")) {
	    c_token++;
	    do_help();
	}
	else if (almost_equals(c_token,"test")) {
	    c_token++;
		test_term();
	}
	else if (almost_equals(c_token,"pa$use")) {
		struct value a;
		int stime, text=0;
		char buf[MAX_LINE_LEN+1];

		c_token++;
		stime = (int )real(const_express(&a));
		if (!(END_OF_COMMAND)) {
			if (!isstring(c_token))
				int_error("expecting string",c_token);
			else {
				quotel_str(buf,c_token);
				(void) fprintf (stderr, "%s",buf);
				text = 1;
			}
		}
		if (stime < 0) (void) fgets (buf,MAX_LINE_LEN,stdin);  
						/* Hold until CR hit. */
#ifdef __ZTC__
		if (stime > 0) usleep((unsigned long) stime);
#else
		if (stime > 0) sleep((unsigned int) stime);
#endif
		if (text != 0 && stime >= 0) (void) fprintf (stderr,"\n");
		c_token++;
		screen_ok = FALSE;
	}
	else if (almost_equals(c_token,"pr$int")) {
		struct value a;

		c_token++;
		(void) const_express(&a);
		(void) putc('\t',stderr);
		disp_value(stderr,&a);
		(void) putc('\n',stderr);
		screen_ok = FALSE;
	}
	else if (almost_equals(c_token,"p$lot")) {
		plot_token = c_token++;
		plotrequest();
	}
	else if (almost_equals(c_token,"rep$lot")) {
		if (replot_line[0] == '\0') 
			int_error("no previous plot",c_token);
		c_token++;
		replotrequest();
	}
	else if (almost_equals(c_token,"se$t"))
		set_command();
	else if (almost_equals(c_token,"sh$ow"))
		show_command();
	else if (almost_equals(c_token,"cl$ear")) {
		if (!term_init) {
			(*term_tbl[term].init)();
			term_init = TRUE;
		}
		(*term_tbl[term].graphics)();
		(*term_tbl[term].text)();
		(void) fflush(outfile);
		screen_ok = FALSE;
		c_token++;
	}
	else if (almost_equals(c_token,"she$ll")) {
		do_shell();
		screen_ok = FALSE;
		c_token++;
	}
	else if (almost_equals(c_token,"sa$ve")) {
		if (almost_equals(++c_token,"f$unctions")) {
			if (!isstring(++c_token))
				int_error("expecting filename",c_token);
			else {
				quote_str(sv_file,c_token);
				save_functions(fopen(sv_file,"w"));
			}
		}
		else if (almost_equals(c_token,"v$ariables")) {
			if (!isstring(++c_token))
				int_error("expecting filename",c_token);
			else {
				quote_str(sv_file,c_token);
				save_variables(fopen(sv_file,"w"));
			}
		}
		else if (almost_equals(c_token,"s$et")) {
			if (!isstring(++c_token))
				int_error("expecting filename",c_token);
			else {
				quote_str(sv_file,c_token);
				save_set(fopen(sv_file,"w"));
			}
		}
		else if (isstring(c_token)) {
			quote_str(sv_file,c_token);
			save_all(fopen(sv_file,"w"));
		}
		else {
			int_error(
		"filename or keyword 'functions', 'variables', or 'set' expected",
					c_token);
		}
		c_token++;
	}
	else if (almost_equals(c_token,"l$oad")) {
		if (!isstring(++c_token))
			int_error("expecting filename",c_token);
		else {
			quote_str(sv_file,c_token);
			load_file(fopen(sv_file,"r"), sv_file);	
		/* input_line[] and token[] now destroyed! */
			c_token = num_tokens = 0;
		}
	}
	else if (almost_equals(c_token,"cd")) {
		if (!isstring(++c_token))
			int_error("expecting directory name",c_token);
		else {
			quotel_str(sv_file,c_token);
			if (chdir(sv_file)) {
			  int_error("Can't change to this directory",c_token);
			}
		c_token++;
		}
	}
	else if (almost_equals(c_token,"pwd")) {
#ifdef unix
#ifdef GETCWD
	  (void) getcwd(sv_file,MAX_ID_LEN); /* some Unix's use getcwd */
#else
	  (void) getwd(sv_file); /* most Unix's use getwd */
#endif
#else
/* Turbo C and VMS have getcwd() */
	  (void) getcwd(sv_file,MAX_ID_LEN);
#endif
	  fprintf(stderr,"%s\n", sv_file);
	  c_token++;
	}
	else if (almost_equals(c_token,"ex$it") ||
			almost_equals(c_token,"q$uit")) {
		done(IO_SUCCESS);
	}
	else if (!equals(c_token,";")) {		/* null statement */
		int_error("invalid command",c_token);
	}
}

replotrequest()
{
char str[MAX_LINE_LEN+1];
		if(equals(c_token,"["))
			int_error("cannot set range with replot",c_token);
		if (!END_OF_COMMAND) {
			capture(str,c_token,num_tokens-1);
			if ( (strlen(str) + strlen(replot_line)) <= MAX_LINE_LEN-1) {
				(void) strcat(replot_line,",");
				(void) strcat(replot_line,str); 
			} else {
				int_error("plot line too long with replot arguments",c_token);
			}
		}
		(void) strcpy(input_line,replot_line);
		screen_ok = FALSE;
		num_tokens = scanner(input_line);
		c_token = 1;					/* skip the 'plot' part */
		plotrequest();
}


plotrequest()
/*
   In the parametric case we can say 
      plot [a= -4:4] [-2:2] [-1:1] sin(a),a**2
   while in the non-parametric case we would say only
      plot [b= -2:2] [-1:1] sin(b)
*/
{
     BOOLEAN changed;
	int dummy_token = -1;

    autoscale_lt = autoscale_t;
     autoscale_lx = autoscale_x;
     autoscale_ly = autoscale_y;
	
	if (!term)					/* unknown */
		int_error("use 'set term' to set terminal type first",c_token);

	if (equals(c_token,"[")) {
		c_token++;
		if (isletter(c_token)) {
		    if (equals(c_token+1,"=")) {
			   dummy_token = c_token;
			   c_token += 2;
		    } else {
			   /* oops; probably an expression with a variable. */
			   /* Parse it as an xmin expression. */
			   /* used to be: int_error("'=' expected",c_token); */
		    }
		}
		changed = parametric ? load_range(&tmin,&tmax):load_range(&xmin,&xmax);
		if (!equals(c_token,"]"))
			int_error("']' expected",c_token);
		c_token++;
		if (changed) {
			if (parametric)
				autoscale_lt = FALSE;
			else
				autoscale_lx = FALSE;
		}
	}

	if (parametric && equals(c_token,"[")) { /* set optional x ranges */
		c_token++;
		changed = load_range(&xmin,&xmax);
		if (!equals(c_token,"]"))
			int_error("']' expected",c_token);
		c_token++;
		if (changed)
		  autoscale_lx = FALSE;
	}

	if (equals(c_token,"[")) { /* set optional y ranges */
		c_token++;
		changed = load_range(&ymin,&ymax);
		if (!equals(c_token,"]"))
			int_error("']' expected",c_token);
		c_token++;
		if (changed)
		  autoscale_ly = FALSE;
	}

     /* use the default dummy variable unless changed */
	if (dummy_token >= 0)
	  copy_str(c_dummy_var,dummy_token);
	else
	  (void) strcpy(c_dummy_var,dummy_var);

	eval_plots();
}


define()
{
register int start_token;  /* the 1st token in the function definition */
register struct udvt_entry *udv;
register struct udft_entry *udf;

	if (equals(c_token+1,"(")) {
		/* function ! */
		start_token = c_token;
		copy_str(c_dummy_var, c_token + 2);
		c_token += 5; /* skip (, dummy, ) and = */
		if (END_OF_COMMAND)
			int_error("function definition expected",c_token);
		udf = dummy_func = add_udf(start_token);
		if (udf->at)				/* already a dynamic a.t. there */
			free((char *)udf->at);	/* so free it first */
		if ((udf->at = perm_at()) == (struct at_type *)NULL)
			int_error("not enough memory for function",start_token);
		m_capture(&(udf->definition),start_token,c_token-1);
	}
	else {
		/* variable ! */
		start_token = c_token;
		c_token +=2;
		udv = add_udv(start_token);
		(void) const_express(&(udv->udv_value));
		udv->udv_undef = FALSE;
	}
}


get_data(this_plot)
struct curve_points *this_plot;
{
static char data_file[MAX_LINE_LEN+1], line[MAX_LINE_LEN+1];
register int i, l_num, datum;
register FILE *fp;
float x, y;
int npoints;				/* number of points and breaks read */
float temp;
BOOLEAN yfirst;
char format[MAX_LINE_LEN+1];

	quote_str(data_file, c_token);
	this_plot->plot_type = DATA;
	if ((fp = fopen(data_file, "r")) == (FILE *)NULL)
		os_error("can't open data file", c_token);

	format[0] = '\0';
	yfirst = FALSE;
	c_token++;	/* skip data file name */
	if (almost_equals(c_token,"u$sing")) {
		c_token++;  	/* skip "using" */
		if (!isstring(c_token)) {
			if ( equals(c_token,"xy") || equals(c_token,"y") )
				yfirst = FALSE;
			else if (equals(c_token,"yx"))
				yfirst = TRUE;
			else 
				int_error("expecting xy, yx, y or format (scanf) string",c_token);
			c_token++;	/* skip "xy", "yx" or "y" */
		}
		if (isstring(c_token)) {
			quotel_str(format, c_token);
			c_token++;	/* skip format */
		}
		else
			int_error("expecting format (scanf) string",c_token);
	}
	if (strlen(format) == 0)
		strcpy(format,"%f %f");

	l_num = 0;
     datum = 0;
	i = 0;
     npoints = 0;
	while ( fgets(line, MAX_LINE_LEN, fp) != (char *)NULL ) {
		l_num++;
		if (is_comment(line[0]))
			continue;		/* ignore comments */
		npoints++;
		if (i >= samples)	/* should never be > samples */
		  continue;		/* Overflow! keep looping to get count */
		if (!line[1]) { /* is it blank line ? */
			/* break in data, make next point undefined */
			this_plot->points[i].type = UNDEFINED;
			i++;
			continue;
		}

		switch (sscanf(line, format, &x, &y)) {
		    case 1: {		/* only one number on the line */
			   y = x;		/* assign that number to y */
			   x = datum;	/* and make the index into x */
			   /* no break; !!! */
		    }
		    case 2: {
			   if (yfirst) { /* exchange x and y */
				  temp = y;
				  y = x;
				  x = temp;
			   }
			   datum++;
			   this_plot->points[i].type = INRANGE;

			   if (log_x) {
				  if (x < 0.0) {
					 this_plot->points[i].type = UNDEFINED;
				  } else if (x == 0.0) {
					 this_plot->points[i].type = OUTRANGE;
					 this_plot->points[i].x = -VERYLARGE;
				  } else
				    this_plot->points[i].x = log10(x);
			   } else
				this_plot->points[i].x = x;

			   if (log_y) {
				  if (y < 0.0) {
					 this_plot->points[i].type = UNDEFINED;
				  } else if (y == 0.0) {
					 this_plot->points[i].type = OUTRANGE;
					 this_plot->points[i].y = -VERYLARGE;
				  } else
				    this_plot->points[i].y = log10(y);
			   } else
				this_plot->points[i].y = y;

			   if (this_plot->points[i].type == INRANGE)
				if (   (autoscale_lx || inrange(x,xmin,xmax))
				    && (autoscale_ly || inrange(y,ymin,ymax) || polar)) {
				    if (autoscale_lx) {
					   if (x < xmin) xmin = x;
					   if (x > xmax) xmax = x;
				    }
				    if (autoscale_ly) {
					   if (y < ymin) ymin = y;
					   if (y > ymax) ymax = y;
				    }
				} else {
				    this_plot->points[i].type = OUTRANGE;
				}
			   i++;
			   break;
		    }
		    default: {
			   (void) sprintf(line, "bad data on line %d", l_num);
			   int_error(line,c_token);
		    }
		}
	}
	if (npoints > samples) {
		(void) sprintf(line,
 "%d data points and breaks found--samples must be set at least this high",
					npoints); 
		int_error(line,c_token);
	}
	this_plot->p_count = i;
     (void) fclose(fp);
}

/* This parses the plot command after any range specifications. 
 * To support autoscaling on the x axis, we want any data files to 
 * define the x range, then to plot any functions using that range. 
 * We thus parse the input twice, once to pick up the data files, 
 * and again to pick up the functions. Definitions are processed 
 * twice, but that won't hurt.
 */
eval_plots()
{
register int i;
register struct curve_points *this_plot, **tp_ptr;
register int start_token, end_token, mysamples;
register int begin_token;
double x_min, x_max, y_min, y_max;
register double x, xdiff, temp;
static struct value a;
BOOLEAN ltmp, some_data_files = FALSE;
int plot_num, line_num, point_num, xparam=0;
char *xtitle;
void parametric_fixup();

	mysamples = samples;

	if (autoscale_ly) {
		ymin = VERYLARGE;
		ymax = -VERYLARGE;
	} else if (log_y && (ymin <= 0.0 || ymax <= 0.0))
			int_error("y range must be above 0 for log scale!",
				NO_CARET);

	tp_ptr = &(first_plot);
	plot_num = 0;
	line_num = 0; 	/* default line type */
	point_num = 0;	/* default point type */

     begin_token = c_token;

/*** First Pass: Read through data files ***/
/* This pass serves to set the xrange and to parse the command, as well 
 * as filling in every thing except the function data. That is done after
 * the xrange is defined.
 */
	while (TRUE) {
		if (END_OF_COMMAND)
			int_error("function to plot expected",c_token);

		start_token = c_token;

		if (is_definition(c_token)) {
			define();
		} else {
			plot_num++;
			if (*tp_ptr)
				this_plot = *tp_ptr;
			else {		/* no memory malloc()'d there yet */
				this_plot = (struct curve_points *)
					alloc((unsigned int) (sizeof(struct curve_points) -
					(MAX_POINTS -(samples+1))*sizeof(struct coordinate)),
					"curve points");
				this_plot->next_cp = NULL;
				this_plot->title = NULL;
				*tp_ptr = this_plot;
			}

			if (isstring(c_token)) {			/* data file to plot */
				if (parametric && xparam) 
					int_error("previous parametric function not fully specified",
																	c_token);

				if (!some_data_files && autoscale_lx) {
				    xmin = VERYLARGE;
				    xmax = -VERYLARGE;
				}
				some_data_files = TRUE;

				this_plot->plot_type = DATA;
				this_plot->plot_style = data_style;
				end_token = c_token;
				get_data(this_plot); /* this also parses the using option */
			} 
			else {							/* function to plot */
				if (parametric)			/* working on x parametric function */
					xparam = 1 - xparam;
				this_plot->plot_type = FUNC;
				this_plot->plot_style = func_style;
				dummy_func = &plot_func;
				plot_func.at = temp_at();
				/* ignore it for now */
				end_token = c_token-1;
			}

			if (almost_equals(c_token,"t$itle")) {
				if (parametric) {
					if (xparam) 
						int_error(
		"\"title\" allowed only after parametric function fully specified",
																	c_token);
					else if (xtitle != NULL)
						xtitle[0] = '\0';  /* Remove default title .*/
				}
				c_token++;
				if ( (input_line[token[c_token].start_index]=='\'')
				   ||(input_line[token[c_token].start_index]=='"') ) {
					m_quote_capture(&(this_plot->title),c_token,c_token);
				}
				else {
					int_error("expecting \"title\" for plot",c_token);
				}
				c_token++;
			}
  			else {
  				m_capture(&(this_plot->title),start_token,end_token);
 				if (xparam) xtitle = this_plot->title;
  			}
  
  			this_plot->line_type = line_num;
			this_plot->point_type = point_num;

			if (almost_equals(c_token,"w$ith")) {
				if (parametric && xparam) 
					int_error("\"with\" allowed only after parametric function fully specified",
																	c_token);
			    this_plot->plot_style = get_style();
			}

			if ( !equals(c_token,",") && !END_OF_COMMAND ) {
				struct value t;
				this_plot->line_type = (int)real(const_express(&t))-1;
			}
			if ( !equals(c_token,",") && !END_OF_COMMAND ) {
				struct value t;
				this_plot->point_type = (int)real(const_express(&t))-1;
			}
			if ( (this_plot->plot_style == POINTS) ||
				 (this_plot->plot_style == LINESPOINTS) )
					if (!xparam) point_num++;
			if (!xparam) line_num++;

			tp_ptr = &(this_plot->next_cp);
		}

		if (equals(c_token,",")) 
			c_token++;
		else  
			break;
	}

	if (parametric && xparam) 
		int_error("parametric function not fully specified", NO_CARET);

	if (parametric) {
	/* Swap t and x ranges for duration of these eval_plot computations. */
		ltmp = autoscale_lx; autoscale_lx = autoscale_lt; autoscale_lt = ltmp;
		temp = xmin; xmin = tmin; tmin = temp;
		temp = xmax; xmax = tmax; tmax = temp;
	}

/*** Second Pass: Evaluate the functions ***/
/* Everything is defined now, except the function data. We expect
 * no syntax errors, etc, since the above parsed it all. This makes 
 * the code below simpler. If autoscale_ly, the yrange may still change.
 */
     if (xmin == xmax)
	  if (autoscale_lx) {
		 fprintf(stderr, "Warning: empty %c range [%g:%g], ", 
			parametric ? 't' : 'x', xmin,xmax);
		 if (xmin == 0.0) {
			/* completely arbitary */
			xmin = -1.;
			xmax = 1.;
		 } else {
			/* expand range by 10% in either direction */
			xmin = xmin * 0.9;
			xmax = xmax * 1.1;
		 }
		 fprintf(stderr, "adjusting to [%g:%g]\n", xmin,xmax);
	  } else {
		 int_error("x range is empty", c_token);
	  }

	/* give error if xrange badly set from missing datafile error */
	if (xmin == VERYLARGE || xmax == -VERYLARGE) {
		int_error("x range is invalid", c_token);
	}

    if (log_x) {
	   if (xmin <= 0.0 || xmax <= 0.0)
		int_error("x range must be greater than 0 for log scale!",NO_CARET);
	   x_min = log10(xmin);
	   x_max = log10(xmax);
    } else {
	   x_min = xmin;
	   x_max = xmax;
    }
    
    xdiff = (x_max - x_min) / mysamples;
    
    plot_num = 0;
    this_plot = first_plot;
    c_token = begin_token;	/* start over */

    /* Read through functions */
	while (TRUE) {
		if (is_definition(c_token)) {
			define();
		} else {
			plot_num++;
			if (isstring(c_token)) {			/* data file to plot */
			    /* ignore this now */
				c_token++;
				if (almost_equals(c_token,"u$sing")) {
					c_token++;  	/* skip "using" */
                    if (!isstring(c_token))
						c_token++;	/* skip "xy", "yx" or "y" */
					c_token++;		/* skip format string */
				}
			} 
			else {							/* function to plot */
				if (parametric)			/* working on x parametric function */
					xparam = 1 - xparam;
				dummy_func = &plot_func;
				plot_func.at = temp_at(); /* reparse function */
    
				for (i = 0; i <= mysamples; i++) {
				    if (i == samples+1)
					 int_error("number of points exceeded samples",
							 NO_CARET);
				    x = x_min + i*xdiff;
				    /* if (log_x) PEM fix logscale x axis */
				    /* x = pow(10.0,x); 26-Sep-89 */
				    (void) complex(&plot_func.dummy_value,
							    log_x ? pow(10.0,x) : x,
							    0.0);
				    
				    evaluate_at(plot_func.at,&a);
				    
				    if (undefined || (fabs(imag(&a)) > zero)) {
					   this_plot->points[i].type = UNDEFINED;
					   continue;
				    }
				    
				    temp = real(&a);

				    if (log_y && temp < 0.0) {
					   this_plot->points[i].type = UNDEFINED;
					   continue;
				    }

				    this_plot->points[i].x = x;
				    if (log_y) {
					   if (temp == 0.0) {
						  this_plot->points[i].type = OUTRANGE;
						  this_plot->points[i].y = -VERYLARGE;
						  continue;
					   } else {
						  this_plot->points[i].y = log10(temp);
					   }
				    } else
					 this_plot->points[i].y = temp;

				    if (autoscale_ly || polar
					   || inrange(temp, ymin, ymax)) {
					   this_plot->points[i].type = INRANGE;
					/* When xparam is 1 we are not really computing y's! */
						if (!xparam && autoscale_ly) {
					   	if (temp < ymin) ymin = temp;
					   	if (temp > ymax) ymax = temp;
						}
				    } else
					 this_plot->points[i].type = OUTRANGE;
				}
				this_plot->p_count = i; /* mysamples + 1 */
			 }
			
			/* title was handled above */
			if (almost_equals(c_token,"t$itle")) {
				c_token++;
				c_token++;
			}

			/* style was handled above */
			if (almost_equals(c_token,"w$ith")) {
			    c_token++;
			    c_token++;
			}

			/* line and point types were handled above */
			if ( !equals(c_token,",") && !END_OF_COMMAND ) {
				struct value t;
				(void)real(const_express(&t));
			}
			if ( !equals(c_token,",") && !END_OF_COMMAND ) {
				struct value t;
				(void)real(const_express(&t));
			}

			this_plot = this_plot->next_cp;
		 }
		
		if (equals(c_token,",")) 
		  c_token++;
		else  
		  break;
	 }

    if (ymin == ymax)
	 /* if autoscale, widen range */
	 if (autoscale_ly) {
		fprintf(stderr, "Warning: empty y range [%g:%g], ", ymin, ymax);
		if (ymin == 0.0) {
		    ymin = -1.;
		    ymax = 1.;
		} else {
		    /* expand range by 10% in either direction */
		    ymin = ymin * 0.9;
		    ymax = ymax * 1.1;
		}
		fprintf(stderr, "adjusting to [%g:%g]\n", ymin, ymax);
	 } else {
		int_error("y range is empty", c_token);
	 }

/* Now we finally know the real ymin and ymax */
	if (log_y) {
		y_min = log10(ymin);
		y_max = log10(ymax);
	} else {
		y_min = ymin;
		y_max = ymax;
	}
	capture(replot_line,plot_token,c_token);	

	if (parametric) {
	/* Now put t and x ranges back before we actually plot anything. */
		ltmp = autoscale_lx; autoscale_lx = autoscale_lt; autoscale_lt = ltmp;
		temp = xmin; xmin = tmin; tmin = temp;
		temp = xmax; xmax = tmax; tmax = temp;
		if (some_data_files && autoscale_lx) {
		/* 
			Stop any further autoscaling in this case (may be a mistake, have
  			to consider what is really wanted some day in the future--jdc). 
		*/
		    autoscale_lx = 0;
		}
	/* Now actually fix the plot pairs to be single plots. */
		parametric_fixup (first_plot, &plot_num, &x_min, &x_max);
	}
	do_plot(first_plot,plot_num,x_min,x_max,y_min,y_max);
}



done(status)
int status;
{
	if (term && term_init)
		(*term_tbl[term].reset)();
#ifdef VMS
	vms_reset();
#endif
	exit(status);
}

void parametric_fixup (start_plot, plot_num, x_min, x_max)
struct curve_points *start_plot;
int *plot_num;
double *x_min, *x_max;
/*
	The hardest part of this routine is collapsing the FUNC plot types
   in the list (which are gauranteed to occur in (x,y) pairs while 
	preserving the non-FUNC type plots intact.  This means we have to
	work our way through various lists.  Examples (hand checked):
		start_plot:F1->F2->NULL ==> F2->NULL
		start_plot:F1->F2->F3->F4->F5->F6->NULL ==> F2->F4->F6->NULL
		start_plot:F1->F2->D1->D2->F3->F4->D3->NULL ==> F2->D1->D2->F4->D3->NULL
	
	Of course, the more interesting work is to move the y values of
	the x function to become the x values of the y function (checking
	the mins and maxs as we go along).
*/
{
	struct curve_points *xp, *new_list, *yp = start_plot, *tmp, 
			*free_list, *free_head=NULL;
	int i, tlen, curve; 
	char *new_title;
	double lxmin, lxmax, temp;

	if (autoscale_lx) {
		lxmin = VERYLARGE;
		lxmax = -VERYLARGE;
	} else {
		lxmin = xmin;
		lxmax = xmax;
	}

/* 
	Ok, go through all the plots and move FUNC types together.  Note: this
	originally was written to look for a NULL next pointer, but gnuplot 
	wants to be sticky in grabbing memory and the right number of items
	in the plot list is controlled by the plot_num variable.

	Since gnuplot wants to do this sticky business, a free_list of 
	curve_points is kept and then tagged onto the end of the plot list as
	this seems more in the spirit of the original memory behavior than
	simply freeing the memory.  I'm personally not convinced this sort
	of concern is worth it since the time spent computing points seems
	to dominate any garbage collecting that might be saved here...
*/
	new_list = xp = start_plot; 
	yp = xp->next_cp;
   curve = 0;
	for (; curve < *plot_num; xp = xp->next_cp,yp = yp->next_cp,curve++) {
		if (xp->plot_type != FUNC) {
			continue;
		}
	/* Here's a FUNC parametric function defined as two parts. */
		--(*plot_num);
	/* 
		Go through all the points assigning the y's from xp to be the
		x's for yp.  Check max's and min's as you go.
	*/
		for (i = 0; i < yp->p_count; ++i) {
		/* 
			Throw away excess xp points, mark excess yp points as OUTRANGE.
		*/
			if (i > xp->p_count) {
				yp->points[i].type == OUTRANGE;
				continue;
			}
		/* 
			Just as we had to do when we computed y values--now check that
			x's (computed parametrically) are in the permitted ranges as well.
		*/
			temp = xp->points[i].y;   /* New x value for yp function. */
			yp->points[i].x = temp;
		/* For legitimate y values, let the x values decide if they plot. */
			if (yp->points[i].type == INRANGE)
				yp->points[i].type = xp->points[i].type;  
			if (autoscale_lx || polar
					   || inrange(temp, lxmin, lxmax)) {
			   if (autoscale_lx && temp < lxmin) lxmin = temp;
				if (autoscale_lx && temp > lxmax) lxmax = temp;
			} else
			yp->points[i].type = OUTRANGE;  /* Due to x value. */
		}
   /* Ok, fix up the title to include both the xp and yp plots. */
		if (xp->title && xp->title[0] != '\0') {
			tlen = strlen (yp->title) + strlen (xp->title) + 3;
      	new_title = alloc ((unsigned int) tlen, "string");
			strcpy (new_title, xp->title);  
			strcat (new_title, ", ");       /* + 2 */
			strcat (new_title, yp->title);  /* + 1 = + 3 */
			free (yp->title);
			yp->title = new_title;
		}
	/* Eliminate the first curve (xparam) and just use the second. */
		if (xp == start_plot) {
		/* Simply nip off the first element of the list. */
			new_list = first_plot = yp;
			xp = xp->next_cp;
			if (yp->next_cp != NULL)
				yp = yp->next_cp;
		/* Add start_plot to the free_list. */
			if (free_head == NULL) {
				free_list = free_head = start_plot;
				free_head->next_cp = NULL;
			} else {
				free_list->next_cp = start_plot;
				start_plot->next_cp = NULL;
				free_list = start_plot;
			}
		}
		else {
		/* Here, remove the xp node and replace it with the yp node. */
	   	tmp = xp;
		/* Pass over any data files that might have been in place. */
			while (new_list->next_cp && new_list->next_cp != xp) 
				new_list = new_list->next_cp;
			new_list->next_cp = yp;
			new_list = new_list->next_cp;
			xp = xp->next_cp;
			if (yp->next_cp != NULL)
				yp = yp->next_cp;
		/* Add tmp to the free_list. */
			if (free_head == NULL) {
				free_list = free_head = tmp;
			} else {
				free_list->next_cp = tmp;
				tmp->next_cp = NULL;
				free_list = tmp;
			}
		}
	}
/* Ok, stick the free list at the end of the curve_points plot list. */
	while (new_list->next_cp != NULL)
		new_list = new_list->next_cp;
	new_list->next_cp = free_head;

/* Report the overall graph mins and maxs. */
	*x_min = lxmin;
	*x_max = lxmax;
}


#ifdef MSDOS
#ifndef __TURBOC__	/* Turbo C already has sleep() */
#ifndef __ZTC__ 	/* ZTC already has usleep() */
/* kludge to provide sleep() for msc 5.1 */
void sleep(delay)
unsigned int delay;
{
unsigned long time_is_up;
	time_is_up = time(NULL) + (unsigned long) delay; 
	while (time(NULL)<time_is_up)
		/* wait */ ;
}
#endif /* not ZTC */
#endif /* not TURBOC */
#endif /* MSDOS */


/* Support for input, shell, and help for various systems */

#ifdef vms

#include <descrip.h>
#include <rmsdef.h>
#include <errno.h>

extern lib$get_input(), lib$put_output();

int vms_len;

unsigned int status[2] = {1, 0};

static char help[MAX_LINE_LEN+1] = "gnuplot";

$DESCRIPTOR(prompt_desc,PROMPT);
$DESCRIPTOR(line_desc,input_line);

$DESCRIPTOR(help_desc,help);
$DESCRIPTOR(helpfile_desc,"GNUPLOT$HELP");


read_line(prompt)
char *prompt;
{
    int more, start=0;
    char expand_prompt[40];

    prompt_desc.dsc$w_length = strlen (prompt);
    prompt_desc.dsc$a_pointer = prompt;
    (void) strcpy (expand_prompt, "_");
    (void) strncat (expand_prompt, prompt, 38);
    do {
        line_desc.dsc$w_length = MAX_LINE_LEN - start;
        line_desc.dsc$a_pointer = &input_line[start];
        switch(status[1] = lib$get_input(&line_desc, &prompt_desc, &vms_len)){
		  case RMS$_EOF:
		  done(IO_SUCCESS);	/* ^Z isn't really an error */
		  break;
		  case RMS$_TNS:	/* didn't press return in time *
						   /
						   vms_len--; /* skip the last character */
		  break;			/* and parse anyway */
		  case RMS$_BES:	/* Bad Escape Sequence */
		  case RMS$_PES:	/* Partial Escape Sequence */
		  sys$putmsg(status);
		  vms_len = 0;		/* ignore the line */
		  break;
		  case SS$_NORMAL:
		  break;			/* everything's fine */
		  default:
		  done(status[1]);	/* give the error message */
        }
        start += vms_len;
        input_line[start] = '\0';
	   inline_num++;
        if (input_line[start-1] == '\\') {
		  /* Allow for a continuation line. */
		  prompt_desc.dsc$w_length = strlen (expand_prompt);
		  prompt_desc.dsc$a_pointer = expand_prompt;
		  more = 1;
		  --start;
        }
        else {
		  line_desc.dsc$w_length = strlen(input_line);
		  line_desc.dsc$a_pointer = input_line;
		  more = 0;
        }
    } while (more);
}


do_help()
{
	help_desc.dsc$w_length = strlen(help);
	if ((vaxc$errno = lbr$output_help(lib$put_output,0,&help_desc,
		&helpfile_desc,0,lib$get_input)) != SS$_NORMAL)
			os_error("can't open GNUPLOT$HELP");
}


do_shell()
{
	if ((vaxc$errno = lib$spawn()) != SS$_NORMAL) {
		os_error("spawn error",NO_CARET);
	}
}


do_system()
{
	input_line[0] = ' ';	/* an embarrassment, but... */

	if ((vaxc$errno = lib$spawn(&line_desc)) != SS$_NORMAL)
		os_error("spawn error",NO_CARET);

	(void) putc('\n',stderr);
}

#else /* vms */

/* do_help: (not VMS, although it would work)
 * Give help to the user. 
 * It parses the command line into helpbuf and supplies help for that 
 * string. Then, if there are subtopics available for that key,
 * it prompts the user with this string. If more input is
 * given, do_help is called recursively, with the argument the index of 
 * null character in the string. Thus a more specific help can be 
 * supplied. This can be done repeatedly. 
 * If null input is given, the function returns, effecting a
 * backward climb up the tree.
 * David Kotz (dfk@cs.duke.edu) 10/89
 */
do_help()
{
    static char helpbuf[MAX_LINE_LEN] = "";
    static char prompt[MAX_LINE_LEN] = "";
    int base;				/* index of first char AFTER help string */
    int len;				/* length of current help string */
    BOOLEAN more_help;
    BOOLEAN only;			/* TRUE if only printing subtopics */
    int subtopics;			/* 0 if no subtopics for this topic */
    int start;				/* starting token of help string */
	char *help_ptr;			/* name of help file */

	if ( (help_ptr = getenv("GNUHELP")) == (char *)NULL )
		/* if can't find environment variable then just use HELPFILE */
		help_ptr = HELPFILE;

    len = base = strlen(helpbuf);

    /* find the end of the help command */
    for (start = c_token; !(END_OF_COMMAND); c_token++)
	 ;
    /* copy new help input into helpbuf */
    if (len > 0)
	 helpbuf[len++] = ' ';	/* add a space */
    capture(helpbuf+len, start, c_token-1);
    squash_spaces(helpbuf+base); /* only bother with new stuff */
    lower_case(helpbuf+base); /* only bother with new stuff */
    len = strlen(helpbuf);

    /* now, a lone ? will print subtopics only */
    if (strcmp(helpbuf + (base ? base+1 : 0), "?") == 0) {
	   /* subtopics only */
	   subtopics = 1;
	   only = TRUE;
	   helpbuf[base] = '\0';	/* cut off question mark */
    } else {
	   /* normal help request */
	   subtopics = 0;
	   only = FALSE;
    }

    switch (help(helpbuf, help_ptr, &subtopics)) {
	   case H_FOUND: {
		  /* already printed the help info */
		  /* subtopics now is true if there were any subtopics */
		  screen_ok = FALSE;
    
		  do {
			 if (subtopics && !only) {
				/* prompt for subtopic with current help string */
				if (len > 0)
				  (void) sprintf(prompt, "Subtopic of %s: ", helpbuf);
				else
				  (void) strcpy(prompt, "Help topic: ");
				read_line(prompt);
				num_tokens = scanner(input_line);
				c_token = 0;
				more_help = !(END_OF_COMMAND);
				if (more_help)
				  /* base for next level is all of current helpbuf */
				  do_help();
			 } else 
			   more_help = FALSE;
		  } while(more_help);
    
		  break;
	   }
	   case H_NOTFOUND: {
		  printf("Sorry, no help for '%s'\n", helpbuf);
		  break;
	   }
	   case H_ERROR: {
		  perror(help_ptr);
		  break;
	   }
	   default: {		/* defensive programming */
		  int_error("Impossible case in switch\n", NO_CARET);
		  /* NOTREACHED */
	   }
    }
    
    helpbuf[base] = '\0';	/* cut it off where we started */
}

do_system()
{
	if (system(input_line + 1))
		os_error("system() failed",NO_CARET);
}

#ifdef MSDOS

read_line(prompt)
	char *prompt;
{
    register int i;
    int start = 0, ilen = 0;
    BOOLEAN more;
    int last;
    
#ifndef __ZTC__
	if (interactive) { /* if interactive use console IO so CED will work */
		cputs(prompt);
		do {
		   ilen = MAX_LINE_LEN-start-1;
		   input_line[start] = ilen > 126 ? 126 : ilen;
		   (void) cgets(&(input_line[start]));
		   (void) putc('\n',stderr);
		   if (input_line[start+2] == 26) {
			  /* end-of-file */
			  (void) putc('\n',stderr);
			  input_line[start] = '\0';
			  inline_num++;
			  if (start > 0)	/* don't quit yet - process what we have */
				more = FALSE;
			  else {
				 (void) putc('\n',stderr);
				 done(IO_SUCCESS);
				 /* NOTREACHED */
			  }
		   } else {
			  /* normal line input */
			  register i = start;
			  while ( (input_line[i] = input_line[i+2]) != (char)NULL )
				i++;		/* yuck!  move everything down two characters */

			  inline_num++;
			  last = strlen(input_line) - 1;
			  if (last + 1 >= MAX_LINE_LEN)
				int_error("Input line too long",NO_CARET);
					 
			  if (input_line[last] == '\\') { /* line continuation */
				 start = last;
				 more = TRUE;
			  } else
				more = FALSE;
		   }
		   if (more)
			cputs("> ");
		} while(more);
	}
	else { /* not interactive */
#endif /* not ZTC */
		if (interactive)
		 fputs(prompt,stderr);
		do {
		   /* grab some input */
		   if ( fgets(&(input_line[start]), MAX_LINE_LEN - start, stdin) 
					== (char *)NULL ) {
			  /* end-of-file */
			  if (interactive)
				(void) putc('\n',stderr);
			  input_line[start] = '\0';
			  inline_num++;
			  if (start > 0)	/* don't quit yet - process what we have */
				more = FALSE;
			  else
				done(IO_SUCCESS); /* no return */
		   } else {
			  /* normal line input */
			  last = strlen(input_line) - 1;
			  if (input_line[last] == '\n') { /* remove any newline */
				 input_line[last] = '\0';
				 /* Watch out that we don't backup beyond 0 (1-1-1) */
				 if (last > 0) --last;
				 inline_num++;
			  } else if (last+1 >= MAX_LINE_LEN)
				int_error("Input line too long",NO_CARET);
					 
			  if (input_line[last] == '\\') { /* line continuation */
				 start = last;
				 more = TRUE;
			  } else
				more = FALSE;
		   }
			if (more && interactive)
			fputs("> ", stderr);
		} while(more);
#ifndef __ZTC
	}
#endif
}


do_shell()
{
register char *comspec;
	if ((comspec = getenv("COMSPEC")) == (char *)NULL)
		comspec = "\command.com";
	if (spawnl(P_WAIT,comspec,NULL) == -1)
		os_error("unable to spawn shell",NO_CARET);
}

#else /* MSDOS */
		/* plain old Unix */

read_line(prompt)
	char *prompt;
{
    int start = 0;
    BOOLEAN more;
    int last = 0;

    if (interactive)
	 fputs(prompt,stderr);
    do {
	   /* grab some input */
	   if ( fgets(&(input_line[start]), MAX_LINE_LEN - start, stdin) 
				== (char *)NULL ) {
		  /* end-of-file */
		  if (interactive)
		    (void) putc('\n',stderr);
		  input_line[start] = '\0';
		  inline_num++;
		  if (start > 0)	/* don't quit yet - process what we have */
		    more = FALSE;
		  else
		    done(IO_SUCCESS); /* no return */
	   } else {
		  /* normal line input */
		  last = strlen(input_line) - 1;
		  if (input_line[last] == '\n') { /* remove any newline */
			 input_line[last] = '\0';
                /* Watch out that we don't backup beyond 0 (1-1-1) */
			 if (last > 0) --last;
			 inline_num++;
		  } else if (last+1 >= MAX_LINE_LEN)
		    int_error("Input line too long",NO_CARET);
				 
		  if (input_line[last] == '\\') { /* line continuation */
			 start = last;
			 more = TRUE;
		  } else
		    more = FALSE;
	   }
        if (more && interactive)
		fputs("> ", stderr);
    } while(more);
}

#ifdef VFORK

do_shell()
{
register char *shell;
register int p;
static int execstat;
	if (!(shell = getenv("SHELL")))
		shell = SHELL;
	if ((p = vfork()) == 0) {
		execstat = execl(shell,shell,NULL);
		_exit(1);
	} else if (p == -1)
		os_error("vfork failed",c_token);
	else
		while (wait(NULL) != p)
			;
	if (execstat == -1)
		os_error("shell exec failed",c_token);
	(void) putc('\n',stderr);
}
#else /* VFORK */

#define EXEC "exec "
do_shell()
{
static char exec[100] = EXEC;
register char *shell;
	if (!(shell = getenv("SHELL")))
		shell = SHELL;

	if (system(strncpy(&exec[sizeof(EXEC)-1],shell,
		sizeof(exec)-sizeof(EXEC)-1)))
		os_error("system() failed",NO_CARET);

	(void) putc('\n',stderr);
}
#endif /* VFORK */
#endif /* MSDOS */
#endif /* vms */
