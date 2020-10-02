
/***************************************************************************/
/* TIMER - Utility used to time the execution of other MSDOS programs.	   */
/*									   */
/*									   */
/*									   */
/***************************************************************************/
/*			     Modification Log				   */
/***************************************************************************/
/* Version   Date   Programmer	 -----------  Description  --------------- */
/*									   */
/* V01.00   010788  Bob Withers  Program intially complete.		   */
/*									   */
/*									   */
/***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <process.h>
#include <string.h>
#ifdef __TURBOC__
#include <alloc.h>
#else
#include <malloc.h>
#endif

#define MSDOS_GET_TIME		     0x2c


static void    near pascal ErrorMsg(char *);

static short		nStartHour;
static short		nStartMin;
static short		nStartSec;
static short		nStartHund;
static short		nEndHour;
static short		nEndMin;
static short		nEndSec;
static short		nEndHund;
static union REGS	r;
static char		cPathName[64];
static char		cWrk[80];


main(argc, argv)
int	    argc;
char	  **argv;
{
    auto     short	nRetCode;
    auto     char     **pArgs;


    if (argc < 2)
    {
	ErrorMsg("\nError - command usage is:");
	ErrorMsg("        TIMER pgmname [pgmarg ...]");
	ErrorMsg("           ^     ^         ^");
	ErrorMsg("           |     |         |");
	ErrorMsg("           |     |         +-- Optional pgm parameters");
	ErrorMsg("           |     +------------ Name of program to run");
	ErrorMsg("           +------------------ Utility program name");
	return(1);
    }
    strcpy(cPathName, strupr(argv[1]));
    pArgs = (char **) calloc(1, argc * sizeof(char *));
    if (NULL == pArgs)
    {
	ErrorMsg("\nUnable to allocate memory - aborting");
	return(2);
    }
    while (--argc >= 0)
	pArgs[argc] = argv[argc + 1];
    r.h.ah = MSDOS_GET_TIME;
    intdos(&r, &r);
    nStartHour = r.h.ch;
    nStartMin  = r.h.cl;
    nStartSec  = r.h.dh;
    nStartHund = r.h.dl;
    nRetCode = spawnvp(P_WAIT, cPathName, pArgs);
    r.h.ah = MSDOS_GET_TIME;
    intdos(&r, &r);
    nEndHour = r.h.ch;
    nEndMin  = r.h.cl;
    nEndSec  = r.h.dh;
    nEndHund = r.h.dl;
    if (-1 == nRetCode)
    {
	sprintf(cWrk, "Unable to run program %s", cPathName);
	ErrorMsg(cWrk);
	return(3);
    }
    printf("\nProgram started %d:%02d:%02d.%d Ended %d:%02d:%02d.%d\n",
	   nStartHour, nStartMin, nStartSec, nStartHund,
	   nEndHour,   nEndMin,   nEndSec,   nEndHund);
    if (nEndHour < nStartHour)
	nEndHour += 24;
    nEndHund -= nStartHund;
    if (nEndHund < 0)
    {
	nEndHund += 99;
	nEndSec--;
    }
    nEndSec -= nStartSec;
    if (nEndSec < 0)
    {
	nEndSec += 59;
	nEndMin--;
    }
    nEndMin -= nStartMin;
    if (nEndMin < 0)
    {
	nEndMin += 59;
	nEndHour--;
    }
    nEndHour -= nStartHour;
    printf("Elapsed time %d hours, %d minutes, %d.%02d seconds\n",
	   nEndHour,   nEndMin,   nEndSec,   nEndHund);
    free((char *) pArgs);
    return(0);
}


static void near pascal ErrorMsg(pStr)
char	    *pStr;
{
    fprintf(stderr, "%s\n", pStr);
    return;
}
