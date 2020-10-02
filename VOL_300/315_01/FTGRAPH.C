/* modified 11/89 by T Clune to allow override of autoscaling on graphs. */
/* This lets the user set two or more graphs to the same scale if he likes. */
/* See readme.txt for details on usage. */ 

/* modified 10/89 to use MSHERC.COM for Hercules support instead of my */
/* Hercules driver.  Microsoft verified that MSHERC.COM can be distributed */
/* with applications without infringing on their rights, 10/89. */
/* Also, changed to accept defaults for all inputs as a mouse support */
/* feature.  In addition, mgetch() and mgets() from MENU.C are now used */
/* to automatically determine whether the mouse is being supported for */
/* input, instead of changing the function pointers for input statements */
/* depending on how mouse_flag is initialized.  Since the function pointers */
/* have not been removed, this change is essentially meaningless here. */
/* Modified by T Clune */

/* Modified 4/89 by Tom Clune to circumvent some versions of DOS that */
/* do not contain the filespec of the program in ARGV[0].  graph_init() */
/* first tries the argv[0] value, and if that fails, tries the name in */
/* the #define, which is the filename with no  path (i.e., requires that */
/* you use the default path for the config file) value before aborting. */
/* Because the name is in FTGRAPH.H, you must edit FTGRAPH.H to use the */
/* name you will be using for the program before compiling. */

/* modified 4/89 by Tom Clune to support CGA, VGA, and EGA graphics */
/* to link the program, use: LINK FTGRAPH,,,FTPLOT MOUSELIB MOUSE GRAPHICS */
/* Note that the library GRAPHICS of MS C v. 5.1 must be invoked */
/* explicitly during linking.  Similarly, for the no-mouse version */
/* of ftgraph, link: FTGRAPH,,,FTPLOT NONMOUSE GRAPHICS. */

/* Written 3/89 by T Clune to plot FFT and IFT data. */
/* ftgraph.c uses text-mode (ASCII-format) data files structured as */
/* follows: Line 1 will contain an integer specifying the number of data */
/* points in the data set and two floating point values, the minimum and */
/* maximum value for the y-axis of the graph.                           */
/* The rest of the file is the floating-point data values, separated by */
/* white-space characters (space, carriage-return/line-feed, or tab). */
/* To compile this program, use Microsoft C, v.5, large memory model. */
/* I compiled it will optimization disabled (as I do for all my programs) */
/* but I have no reason to believe that optimization will cause the */
/* the program to fail. */
/* To run the program, FTGRAPH.CNF must be in the same directory as */
/* FTGRAPH.EXE, and you must have DEVICE=ANSI.SYS in your config.sys file */
/* Copyright (c) 1989, Eye Research Institute.  All Rights Reserved. */

#include "mouselib.h"
#include "msc_grph.h"
#include "ansi.h"
#include "fft.h"
#include "hpgl.h"
#include "hpglplot.h"
#include "ftgraph.h"
#include "menu.h"
#include "keys.h"
#include <stdlib.h>
#include <process.h>
#include <graph.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

static void graph_init(), reset_options(), ft(), graph_out(), correlate();
static void write_data(), ft_multiply(), autopower(), crosspower();
static int ft_process(), binary_choice();
static void ft_save(), display_menu(), printout(), peak_extract();
static char *get_prompt();
static double *get_data();
static void filter_main(), ft_filter();
static getsfuncptr get_string;
static intptr pause_func;

static double min_amp, filter_units;
static int ft_coords, ft_pos_neg, ft_precis, mouse_flag;

void main(argc, argv)
int argc;
char *argv[];
{
    FILE *f;

    static char *options[] =
    {
	"Quit",
	"Reset options",
	"Forward transform (file output)",
	"Inverse transform (file output)",
	"Multiply two data files (file output)",
	"Auto-power spectrum (file output)",
	"Cross-power spectrum (file output)",
	"Correlation from power spectrum data (file output)",
	"Filter time-domain real data (file output)",
	"Display data file (HPGL, graphics screen, or printer)"
    };

	/* menu parameters.  See menu.c documentation */
    int top, left, tab, columns, spacing, entries, auto_label;

    int choice; /* menu selection variable */
    top=3;      /* initialize menu variables */
    left=20;
    tab=0;
    columns=1;
    spacing=2;
    entries=10;
    auto_label= -2;

    graph_init(argv[0]);

	/* if mouse enabled, set menu variable for mouse operation */
    if(mouse_flag==MOUSE_ON)
	mouse_flag_toggle(MOUSE_PLUS_KEYBOARD);

    reset_menu(0);

    for(;;)
    {
	    /* CLS, POSITION, and CHAR_ATTRIBUTE are ANSI.H macros */
	CLS;

	POSITION(20,1); /* center heading */
	CHAR_ATTRIBUTE(UNDERSCORE);

	printf("MAIN MENU -- FFT GRAPHING FUNCTIONS\n");

	CHAR_ATTRIBUTE(NORMAL);

	while(kbhit())
	    getch();


		/* get a menu selection */
	choice=menu(top,left,tab,columns,spacing,entries,auto_label,options);

	switch(choice)  /* see the OPTION strings for meaning of choice */
	{
	    case 0:
		    CLS;
		    exit(0);
	    case 1:
		    CLS;
		    reset_options();
		    reset_menu(choice);
		    break;
	    case 2:
		    CLS;
		    ft(FORWARD);
		    reset_menu(choice);
		    break;
	    case 3:
		    CLS;
		    ft(INVERSE);
		    reset_menu(choice);
		    break;
	    case 4:
		    CLS;
		    ft_multiply();
		    reset_menu(choice);
		    break;
	    case 5:
		    CLS;
		    autopower();
		    reset_menu(choice);
		    break;

	    case 6:
		    CLS;
		    crosspower();
		    reset_menu(choice);
		    break;
	    case 7:
		    CLS;
		    correlate();
		    reset_menu(choice);
		    break;
	    case 8:
		    CLS;
		    filter_main();
		    reset_menu(choice);
		    break;
	    case 9:
		    CLS;
		    display_menu();
		    reset_menu(choice);
		    break;
	    default:
		    break;

	}

    }

}






/* autopower() performs an auto power spectrum on the selected data */

static void autopower()
{
    static double *x, *y, *minval, *maxval;
    static string_struc f;
    static int n;
    int i;
    char c;
    int coords;

    CLS;
    printf("This auto power routine asks only for the real data set.\n");
    printf("NOTE WELL:  The autopower FT has different units than a normal FT, and is\n");
    printf("called variously an auto spectrum, a power spectrum or the power spectral\n");
    printf("density of the data set.\n");
    printf("NOTICE: Since a power spectrum is defined relative to the area under\n");
    printf("the PSD AMPLITUDE graph, this routine produces polar format output,\n");
    printf("no matter what coordinate system you are using for FTs.\n");
    printf("Phase data for a power spectrum is meaningless, and will not be saved\n");
    printf("The inverse FT of this routine's output is called an autocorrelation\n");
    printf("and is a measure of the extent to which the data set is self-similar\n");
    printf("If you want to inverse-transform the output, you should make sure\n");
    printf("that you have positive/negative selected for your transform option.\n");
    printf("Because of the way that various flags internal to the program are set,\n");
    printf("you should use the CORRELATION menu option instead of INVERSE TRANSFORM\n");
    printf("to obtain the correlation IFT.\n");
    printf("The IFT (auto-correlation) will have 0 lag in center-screen.\n");
    printf("0 lag is perfect correlation (unshifted data correlates exactly to itself)\n\n");
    printf("Now, select the data file.  Press any key to continue.\n");
    (* pause_func)();

    coords=ft_coords;
    ft_coords=POWER;

    do
    {
	f=get_file();
	if(f.error_flag != 0)
	{
	    printf("Do you want to abort this function and return to the main menu (y/N)?\n");
	    c=binary_choice('N','Y');
	    if(c=='Y')
	    {
		ft_coords=coords;
		return;
	    }
	}
    }while(f.error_flag != 0);

    x=get_data(f.string,x,&n,minval, maxval);

    y=(double *)calloc(n, sizeof(double));
    if(y==NULL)
    {
	printf("Error allocating memory.  Program aborting.\n");
	exit(-1);
    }

    n=ft_process(x,y,n,FORWARD);

	/* square the transform for the power spectrum */
    for(i=0;i<n;i++)
    {
	x[i] = x[i]*x[i]+y[i]*y[i];
	y[i] =0.0;
    }

    ft_save(x,y,n,FORWARD);

    free(x);
    free(y);
    ft_coords=coords;

}



/* binary_choice() is a generic input for y/n-type responses, in which */
/* PRIMARY is the default value if <CR> or (assuming that it is active) */
/* RIGHT mouse button is pressed, and ALTERNATE is the other valid input. */
/* The prompt that is to be printed for binary_choice() is separate */
/* from this function. */

static int binary_choice(primary, alternate)
int primary, alternate;
{
    int c;
    primary=toupper(primary);
    alternate=toupper(alternate);

    while((c=(toupper((* pause_func)()))) != primary && c != alternate && c != CARRIAGE_RETURN)
	error_msg();

    if(c==CARRIAGE_RETURN)
	c=primary;

    return c;

}





/* correlate() performs an auto- or cross-correlation from spectra generated */
/* by autopower() or crosspower() under the pos/neg spectra option */

static void correlate()
{
    int coords;

    coords=ft_coords;
    ft_coords=POWER;

    CLS;
    printf("In order to perform a correlation, you must use data files that\n");
    printf("were generated by the AUTO POWER or CROSS POWER option ONLY. In addition,\n");
    printf("the spectra must have used the pos/neg frequencies format for the\n");
    printf("correlation data to represent the same time units as the original spectrum.\n\n");
    printf("Press any key to begin...\n");
    (* pause_func)();
    ft(INVERSE);
    ft_coords=coords;


}






/* crosspower() performs an cross-power spectrum on the selected data sets */

static void crosspower()
{
    static double *x1, *y1, *x2, *y2;
    static double *minval, *maxval;
    static string_struc f;
    static int n,m;
    int i;
    char c;
    int coords;


    CLS;
    printf("This cross-power spectrum routine asks only for the two real data sets.\n");
    printf("NOTE WELL: the units of the cross-power data set are not equivalent to\n");
    printf("those of a normal FT.  The cross-power data set FT is\n");
    printf("called variously a cross spectrum, a power spectrum, or a cross power spectrum.\n");
    printf("NOTICE: Since a power spectrum is defined relative to the area under\n");
    printf("the PSD AMPLITUDE graph, this routine produces polar format output,\n");
    printf("no matter what coordinate system you are using for FTs.\n");
    printf("Phase data for a power spectrum is meaningless, and will not be saved\n");
    printf("The inverse FT of this routine's output is called a cross-correlation.\n");
    printf("If you want to inverse-transform the output, you should make sure\n");
    printf("that you have positive/negative selected for your transform option.\n");
    printf("Because of the way that various flags internal to the program are set,\n");
    printf("you should use the CORRELATION menu option instead of INVERSE TRANSFORM\n");
    printf("to obtain the correlation IFT.\n");
    printf("The IFT (cross-correlation) will have 0 lag in center-screen.\n\n");
    printf("Now, select the first data file.  Press any key to continue.\n");
    (* pause_func)();
    CLS;

    coords=ft_coords;
    ft_coords=POWER;


    do
    {
	f=get_file();
	if(f.error_flag != 0)
	{
	    printf("Do you want to abort this function and return to the main menu (y/N)?\n");
	    c=binary_choice('N','Y');
	    if(c=='Y')
	    {
		ft_coords=coords;
		return;
	    }
	}
    }while(f.error_flag != 0);

    x1=get_data(f.string,x1,&n,minval, maxval);

    y1=(double *)calloc(n, sizeof(double));
    if(y1==NULL)
    {
	printf("Error allocating memory.  Program aborting.\n");
	exit(-1);
    }

    CLS;
    printf("Now, select the second file for cross-analysis.  Press any key to continue.\n");
    (* pause_func)();
    CLS;
    do
    {
	f=get_file();
	if(f.error_flag != 0)
	{
	    printf("Do you want to abort this function and return to the main menu (y/N)?\n");
	    c=binary_choice('N','Y');
	    if(c=='Y')
	    {
		ft_coords=coords;
		free(x1);
		free(y1);
		return;
	    }
	}
    }while(f.error_flag != 0);

    x2=get_data(f.string,x2,&m,minval, maxval);

    if(n != m)
    {
	printf("The two data files are not the same size. Press any key to exit routine \n");
	(* pause_func)();
	free(x1);
	free(y1);
	free(x2);
	ft_coords=coords;
	return;
    }

    CLS;

    y2=(double *)calloc(m, sizeof(double));
    if(y2==NULL)
    {
	printf("Error allocating memory.  Program aborting.\n");
	exit(-1);
    }

    n=ft_process(x1,y1,n,FORWARD);
    m=ft_process(x2,y2,m,FORWARD);

    for(i=0;i<n;i++)
    {
	x1[i] =x1[i]*x2[i]+y1[i]*y2[i];
	y1[i] =0.0;
    }

    ft_save(x1,y1,n,FORWARD);

    free(x1);
    free(y1);
    free(x2);
    free(y2);
    ft_coords=coords;

}




/* display_menu() provides the various data display options */

static void display_menu()
{
    FILE *f;

    static char *options[] =
    {
	"Return to Main menu",
	"HPGL bar chart (file output)",
	"HPGL line graph (file output)",
	"Bar chart display of file data (graphics screen)",
	"Line chart display of file data (graphics screen)",
	"Print data file to printer"
    };

	/* menu parameters.  See menu.c documentation */
    int top, left, tab, columns, spacing, entries, auto_label;

    int choice; /* menu selection variable */
    top=4;      /* initialize menu variables */
    left=20;
    tab=0;
    columns=1;
    spacing=2;
    entries=6;
    auto_label= -2;

    reset_menu(0);

    for(;;)
    {
	    /* CLS, POSITION, and CHAR_ATTRIBUTE are ANSI.H macros */
	CLS;

	POSITION(20,1); /* center heading */
	CHAR_ATTRIBUTE(UNDERSCORE);

	printf("FFT DATA DISPLAY FUNCTIONS\n");

	CHAR_ATTRIBUTE(NORMAL);

	while(kbhit())
	    getch();


		/* get a menu selection */
	choice=menu(top,left,tab,columns,spacing,entries,auto_label,options);

	switch(choice)  /* see the OPTION strings for meaning of choice */
	{
	    case 0:
		    CLS;
		    return;
		    break;
	    case 1:
		    CLS;
		    graph_out(HPGL_BAR);
		    reset_menu(choice);
		    break;
	    case 2:
		    CLS;
		    graph_out(HPGL_LINE);
		    reset_menu(choice);
		    break;
	    case 3:
		    CLS;
		    graph_out(GRAPH_BAR);
		    reset_menu(choice);
		    break;
	    case 4:
		    CLS;
		    graph_out(GRAPH_LINE);
		    reset_menu(choice);
		    break;
	    case 5:
		    CLS;
		    printout();
		    reset_menu(choice);
		    break;
	    default:
		    break;

	}

    }

}







/* filter_main() is the main selection menu for filtering options */

static void filter_main()
{
    static char *options[] =
    {
	"Return to Main menu",
	"Low-pass filter",
	"High-pass filter"
    };

	/* menu parameters.  See menu.c documentation */
    int top, left, tab, columns, spacing, entries, auto_label;

    int choice; /* menu selection variable */
    top=4;      /* initialize menu variables */
    left=20;
    tab=0;
    columns=1;
    spacing=2;
    entries=3;
    auto_label= -2;

    reset_menu(0);

    for(;;)
    {
	    /* CLS, POSITION, and CHAR_ATTRIBUTE are ANSI.H macros */
	CLS;

	POSITION(20,1); /* center heading */
	CHAR_ATTRIBUTE(UNDERSCORE);

	printf("FFT FILTERING MENU\n");

	CHAR_ATTRIBUTE(NORMAL);

	while(kbhit())
	    getch();


		/* get a menu selection */
	choice=menu(top,left,tab,columns,spacing,entries,auto_label,options);

	switch(choice)  /* see the OPTION strings for meaning of choice */
	{
	    case 0:
		    CLS;
		    return;
		    break;
	    case 1:
		    CLS;
		    ft_filter(LOWPASS);
		    reset_menu(choice);
		    break;
	    case 2:
		    CLS;
		    ft_filter(HIGHPASS);
		    reset_menu(choice);
		    break;
	    default:
		    break;
	}

    }

}





/* ft() keeps track of what kind of spectrum is being transformed, */
/* and shuffling the data file as appropriate for the particular format */
/* of data used.  FLAG specifies a forward or inverse transform */

static void ft(flag)
int flag;
{
    static double *x, *y;
    static double *minval, *maxval;
    static string_struc f;
    int i;
    static int n,m;
    char str1[20], str2[20], string[80], c;


    CLS;
    strcpy(str1,get_prompt(flag, 1));
    strcpy(str2,get_prompt(flag, 2));

    printf("First, get the %s component data file.  Press any key to continue.\n",str1);
    (* pause_func)();
    do
    {
	f=get_file();
	if(f.error_flag != 0)
	{
	    printf("Do you want to abort this function and return to the main menu (y/N)?\n");
	    c=binary_choice('N','Y');
	    if(c=='Y')
		return;
	}
    }while(f.error_flag != 0);

    x=get_data(f.string,x,&m,minval, maxval);

    if(ft_coords != POWER)
    {
	printf("Now, load the %s component data.  You will use the same kind\n", str2);
	printf("of menu as before.  Select the 'Quit' option (last menu item) if you are not\n");
	printf("going to enter a second data component array.\n");
	printf("Press any key to continue\n");
	(* pause_func)();
	CLS;
	do
	{
	    f=get_file();
	}while(f.error_flag < 0);

    }
    if(ft_coords !=POWER && f.error_flag==0)
	y=get_data(f.string, y, &n,minval, maxval);
    else
    {
	y=(double *)calloc(m, sizeof(double));
	if(y==NULL)
	{
	    printf("Error allocating memory.  Program aborting.\n");
	    exit(-1);
	}
	n=m;
    }
    if(n != m)
    {
	printf("The two data files are not the same size. Press any key to exit routine \n");
	(* pause_func)();
	free(x);
	free(y);
	return;
    }

    n=ft_process(x,y,n,flag);
    ft_save(x,y,n,flag);

    if(ft_coords==POWER && flag==INVERSE)
	peak_extract(x,n);

    free(x);
    free(y);

}



/* ft_filter() creates a filter according to user input and filters the */
/* selected data file by transforming it to the frequency domain and */
/* multiplying by the filter, then inverse transforming the result back to */
/* the time domain. */

static void ft_filter(flag)
int flag;
{

    static double *x, *y, *filter, *minval, *maxval;
    double rate, cutoff, rolloff;
    static string_struc f;
    char string[80], c;
    int i,sign;
    static int n;
    int pos_neg;
    int coords;

    printf("First, get the time-domain data file for filtering.  Press any key...\n");
    (* pause_func)();
    do
    {
	f=get_file();
	if(f.error_flag != 0)
	{
	    printf("Do you want to abort this function and return to the main menu (y/N)?\n");
	    c=binary_choice('N','Y');
	    if(c=='Y')
		return;
	}
    }while(f.error_flag != 0);

    x=get_data(f.string,x,&n,minval, maxval);

    y=(double *)calloc(n, sizeof(double));
    if(y==NULL)
    {
	printf("Error allocating memory.  Program aborting.\n");
	exit(-1);
    }

    CLS;
    printf("Now, enter the sampling frequency (reciprocal of the sampling rate)\n");
    printf("(e.g., if you collected data every 1/100 of a second, enter 100.0)\n");
    while((rate=atof((* get_string)(string)))<=0.0)
	error_msg();

	/* turn sampling frequency into Nyquist limit */
    rate /= 2.0;

    CLS;
    printf("Enter the cutoff frequency for the filter, in Hz\n");
    while((cutoff=atof((* get_string)(string)))<=0.0)
	error_msg();

    if(cutoff>=rate)
    {
	printf("Cutoff frequency exceeds Nyquist limit of data set.  Press any key...\n");
	(* pause_func)();
	free(x);
	free(y);
	return;
    }

    CLS;
    printf("Enter the rolloff in ");
    if(filter_units==10.0)
	printf("dB/decade for the filter (POSITIVE number only)\n");
    else
	printf("dB/octave for the filter (POSITIVE number only)\n");
    while((rolloff=atof((* get_string)(string)))<=0.0)
	error_msg();

    pos_neg=ft_pos_neg;
    ft_pos_neg=ALL;
    coords=ft_coords;
    ft_coords=AMP;

    filter=(double *)calloc(n, sizeof(double));
    if(filter==NULL)
    {
	printf("Error allocating memory.  Program aborting.\n");
	exit(-1);
    }

    if(flag==LOWPASS)
	sign= -1;
    else
	sign= 1;

    filter=get_filter(filter,n,sign,rate,cutoff,rolloff,filter_units);

    CLS;
    printf("We will now do the filtering.  Press any key...\n");
    (* pause_func)();

    n=ft_process(x,y,n,FORWARD);

    for(i=0;i<n;i++)
	x[i]=x[i]*filter[i];

    n=ft_process(x,y,n,INVERSE);
    ft_save(x,y,n,INVERSE);

    CLS;
    printf("Do you want a POLAR FORMAT frequency-domain file of the filter (y/N)?\n");
    c=binary_choice('N','Y');

    if(c=='Y')
    {
	for(i=0;i<n;i++)
	    y[i]=0.0;
	if(pos_neg==ALL)
	    ft_save(filter,y,n,FORWARD);
	else
	    ft_save(&filter[n/2],&y[n/2],n/2,FORWARD);
    }

    ft_coords=coords;
    ft_pos_neg=pos_neg;
    free(x);
    free(y);
    free(filter);

}






/* ft_multiply() gets two data files to multiply together */

static void ft_multiply()
{
    char str[2][80];
    int i, c;
    string_struc f;

    CLS;
    for(i=0;i<2;i++)
    {
	do
	{
	    f=get_file();
	    if(f.error_flag != 0)
	    {
		printf("Do you want to abort this function and return to the main menu (y/N)?\n");
		c=binary_choice('N','Y');
		if(c=='Y')
		    return;
	    }
	}while(f.error_flag != 0);

	strcpy(str[i], f.string);
    }

    data_multiply(str[0], str[1]);

}




/* ft_process() does the actual calling of the FT, and keeps track of whether */
/* this is a foreward or inverse transform, and reshuffles the data accordingly */

static int ft_process(x,y,n,flag)
double x[], y[];
int n,flag;
{

    CLS;
    printf("Processing...");

	/* put the order back the way it was in the transform */
    if((flag==INVERSE) && (ft_pos_neg==ALL))
    {
	unscramble_transform(x,n);
	unscramble_transform(y,n);
    }

    if((flag==INVERSE) && (ft_coords == AMP))
	polar_to_xy(x,y,x,y,n);

    fft(x,y,n,flag);

/* power,phase IFTs are shifted with this transform, & must be unshifted */

    if((flag==INVERSE) && (ft_coords == POWER))
    {
	unscramble_transform(x,n);
	unscramble_transform(y,n);
    }


    if(flag==FORWARD)
    {
	if(ft_pos_neg==ALL)
	{
	    unscramble_transform(x,n);
	    unscramble_transform(y,n);
	}
	else
	    n /=2;

	if(ft_coords == AMP)
	    xy_to_polar(x,y,x,y,n,min_amp);

    }
    return n;
}






/* ft_save() is the front-end for saving FT data files */

static void ft_save(x,y,n,flag)
double x[], y[];
int n, flag;
{

    char str1[20], str2[20], string[80];

    flag *= (-1);
    strcpy(str1,get_prompt(flag, 1));
    strcpy(str2,get_prompt(flag, 2));

    CLS;
    while(kbhit())
	getch();
    printf("Enter the filespec for the %s data file or <CR> to discard\n", str1);
    (* get_string)(string);
    if(strlen(string))
	write_data(string,x,n);
    while(kbhit())
	getch();
    if(ft_coords != POWER)
    {
	printf("Enter the filespec for the %s data file or <CR> to discard\n", str2);
	(* get_string)(string);
	if(strlen(string))
	    write_data(string,y,n);
    }

}




/* get_data() reads a data file and allocates memory to load it into. */
/* STRING is the file name, X is the pointer to the dat array, M is a */
/* pointer to the counter for number of elements in the data array, MINVAL */
/* is a pointer to the min data file value variable, and MAXVAL points to */
/* the max. data file value variable.  The routine automatically zero-fills */
/* data sets that are not a power of 2. */

static double *get_data(string, x, m, minval, maxval)
char string[];
double *x;
int *m;
double *minval, *maxval;
{
    FILE *fi;
    int i=0;
    static int n;
    unsigned int mask=0x8000;

   /* the I counter is to try to circumvent floppy disk opening errors */
    do
    {
	fi=fopen(string,"r");
	i++;
    }while(fi==NULL && i<4);

    if(fi==NULL)
    {
	printf("Error opening data file %s\n.", string);
	perror("Program aborting");
	exit(-1);
    }
    fscanf(fi,"%d %lg %lg",&n, minval, maxval);

    while(!(mask & n))
	mask /=2;

    if((mask & n) != n)
    {
	printf("NOTICE: Number of points in data file is not a power of 2.\n");
	printf("The program will zero-fill the %d data points to make a %u point set\n", n, mask*2);
	printf("Press any key to continue\n");
	(* pause_func)();
	CLS;
	mask *=2;
    }

    x=(double *)calloc(mask,sizeof(double));
    if(x==NULL)
    {
	printf("Error allocating memory.  Program aborting.\n");
	exit(-1);
    }
    for(i=0;i<n;i++)
	fscanf(fi,"%lg",&x[i]);
    fclose(fi);

    *m=mask;

    return x;

}






/* get_prompt() assigns a string for ft() prompts.  FLAG tells whether */
/* the prompt is for FORWARD or INVERSE data files, TERM says whether it is */
/* the x or y data file prompt. */

static char *get_prompt(flag, term)
int flag, term;
{
    int i;
    static char string[20];

    if(term==1)
    {

	if((flag==FORWARD)|| (ft_coords==XY))
	    strcpy(string, "Real");
	else
	    if(ft_coords == AMP)
		strcpy(string, "Amplitude");
	    else
		strcpy(string, "Power");
    }
    else
    {
	if((flag==FORWARD)|| (ft_coords==XY))
	    strcpy(string, "Imaginary");
	else
	    strcpy(string, "Phase");
    }

    return string;

}




/* graph_init() reads the configuration file for the program */
/* and initializes the graphics adaptor variables in msc_grph.c */

static void graph_init(string)
char string[];
{
    FILE *f;
    char str[80];
    char *sptr;
    int i=0;


    strcpy(str, string);
    sptr=strchr(str, '.');
    sptr++;
    *sptr='\0';
    strcat(str, "CNF");

    /* the I counter is to try to circumvent floppy disk opening errors */
    do
    {
	f=fopen(str, "r");
	i++;
    }while(f==NULL && i<4);

    if(f==NULL)
    {

    /* this part is a kludge to get around some versions of DOS that do */
    /* not pass the filespec in ARGV[0]. */

	i=0;
	do
	{
	    f=fopen(CONFIG_FILE, "r");
	    i++;
	}while(f==NULL && i<4);
    }

    if(f==NULL)
    {
	printf("Unable to open initialization file %s.\n", str);
	perror("Program aborting");
	exit(-1);
    }
    fscanf(f, "%lg %d %d %lg %d %d", &min_amp, &ft_pos_neg, &ft_coords, &filter_units, &ft_precis, &mouse_flag);
    fclose(f);

    if(mouse_flag==MOUSE_ON)
    {
	    /* check that system has a mouse if mouse option activated */
	i=m_install().opcode;
	if(i != (-1))
	{
	    printf("Mouse not found.  Disabling mouse option.\n");
	    printf("Press any key to continue...\n");
	    getch();
	    mouse_flag=0;
	}
    }
    if(mouse_flag==MOUSE_ON)
    {
	get_string=mouse_gets;
	pause_func=inpause;
    }
    else
    {
	get_string=gets;
	pause_func=getch;
    }
    get_adaptor();

}





/* graph_out() draws a graph to the hercules screen or creates an HPGL */
/* file of the graph, that can then be output to an HP plotter or plotter */
/* emulator.  FLAG identifies whether the graph goes to screen or to file */
/* and whether its a bar graph or line graph */

static void graph_out(flag)
int flag;
{
    static string_struc f;
    static double *x;
    static double minval, maxval;
    static int n;
    int i;
    char string[80];

    CLS;
    do
    {
	f=get_file();
    }while(f.error_flag != 0);

    x=get_data(f.string, x, &n, &minval, &maxval);

	/* verify that min/max were meaningful here */
    if(minval==maxval)
	minmax(x,n,&minval,&maxval);

    if(minval==maxval)
    {
	printf("All datapoints have the same value (%lf).  Routine aborting.\n",minval);
	printf("press any key to continue\n");
	(* pause_func)();
	free(x);
	return;
    }

    if(flag==HPGL_BAR)
	hpgl_bar(x,n,minval,maxval);
    if(flag==HPGL_LINE)
	hpgl_graph(x,n,minval,maxval);
    if(flag==GRAPH_BAR)
	msc_bar(x,n,minval, maxval);
    if(flag==GRAPH_LINE)
	msc_line(x,n,minval,maxval);
    free(x);

}


/* peak_extract() finds and prints the positive lag peaks in descending */
/* magnitude from a correlation.  Output is to a printer.  X[] is the */
/* array of time-domain points to order, and N is the number of points */
/* in the data set. */

static void peak_extract(x,n)
double x[];
int n;
{
    int i, j, peak_count=0, zeropt, *index, temp_i;
    double temp_var, *p_temp, interval;
    char c, string[80];


    CLS;
    printf("Would you like to get a hard-copy printout of positive lag peaks\n");
    printf("by descending magnitude (y/N)?\n");
    c=binary_choice('N','Y');

    if(c=='N')
	return;

    p_temp=(double *)malloc(n/2*sizeof(double));
    index=(int *)malloc(n/2*sizeof(int));

    if(p_temp==NULL || index==NULL)
    {
	printf("Error allocating internal memory.  Program aborting.\n");
	exit(-1);
    }
    CLS;
    printf("Enter the sampling interval in seconds between datapoints\n");
    while((interval=(atof((* get_string)(string)))) <=0.0)
	error_msg();

    zeropt=n/2;

    for(i=zeropt;i<n;i++)
    {
	if((x[i]>x[i-1]) && (x[i]>=x[i+1]))
	{
	    p_temp[peak_count]=x[i];
	    index[peak_count]=i-zeropt;
	    peak_count++;
	}

    }


	/* do a bubble sort.  Who cares? */
    for(i=0;i<(peak_count-1);i++)
    {
	for(j=(i+1);j<peak_count;j++)
	{
	    if(p_temp[j]>p_temp[i])
	    {
		temp_var=p_temp[i];
		p_temp[i]=p_temp[j];
		p_temp[j]=temp_var;
		temp_i=index[i];
		index[i]=index[j];
		index[j]=temp_i;
	    }
	}
    }

    fprintf(stdprn, "TABLE OF CORRELATION PEAKS (POSITIVE LAG ONLY) IN DESCENDING ORDER\n\n");
    fprintf(stdprn, "Format: lag count, lag in seconds, normalized correlation.\n");
    fprintf(stdprn,"LAG COUNT is the number of sampling intervals the spectrum was shifted.\n");
    fprintf(stdprn,"LAG in seconds is just LAG COUNT times the sampling interval.\n\n");

    temp_var=p_temp[0];

    for(i=0;i<peak_count;i +=PTS_PER_LINE)
    {
	for(j=0;j<PTS_PER_LINE;j++)
	{
	    if((i+j)<peak_count)
		fprintf(stdprn,"%d,%.*g,%.*g    ", index[i+j], ft_precis, (double)index[i+j]*interval, ft_precis, p_temp[i+j]/temp_var);
	}
	fprintf(stdprn,"\n");
    }

    fflush(stdprn);

    free(p_temp);
    free(index);
}






/* printout() prints a selected data file to the printer */

static void printout()
{
    static double *x, *y, *minval, *maxval;
    static string_struc f;
    static int n;
    double rate, startpt;
    int i,j;
    char string[80],c;

    CLS;
    printf("First, select the data file for printing.  Press any key to continue.\n");
    (* pause_func)();
    do
    {
	f=get_file();
	if(f.error_flag != 0)
	{
	    printf("Do you want to abort this function and return to the main menu (y/N)?\n");
	    c=binary_choice('N','Y');
	    if(c=='Y')
		return;
	}
    }while(f.error_flag != 0);

    x=get_data(f.string,x,&n,minval, maxval);

    CLS;
    printf("Is this a time domain or a frequency domain file (t/F)?\n");
    c=binary_choice('F', 'T');
    CLS;

    if(c=='T')
    {
	printf("Enter the sampling rate in seconds\n");
	while((rate=atof((* get_string)(string))) <= 0.0)
	    error_msg;
	startpt=0.0;
    }
    else
    {
	printf("Enter the Nyquist frequency for the data set\n");
	while((rate=atof((* get_string)(string))) <= 0.0)
	    error_msg;
	if(ft_pos_neg==ALL)
	{
	    startpt= (-rate);
	    rate /=(n/2);
	}
	else
	{
	    startpt = 0.0;
	    rate /=n;
	}
    }
    fprintf(stdprn,"DATAFILE: %s\n\n",f.string);
    if(c=='T')
	fprintf(stdprn,"Data in (seconds, magnitude) format\n");
    else
	fprintf(stdprn,"Data in (frequency, magnitude) format\n");

    for(i=0;i<n;i +=PTS_PER_LINE)
    {
	for(j=0;j<PTS_PER_LINE;j++)
	{
	    if((i+j)<n)
		fprintf(stdprn,"%d. %.*g, %.*g  ",i+j+1, ft_precis, startpt+rate*(i+j),ft_precis, x[i+j]);
	}
	fprintf(stdprn,"\n");
    }

    fflush(stdprn);
    free(x);

}




/* reset_options() lets you view the current settings for the program and */
/* allows you to temporarily override the defaults.  To permanently */
/* change the defaults, edit the .CNF file. */

static void reset_options()
{
    char string[80];
    int wrong=0;

    CLS;
    printf("The FT coordinate system in use is: ");
    if(ft_coords==XY)
	printf("real/imaginary\n");
    if(ft_coords==AMP)
	printf("Amplitude/phase\n");
    while(kbhit())
	getch();
    printf("Enter 'X' for real/imaginary (x/y), 'A' amplitude/phase, or <CR> for no change\n");
    do
    {
	(* get_string)(string);
	if(strlen(string) && (toupper(string[0]) != 'X') && (toupper(string[0]) != 'A'))
	    wrong=1;
	else
	    wrong=0;
	if(wrong)
	    error_msg();
    }while(wrong);

    if(strlen(string))
    {
	string[0]=toupper(string[0]);
	if(string[0]=='X')
	    ft_coords=XY;
	if(string[0]=='A')
	    ft_coords=AMP;
    }
    printf("\n\n");

    if(ft_coords == AMP)
    {
	while(kbhit())
	    getch();
	printf("The minimum amplitude for phase to be defined is %lf\n", min_amp);
	printf("(Lower amplitude signals have their phase set to 0)\n");
	printf("Enter a new minimum amplitude or <CR> to leave as is\n");
	(* get_string)(string);
	if(strlen(string))
	    min_amp=atof(string);
	printf("\n\n");

    }
    while(kbhit())
	getch();
    printf("The frequency spectrum is set to ");
    if(ft_pos_neg==ALL)
	printf("display positive and negative frequencies\n");
    if(ft_pos_neg==POS)
	printf("display only positive frequencies\n");
    printf("Enter 'P' for positive frequencies only, 'A' for all frequencies, or <CR>\n");
    do
    {
	(* get_string)(string);
	if(strlen(string) && (toupper(string[0]) != 'P') && (toupper(string[0]) != 'A'))
	    wrong=1;
	else
	    wrong=0;
	if(wrong)
	    error_msg();
    }while(wrong);

    if(strlen(string))
    {
	if(toupper(string[0])=='P')
	    ft_pos_neg=POS;
	else
	    ft_pos_neg=ALL;
    }

}


/* write_data() writes the data array to the file specified in STRING. */
/* N is th number of datapoints in ARRAY */

static void write_data(string,array, n)
char string[];
double array[];
int n;
{
    FILE *f;
    double minval, maxval;
    int i=0;
    char c;

    if(!fname_unused(string))
    {
	printf("WARNING!  You already have a filespec named %s.\n", string);
	printf("If you continue, that file will be overwritten.  Do you want to continue (y/N)?\n");
	c=binary_choice('N','Y');
	CLS;
	if(c=='N')
	    return;

    }
   /* the I counter is to try to circumvent floppy disk opening errors */
    do
    {
	f=fopen(string,"w");
	i++;
    }while(f==NULL && i<4);

    if(f==NULL)
    {
	printf("Error opening data file %s\n.", string);
	perror("Program aborting");
	exit(-1);
    }
    minmax(array,n,&minval,&maxval);
    fprintf(f,"%d %lg %lg\n",n, minval, maxval);
    for(i=0;i<n;i++)
	fprintf(f,"%lg\n", array[i]);
    fclose(f);

}

