/*-------------------------------------------------------------------*/
/*	LIBRARY FILE:     PRINTERR.C
	------------
	WRITTEN:          26/10/87
	-------
	PURPOSE:          Prints the message corresponding to the error
	-------           code returned by a function.

	USAGE:            void PRINTERR(errcode)
	-----             int errcode;

	DEPENDENCIES:     De Smet C V 2.44+
	------------
	Copyright 1987 - Cogar Computer Services Pty. Ltd                */
/*-------------------------------------------------------------------*/
#include <stdio.h>
#include <c:errlist.c>

void printerr(errcode)
int errcode;
{
	if (abs(errcode) >= errtablesize)  /* Then error code is unknown */
	{
		printf("\nError Code - %d - is unknown to us.....bye!\n", errcode);
		exit(0);
	}
	else errcode = abs(errcode);
	printf("\n%s\n", errlist[errcode]);
}
/*-------------------------------------------------------------------*/
