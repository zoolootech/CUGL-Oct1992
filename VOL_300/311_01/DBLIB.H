/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      dblib.h  v1.3  (c) 1990  Ken Harris                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*      This software is made available on an AS-IS basis. Unrestricted     */
/*      use is granted provided that the copyright notice remains intact.   */
/*      The author makes no warranties expressed or implied.                */
/*                                                                          */
/****************************************************************************/

/*
 *  This file contains header information for building the DB library
 *  routines. It does not need to be included in applications. It defines 
 *  the right set of include files for each configuration.
 */

#include <stdio.h>

/*
 * Microsoft C V5.10
 */
#ifdef MSC
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#endif

/*
 * Turbo C V2.0
 */
#ifdef TURBO
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#endif

/*
 * DEC Ultrix gcc V1.36
 */
#ifdef ULTRIX
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#endif

/*
 * SYSV - 3b1 rel 3
 */
#ifdef SYSV
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#endif

#include "db.h"


void *memcpy();
