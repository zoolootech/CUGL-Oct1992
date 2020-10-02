
/* this entire mess -- IN ORDER, AS SHOWN! -- is required to use fstat()
*/
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>


/* dev_t, etc., are declared in sys/types.h
struct stat {
    dev_t st_dev;
    ino_t st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    dev_t st_rdev;
    off_t st_size;  <-- this is the one, and off_t is just a typedef for long!
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
    };
*/

long filesize( FILE *fptr )
{
     struct stat buf;
     fstat( fileno(fptr), &buf );
     return ( (long) buf.st_size );
}

