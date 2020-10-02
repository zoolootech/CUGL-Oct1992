/*
HEADER:         CUG241;
TITLE:          Simple weather predictor expert system to demonstrate
                the IFRUN, ANDTHENRUN, etc., keywords. 
DATE:           12/30/85;
VERSION:
DESCRIPTION:   "Source code for a simple Expert System.";
KEYWORDS:       Artificial Intelligence, expert systems, weather predictor;
SYSTEM:         MS-DOS or UNIX System V;
FILENAME:       ROUTINE.H;
WARNINGS:      "User-supported, non-commercial"
AUTHORS:        George Hageman; 
COMPILERS:      Microsoft C V3.00 or UNIX System V Portable C Compiler;
REFERENCES:     ;
ENDREF
*/

/*
**	these are the two return values
**	which must be returned as the exit
**	value.  Any other will result in
**	an assumption that the result is true
**	or that some sort of error occured.
*/

#define RETURN_ROUTINE_TRUE	254
#define RETURN_ROUTINE_FALSE	255

