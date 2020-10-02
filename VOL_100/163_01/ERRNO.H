/*
** include "errno.h"
**
** UNIX-like error codes
*/
 
#define ENOENT 2       /* no such file or directory */
#define EIO    5       /* physical I/O error */
#define ENXIO  6       /* device not available (not mounted) */
#define E2BIG  7       /* argument list too long */
#define EBADF  9       /* bad file number (or wrong file read/write mode) */
#define ENOMEM 12      /* out of main storage (or data seg full) */
#define EACCES 13      /* permission denied */
#define EFAULT 14      /* bad address (or address range) */
#define EEXIST 17      /* file exists */
#define ENODEV 19      /* no such device or invalid mode for device */
#define EINVAL 22      /* invalid argument */
#define ENFILE 23      /* too many open files (system wide) */
#define EMFILE 24      /* too many open files (process) */
#define ENOTTY 25      /* not a terminal device */
#define EFBIG  27      /* file too big (in excess of max possible size) */
#define ENOSPC 28      /* no space left on device (or in device directory) */
#define ESPIPE 29      /* seek to non-seekable device */
#define EROFS  30      /* attempt to write to write-protected device */
#define EDOM   33      /* math argument domain error */
#define ERANGE 34      /* math argument range error */
#define EUSER  100     /* user error codes begin here */
