
/*@******************************************************************
 *@ 
 *@  VBAR generates a vertical bar graph from a pair of vectors.
 *@
 *@  vbar(x, y, num, title, xtitle, ytitle, maxrows, maxcols, xdec, ydec)
 *@
 *@ where:
 *@		x 		- vector of x coordinates
 *@		y 		- vector of y coordinates
 *@		num 	- number of elements in both x and y
 *@		title	- graph title (printed at top of page)
 *@		xtitle	- title of x coordinate (printed at bottom-horizontal)
 *@		ytitle	- title of y coordinate (printed at left side-horizontally)
 *@		maxrows - maximum number of y lines to be printed
 *@		maxcols - maximum number of x lines to be printed
 *@		xdec	- number of decimal places in x values
 *@		ydec	- number of decimal places in y values
 *@
 *@******************************************************************** */


#define	YTICKW	16			/* y tick width */

int vbar(x, y, num, title, xtitle, ytitle, maxrows, maxcols, xdec, ydec)
int *x, *y, num, maxrows, maxcols, xdec, ydec;
char *title, *xtitle, *ytitle;
{
	char mytitle[80], pad[80], c[2], strbuf[80];
	char *r_just(), *itoa(), *c_just(), *p, *mak_dec();
	int i, j, xmax, xmin, ymax, ymin, xinc, yinc;
	int xbuck, xbuckv, ybuck, ybuckv, yval, xrange, yrange;
	int gutter, xwide, ywide, temp, k, lower, upper, r, l;

	gutter = 1;		/* dist between bars */
	maxcols -= YTICKW;	/* subtract y-tick width */
	xmax = xmin = x[0];   
	ymax = ymin = y[0];   

	for (i=0; i < num; i++) {	/* determine x and y data ranges */
		if (xmax < x[i])
			xmax = x[i];
		if (ymax < y[i])
			ymax = y[i];
		if (xmin > x[i])
			xmin = x[i];
		if (ymin > y[i])
			ymin = y[i];
	}

	/* compute the x and y bucket sizes */
	xrange = xmax - xmin;
	yrange = ymax - ymin;
	for (i=1; i < (maxcols - 2); i++) {
		xbuck = maxcols / (gutter + i);		/* i is size of one bucket */
		xinc = (xrange + xbuck - 1) / xbuck;
		if (xinc >= 1)			/* continue until you fill the avail space */
			break;
	}
	xwide = i;
	if (xinc < 1) {
		xbuck = maxcols / (gutter + 1);
		xinc = 1;
	}
	else
		xbuck = (xrange + xinc - 1) / xinc;		/* readjust buckets, if necessary */
	
	for (i=1; i < maxrows; i++) {
		ybuck = maxrows / i;
		yinc = (yrange + ybuck - 1) / ybuck;
		if (yinc >= 1)
			break;
	}
	ywide = i;
	if (yinc < 1) {
		ybuck = maxrows;
		yinc = 1;
	}
	else
		ybuck = (yrange + yinc - 1) / yinc;		/* readjust buckets, if necessary */
	

	/* compute pad on left of graph to center graph horizontally */
	temp = maxcols - xbuck * (xwide + gutter);
	for (i=0; i < temp;)
		pad[i++] = ' ';
	pad[i] = '\0';

	/* print the graph title at the top  */
	puts("\n\n\n");
	for (i=0; i < YTICKW; i++)
		puts(" ");
	puts(c_just(title, xbuck * (gutter + xwide)));
	puts("\n\n");

	/* print the y-axis title at the left of the page */
	puts("    ");
	puts(ytitle);
	puts("\n");

	/* draw the chart */
	c[1] = '\0';	/* set EOS */
	for (i=0; i < ybuck; i++) {	/* once per line (y buckets) */
		yval = ymax - (i * yinc);
		puts("\n  ");
		puts(r_just(mak_dec(itoa(yval, strbuf), ydec), 10));
		puts("  |");
		for (j=1; j <= xbuck; j++) {	/* once per column (x buckets) */
			temp = 0;
			lower = (j-1) * xinc;		/* compute proper x-range */
			upper = lower + xinc;
			r = 0;
			for (k=0; k < num; k++)		/* sum all y values in this x-range */
				if ((x[k] > lower) && (x[k] <= upper)) {
					temp += y[k];
					r++;		/* indicate that at least one was found */
				}
			if ((temp >= yval) && r) 	/* if in range and at least one hit */
				c[0] = '*';
			else
				c[0] = ' ';
			for (k=0; k < gutter; k++)
				puts(" ");
			for (k=0; k < xwide; k++)
				puts(c);
		}
	}
	puts("\n");

	/* print the x-scale */
	puts("              |\b__");	/* skip to start of Y axis */
	for (i=0; i <= xbuck; i++)
		for (k=0; k < (gutter + xwide); k++)
			puts("_");
	puts("\n");
	for (i=1; i < YTICKW; i++)	/* skip to start of Y axis */
		puts(" ");
	temp = xinc + xmin;
	while (temp <= xmax) 
		for (r=1; r > 0; r++, temp += xinc) {
			itoa(temp, strbuf);
			if (xdec)			/* suppress decimal point if zero decimal places */
				l = strlen(p = mak_dec(strbuf, xdec));	/* make value a string */
			else
				l = strlen(p = strbuf);	/* make value a string */
			i = r * (gutter + xwide);
/*			printf("r=%d, temp=%d, l=%d, i=%d\n", r, temp, l, i);  */
			if ((i / (l + 1)) >= 1) {	/* does it fit? */
				puts(r_just(p, i));		/* yes, output it */
				r = -1;					/* .. and quit trying */
			}
		}
		 
	puts("\n");
	for (i=0; i < YTICKW; i++)
		puts(" ");
	puts(c_just(xtitle, xbuck * (gutter + xwide)));
	puts("\n");
}

