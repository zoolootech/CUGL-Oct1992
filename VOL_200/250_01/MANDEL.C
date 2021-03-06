/*
CUG-HEADER:       ;
   TITLE:         Mandelbrot set plotter;
   DATE:          86-SEP-03;
   DESCRIPTION:   "Plots fractals, using the Mandelbrot set (or parts of it),
                   on an Epson MX-80 matrix printer";
   KEYWORDS:      Graphics, bit image printing;
   FILENAME:      mandel.c;
   WARNINGS:      "Change the printer control sequences if your printer is
                   not Epson like. Be prepared to do without your computer
                   for hours if not days, if it has no hardware floating point
                   processor."
   AUTHORS:       E.H. Ramm (converted to C from VMS-FORTRAN);
   O.S.:          CP/M-68K;
   COMPILER(S):   DRI (Alcyon) C;

REFERENCES:
      AUTHOR(S):  Benoit Mandelbrot;
      TITLE:      "Fractals: Form, Chance, & Dimension";
      CITATION:   "";
ENDREF
*/

/* ------------------------------------------------------------------------- */

#include <stdio.h>

#define void     VOID
#define LF       '\012'
#define CR       '\015'
#define FF       '\014'
#define ESC      '\033'
#define MAXLINES 67                           /* 67 * 8/72" + 3 lines header */


/* ------------------------------------------------------------------------- */

extern char     *gets();
extern int      atoi(),
                odd();
extern double   atof();
FILE            *fopenb(),
                *outfile;

/* ------------------------------------------------------------------------- */

main(argc, argv)
int   argc;
char  *argv[];
{
   register int   k,
                  l,
                  ix,
                  iy;

   int      kmax,
            xdots,
            ydots,
            n8;

   double   xmax,
            xmin,
            xq,
            xc,
            x,
            ymax,
            ymin,
            yq,
            yc,
            y,
            rqlim,
            rq,
            dx,
            dy;

   static char  buf[961],
                conbuf[129];


   if (argc != 2)
   {
      fprintf(stderr, "%s", "usage: mandel <picture file>");
      exit(1);
   }

   if ( !(outfile = fopenb(argv[1], "w")) )
   {
      fprintf(stderr, "%s%s", "unable to open ", argv[1]);
      exit(1);
   }
   fputc(FF, stdout);                                        /* clear screen */
   fprintf(stdout, "%s", "Xmax: ");
   xmax = atof(gets(conbuf));
   fprintf(stdout, "%s", "Xmin: ");
   xmin = atof(gets(conbuf));
   fprintf(stdout, "%s", "Ymax: ");
   ymax = atof(gets(conbuf));
   fprintf(stdout, "%s", "Ymin: ");
   ymin = atof(gets(conbuf));
   fprintf(stdout, "%s", "Limit: ");
   rqlim = atof(gets(conbuf));
   fprintf(stdout, "%s", "max. iterations: ");
   kmax = atoi(gets(conbuf));

   xdots = 880;                                 /* # of horizontal dots/line */
   n8 = xdots / 8;
   xdots = n8 * 8;                    /* make sure xdots are a multiple of 8 */
   ydots = MAXLINES * 8;
   n8 = MAXLINES;

   dx = (xmax - xmin) / (double)xdots;
   dy = (ymax - ymin) / (double)ydots;
   yc = ymin;

   fprintf(outfile, "%c%c", ESC, '@');                      /* reset printer */
   fprintf(outfile, "Mandelbrot Set\r\n");
   fprintf(outfile, "%s%9.5f%s%9.5f%s", "X:", xmin, " to", xmax, ", ");
   fprintf(outfile, "%s%9.5f%s%9.5f%s", "Y:", ymin, " to", ymax, ", ");
   fprintf(outfile, "%s%5.1f%s", "Limit:", rqlim, ", ");
   fprintf(outfile, "%s%3d\r\n\n", "itmax:", kmax);
   fprintf(outfile, "%c%c%c", ESC, 'A', '\010');          /* 8/72" line feed */
   fprintf(outfile, "%c%c%c", ESC, 'U', '\001');     /* unidirectional print */

   for (iy = 1; iy <= n8; iy++)
   {
      clrbuf(buf, xdots);
      fprintf(stdout, "\r%c%s%3d%s%3d", CR, "Line", iy, " of", MAXLINES);

      for (l = 7; l >= 0; l--)
      {
         xc = xmin;
         for (ix = 1; ix <= xdots; ix++)
         {
            x = xc;
            y = yc;
            k = 0;

            do
            {
               yq = y * y;              /* z**2 = (x+jy)**2 = x**2-y**2+2jxy */
               xq = x * x;
               y = x * (y + y) + yc;
               x = xc + xq - yq;        /* (x,y) <-- (xc,yc)+(x**2-y**2,2xy) */
               rq = xq + yq;
               k++;
            }
            while ( (rq < rqlim) && (k < kmax) );

            if ( (rq >= rqlim) && (odd(k)) )
               bitset(buf + ix, l);
            xc += dx;
         }
         yc += dy;
      }

      fputc(ESC, outfile);
      fputc('L', outfile);          /* set bit image mode, max 960 dots/line */
      fputc(xdots % 256, outfile);                     /* # of dots, lo byte */
      fputc(xdots / 256, outfile);                     /* # of dots, hi byte */
      for (ix = 1; ix <= xdots; ix++)
         fputc(buf[ix], outfile);
      fputc(CR, outfile);
      fputc(LF, outfile);
   }
   fputc(FF, outfile);
   fclose(outfile);
}

/* ------------------------------------------------------------------------- */

void clrbuf(bufr, length)
char   *bufr;
int    length;
{
   while (length)
      *(bufr + length--) = 0;
}

/* ------------------------------------------------------------------------- */
                          