/****************************************************************************/
/* file name: error.c                                                       */
/* requires error.h                                                         */
/*  (c) by Ronald Michaels. This program may be freely copied, modified,    */
/*  transmitted, or used for any non-commercial purpose.                    */
/* this file contains the error messages for bp.c                           */
/****************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include"error.h"

#ifdef ECO
     #include<stdlib.h>             /*  required for eco-c compiler  */
#endif

char *err_mgs[] = {   /* array of pointers to error messages  */
     "\n\rCANNOT OPEN FILE bp1.dat",
     "\n\rCANNOT OPEN FILE bp2.dat",
     "\n\rCANNOT OPEN FILE bp3.dat",
     "\n\rERROR READING FILE bp1.dat",
     "\n\rERROR READING FILE bp2.dat",
	  "\n\rERROR WRITING TO bp3.dat",
     "\n\rINSUFFICIENT MEMORY, alloc FAILURE",
     };

/***************************************************************************/
/*  error                                                                  */
/*  function prints error messages and closes program in the event of      */
/*  some malfunction                                                       */
/*  inspired by a function in The C Compendium by Lawrence and England     */
/***************************************************************************/

void error(
   int err_no,                    /*  error number  */
   int err_type                   /*  error type  */
)
{
	if(err_no>=0 && err_no<=NO_OF_ERRORS){
      printf("\n\r%s",err_mgs[err_no]);
	}
	else{
		printf("\n\rINVALID ERROR NUMBER");
	}
	if(err_type==FATAL){
		printf("\n\rFATAL ERROR IN PROGRAM, TERMINATING");
		exit(1);
	}
	if(err_type==WARN){
		printf("\n\rWARNING, PRESS ANY KEY TO CONTINUE");
		getch();
	}
	else{
		printf("\n\rINVALID ERROR TYPE, TERMINATING");
		exit(1);
	}
}

