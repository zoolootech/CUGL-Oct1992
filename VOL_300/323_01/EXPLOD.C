/****************************************************************************
* EXPLOD.C - simple fireworks for Hercules/CGA/EGA
*
* This program was developed with Datalight C 2.20 and Arrowsoft ASM 1.00d.
* (It also works with Turbo C 1.5/MASM 5.0 with one EQU change in EXPA.ASM).
* The code should be compilable by most other C compilers. However, the
* segment names in EXPA.ASM will have to be changed to suit the compiler.
*
* To compile:
*       asm expa ;		    OR		masm /mx expa ;
*       dlc explod.c expa.obj			(don't forget the /mx !!!)
*						tcc explod.c expa.obj
* (C) 1989 Dennis Lo
* You are free to use and distribute this source code, provided that the
* authors' names remain in the code, and that both the source and executables 
* of modified versions are clearly distinguished from the originals with
* descriptive messages.
*
* Change Log
* 89/06/24 Dennis Lo         V1.0 Initial release.
* 89/07/03 Dennis Lo         V1.1 Added cmd line options.  Code cleanups.
* 89/07/26 Erik Liljencrantz Added EGA support with autodetection
*                            Restores previous CRT textmode on exit
* 89/08/19 Dennis Lo	     V1.2 Code cleanups.  Changed references to frames
*			     table to use segment and offsets so that multiple
*			     frames tables can be supported in the future 
*			     without switching to a large data memory model.
* 89/09/03 Erik Liljencrantz Added VGA support with autodetection
* 90/01/20 Dennis Lo	     V2.0 Allow multiple explosion frame tables.
*			     Read in explosion shape tables from data files.
*			     Added -f and keypress options.  Split part of
*			     functionality into expgen.c.  Keyboard control.
*			     -n option.
****************************************************************************/

#include <stdio.h>
#include <ctype.h>
#ifndef DLC
#   include <dir.h>
#endif
#include <dos.h>
#include "explod.h"

#define PUT(s) fputs(s,stdout)

/*=====================================================================
 * External assembler routines.  
 *=====================================================================
 */
/* Datalight C does not prepend underscores to external names */
#ifdef DLC
#   define DosAlloc	_DosAlloc
#   define DosFree	_DosFree
#   define DosMemLeft	_DosMemLeft
#   define ChkKey	_ChkKey
#   define gr_card	_gr_card
#   define gr_setcard	_gr_setcard
#   define gr_gmode	_gr_gmode
#   define gr_tmode	_gr_tmode
#   define gr_addr	_gr_addr
#   define gr_store_pt	_gr_store_pt
#   define gr_frplot	_gr_frplot
#endif

extern unsigned short DosAlloc();
extern unsigned short DosFree();
extern unsigned short DosMemLeft();
extern int ChkKey();
extern int gr_card();
extern gr_setcard();
extern gr_gmode();
extern gr_tmode();
extern int gr_addr();
extern gr_store_pt();
extern gr_frplot();
#define DEADPOINT 32767    /* this must match the definition in expa.asm */

/*=====================================================================
 * Configuration Parameters
 *=====================================================================
 */
/* Video parameters */
static int Interlace_factor;
static int Screen_xsize;
static int Screen_ysize;

/* Animation parameters */
static int Simul_exps = -1;      /* # of simultaneous explosions */
static int Delay_factor = 0;    /* amount to delay after each frame */

/* Explosion parameters */
static int Gravity = 0;		/* Gravity override */
static int Wind = 0;		/* Wind override */
static int Centre_x;		/* Coords of centre of screen */
static int Centre_y;
static unsigned int Centre_y_mask;

/* Explosion control parameters */
static int Leave_trail = 0;	/* If non-0 then don't erase explosions */
static int Event_limit = 0;	/* # of exp events before exiting */
static int Total_frames = 0;	/* used to find avg #frames per explosion */

/*=====================================================================
 * Explosion Creation table:
 * Temporarily keeps track of the parameters and points of an explosion
 * while its frames are being calculated.
 *=====================================================================
 */
/* 16.16 fixed point type: overlays a 32-bit long with its 16-bit
   whole number part and its 16-bit fractional part */
typedef union 		
{
    long l;
    struct 			/* Note: this is endian dependent */
    {
        unsigned short ls;
        short ms;
    } s;
} fixed; 

/* Structure of an explosion point at explosion creation */
typedef struct 
{
    fixed x, y;                 /* location */
    fixed xv, yv;               /* velocity */
    fixed xa, ya;               /* acceleration */
    int alive;                  /* liveness: 0=dead */
} exp_pt_type;

/* Explosion creation table */
exp_pt_type Pt_table [MAX_POINTS+1];

/*=====================================================================
 * Explosion type descriptor - contains all information in a ready-to-
 * playback explosion, including a pointer to its frames table.  The
 * frames table contains an array of MAX_FRAMES frames.  Each frame is
 * one frame in the animation of the explosion, and consists of 
 * MAX_POINTS structures of the form {offset (2 bytes), value (1 byte)}.
 * The frame table structure treated only as a block of memory by the
 * C code, so it is not explicitly declared. (It cannot be declared
 * in C in a portable way due to structure alignment padding.)
 *=====================================================================
 */
#define FRAME_PT_SIZE 3		/* size of one point in the frame table */
#define MAX_EXP_DESC	30	/* Max # of explosion types */

/* Explosion descriptor type */
typedef struct
{
    int nframes;
    int npoints;
    int xsize;
    int ysize;
    int gravity;
    int wind;
    int trail_len;
    unsigned short frame_seg;
    unsigned short frame_ofs;
} exp_desc_type;

/* Array of explosion descriptors for the explosions read in from disk */
static exp_desc_type	Exp_desc [MAX_EXP_DESC];/* Explosion descriptors */
static int		Num_exp_desc;		/* # of explosions read in */


/*=====================================================================
 * Explosion Event Table - each entry controls one instance of an 
 * active explosion event during playback.
 *=====================================================================
 */
#define MAX_EXPS 48     /* max # of simultaneously active explosion events */

/* Explosion events table */
typedef struct 
{
    int centre;         	/* addr of centre of explosion */
    int frame_num;      	/* frame #. Dead if -1 */
    exp_desc_type *exp_desc;	/* explosion type */
} exp_event_type;
static exp_event_type Exp_ev_tab [MAX_EXPS]; 



/*====================================================================
 * Return a random number between 0..maxval
 *====================================================================
 */
int
rnd (maxval)
int maxval;
{
#   define MAX_RAND 32767       /* max val returned by rand() */
    long l;

    l = (long) maxval * rand() / MAX_RAND;
    return ((int) l);
}

/*====================================================================
 * Read a number from a string - replaces the need for sscanf() which
 * reduces the size of the executable by about 2.5K.
 * Returns a ptr to the next word in the string.
 *====================================================================
 */
char *
str_to_num (str, num)
char *str;	/* IN: ptr to string containing the number */
int *num;	/* OUT: the number read */
{
    /* Skip leading spaces */
    while (*str  &&  *str == ' ') str++;

    *num = atoi (str);

    /* Skip non spaces (skip past the number) */
    while (*str  &&  *str != ' ') str++;

    return (str);
}

/*====================================================================
 * Local memory allocator:
 * MemInit() - allocates as much memory from DOS as possible.
 *	       Return 1 if successful, 0 if not.
 * MemAlloc() - allocates a block of memory to the caller.
 *		Returns segment addr of block, or 0 if failed.
 * MemCleanup() - returns all memory to DOS.
 *
 * Why a local memory allocator?  malloc() won't work in the small 
 * memory model because we need >64K of memory.  DOS's malloc()
 * (int21 ah=48h) seems to waste a lot of memory when called several
 * times in a row -- it was leaving ~5K gaps between each allocated
 * block for reasons that I don't understand.  I tried a DOS realloc()
 * (int21 ah=4ah) but that didn't help.  After a month of playing with
 * it I gave up in disgust and wrote these routines instead.
 *====================================================================
 */
unsigned mem_start;
unsigned mem_curr;
unsigned mem_left;

int
MemInit()
{
    /* 
     * Allocate all available memory from DOS:
     * Try to allocate 640K at first so DOS will return the actual 
     * amount available.
     */
    mem_left = DosMemLeft ();
    if (mem_left == 0)
    {
	PUT ("DosMemLeft failed!\n");
	return (0);
    }
    mem_start = DosAlloc (mem_left);
    if (mem_start == 0)
    {
	PUT ("MemInit: DosAlloc failed!\n");
	return (0);
    }
    mem_curr = mem_start;
    return (1);
}

int
MemAlloc (size)
unsigned size;		/* size in bytes */
{
    int addr;

    /* Convert the requested size to # of paragraphs */
    size = (size + 15) >> 4;

    if (size > mem_left)
	return (0);
    addr = mem_curr;
    mem_curr += size;
    mem_left -= size;

    return (addr);
}

MemCleanup()
{
    if (DosFree (mem_start) != 0)
	PUT ("DosFree failed!\n");
}


/************************ User Interface ***************************/


/*=====================================================================
 * Return the path name portion of a filename.
 *=====================================================================
 */
void
GetPathName (filename, pathname)
char *filename;		/* in */
char *pathname;		/* out */
{
    char *path_end;
    char path_len = 0;

    /* Scan backwards in the filename for the end of the path */
    path_end = filename + strlen (filename);
    while (path_end >= filename  &&  *path_end != ':'  &&
	   *path_end != '/'  &&  *path_end != '\\')
	path_end--;

    /* Copy the path to the output string */
    if (path_end >= filename)
    {
	path_len = path_end - filename;
	strncpy (pathname, filename, path_len);
	pathname [path_len++] = '\\';
    }

    pathname [path_len] = '\0';
}

/*=====================================================================
 * Process command line arguments.  
 * Returns 1 if successful, 0 if not.
 * Also sets up explosion parameters according to video card type.
 *=====================================================================
 */
int
GetArgs (argc, argv)
    int argc;
    char *argv[];
{
    int video_type;
    static char name [80];		/* Current data file name */
    static char path [80];		/* Current path */

    /* Set video params */
    video_type = gr_card ();		/* guess video card type */
    SetVideoParams (video_type);         

    /* Print instructions if no command line parameters are given */
    if (argc == 1)
        Instructions();

    /*
     * Loop to parse each command line parameter
     */
    while (--argc > 0) 
    {
        if (**++argv == '-') 
        {
            switch ((*argv)[1])
            {
                case 'v':               /* -v: video card type (c, h)*/
                    video_type = (*argv)[2];
		    SetVideoParams (video_type);         
                    break;
                case 's':               /* -s: # of simultaneous explosions */
                    Simul_exps = atoi ((*argv) + 2);
                    break;
                case 'd':               /* -d: delay factor */
                    Delay_factor = atoi ((*argv) + 2);
                    break;
                case 'g':               /* -g: gravity override */
                    Gravity = atoi ((*argv) + 2);
                    break;
                case 'w':               /* -w: wind override */
                    Wind = atoi ((*argv) + 2);
                    break;
		case 'f':		/* -f: fill screen with trails */
		    Leave_trail = 1;
		    break;
		case 'n':		/* -n: total number of explosions */
		    Event_limit = atoi ((*argv) + 2);
		    break;
                default:
                    PUT ("*** Invalid option: "); PUT (*argv); PUT ("\n");
                    Instructions();
                    return (0);
            }
        }
	/*
	 * Else a filename argument:
	 *   Wildcard expand the filename and read in each of the resulting
	 *   explosion data files.
	 */
	else
	{
#ifdef DLC
	    struct FIND *blk;
#	    define FFNAME blk->name

	    GetPathName (*argv, path);
	    blk = findfirst (*argv, 0);
	    while (blk != NULL)
	    {
		strcpy (name, path);
		strcat (name, FFNAME);
	        PUT ("Reading "); PUT (name); PUT ("\n");
	        if (ReadDatFile (name, &Exp_desc[Num_exp_desc], Pt_table))
		    Num_exp_desc++;
		blk = findnext ();
	    } 
#else /* TURBOC */
	    struct ffblk blk;
#	    define FFNAME blk.ff_name
	    int more_files;

	    GetPathName (*argv, path);
	    more_files = findfirst (*argv, &blk, 0);
	    while (more_files == 0)
	    {
		strcpy (name, path);
		strcat (name, FFNAME);
	        PUT ("Reading "); PUT (name); PUT ("\n");
	        if (ReadDatFile (name, &Exp_desc[Num_exp_desc], Pt_table))
		    Num_exp_desc++;
		more_files = findnext (&blk);
	    } 
#endif
        }
    }
    return (1);
}

/*=====================================================================
 * Set video card-related parameters
 *=====================================================================
 */
SetVideoParams (video_mode)
    char video_mode;
{
#   define NUM_VIDEO_MODES 4
    /*
     * Default parameters for each video mode
     */
    static struct defaults_struct {
	char video_mode;
	int interlace_factor;
	int screen_xsize;
	int screen_ysize;
	int simul_exps;
    } defaults [NUM_VIDEO_MODES] = { 
	'c', 2, 640, 200, 8,		/* cga */
	'h', 4, 720, 348, 10,		/* hgc */
	'e', 1, 640, 350, 10,		/* ega */
        'v', 1, 640, 480, 12		/* vga */
    };
    int i;
        
    /* 
     * Find the defaults table entry for the given video mode and set
     * the video configuration parameters to the values in that entry.
     */
    for (i=0; i<NUM_VIDEO_MODES; i++)
    {
        if (video_mode == defaults[i].video_mode)
	{
	    Interlace_factor = defaults [i].interlace_factor;
	    Screen_xsize = defaults [i].screen_xsize;
	    Screen_ysize = defaults [i].screen_ysize;

	    /* If not specified on the command line then set to default */
	    if (Simul_exps == -1) 
		Simul_exps = defaults [i].simul_exps;

	    break;
	}
    }
    Centre_y_mask = (0xffff - (Interlace_factor - 1));
    Centre_x = (Screen_xsize / 2);
    Centre_y = (Screen_ysize / 2) & Centre_y_mask;
    gr_setcard (video_mode);
}

/*=====================================================================
 * Print instructions
 *=====================================================================
 */
Instructions ()
{
    PUT ("EXPLOD 2.0          by Dennis Lo  90/01/20\n");
    PUT ("Usage: explod [optional parameters] <data files>\n");
    PUT ("Parameters can be one of\n");
    PUT ("  -vC :C=video type.  CGA:'-vc'  HGC:'-vh'  EGA:'-ve'  VGA:'-vv'\n");
    PUT ("       Default is auto-detect\n");
    PUT ("  -sN :N = # of simultaneous explosions. Default: 10 (8 for CGA)\n");
    PUT ("  -dN :N = Delay factor.  Default: 0\n");
    PUT ("  -gN :N = Gravity.       Default: CGA=15 HGC=20 EGA=20\n");
    PUT ("  -wN :N = Wind.          Default: 0\n");
    PUT ("  -k  : Keyboard control - start explosions with keypresses.\n");
    PUT ("  -f  : Fill screen with explosion trails.\n");
    PUT ("  -nN :N = # of explosions before exiting.  Default: 0 (infinite)\n");
    PUT ("eg. (for 12Mhz AT with VGA, in EGA mode): explod -ve -d6 *.dat\n");
    PUT ("Press ESCAPE to return to DOS\n");
}



/********************* Explosion event handling *************************/

/*=====================================================================
 * Initialize the explosion events table 
 *=====================================================================
 */
InitExpEvents ()
{
    int i;

    /* Set all events inactive */
    for (i=0; i<MAX_EXPS; i++)
        Exp_ev_tab[i].frame_num = -1;
}

/*=====================================================================
 * Add an explosion to the events table 
 *=====================================================================
 */
StartExpEvent (x, y)
    int x, y;
{
    int i;
    exp_desc_type *e;

    y &= Centre_y_mask;

    /* Find the first free entry in the table */
    for (i=0; i<MAX_EXPS; i++)
        if (Exp_ev_tab[i].frame_num == -1) 
            break;

    /* don't do anything if table is full */
    if (i >= MAX_EXPS) 
        return;

    Exp_ev_tab[i].centre = gr_addr (x, y);
    Exp_ev_tab[i].frame_num = 0;
    e = &Exp_desc [rnd (Num_exp_desc)];
    Exp_ev_tab[i].exp_desc = e;

    /* Display the first frame */
    gr_frplot (e->npoints, e->frame_seg,  e->frame_ofs,
		Exp_ev_tab[i].centre, 1);
}

/*=====================================================================
 * Play back 1 frame of each event in the explosion events table
 *=====================================================================
 */
PlayExpEvents ()
{
    int i, j;
    unsigned short frame_seg;
    unsigned short frame_ofs;
    unsigned short frame_i_addr;
    exp_event_type *event;
    exp_desc_type *e;

    /*
     * Loop once for each event in the events table:
     */
    for (i=0; i < MAX_EXPS; i++)
    {
	event = &Exp_ev_tab[i];
	e = event->exp_desc;

        if (event->frame_num != -1)	/* if the event is active */
        {
	    frame_seg = e->frame_seg;
	    frame_ofs = e->frame_ofs;

            /* if finished last frame of this explosion event */
            if (++event->frame_num == (e->nframes + e->trail_len))
            {
                /* turn off the final frame's points */
		if (!Leave_trail)
                    gr_frplot (e->npoints, frame_seg, 
		    	frame_ofs + (event->frame_num - e->trail_len-1) *
		    	e->npoints * FRAME_PT_SIZE, Exp_ev_tab[i].centre, 0);

                /* free the current event's in the explosion events table */
                event->frame_num = -1;
            }

            else
            {
                /* Turn off previous frame's points (unless no prev frame) */
                if (event->frame_num - e->trail_len - 1 >= 0  &&
		    !Leave_trail)
                    gr_frplot (e->npoints, frame_seg,
			frame_ofs + (event->frame_num - e->trail_len-1) *
			e->npoints * FRAME_PT_SIZE, Exp_ev_tab[i].centre, 0);

                /* Turn on current frame's points */
                if (event->frame_num < e->nframes)
                    gr_frplot (e->npoints, frame_seg, 
			frame_ofs + event->frame_num * 
			e->npoints * FRAME_PT_SIZE, Exp_ev_tab[i].centre, 1);
            }
        }
    }
}



/*********************** Explosion Creation ************************/

/*====================================================================
 * Generate the frames of an explosion, storing them in a frame table
 * at the given address.
 *====================================================================
 */
FrameInit (e, pt_table)
    exp_desc_type *e;
    exp_pt_type   pt_table[];
{
    exp_pt_type	   *curr_pt;	     /* current explosion point */
    unsigned	   frame_i, i;	     /* loop counters		*/
    unsigned short frame_i_addr;     /* addr of current frame	*/
    int		   fade_window = 0;  /* fade counter (can fade if 0) */
    unsigned short centre_addr = gr_addr (Centre_x, Centre_y);
    unsigned short frame_seg   = e->frame_seg;
    unsigned short frame_ofs   = e->frame_ofs;

    /*
     * Loop once to create each frame of the explosion
     */
    for (frame_i = 0; frame_i < e->nframes; frame_i++)
    {
	/* Find the address of this frame in the frame table */
	frame_i_addr = frame_ofs + (frame_i * e->npoints * FRAME_PT_SIZE);

        /*
	 * Loop once for each point in the explosion
         */
        for (i=0; i<e->npoints; i++)
        {
            curr_pt = &(pt_table[i]);

            /* If the point is dead, mark it as dead and go on to the next */
            if (!curr_pt->alive)
	    {
	        gr_store_pt (DEADPOINT, 0, frame_seg, 
			     /* below is &frame table [frame_i] [i] */
			     frame_i_addr + (i * FRAME_PT_SIZE),  0);
		continue;
	    }

            /* calculate the next position of the point */
            curr_pt->x.l += curr_pt->xv.l;
            curr_pt->y.l += curr_pt->yv.l;
            curr_pt->xv.l += curr_pt->xa.l;
            curr_pt->yv.l += curr_pt->ya.l;

	    /* Store the point in the frame */
	    gr_store_pt (curr_pt->x.s.ms, curr_pt->y.s.ms, frame_seg, 
		/* below is &frame table [frame_i] [i] */
		frame_i_addr + (i * FRAME_PT_SIZE), 
		centre_addr);

	    /* 
	     * Do point fadeout - one point dies 1 out of 8 times here,
	     * starting at 60% of the way through the explosion.
             */
            fade_window = (fade_window + 1) & 7;
	    if (frame_i > (e->nframes * 3 / 5)  &&  fade_window == 0)
                curr_pt->alive = 0;
        }
    }
}

/*====================================================================
 * Read in an explosion description from the given disk file.
 * Returns 1 if successful, 0 if not.
 *====================================================================
 */
int
ReadDatFile (filename, e, pt_table)
    char *filename;
    exp_desc_type *e;
    exp_pt_type pt_table[];
{
    int	x1, y1, x2, y2;
    long src_x, src_y, dest_x, dest_y;
    long accel, vel;
    int cx = Centre_x; 
    int cy = Centre_y;
    int i;
    FILE *fp;
#   define LINELEN 80
    char linebuf [LINELEN];
    char *lineptr;
    unsigned short frame_buf_size;

    /* 
     * Read in the explosion description file header information 
     */
    if ((fp = fopen (filename, "r")) == NULL)
    {
	PUT ("Error reading file "); PUT (filename); PUT ("\n");
	return (0);
    }
    fgets (linebuf, LINELEN, fp);
    str_to_num (linebuf, &e->npoints);
    fgets (linebuf, LINELEN, fp);
    str_to_num (linebuf, &e->nframes);
    fgets (linebuf, LINELEN, fp);
    str_to_num (linebuf, &e->gravity);
    fgets (linebuf, LINELEN, fp);
    str_to_num (linebuf, &e->wind);
    fgets (linebuf, LINELEN, fp);
    str_to_num (linebuf, &e->trail_len);

    /* 
     * Perform any user-specified command-line parameter overrides 
     */
    if (Gravity != 0)
	e->gravity = Gravity;
    if (Wind != 0)
	e->wind = Wind;

    /* 
     * Allocate memory for the explosion's frames 
     */
    e->frame_ofs = 0;
    frame_buf_size = (unsigned short) e->nframes * e->npoints * FRAME_PT_SIZE;
    if ((e->frame_seg = MemAlloc (frame_buf_size)) == 0)
    { 
	PUT ("Not enough memory for file "); 
	PUT (filename);
	PUT ("\n");
	return (1);
    }

    /* 
     * Read in the points for the explosion 
     */
    for (i=0; i<e->npoints; i++) 
    {
	/* read in a (srcx, srcy), (destx, desty) coordinate pair */
        fgets (linebuf, LINELEN, fp);
	lineptr = str_to_num (linebuf, &x1);
	lineptr = str_to_num (lineptr, &y1);
	lineptr = str_to_num (lineptr, &x2);
	lineptr = str_to_num (lineptr, &y2);

        /* Convert to fixed pt. (Can't use shifts because they are unsigned) */
        src_x = (x1 + cx) * 65536;
        src_y = (y1 + cy) * 65536;
        dest_x = (x2 + cx) * 65536;
        dest_y = (y2 + cy) * 65536;

	/* Store src coordinate in explosion table's current position field */
        pt_table [i].x.s.ms = (int) (src_x >> 16);
        pt_table [i].x.s.ls = 0;
        pt_table [i].y.s.ms = (int) (src_y >> 16);
        pt_table [i].y.s.ls = 0;

        /* 
	 * Calculate velocity and acceleration.
         *   accel = 2 * distance / #steps^2   (#steps is equivalent to time)
         *   vel = accel * #steps 
         */
        accel = (2 * (dest_x - src_x))  /  ((long) e->nframes*e->nframes);
        vel = (2 * (dest_x - src_x))  /  (long) e->nframes;
        pt_table [i].xa.l = -accel + (e->wind * 100);
        pt_table [i].xv.l = vel;

        accel = (2 * (dest_y - src_y))  /  ((long) e->nframes*e->nframes);
        vel = (2 * (dest_y - src_y))  /  (long) e->nframes; 
        pt_table [i].ya.l = -accel + (e->gravity * 100);
        pt_table [i].yv.l = vel;

        pt_table [i].alive = 1;
    }

    /* Generate the frame table for the explosion */
    FrameInit (e, pt_table);

    /* Update stats used for calculating # of simultaneous explosions */
    Total_frames += e->nframes;

    return (1);
}



/****************************** Main *******************************/


/*=====================================================================
 * Given a keyboard key, return an explosion x-y starting point 
 * roughly equivalent to the key's location on the keyboard.
 *=====================================================================
 */
KeyToScreenPos (key, xsize, ysize, xpos, ypos)
int key;	/* in: key's ascii value */
int xsize;	/* in: x size of screen */
int ysize;	/* in: y size of screen */
int *xpos;	/* out: screen x position */
int *ypos;	/* out: screen y position */
{
    int x, y;
    static unsigned char key_xpos [] =
    {
	/*spc*/ 8, /*!*/ 1, /*"*/ 11, /*#*/ 3, /*$*/ 4, /*%*/ 5, /*&*/ 6,
	/*'*/ 11, /*(*/ 9, /*)*/ 10, /***/ 8, /*+*/ 12, /*,*/ 8, 
	/*-*/ 11, /*.*/ 9, /*slash*/ 10, /*0*/ 10, /*1*/ 1, /*2*/ 2,
	/*3*/ 3, /*4*/ 4, /*5*/ 5, /*6*/ 6, /*7*/ 7, /*8*/ 8, /*9*/ 9,
	/*:*/ 10, /*;*/ 10, /*<*/ 8, /*=*/ 12, /*>*/ 9, /*?*/ 10,
	/*@*/ 2, /*A*/ 1, /*B*/ 5, /*C*/ 3, /*D*/ 3, /*E*/ 3, /*F*/ 4,
	/*G*/ 5, /*H*/ 6, /*I*/ 8, /*J*/ 7, /*K*/ 8, /*L*/ 9, /*M*/ 7,
	/*N*/ 6, /*O*/ 9, /*P*/ 10, /*Q*/ 1, /*R*/ 4, /*S*/ 2, /*T*/ 5,
	/*U*/ 7, /*V*/ 4, /*W*/ 2, /*X*/ 2, /*Y*/ 6, /*Z*/ 1, /*[*/ 11,
	/*\*/ 11, /*]*/12, /*^*/ 6, /*_*/ 10, /*`*/ 0
    };
    static unsigned char key_ypos [] =
    {
	/*spc*/ 5, /*!*/ 1, /*"*/ 3, /*#*/ 1, /*$*/ 1, /*%*/ 1, /*&*/ 1,
	/*'*/ 3, /*(*/ 1, /*)*/ 1, /***/ 1, /*+*/ 1, /*,*/ 4, 
	/*-*/ 1, /*.*/ 4, /*slash*/ 4, /*0*/ 1, /*1*/ 1, /*2*/ 1,
	/*3*/ 1, /*4*/ 1, /*5*/ 1, /*6*/ 1, /*7*/ 1, /*8*/ 1, /*9*/ 1,
	/*:*/ 3, /*;*/ 3, /*<*/ 4, /*=*/ 1, /*>*/ 4, /*?*/ 4,
	/*@*/ 1, /*A*/ 3, /*B*/ 4, /*C*/ 4, /*D*/ 3, /*E*/ 2, /*F*/ 3,
	/*G*/ 3, /*H*/ 3, /*I*/ 2, /*J*/ 3, /*K*/ 3, /*L*/ 3, /*M*/ 4,
	/*N*/ 4, /*O*/ 2, /*P*/ 20, /*Q*/ 2, /*R*/ 2, /*S*/ 3, /*T*/ 2,
	/*U*/ 2, /*V*/ 4, /*W*/ 2, /*X*/ 4, /*Y*/ 2, /*Z*/ 4, /*[*/ 2,
	/*\*/ 5, /*]*/ 2, /*^*/ 1, /*_*/ 1, /*`*/ 1
    };
#   define MAX_KEYX 13
#   define MAX_KEYY 5

    /* Special case: if space bar then make it random */
    if (key == 32)
    {
	*xpos = rnd (xsize);
	*ypos = rnd (ysize);
	return;
    }

    if (key > 96) key -= 32;
    if (key > 96) key &= 63;
    key -= 32;
    if (key < 0) key =- key;

    x = key_xpos [key];
    if (x >= MAX_KEYX) 
	x = MAX_KEYX - 1;
    x = (long) x * xsize / MAX_KEYX;
    *xpos = x;

    y = key_ypos [key];
    if (y >= MAX_KEYY) 
	y = MAX_KEYY - 1;
    y = (long) y * ysize / MAX_KEYY;
    *ypos = y;
}

/*=====================================================================
 * Main - loop to check input and animate explosion events
 *=====================================================================
 */
main (argc, argv)
    int argc;
    char *argv[];
{
    int key;
    int start_count;
    int start_interval;
    int avg_num_frames;
    int event_count = 0;
    int last_event_frame_count = 0;
    int i, j;
    int xpos, ypos;

    /* Initialize the local memory allocator */
    if (!MemInit())		
	exit (1);

    /* Read in the command line arguments and explosion files */
    Num_exp_desc = 0;
    if (!GetArgs (argc, argv))
	goto exit;
    if (Num_exp_desc == 0) 
    {
	PUT ("\nERROR: no data files specified or all files are invalid.\n");
	goto exit;
    }

    /* Calculate the number of frames to wait between starting explosions */
    avg_num_frames = Total_frames / Num_exp_desc;
    if (Simul_exps > 0)
    {
        start_interval = avg_num_frames / Simul_exps;
	start_count = start_interval;
    }

    InitExpEvents();
    gr_gmode();		/* enter graphics mode */

    /*
     * Main loop: animate one explosion frame once each loop
     * Exit loop when ESC pressed or when Event_limit explosions are done.
     */
    while ((key = ChkKey()) != 27)
    {
        /* 
	 * Start a new explosion event every start_interval loops or
	 * when a key is pressed.
	 */
	if ((Simul_exps != 0  &&  start_count++ == start_interval)  ||  key)
        {
	    /* Start only if event limit hasn't been reached */
	    if (Event_limit == 0  ||  event_count++ < Event_limit) 
	    {
		if (key)
		    KeyToScreenPos (key, Screen_xsize, Screen_ysize, 
				    &xpos, &ypos);
		else
		{
		    xpos = Screen_xsize/7 + rnd (Screen_xsize*5/7);
		    ypos = Screen_ysize/7 + rnd (Screen_ysize*5/7);
		}
                StartExpEvent (xpos, ypos);
                start_count = 0;
	    }
        }

        /* Play back one frame of each explosion event */
        PlayExpEvents ();

	/* Exit if the event limit has been reached and the last explosion
	   has ended. */
	if (Event_limit  &&  event_count >= Event_limit  &&
	    ++last_event_frame_count > avg_num_frames)
	    break;

        /* 
         * Optional delay for machines that are too fast.
         * The function call in the inner loop prevents optimizing
         * compilers from optimizing away the loop.
         */
        if (Delay_factor > 0)
            for (i=0; i < Delay_factor; i++) 
                for (j=0; j<100; j++)
                    DummyDelay (i*j);     
    }
    gr_tmode ();

exit:
    MemCleanup();	/* Release memory used by the local mem allocator */
}

/* 
 * Dummy function used for delays
 */
DummyDelay (i)
int i;
{
    return (i * i);
}


