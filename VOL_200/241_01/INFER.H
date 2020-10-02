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
**	the following are the global common variables which
**	are used in the inference engine...
**
**	all routines except inference.c should have this
**	file included.
*/

#define	MAX_KNOWN	500

int	numHypot, hypStack[MAX_HYPS],strBuffOfst ;
char	strBuff[MAX_STRING_BUFF] ;
int	ruleBuffOfst ;
int	knownTrue[MAX_KNOWN], knownFalse[MAX_KNOWN] ;
int	numTrue, numFalse ;
struct  rule_statement_r ruleBuff[MAX_RULE_STATEMENTS] ;

