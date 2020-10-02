/****************************************************************************
* EXPGEN.C - Generates explosion data files for EXPLOD.C
*
* This program should be compilable with most C compilers.
* To compile:
*       Datalight C:   dlc expgen.c
*       Turbo C:       tcc expgen.c
*
* The contents of this file is in the public domain.
*
* Change Log
* 90/01/20 Dennis Lo         Verson 1.0 
****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include "explod.h"

/*
 * File format:
 *  <num_points>
 *  <num_frames>
 *  <gravity>
 *  <wind>
 *  <trail length>
 *  <x> <y>
 *  <x> <y>
 *  <x> <y>
 *   ...
 */

/* explosion parameters */
int Num_points = 250;
int Num_frames = 50;
int Xsize = 200;
int Ysize = 120;
int Par1 = -1;
int Par2 = -1;
int Par3 = -1;
int Gravity = 9;
int Wind = 0;
int Trail_length = 5;
char Shape = '\0';

/*=====================================================================
 * main
 *=====================================================================
 */
main(argc, argv)
    int argc;
    char **argv;
{
    GetArgs(argc, argv);
    srand (Par1 + Par2 + Xsize + Ysize + Num_points + Num_frames 
	    + Gravity + Wind + Trail_length);
    switch (Shape)
    {
	case 'r':
	    PrintHeader();
	    if (Par1 == -1) Par1 = 0;  /* default no clumping */
	    if (Par2 == -1) Par2 = 8;  /* default clump size */
	    if (Par3 == -1) Par3 = 16; /* default clump spread size */
	    OvalGen (Num_points, Xsize, Ysize, Par1, Par2, Par3);
	    break;
	case 'x':
	    PrintHeader();
	    DStarGen (Num_points, Xsize, Ysize, Par2);
	    break;
	case '+':
	    PrintHeader();
	    HStarGen (Num_points, Xsize, Ysize, Par1);
	    break;
	case 't':
	    PrintHeader();
	    TriangGen (Num_points, Xsize, Ysize, Par1, Par2);
	    break;
	case 's':
	    PrintHeader();
	    TStarGen (Num_points, Xsize, Ysize, Par1, Par2);
	    break;
	default:
	    printf ("No valid shape specified\n", Shape);
	    Instructions();
	    exit (1);
    }
}

/*=====================================================================
 * Output the file header
 *=====================================================================
 */
PrintHeader ()
{
    printf ("# EXPLOD DATA FILE\n");
    printf ("%3d  # number of points\n", Num_points);
    printf ("%3d  # number of frames\n", Num_frames);
    printf ("%3d  # gravity\n", Gravity);
    printf ("%3d  # wind\n", Wind);
    printf ("%3d  # trail length\n", Trail_length);
}

/*=====================================================================
 * Process command line arguments.
 *=====================================================================
 */
GetArgs (argc, argv)
    int argc;
    char *argv[];
{
    /*
     * Loop to parse each command line parameter
     */
    while (--argc > 0) 
    {
        if (**++argv == '-') 
        {
            switch ((*argv)[1])
            {
		case 'a':		/* -a: Par 1 */
		    Par1 = atoi ((*argv) + 2);
		    break;
		case 'b':		/* -b: Par 2 */
		    Par2 = atoi ((*argv) + 2);
		    break;
		case 'c':		/* -c: Par 3 */
		    Par3 = atoi ((*argv) + 2);
		    break;
                case 'g':               /* -g: gravity (vert accel) */
                    Gravity = atoi ((*argv) + 2);
                    break;
                case 'w':               /* -w: wind (horiz accel) */
                    Wind = atoi ((*argv) + 2);
                    break;
                case 'x':               /* -x: explosion X size */
                    Xsize = atoi ((*argv) + 2);
                    break;
                case 'y':               /* -y: explosion Y size */
		    Ysize = atoi ((*argv) + 2);
                    break;
                case 'p':               /* -p: # of explosion points */
                    Num_points = atoi ((*argv) + 2);
		    if (Num_points > MAX_POINTS)
		    {
			printf ("Maximum # of points is %d\n", MAX_POINTS);
			exit (1);
		    }
                    break;
                case 'f':               /* -f: # of frames */
                    Num_frames = atoi ((*argv) + 2);
		    if (Num_frames > MAX_FRAMES)
		    {
			printf ("Maximum # of frames is %d\n", MAX_FRAMES);
			exit (1);
		    }
                    break;
                case 't':               /* -t: trail length */
                    Trail_length = atoi ((*argv) + 2);
                    break;
                case 's':               /* -s[rsd]: shape */
                    Shape = (*argv)[2];
                    break;
                default:
                    printf ("*** Invalid option: %s\n", *argv);
                    Instructions();
                    exit (1);
            }
        }
    }
}

/*=====================================================================
 * Print instructions
 *=====================================================================
 */
Instructions ()
{
    puts ("Usage: expgen [parameters] >output_file");
    puts ("Parameters can be one of");
    puts ("  -sC   :C = Explosion shape: r (round), + (star1), x (star2),");
    puts ("             t (triangle), or s (star transformation). No default.");
    puts ("  -xN   :N = Explosion X size.  Default: 200");
    puts ("  -yN   :N = Explosion Y size.  Default: 120");
    puts ("  -pN   :N = No. of points (max 300) Default: 250");
    puts ("  -fN   :N = No. of frames (max 60) Default: 50");
    puts ("  -tN   :N = Trail length.      Default: 5");
    puts ("  -gN   :N = Gravity.           Default: 9");
    puts ("  -wN   :N = Wind.              Default: 0");
    puts ("  -aN   :N = Extra parameter 1.  Affects each shape differently.");
    puts ("  -bN   :N = Extra parameter 2.  Affects each shape differently.");
    puts ("  -cN   :N = Extra parameter 3.  Affects each shape differently.");
}

/*====================================================================
 * Return a random number between 1..maxval
 *====================================================================
 */
int
rnd (maxval)
int maxval;
{
#   define MAX_RAND 32767       /* max value returned by rand() */
    long l;

    l = (long) maxval * rand() / MAX_RAND;
    return ((int) l);
}

/*====================================================================
 * Calculate/write the src & dest points for an oval explosion.
 *====================================================================
 */
OvalGen (num_points, xsize, ysize, par1, par2, par3)
    int num_points;
    int xsize, ysize;
    int par1;  /* 1 = clump */
    int par2;  /* clump size */
    int par3;  /* clump spread range */
{
    int dest_x, dest_y;
    int src_x, src_y;
    long x2 = (long) xsize * xsize;
    long y2 = (long) ysize * ysize;
    long x2y2 = (long) xsize * xsize * ysize * ysize;
    int i = 0;
    int j;
    int par3div2 = par3 >> 1;
    int par3div4 = par3 >> 2;

    src_x = 0;
    src_y = 0;
    for (i=0; i<num_points; i++) 
    {
        /* 
         * Find a dest coordinates that is inside the ellipse with
         * X radius of xsize and Y radius of ysize. */
        while (1)
        {
            dest_x = rnd (2*xsize) - xsize;
            dest_y = rnd (2*ysize) - ysize;

	    /* Accept the point if it is within the oval */
	    if (y2 * dest_x * dest_x  +  x2 * dest_y * dest_y  <  x2y2)
		break;
        }
	printf ("%d %d  %d %d\n", src_x, src_y, dest_x, dest_y);

 	/* If -a1 then do clumping */
	if (par1 == 1)
	{
	    for (j=0; j<par2; j++)
		printf ("%d %d  %d %d\n", src_x, src_y, 
			dest_x + rnd(par3) - par3div2, 
			dest_y + rnd(par3div2) - par3div4);
	    i += par2;
	}
    }
}

/*====================================================================
 * Calculate/write the src & dest points for a diagonal star explosion.
 *====================================================================
 */
DStarGen (num_points, xsize, ysize, par1)
    int num_points;
    int xsize, ysize;
    int par1;
{
    int dest_x, dest_y;
    int src_x, src_y;
    int i = 0;
    long x2 = (long) xsize * xsize;
    long y2 = (long) ysize * ysize;
    long focus1;

    if (par1 == -1)
        focus1 = x2 * y2 / 6;
    else
        focus1 = x2 * y2 / par1;

    for (i=0; i<num_points; i++) 
    {
        /* 
         * Calculate src and dest coordinates.
         * Select dest coordinate that is inside the hyperbolas with
         * focii of xsize and ysize.
         */
        src_x = 0;
        src_y = 0;
        while (1)
        {
            dest_x = rnd (2*xsize) - xsize;
            dest_y = rnd (2*ysize) - ysize;
	    if ((y2 * dest_x * dest_x  -  x2 * dest_y * dest_y  <  focus1)
	    &&  (x2 * dest_y * dest_y  -  y2 * dest_x * dest_x  <  focus1))
		break;
        }
	printf ("%d %d  %d %d\n", src_x, src_y, dest_x, dest_y);
    }
}

/*====================================================================
 * Calculate & write the points for a horiz star shaped explosion.
 *====================================================================
 */
HStarGen (num_points, xsize, ysize, par1)
    int num_points;
    int xsize, ysize;
    int par1;
{
    int dest_x, dest_y;
    int src_x, src_y;
    int i = 0;
    long a;

    if (par1 == -1)
        a = xsize * ysize / 10;
    else
        a = xsize * ysize / par1;

    for (i=0; i<num_points; i++) 
    {
        /* 
         * Calculate src and dest coordinates.
         * Select dest coordinate that is inside the hyperbolas with
         * focii of xsize and ysize.
         */
        src_x = 0;
        src_y = 0;
        while (1)
        {
            dest_x = rnd (2*xsize) - xsize;
            dest_y = rnd (2*ysize) - ysize;
	    if (dest_x > 0  &&  dest_y > 0  &&
		dest_x * dest_y < a)
		break;
	    if (dest_x > 0  &&  dest_y < 0  &&
		dest_x * dest_y > -a)
		break;
	    if (dest_x < 0  &&  dest_y > 0  &&
		dest_x * dest_y > -a)
		break;
	    if (dest_x < 0  &&  dest_y < 0  &&
		dest_x * dest_y < a)
		break;
	}
	printf ("%d %d  %d %d\n", src_x, src_y, dest_x, dest_y);
    }
}

/*====================================================================
 * Calculate & write the points for a star transformation from a 
 * diagonal star to a '+' shaped star.
 *====================================================================
 */
TStarGen (num_points, xsize, ysize, par1, par2)
    int num_points;
    int xsize, ysize;
    int par1;
    int par2;
{
    int dest_x, dest_y;
    int src_x, src_y;
    int i = 0;
    long a;

    int xsize2 = xsize>>1;
    int ysize2 = ysize>>1;
    long x2 = (long) xsize2 * xsize2;
    long y2 = (long) ysize2 * ysize2;
    long focus1;

    if (par1 == -1)
        a = xsize * ysize / 10;
    else
        a = xsize * ysize / par1;

    if (par2 == -1)
        focus1 = x2 * y2 / 6;
    else
        focus1 = x2 * y2 / par2;

    for (i=0; i<num_points; i++) 
    {
        /* 
         * Calculate src and dest coordinates.
         */
        while (1)
        {
            src_x = rnd (2*xsize2) - xsize2;
            src_y = rnd (2*ysize2) - ysize2;
	    if ((y2 * src_x * src_x  -  x2 * src_y * src_y  <  focus1)
	    &&  (x2 * src_y * src_y  -  y2 * src_x * src_x  <  focus1))
		break;
	}
        while (1)
        {
            dest_x = rnd (2*xsize) - xsize;
            dest_y = rnd (2*ysize) - ysize;
	    if (dest_x > 0  &&  dest_y > 0  &&
		dest_x * dest_y < a)
		break;
	    if (dest_x > 0  &&  dest_y < 0  &&
		dest_x * dest_y > -a)
		break;
	    if (dest_x < 0  &&  dest_y > 0  &&
		dest_x * dest_y > -a)
		break;
	    if (dest_x < 0  &&  dest_y < 0  &&
		dest_x * dest_y < a)
		break;
	}
	printf ("%d %d  %d %d\n", src_x, src_y, dest_x, dest_y);
    }
}


/*====================================================================
 * Calculate/write the src & dest points for an triangular explosion.
 *====================================================================
 */
TriangGen (num_points, xsize, ysize, par1, par2)
    int num_points;
    int xsize, ysize;
    int par1, par2;
{
    int dest_x, dest_y;
    int src_x, src_y;
    int x1;		/* upper vertex */
    int y1;
    int x2;		/* lower vertex */
    int y2;
    long y_intercept;	/* y_intercept of line between (x1,y1) and (x2,y2) */
    int i = 0;
int loop_count=0;

int invert = 0;
if (xsize < 0) { xsize = -xsize; invert = 1; }

    /* 
     * If not given as par1 and par2, then randomly select
     * the 'bottom' and 'right' vertices of the triangle (the 
     * left vertex is always (0,0).)
     */
    /* upper vertex */
    y1 = ysize;
    if (par1 == -1)
        x1 = xsize/4 + rnd (xsize*3/4);
    else
	x1 = par1;

    /* lower vertex */
    x2 = xsize;
    if (par2 == -1)
        y2 = ysize/4 + rnd (ysize*3/4);
    else
	y2 = par2;

    /* y-intercept = y - slope * x */
    y_intercept = (long) y1 - ((long) x1 * (y2 - y1) / (x2 - x1));

    for (i=0; i<num_points; i++) 
    {
        src_x = xsize >> 1;
        src_y = ysize >> 1;
        /* 
         * Calculate src and dest coordinates.
         * Select dest coordinate that is inside the triangle defined by
         * the following lines: 1. (0, 0) to (x1, y1)
	 *			2. (x2, y2) to (0, 0)
	 *			3. (x1, y1) to (x2, y2)
	 * Keep looping until a suitable point is found.
         */
        while (1)
        {
	    /* Pick a random destination point */
            dest_x = rnd (xsize);
            dest_y = rnd (ysize);

	    /*
	     * Accept the point only if it is within the triangle:
	     * 1. The slope of the line from (0, 0) to the point is < the 
	     *    slope of the line between (0, 0) and (x1, y1).
	     *    The proper expression is (dest_y/dest_x < y1/x1), but
	     *    in order to maintain precision, the equivalent expression
	     *    (dest_y < dest_x * y1/x1) is used instead.
	     * 2. The slope of the line between (0, 0) and the point is > the
	     *    slope of the line between (0, 0) and (x2, y2).
	     * 3. The point, when fitted into the equation y=mx+b of the line
	     *    between (x1, y1) and (x2, y2), results in y<mx+b.
	     */
	    if ( ((long) dest_y  <  (long) dest_x * y1 / x1)  &&
	 	 ((long) dest_y  >  (long) dest_x * y2 / x2)  &&
		 ((long) dest_y  <  
		   (long) dest_x * (y2 - y1) / (x2 - x1) + y_intercept))
		break;
        }

        /* Shift the point's origin so that (xsize/2, ysize/2) becomes (0,0) */
	src_x -= xsize >> 1;
	src_y -= ysize >> 1;
	dest_x -= xsize >> 1;
	dest_y -= ysize >> 1;

	if (invert) 
	{ 
	    src_x = -src_x; src_y = -src_y; 
	    dest_x = -dest_x; dest_y = -dest_y; 
	}
	printf ("%d %d  %d %d\n", src_x, src_y, dest_x, dest_y);
    }
}

