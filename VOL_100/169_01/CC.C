/*
 
        compile flow generator v1.0 for computer innovations c86
 
        by Bob Lafleur and Tom Vaughan of Farm Credit Banks of Springfield
 
        Comments are welcome! Contact:
 
                Bob Lafleur
                45 Ionia Street
                Springfield, MA  01109
                (413) 737-8503
                CompuServe: 75146,3122
 
        ===============================================================
 
        This program will accept a command line containing compiler switch(es)
        and program name(s) and execute the four passes of the ci c86 compiler.
        A list of files containing errors will appear at the end after all
        programs have been compiled.
 
        Note that this program uses the ANSI sequences to turn the bold
        attribute on and off - if you're not using the ANSI driver, it is
        a simple matter to remove them.
 
        I keep my compiler in "c\exe", if yours is somewhere else, you'll
        have to change the constants below.
 
        ===============================================================
 
        If you come up with any additional enhancements/features for this,
        please let me know!
 
*/
 
 
 
 
 
#include <stdio.h>
 
#define         NODEBUG 1
 
main (argc, argv)
 
int     argc;
char   *argv[];
 
{
    static char cc[4][80] =
    {
        {
            "CC1 -i "   /* -i (long identifiers) is standard! */
        },
        {
            "CC2 "
        },
        {
            "CC3 "
        },
        {
            "CC4 "
        }
    };
    char errlst[30][80];
    int errcnt = 0;
    static char *ccprg[] =
    {
        "C:\\EXE\\CC1.EXE",     /* where the compiler is on disk */
        "C:\\EXE\\CC2.EXE",
        "C:\\EXE\\CC3.EXE",
        "C:\\EXE\\CC4.EXE"
    };
 
    char    fnam[30];
    int     phase;              /* current phase number */
    int     cnt;
    struct
    {
        unsigned int    ax,
                        bx,
                        cx,
                        dx,
                        si,
                        di,
                        ds,
                        es;     /* register struct */
    }       srv;
    struct
    {                           /* param block structure */
        int     env_seg;
        char   *line_off,
               *line_seg;
        char   *fcb1_off,
               *fcb1_seg;
        char   *fcb2_off,
               *fcb2_seg;
    }       ctrl;
 
    unsigned char   cline[128]; /* command line */
    int     status;             /* status return variable */
    int     argcnt;
    extern  FILE * stdout;
 
#ifdef DEBUG
    printf ("CCSML in DEBUG mode with %d arguments\n", argc);
#endif
 
 
    if (argc < 2)
    {
        printf ("cc - ci c86 compile flow generator v1.0\n\n");
        printf ("Usage: cc [-pa] [-pa]... [filename] [filename]...\n");
        printf ("   p        - pass number (1, 2, 3, or 4)\n");
        printf ("   a        - argument for pass p\n");
        printf ("   filename - name of file(s) to compile\n");
    }
    else
    {                           /* ONLY if given FILENAME */
        for (argcnt = 1; argcnt < argc; argcnt++)
        {
            if (argv[argcnt][0] == '-')
            {
                int     cl;
                sscanf (&argv[argcnt][1], "%d", &cl);
                cl--;
#ifdef DEBUG
                printf ("Arg %d is %s\n", argcnt, argv[argcnt]);
#endif
                strcat (cc[cl], "-");
                strcat (cc[cl], &argv[argcnt][2]);
                strcat (cc[cl], " ");
#ifdef DEBUG
                printf ("cc line %d is now %s\n", cl + 1, cc[cl]);
#endif
            }
        }
        for (argcnt = 1; argcnt < argc; argcnt++)
        {
            if (argv[argcnt]00] != '-')
            {
                sscanf (argv[argcnt], "%s", fnam);
 
        /* edit the following line if you're not using the ANSI driver!!! */
 
                fprintf (stdout, "\033[1m...compiling %s\033[0m\n", fnam);
 
                status = 0;     /* setup status */
                for (phase = 0; phase < 4 && status == 0; phase++)
                {
                /*
                 *-- CREATE command line from FILENAME and COMMAND OPTIONS
                 */
                    cline[0] = strlen (fnam) + strlen (cc[phase]);
                    cline[1] = 0;
                    strcat (cline, cc[phase]);
                    strcat (cline, fnam);
                    strcat (cline, "\r");
                /*
                 *-- INSERT into the PARAM block
                 */
                    segread (&srv.si);/* get segment address */
                    ctrl.env_seg = 0;/* NO environment */
                    ctrl.line_off = cline;/* setup command line */
                    ctrl.line_seg = srv.ds;/* segment for this */
 
#ifdef DEBUG
                    {
                        char    temp[128];
                        printf ("Command line length = %u\n", cline[0]);
                        strncpy (temp, &cline[1], cline[0]-1);
                        temp[cline[0]] = 0;
                        printf ("Command line: %s\n", temp);
                        printf ("Filename: %s\n", ccprg[phase]);
                    }
#endif
 
 
                    status = loadexec (ccprg[phase], srv.ds, &ctrl, srv.ds, 0);
                    if (status == 0)/* check REAL status */
                        status = bdos (0x4d);
#ifdef DEBUG
                    printf ("LOADEXEC status: %d\n", status);
#endif
                    if (status) /* there was a compile error */
                     {
                       sprintf(errlst[errcnt++], "%s", fnam);
                       printf("\n");
                     }
                }
            }
        }
      if (errcnt)  /* there was a compile error */
         {
            printf("\n\033[1mThere were errors in the file%s:\n", (errcnt != 1) ? "s" : "");
            for (cnt = 0; cnt < errcnt; cnt++)
               printf("%s\n", errlst[cnt]);
            printf("\033[0m"); /* remove this if you're not using the ANSI driver */
         }
    }
}
/* END */
