/*
	Dummy Header file for Sherlock.

	This file contains null macro definitions for all Sherlock macros.

	Include this file in every file that contains any Sherlock macro.

	source:  dummysl.h (rename this file to be sl.h)
	started: January 10, 1990
	version: January 10, 1990

	Written by
	
		Edward K. Ream
		166 N. Prospect
		Madison WI 53705
		(608) 257-0802


	PUBLIC DOMAIN SOFTWARE

	This file is in the public domain.
*/

#define SL_VERSION_NAME "1.5"

#define LEAVE(a)
#define ENTER(a)
#define ENTER_TRACE(a,b)

#define SL_DISABLE()
#define SL_ENABLE()

#define RETURN_BOOL(a,b)	return b
#define RETURN_CHAR(a,c)	return c
#define RETURN_DOUBLE(a,d)	return d
#define RETURN_FLOAT(a,f)	return f
#define RETURN_INT(a,i)		return i
#define RETURN_LONG(a,l)	return l
#define RETURN_PTR(a,p)		return p
#define RETURN_STRING(a,s)	return s
#define RETURN_UINT(a,u)	return u
#define RETURN_ULONG(a,u)	return u
#define RETURN_VOID(a)		return

#define SL_CLEAR()
#define SL_DUMP()
#define SL_INIT()
#define SL_NAME(a,b)
#define SL_OFF(s)
#define SL_ON(s)
#define SL_PARSE(argc,argv,on,off)

#define STAT(a)
#define STATB(a)
#define STATX(a)

#define TICK(a)
#define TICKB(a)
#define TICKN(a)

#define TICKX(a)
#define TICKX_BOOL(a,b)
#define TICKX_CHAR(a,c)
#define TICKX_DOUBLE(a,d)
#define TICKX_FLOAT(a,f)
#define TICKX_INT(a,i)
#define TICKX_LONG(a,l)
#define TICKX_PTR(a,p)
#define TICKX_STRING(a,s)
#define TICKX_UINT(a,u)
#define TICKX_ULONG(a,u)
#define TICKX_VOID(a)

#define TRACE(a,b)
#define TRACEB(a,b)
#define TRACEN(a,b)
#define TRACEX(a,b)

#define TRACEP(a,b)
#define TRACEPB(a,b)
#define TRACEPN(a,b)
#define TRACEPX(a,b)
