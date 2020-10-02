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

char	*keyWords[NUM_KEYWORDS] =
	{
	"AND           ",
	"ANDIF         ",
	"ANDIFRUN      ",
	"ANDNOT        ",
	"ANDNOTRUN     ",
	"ANDRUN        ",
	"ANDTHEN       ", 
	"ANDTHENHYP    ",
	"ANDTHENRUN    ",
	"ANDTHENRUNHYP ",
	"IF            ",
	"IFNOT         ",
	"IFNOTRUN      ",
	"IFRUN         ",
	"THEN          ",
	"THENHYP       ",
	"THENRUN       ",
	"THENRUNHYP    "
	} ;

