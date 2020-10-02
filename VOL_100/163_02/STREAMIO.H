/*
** stream file control blocks
*/
 
#define _IOBSIZE       6       /* number of words in an iob */
#define _IOB_PTR       0       /* next character position (char ptr) */
#define _IOB_CNT       1       /* number of characters left */
#define _IOB_BASE      2       /* location of buffer (char ptr) */
#define _IOB_SEG       3       /* if non-zero, segment of buffer */
#define _IOB_FLAG      4       /* mode of file access - top byte is unget ch */
#define _IOB_FD        5       /* file descriptor */
 
#define _READ          1       /* file open for reading */
#define _WRITE         2       /* file open for writing */
#define _UNBUF         4       /* file is unbuffered */
#define _BIGBUF        8       /* big buffer allocated */
#define _EOF           16      /* EOF has occurred on this file */
#define _ERR           32      /* error has occurred on this file */
#define _DIRTY         64      /* buffer contains write data */
#define _UNGET         128     /* unget character present */
#define NULL           0
#define EOF            (-1)
