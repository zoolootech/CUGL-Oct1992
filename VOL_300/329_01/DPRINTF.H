/*  >  DPRINTF.H
 *
 *  dprintf -- Header
 *  (C)  April 4  1990  Asaf Arkin
 *  All rights reserved
 */



/*  Uncomment this, if vdprintf is to be used.
 */
#include  <stdarg.h>
#define  STDARG_Included



typedef  int (*dprintf_fp)(int);
int    dprintf(dprintf_fp, const char *, ...);
#ifdef  STDARG_Included
int   vdprintf(dprintf_fp, const char *, va_list Args);
#endif


