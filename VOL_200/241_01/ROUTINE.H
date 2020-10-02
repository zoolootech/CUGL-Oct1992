/*
HEADER:         CUG241;
TITLE:          Rule-Based Compiler for Expert System;
DATE:           12/30/85;
VERSION:
DESCRIPTION:   "Source code for rule-based compiler for an Expert System";
KEYWORDS:       Artificial Intelligence, expert systems, rule-based compiler;
SYSTEM:         MS-DOS or UNIX System V;
FILENAME:       ;
WARNINGS:      "User-supported, non-commercial"
AUTHORS:        George Hageman; 
COMPILERS:      Microsoft C V3.00 or UNIX System V Portable C Compiler;
REFERENCES:     ;
ENDREF
*/

/*****************************************************************
**								**
**	  Inference -- (C) Copyright 1985 George Hageman	**
**								**
**	    user-supported software:				**
**								**
**		    George Hageman				**
**		    P.O. Box 11234				**
**		    Boulder, Colorado 80302			**
**								**
*****************************************************************/

/*
**	these are the two return values
**	which must be returned as the exit
**	value.  Any other will result in
**	an assumption that the result is true
**	or that some sort of error occured.
*/

#define RETURN_ROUTINE_TRUE	254
#define RETURN_ROUTINE_FALSE	255

