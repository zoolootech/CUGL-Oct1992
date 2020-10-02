/*
HEADER:         CUG241;
TITLE:          Simple weather predictor expert system to demonstrate
                the IFRUN, ANDTHENRUN, etc., keywords. 
DATE:           12/30/85;
VERSION:
DESCRIPTION:   "Source code for a simple Expert System.";
KEYWORDS:       Artificial Intelligence, expert systems, weather predictor;
SYSTEM:         MS-DOS or UNIX System V;
FILENAME:       SLO_FALL.C;
WARNINGS:      "User-supported, non-commercial"
AUTHORS:        George Hageman; 
COMPILERS:      Microsoft C V3.00 or UNIX System V Portable C Compiler;
REFERENCES:     ;
ENDREF
*/

#include <stdio.h>
#include "routine.h"
#include "weather.h"

main()
{
FILE *dataFile ;
int	value[20] ;

dataFile = fopen("WEATHER.DAT","rb") ;
fread(value,2,20,dataFile) ;
fclose(dataFile) ;
if(value[1]==FALL_SLOW)
	return(RETURN_ROUTINE_TRUE) ;
else
	return(RETURN_ROUTINE_FALSE) ;
}

