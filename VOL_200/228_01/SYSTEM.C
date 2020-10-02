/*
HEADER:         CUGXXX;
TITLE:          Lattice V2.13 system call;
DATE:           3-20-86;
DESCRIPTION:    Replaces Lattice V2.13 system call which ignores switchar
                setting before invoking secondary command processor. 
KEYWORDS:       System calls, child process, command processor;
FILENAME:       SYSTEM.C;
WARNINGS:       For dated version of Lattice C;
AUTHORS:        Jeff Spidle;
COMPILER:       Lattice C;
REFERENCES:     US-DISK 1310;
ENDREF
*/
/* replaces the system call that is in the lattice 2.13 libraries
   the old call doesn't figure out what the switchar is before
   trying to invoke a secondary command processor. Thus we have
   built a copy that will do just that.
   Program by Jeff Spidle
	      Office of Continuing Education
	      Iowa State University
	      Ames, IA 50011

*/

#include <dos.h>
char *getenv() ;
int system( command_string )
char *command_string ; /* string to be sent to command.com */
{
union REGS in_regs, out_regs ;

/* now determine what the swichar is */
in_regs.h.ah = 0x37 ;
in_regs.h.al = 0 ;
intdos( &in_regs, &out_regs ) ;
if (out_regs.h.dl == 45) /* have a - for a switchar */
  return( forkl( getenv("COMSPEC"), 0, "-C", command_string, 0)) ;
else
  return( forkl( getenv("COMSPEC"), 0, "/C", command_string, 0)) ;

}
