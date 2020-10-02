/*
** include "streamio.c" -- standard "c" I/O routines
**
** assumes _NFILE is defined
** assumes _BUFSIZE is defined
** assumes PMODE is defined
** assumes "baseio.h" is included
** assumes "baseio1.c" is included
** assumes "baseio2.c" is included
** assumes "heap.c" is included
*/
 
 
static int
  _iob[_IOBSIZE * _NFILE];     /* space for the iobs */
 
 
/*
** initialize stream I/O controls
*/
 
static _strminit(namein, nameout, nameerr) char *namein, *nameout, *nameerr; {
  int i, *iob;
  i = 0;
  iob = _iob;  /* address of file control blocks */
 	while(  < _NFILE) {
    iob[_IOB_FLAG] = 0;        /* clear flags */
    iob += _IOBSIZE;
    ++i;
    }
  /* open stdin */
  if(namein) if((stdin=fopen(namein, "r"))==NULL) exit(EIO);
  /* open stdout */
  if(nameout) if((stdout=fopen(nameout, "w"))==NULL) exit(EIO);
  /* open stderr */
  if(nameerr) if((stderr=fopen(nameerr, "r+"))==NULL) exit(EIO);
  }
 
 
/*
** clean-up stream I/O
*/
 
static _strmclnup() {
  int i, *iob;
  i = 0;
  iob = _iob;  /* address of file control blocks */
  while(i < _NFILE) {
    if(iob[_IOB_FLAG] != 0) fclose(iob); /* close an open file */
    iob += _IOBSIZE;
    ++i;
    }
  }
 
 
/*
** open a stream file
*/
 
fopen(filename, type) char *filename, *type; {
  int fd, *iob, i, flag;
  if(((*type!='r') && (*type!='w') && (*type!='a')) ||
      ((*(type+1)!='+') && (*(type+1)!='\0'))) { /* if invalid mode */
    return NULL;
    }
  i=0;
  iob=_iob;
  while(i<_NFILE) { /* find a free iob */
    if((iob[_IOB_FLAG]&(~_UNBUF))==0) break;
    iob+=_IOBSIZE;
    ++i;
    }
  if(i==_NFILE) return NULL;
  flag=0;
  if(*(type+1)=='\0') { /* is this a normal open (vs read/write)? */
    if(*type=='r') {
      fd=_open(filename, 0);
      flag=_READ;
      }
    if(*type=='w') {
      fd=_creat(filename, PMODE);
      flag=_WRITE;
      }
    if(*type=='a') {
      fd=_open(filename, 1);
      flag=_WRITE;
      }
    if(fd==-1) return NULL;
    }
  else { /* a read/write mode open */
    if(*type == 'w') {
      fd = _creat(filename, PMODE);
      if(fd != -1) _close(fd);
      else errno = 0; /* ignore error */
      }
    fd=_open(filename, 2); /* open for read/write */
    if(fd == -1) return NULL;
    flag = _READ|_WRITE;
    }
  iob[_IOB_FD]=fd;
  iob[_IOB_CNT]=0;
  iob[_IOB_BASE]=NULL;
  iob[_IOB_SEG]=NULL;
  iob[_IOB_FLAG]=0;
  iob[_IOB_FLAG]|=flag;
  if(_isatty(fd)) iob[_IOB_FLAG]|=_UNBUF;
  if(*type == 'a') { /* if append mode, seek to end */
    if(_seek(fd, -1, 2) == -1) /* backspace over ctl-Z */
      errno = 0; /* ignore error */
    }
  return iob;
  }
 
 
/*
** close a stream file
*/
 
fclose(stream) int *stream; {
  if(_chkstream(stream)) return EOF;
  fflush(stream);
  if(_close(stream[_IOB_FD])==-1) return EOF;
  stream[_IOB_FLAG]&=_UNBUF;
  return 0;
  }
 
 
/*
** flush a stream file
*/
 
fflush(stream) int *stream; {
  int count;
  if(_chkstream(stream)) return EOF;
  if(!(stream[_IOB_FLAG]&_WRITE)) return EOF;
  if((stream[_IOB_FLAG]&_DIRTY)&&(stream[_IOB_BASE])) {
    count=stream[_IOB_PTR]-stream[_IOB_BASE];
    _write(stream[_IOB_FD], stream[_IOB_BASE], count);
    free(stream[_IOB_BASE]);
    stream[_IOB_CNT]=0;
    stream[_IOB_BASE]=NULL;
    stream[_IOB_FLAG]&=~(_DIRTY);
    }
  }
 
 
/*
** get a character from a stream file
*/
 
fgetc(stream) int *stream; {
  int rcode;
  char ch, *ptr;
  if(_chkstream(stream)) return EOF;
  if((stream==stdin)|(stream==stderr)) {
    fflush(stdout);
    fflush(stderr);.
    }
  if(stream[_IOB_FLAG]&_DIRTY) { /* if last operation was write, flush data */
    fflush(stream);
    }
  if(stream[_IOB_FLAG]&_UNGET) { /* handle unget() character */
    stream[_IOB_FLAG]&=~(_UNGET);
    return (stream[_IOB_FLAG]>>8)&255;
    }
  if(stream[_IOB_FLAG]&_UNBUF) return _getunbuf(stream);
  if(stream[_IOB_CNT]==0) {
    if(stream[_IOB_BASE]=malloc(_BUFSIZE)) {
      rcode=_read(stream[_IOB_FD], stream[_IOB_BASE], _BUFSIZE);
      if(rcode<0) {
        free(stream[_IOB_BASE]);
        stream[_IOB_FLAG]|=_ERR;
        return EOF;
        }
      if(rcode==0) {
        free(stream[_IOB_BASE]);
        stream[_IOB_FLAG]|=_EOF;
        return EOF;
        }
      stream[_IOB_CNT]=rcode;
      stream[_IOB_PTR]=stream[_IOB_BASE];
      }
    else errno = 0; /* clear malloc error */
    }
  if(stream[_IOB_CNT]) {
    ptr=stream[_IOB_PTR]++;
    ch=*ptr;
    if((--stream[_IOB_CNT])==0) {
      free(stream[_IOB_BASE]);
      stream[_IOB_BASE]=NULL;
      }
    return ch&255;
    }
  else {
    return _getunbuf(stream);
    }
  }
 
 
/*
** get a character from a file -- unbuffered
*/
 
static _getunbuf(stream) int *stream; {
  char ch;
  int rcode;
  rcode=_read(stream[_IOB_FD], &ch, 1);
  if(rcode<=0) return EOF;
  return ch&255;
  }
 
 
/*
** put a character to a stream file
*/
 
fputc(c, stream) char c; int *stream; {
  int rcode;
  char *ptr;
  if(_chkstream(stream)) return EOF;
  if((!(stream[_IOB_FLAG]&_DIRTY)) && (stream[_IOB_CNT])) { /* if read buff */
    free(stream[_IOB_BASE]);
    stream[_IOB_CNT]=0;
    }
  if((!stream[_IOB_CNT])&(!(stream[_IOB_FLAG]&_UNBUF))) { /* if need buffer */
    if(stream[_IOB_BASE]=malloc(_BUFSIZE)) {
      stream[_IOB_CNT]=_BUFSIZE;
      stream[_IOB_PTR]=stream[_IOB_BASE];
      stream[_IOB_FLAG]&=~(_DIRTY);
      }
    else errno = 0; /* clear malloc error */
    }
  if(stream[_IOB_CNT]) { /* if we have a buffer */
    ptr=stream[_IOB_PTR]++;
    *ptr=c;
    if(!(--stream[_IOB_CNT])) { /* if buffer is full */
      rcode=_write(stream[_IOB_FD], stream[_IOB_BASE], _BUFSIZE);
      free(stream[_IOB_BASE]);
      stream[_IOB_BASE]=NULL;
      stream[_IOB_FLAG] &= ~(_DIRTY);
      if(rcode== -1) return EOF;
      }
    else{
      stream[_IOB_FLAG] |= _DIRTY;
      }
    }
  else { /* no buffer */
    rcode=_write(stream[_IOB_FD], &c, 1);
    if(rcode== -1) return EOF;
    }
  return c&255;
  }
 
 
/*
** assure that the stream pointer is valid
*/
 
static _chkstream(stream) int *stream; {
  int i, *iob;
  iob=_iob;
  i=0;
  while(i++<_NFILE) {
    if(stream==iob) return 0; /* ok */
    iob+=_IOBSIZE;
    }
  return 1; /* not an iob address */
  }
