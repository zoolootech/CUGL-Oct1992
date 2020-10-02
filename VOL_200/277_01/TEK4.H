/*
 *	tek4.h - hp7470a version
 *	copyright 1987 Ronald Florence
 */

#define Maxpts		1000

#define Points		0
#define Solid		1
#define Longdash	2
#define Dotted		3
#define Dotdash		4
#define Shortdash	5
#define Altcolor	2

extern	FILE	*plw;		/* plotter write file */
extern	int	quit();		/* baleout function */
extern	double	xm,		/* scalars */
		ym,
		xscale,
		yscale;


char	*linetype[] = {
		"LT0",		/* Points */
		"LT",		/* Solid */
		"LT3",		/* Longdash */
		"LT1,1",	/* Dotted */
		"LT4",		/* Dotdash */
		"LT2"		/* Shortdash */
	};


#define Clear_scr()
#define Align(H, V)
#define Set_line(L)	fprintf(plw, "%s", linetype[L])
#define Increment(X,Y)	fprintf(plw, "PU%d,%d", X, Y)
#define Wr_str(X, Y, p)	fprintf(plw, "LB%s\03", p)
#define Switch_pen()	fprintf(plw, "SP%d", Altcolor)

#define Set_charsize(s)	cellw = charwd[s] * xm * (3 / 2);		\
			fprintf(plw, "SI%.3f,%.3f", 			\
				charwd[s] * xm /400, charht[s] * ym /400)

#define	Vector(N, PTS)	plot(N, PTS)

#define Marker(N, PTS)	{			\
			Set_line (Points);	\
			plot (N, PTS);		\
			Set_line (Solid);	\
			}

#define err(a)		if (!strcmp(a, "data"))			 	     \
			fprintf(stderr, "hp: too many data points\n"), quit()
		

plot (num, points)
     int  num, points[];
{
  register	i;
	
  fprintf(plw, "PU%d,%d", points[0], points[1]);
  if (num > 2) 
    {	
      fprintf(plw, "PD");
      for (i = 2; i < num - 1; i++) 
	fprintf(plw, "%d,", points[i]);
      fprintf(plw, "%d", points[i]);
    }
}
