###do.mp
.ds F USER MANUAL
.NM DO "repetitive timing macros"
.SY
#include "timer1.h"

DO_IEXPR(comment)	\f2\s10integer_expression\f6\s8	OD;

DO_FEXPR(comment)	\f2\s10floating_expression\f6\s8	OD;

DO_STMT(comment)	\f2\s10statement\f6\s8			OD;

DO_SKIP(comment)	\f2\s10statement\f6\s8				OD;
char *comment;
.FU
The DO macro generates a loop around the code sample which
executes it enough times to give 100 clock-tick samples for
the code sample.
The comment is printed on the timing report, along with the
space requirement and CPU time for the code sample.
The DO_SKIP macro simply place-holds a report output line.
.MN
###do.mpo



DO                         USER MANUAL                          DO



NAME
    DO - repetitive timing macro

SYNOPSIS
    #include "timer1.h"

    DO_IEXPR(comment) integer_expression  OD;

    DO_FEXPR(comment) floating_expression OD;

    DO_STMT(comment)  statement           OD;
    char *comment;

	DO_SKIP(comment)	statement		  OD;

FUNCTION
    The DO macro generates a loop around  the  code  sample  which
    executes  it  enough  times to give 100 clock-tick samples for
    the code sample.

    The comment is printed on the timing report,  along  with  the
    space requirement and CPU time for the code sample.

	The DO_SKIP macro simply place-holds a report output line.





































###cputim.mp
.ds F USER MANUAL
.NM cputim "report CPU time since last call"
.SY
#include "cputim.h"
cputim_t cputim()
.FU
.PT cputim
tells how many clock ticks have elapsed since the
previous invocation.
The resolution of the CPU clock is system-dependent.
On UNIX/Idris, it is 1/60 second.
The header \f6"cputim.h"\f1 provides a defined constant named
.PT CLOCK_TICKS_PER_SECOND .
.MN
###cputim.mpo



cputim                     USER MANUAL                      cputim



NAME
    cputim - report CPU time since last call

SYNOPSIS
    long cputim()

    char *returnp(argp)
    char *argp;

FUNCTION
    Cputim tells how many clock ticks have elapsed since the  pre-
    vious  invocations.   Its  format  is intended for portability
    between operating systems.

    Returnp tells the address to which its calling  function  will
    return.  It is passed the address of the first argument of the
    calling function.  The location of the return pointer relative
    to  the  first argument may be different on different systems;
    therefore, the implementation of returnp may need tailoring to
    each system.

BUGS
    The resolution of  the  CPU  clock  is  system-dependent.   On
    UNIX/Idris, it is 1/60 second.



































###returnp.mp
.ll 66
.lt 66
.ds F USER MANUAL
.hy 15
.NM returnp "reveal a function's return pointer"
int *returnp(pa)
int **pa;
.FU
.IT
returnp
tells the address to which its calling function will
return.
The argument passed to
.IT
returnp
is system-dependent; it may be the address of the function's
first argument, or the address of the function's first auto
variable.
The location of the return pointer 
may be different on different systems;
therefore, the implementation of returnp may need tailoring
to each system.
(Some simple customization is found in
.IT
"config.h".)
###returnp.mpo



returnp                    USER MANUAL                     returnp



NAME
    returnp - reveal a function's return pointer

SYNOPSIS
    int *returnp(pa)
    int **pa;

FUNCTION
    returnp_______ tells the address to which its calling  function  will
    return.   The  argument passed to returnp_______ is system-dependent;
    it may be the address of the function's first argument, or the
    address  of  the function's first auto variable.  The location
    of the return pointer may be different on  different  systems;
    therefore, the implementation of returnp may need tailoring to
    each  system.   (Some  simple  customization   is   found   in
    "config______.h_".)











































###EOF
