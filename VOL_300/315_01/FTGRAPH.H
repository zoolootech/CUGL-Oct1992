
#ifndef FTGRAPH_H
#define FTGRAPH_H

typedef void (* funcptr)(); /* pointer to a function definition */
typedef char * (* getsfuncptr)();   /* pointer to a gets()-style func */
typedef int (* intptr)();   /* pointer to getch()-style function */

    /* CONFIG_FILE is the name of the configuration file you will use. */
    /* if you want to compile a new version of NMGRAPH, you should either */
    /* substitute NMGRAPH.CNF for FTGRAPH.CNF or use FTGRAPH.EXE as the name */
    /* for the non-mouse version of the program. */

#define CONFIG_FILE  "ftgraph.cnf"


    /* just a list of various flags used in the program */
#define ALL 1
#define POS 2
#define XY 1
#define AMP 2
#define POWER 3
#define HPGL_BAR 1
#define HPGL_LINE 2
#define GRAPH_BAR 3
#define GRAPH_LINE 4
#define MOUSE_OFF 0
#define MOUSE_ON 1
#define LOWPASS 1
#define HIGHPASS 2
#define PTS_PER_LINE 3  /* number of data points to print on each line */
			/* of printer output */

#endif
