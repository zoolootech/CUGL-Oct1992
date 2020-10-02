/*      stdio.h 1.5     83/08/11
 *		fixed for DRI C for CP/M68K
 */

#define BUFSIZ  512
#define _NFILE  16
typedef struct  _iobuf {
	int	_file;		/* file descriptor	*/
	int	_flag;
        char    *_base;
        char    *_ptr;
        int     _cnt;
} FILE;
extern FILE _iob[_NFILE];

#define stdin  (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])

#define _IOREAD	01			/* file open for reading	*/
#define _IOWRT	02			/* file open for writing	*/
#define _IOABUF	04			/* alloc'd buffer		    */
#define _IOMYBUF        04
#define _IONBUF	010			/* file is unbuffered		*/
#define _IOERR	020			/* error has occurred on this file  */
#define _IOEOF	040			/* EOF has occurred on this file    */
#define _IOLBUF 0100			/* handle as line buffer	    */
#define _IOSTRI	0200			/* this stream is really a string   */
#define _IOSTRG 0200
#define _IOASCI	0400			/* this was opened as an ascii file */

/* the following is not defined in CP/M68K
	#define _IORW   0400
 */

#define NULL    0L
#define FILE    struct _iobuf
#define EOF     (-1)

#define clearerr(p) ((p)->_flag &= ~_IOERR) /* clear error flag		    */
#define feof(p) ((p)->_flag & _IOEOF)	/* EOF encountered on stream	    */
#define ferror(p) ((p)->_flag & _IOERR)	/* error encountered on stream	    */
#define fileno(p) ((p)->_file)		/* get stream's file descriptor	    */
#define getchar() fgetc(stdin)		/* get char from stdin 		    */
#define putchar(c) fputc(c,stdout)	/* put char to stdout		    */
#define putc fputc
#define getc fgetc

FILE    *fopen();
