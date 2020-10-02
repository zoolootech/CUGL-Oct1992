/*

GETOPT                                                  GETOPT


NAME
        getopt - get option letter from argv

SYNOPSIS
        int getopt(argc, argv, optstring)
        int argc;
        char **argv;
        char *optstring;

        extern char *optarg;
        extern int optind, opterr;

DESCRIPTION
        Getopt returns the next option letter in argv that matches a letter
        in optstring.  Optstring is a string of recognized option letters;
        if a letter is followed by a colon, the option is expected to have
        an argument that may or may not be seperated from it by white space.
        Optarg is set to point to the start of the option argument on
        return from getopt.

        Getopt places in optind the argv index of the next argument to be
        processed.  Because optind is external, it is normally initialized
        to zero automatically before the first call to getopt.

        When all options have been processed (i.e. up to the first non
        option argument), getopt returns EOF.  The special option -- may
        be used to delimit the end of the options;  EOF will be returned
        and -- will be skipped.

DIAGNOSTICS
        Getopt prints an error message on stderr and returns a question
        mark (?) when it encounters an option letter not included in
        optstring.

EXAMPLE
        The following code fragment shows how one might process the arg-
        uments for a command that can take the mutually exclusive options
        a and b, and the options f and o, both of which require arguments:

        main(argc,argv)
        int argc;
        char **argv;
        {
                int c;
                extern char *optarg;
                extern int optind;
                .
                .
                .
                while ((c = getopt(argc,argv,"abf:o:")) != EOF)
                        switch (c) {
                        case 'a':
                                if (bflg)
                                        errflg++;
                                else
                                        aflg++;
                                break;
                        case 'b':
                                if (aflg)
                                        errflg++;
                                else
                                        bflg++;
                                break;
                        case 'f':
                                ifile = optarg;
                                break;
                        case 'o':
                                ofile = optarg;
                                break;
                        case '?':
                        default:
                                errflg++;
                                break;
                        }
                if (errflg) {
                        fprintf(stderr,"Usage...");
                        exit(2);
                }

                for ( ; optind < argc; optind++) {
                        if (argv[optind]...
                        .
                        .
                        .
}
******************************************************************************/

#include <stdio.h>

/*
 * get option letter from argument vector
 */
int     opterr = 1,             /* useless, never set or used */
        optind = 1,             /* index into parent argv vector */
        optopt;                 /* character checked for validity */
char    *optarg;                /* argument associated with option */

#define BADCH   (int)'?'
#define EMSG    ""
#define tell(s) fputs(*nargv,stderr);fputs(s,stderr); \
                fputc(optopt,stderr);fputc('\n',stderr);return(BADCH);

getopt(nargc,nargv,ostr)
int     nargc;
char    **nargv,
        *ostr;
{
        static char     *place = EMSG;  /* option letter processing */
        register char   *oli;           /* option letter list index */
        char    *strchr();

        if(!*place) {                   /* update scanning pointer */
                if(optind >= nargc || *(place = nargv[optind]) != '-' || !*++place) return(EOF);
                if (*place == '-') {    /* found "--" */
                        ++optind;
                        return(EOF);
                }
        }                               /* option letter okay? */
        if ((optopt = (int)*place++) == (int)':' || !(oli = strchr(ostr,optopt))) {
                if(!*place) ++optind;
                tell(": illegal option -- ");
        }
        if (*++oli != ':') {            /* don't need argument */
                optarg = NULL;
                if (!*place) ++optind;
        }
        else {                          /* need an argument */
                if (*place) optarg = place;     /* no white space */
                else if (nargc <= ++optind) {   /* no arg */
                        place = EMSG;
                        tell(": option requires an argument -- ");
                }
                else optarg = nargv[optind];    /* white space */
                place = EMSG;
                ++optind;
        }
        return(optopt);                 /* dump back option letter */
}
