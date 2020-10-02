/*ib------------------------------------------------------------------*/
/*  FILE NAME:      trace.h.
 *  FILE DESCR:     Macros for Tracing.
 *  PROJECT NAME:   general.
 *  PROJECT DESCR:  general definitions and functions.
 *  SOURCE LANG:    c.
 *  SOURCE VARIANT: Lattice C/6.0.
 *  AUTHOR:         Bill Rogers.
 *  DATE WRITTEN    89/11/19.
 *  COPYRIGHT:      None.
 *  GENERAL DESCR:
 *      These macros are activated by the compiler command line
 *      argument:
 *
 *          -dTRACE
 *
 *      The forms of the printed line to "stderr" are:
 *
 *          <file>:<line>:<func>:Begin: RCsid=
 *              <rcsid>
 *          <file>:<line>:<func>:<type>:<var name>=<var value>
 *          <file>:<line>:<func>:End
 *
 *      The macros are:
 *
 *          T_FUNC(x)   Define function name
 *
 *          T_BEGIN()   Display function "begin"
 *          T_END()     Display function "end"
 *
 *          T_BOOL(x)   Display boolean
 *          T_CHAR(x)   Display character
 *          T_DBL(x)    Display double
 *          T_HEX(x)    Display unsigned integer in hexadecimal
 *          T_INT(x)    Display integer
 *          T_FLOAT(x)  Display float
 *          T_LHEX(x)   Display unsigned long integer in hexadecimal
 *          T_LONG(x)   Display long
 *          T_PTR(x)    Display pointer as segment:offset
 *          T_STR(x)    Display string
 */
#ifndef trace_h
#define trace_h
/*  RCS ID:         */
        const static char rcsid_trace_h[] = 
        "$Id: trace.h 1.10 89/11/24 14:42:36 Bill_Rogers Exp $";
/*ie------------------------------------------------------------------*/

#if defined(TRACE)

#include    <stdio.h>
#include    <string.h>

/*  Predefined, as These Fields Appear on Every Line */

#define TT_PR       fprintf(stderr,"%-10s:%5d:%-12s:%-5s "
#define TT_AR       __FILE__,__LINE__,TT_FUNC

/*  "Trace" Macros */

#define T_FUNC(x)   const static char TT_FUNC[] = # x ;

#define T_BEGIN()   TT_PR "%-16s=\n    %-s\n" ,TT_AR,"begin","RCsid",\
                        RCsid);
#define T_END()     TT_PR "\n"                ,TT_AR,"end"  );

#define T_BOOL(x)   TT_PR "%-16s=%s\n"        ,TT_AR,"bool" ,# x,\
                        x == 0 ? "FALSE" : "TRUE");
#define T_CHAR(x)   TT_PR "%-16s='%c'\n"      ,TT_AR,"char" ,# x,\
                        x >= ' ' ? x : '.');
#define T_DBL(x)    TT_PR "%-16s=%+.15e\n"    ,TT_AR,"dbl"  ,# x,x);
#define T_HEX(x)    TT_PR "%-16s=%04X\n"      ,TT_AR,"hex"  ,# x,x);
#define T_INT(x)    TT_PR "%-16s=%+6d\n"      ,TT_AR,"int"  ,# x,x);
#define T_FLOAT(x)  TT_PR "%-16s=%+e\n"       ,TT_AR,"float",# x,x);
#define T_LHEX(x)   TT_PR "%-16s=%08lX\n"     ,TT_AR,"lhex" ,# x,x);
#define T_LONG(x)   TT_PR "%-16s=%+10ld\n"    ,TT_AR,"long" ,# x,x);
#define T_PTR(x)    TT_PR "%-16s=%0lP\n"      ,TT_AR,"ptr"  ,# x,x);
#define T_STR(x)    TT_PR "%-16s=\n    '%s'\n",TT_AR,"str"  ,# x,x);

#else

#define T_FUNC(x)

#define T_BEGIN()
#define T_END()

#define T_BOOL(x)
#define T_CHAR(x)
#define T_DBL(x) 
#define T_HEX(x) 
#define T_INT(x) 
#define T_FLOAT(x)
#define T_LHEX(x) 
#define T_LONG(x) 
#define T_PTR(x)  
#define T_STR(x)  

#endif      /* TRACE */

#endif      /* trace_h */
/*--------------------------------------------------------------------*/
