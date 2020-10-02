
/*
 *      HEADER:         ;
 *      TITLE:          stristr() PROTOTYPE;
 *      DATE:           10/27/1989;
 *      VERSION:        1.0;
 *      FILENAME:       STRISTR.H;
 *      SEE-ALSO:       STRISTR.C, PB.DOC;
 *      AUTHORS:        Michael Kelly;
 */

/*
 *  avoid multiple includes
 */
#ifndef STRISTR_H
#define STRISTR_H 1
#include <string.h>
#include <mem.h>


/*
 *  case insensitive version of strstr() standard string function
 */
char *stristr(const char *string, const char *substr);

#endif
