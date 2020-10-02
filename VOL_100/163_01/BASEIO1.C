/*
** include baseio1.c -- base I/O routines part 1
**
** simulate UNIX I/O interfaces
**
** assumes that "errno.h" is included
** assumes that "sgtty.h" is included
** assumes that "doscall.h" is included
** assumes that "doscall.c" is included
** assumes that "heap.c" is included
** assumes that NUMFILES is defined
*/
 
 
/*
** file descriptor (handle) array
*/
 
static int             /* would char *_filedes[] if array of * supported */
  _filedes[NUMFILES];
 
 
/*
** keyboard buffer
*/
 
static char
  kbdbuf[255];
 
 
/*
** base I/O initialization
*/
 
static _baseinit() {
  int i;
  i = 0;
  while(i < NUMFILES) {
    _filedes[i] = 0; /* clear pointer to DOS FCB in file descriptor array */
    ++i;
    }
  kbdbuf[0]=kbdbuf[1]=0;
  errno = 0;
  }
 
 
/*
** base I/O clean-up
*/
 
static _baseclnup() {
  int i;
  char *fcb;
  i = 0;
  while(i < NUMFILES) {
    fcb = _filedes[i]; /* get address of DOS FCB */
    if(*fcb != 0) _close(i); /* if file is open, close it */
    ++i;
    }
  }
 
 
/*
** open a file
*/
 
_open(name, mode) char *name; int mode; {
 
  /*
  ** name -- path name
  ** mode -- 0 - read, 1 - write, 2 - read/write
  ** returns file descriptor
  ** note:  always positions file to offset zero
  */
 
  int filedes, mask;
  char *fcb;
  if((mode < 0) | (mode > 2)) {
    errno = EINVAL;   /* invalid argument */
    return -1;
    }
  filedes = _getfcb(name);     /* allocate & initialize DOS FCB */
  if(filedes < 0) {
    errno = EMFILE;   /* too many open files */
    return -1;
    }
  fcb = _filedes[filedes];     /* get address of DOS FCB */
  fcb[FCB_MODE] = mode;        /* save read/write mode */
  if(mode==0)      mask=2;     /* read */
  else if(mode==1) mask=1;     /* write */
  else             mask=3;     /* read/write */
  if((mask&fcb[FCB_FLAG])!=mask) { /* if wrong device type */
    _freefcb(filedes); /* free the FCB we won't be using */
    errno = ENODEV;
    return -1;
    }
  if((fcb[FCB_FLAG]&255)==255) { /* if disk file */
    if(_dosfcall(F_OPEN, fcb) == 0) { /* DOS open */
      fcb[FCB_LRECL] = 1;        /* set "record length" to 1 */
      fcb[FCB_LRECL+1] = 0;
      fcb[FCB_FLGS] = 128 + 64 + 16;
      fcb[FCB_FLGS+1] = 0;
      return filedes;
      }
    else {
      _freefcb(filedes); /* free the FCB we won't be using */
      errno = ENOENT;   /* no such file */
      return -1;
      }
    }
  else {                       /* not a disk file */
    if(fcb[FCB_FLAG] & 192) { /* if keyboard or screen */
      fcb[FCB_FLGS] = 128 + 64 + 16 + 8;
      fcb[FCB_FLGS+1] = 0;
      }
    else if(fcb[FCB_FLAG] & 32) { /* if printer */
      fcb[FCB_FLGS] = 16;
      fcb[FCB_FLGS+1] = 0;
      }
    else if(fcb[FCB_FLAG] & 16) { /* if async */
      fcb[FCB_FLGS] = 128 + 64 + 32;
      fcb[FCB_FLGS+1] = 0;
      }
    return filedes;
    }
  }
 
 
/*
** create and open a file (or reopen if existing)
*/
 
_creat(name, mode) char *name; int mode; {
 
  /*
  ** name -- path name
  ** mode -- file attributes -- 0 - normal, 2 - system, 4 hidden
  ** returns file descriptor
  ** note:  always positions file to offset zero
  ** note:  file is always opened for write
  */
 
  int filedes;
  char *fcb;
  if((mode < 0) | (mode > 2)) {
    errno = EINVAL;   /* invalid argument */
    return -1;
    }
  filedes = _getfcb(name);     /* allocate & initialize DOS FCB */
  if(filedes < 0) {
    errno = EMFILE;   /* too many open files */
    return -1;
    }
  fcb = _filedes[filedes];     /* get address of DOS FCB */
  if((fcb[FCB_FLAG]&255)!=255) { /* if not a disk file */
    _freefcb(filedes); /* free the FCB we won't be using */
    return _open(name, 1);     /* use normal open for write */
    }
  fcb[FCB_ATTR] = mode;        /* set file attributes */
  if(_dosfcall(F_CREATE, fcb) == 0) {       /* DOS create & open */
    fcb[FCB_LRECL] = 1;        /* set "record length" to 1 */
    fcb[FCB_LRECL+1] = 0;
    fcb[FCB_FLGS] = 128 + 64 + 16;
    fcb[FCB_FLGS+1] = 0;
    fcb[FCB_MODE] = 1;         /* write only */
    return filedes;
    }
  else {
    _freefcb(filedes); /* free the FCB we won't be using */
    errno = ENOSPC;   /* no space in disk directory */
    return -1;
    }
  }
 
 
/*
** close a file
*/
 
_close(filedes) int filedes; {
  char *fcb;
  int *ibuf;
  if((filedes >= NUMFILES) | (filedes < 0)) {
    errno = EINVAL;   /* invalid argument */
    return -1;
    }
  fcb = _filedes[filedes];     /* get address of DOS FCB */
  if(fcb[FCB_FLAG] == 0) {
    errno = EBADF;    /* bad file number -- file not open */
    return -1;
    }
  if((fcb[FCB_FLAG]&255) == 255) { /* if file is standard disk file */
    /* if file is DOS format output (only) file, add the ctl-Z (end-of-file) */
    if((fcb[FCB_FLGS]&16) && (fcb[FCB_MODE]&1)) {
      _seek(filedes, 0, 5); /* find end of file */
      _write(filedes, "\32", 1); /* add ctl-Z */
      _iflush(fcb); /* if there is an intermediate buf, flush (ignore errors) */
      ibuf = &fcb[FCB_IBUF];
      if(*ibuf) free(*ibuf); /* free intermediate buffer (if any) */
      }
    if(_dosfcall(F_CLOSE, fcb) == 0) { /* DOS close */
      _freefcb(filedes); /* free the DOS FCB */
      return 0;
      }
    else {
      errno = ENXIO; /* wrong disk may be mounted */
      return -1;
      }
    }
  else {
    if(fcb[FCB_FLAG] & 32) { /* if printer */
      _prnout('\f');
      }
    _freefcb(filedes); /* free the DOS FCB */
    return 0;
    }
  }
 
 
/*
** read from a file
*/
 
_read(filedes, buffer, nbytes) int filedes, nbytes; char *buffer; {
  char *fcb, ch, mode, *source, *dest;
  int rcode, rbytes, count;
  if((filedes >= NUMFILES) | (filedes < 0)) {
    errno = EINVAL;   /* invalid argument */
    return -1;
    }
  if(nbytes <= 0) {
    errno = EINVAL;   /* invalid argument */
    return -1;
    }
  fcb = _filedes[filedes];     /* get address of DOS FCB */
  if(fcb[FCB_FLAG] == 0) {
    errno = EBADF;    /* bad file number -- file not open */
    return -1;
    }
  if(fcb[FCB_MODE] & 128) {  /* if end-of-file exists */
    return 0;        /* report end-of-file */
    }
  if((fcb[FCB_FLAG]&255) == 255) { /* if file is standard disk file */
    if(fcb[FCB_MODE] & 1) {    /* if not read-mode file */
      errno = EBADF;  /* bad file number -- file not read-mode */
      return -1;
      }
    _dosfcall(F_SETDTA, buffer);    /* set disk transfer area */
    rcode = _dosfxcall(F_READ, fcb, nbytes, &rbytes); /* read the file */
    if(rcode==2) {
      errno = EFAULT; /* bad address range (segment wrap) */
      return -1;
      }
    if((rcode==1) | (rcode==3)) {       /* if end-of-file */
      fcb[FCB_MODE] |= 128;    /* remember end-of-file for next call */
      }
    if(fcb[FCB_FLGS] & 16) { /* if CR-LF conversion */
      source = buffer;
      dest = buffer;
      count = rbytes;
      rbytes = 0;
      mode = fcb[FCB_MODE];
      while(count--) {
        if(*source == '\32') { /* check for ctl-Z (EOF) */
          fcb[FCB_MODE] |= 128; /* set EOF flag */
          break;
          }
        else if(*source == '\r') { /* check for CR */
          if(mode & 32) { /* if prior char was LF */
            mode &= ~32; /* ignore the CR */
            }
          else { /* prior char was not LF */
            *dest++ = '\n'; /* convert CR to LF */
            ++rbytes;
            mode |= 64; /* ignore possible following LF */
            }
          }
        else if(*source == '\n') { /* check for LF */
          if(mode & 64) { /* if prior char was CR */
            mode &= ~64; /* ignore the LF */
            }
          else { /* prior char was not CR */
            *dest++ = '\n'; /* "convert" LF to LF */
            ++rbytes;
            mode |= 32; /* ignore possible following CR */
            }
          }
        else{ /* none of the above */
          *dest++ = *source; /* copy the character */
          ++rbytes;
          mode &= ~(64 + 32); /* turn off CR and LF flags */
          }
        ++source; /* next source char */
        } /* end of loop */
      fcb[FCB_MODE] = mode; /* save CR & LF flags */
      }
    return rbytes;
    }
  else {
    if(fcb[FCB_FLAG] & 128) { /* if keyboard */
      dest = buffer;
      rbytes = 0;
      if(fcb[FCB_FLGS] & 34) { /* if raw or half-baked */
        while(rbytes < nbytes) {
          ch = _kbdinc(fcb); /* read a character */
          if(fcb[FCB_FLGS] & 4) { /* if fold upper to lower case */
            if((ch >= 'A') & (ch <= 'Z')) ch -= ('A' - 'a');
            }
          if(ch=='\r') {
            if(fcb[FCB_FLGS] & 16) _scrnout('\n');
            ch = '\n';
            }
          *dest++ = ch;
          ++rbytes;
          if(ch=='\n') break;
          }
        }
      else { /* read keyboard a line at a time */
        if(kbdbuf[0]==kbdbuf[1]) { /* if buffer is empty, fill it */
          kbdbuf[0] = 255; /* size of buffer */
          _dosfcall(F_BUFKBD, kbdbuf); /* read a line from kbd */
          kbdbuf[0] = 0; /* number of characters read from buffer */
          kbdbuf[2+(kbdbuf[1]&255)] = '\n'; /* replace CR with LF */
          kbdbuf[1] += 1; /* make character count include LF */
          if(fcb[FCB_FLGS] & 16) _scrnout('\n');
          }
        while(rbytes < nbytes) { /* read characters from buffer until enough */
          ++rbytes;
          if((*dest++ = kbdbuf[2+(kbdbuf[0]++&255)])=='\n') break;
          if(kbdbuf[0]==kbdbuf[1]) break;
          }
        }
      /* check for ctl-Z (EOF) */
      source = buffer;
      count = rbytes;
      rbytes = 0;
      while(count--) {
        if(*source++ == '\32') { /* if ctl-Z found */
          fcb[FCB_MODE] |= 128; /* set EOF flag */
          break;
          }
        ++rbytes;
        }
      }
    else if(fcb[FCB_FLAG] & 16) { /* if async */
      ; /* read characters from the async port */
      }
    if(fcb[FCB_FLGS] & 4) { /* if UPPER -> lower conversion */
      source = buffer;
      count = rbytes;
      while(count--) {
        if((*source >= 'A') & (*source <= 'Z')) {
          *source += ('a' - 'A');
          }
        ++source;
        }
      }
    return rbytes;
    }
  }
 
 
/*
** read a character from the keyboard
**
** reads with/without CTRL-BREAK check
** and with/without echo
*/
 
static char
  AL2;
static _kbdinc(fcb) char fcb[]; {
  char ch;
  if(fcb[FCB_FLGS] & 32) { /* if raw mode */
    if(fcb[FCB_FLGS] & 8) { /* if echo */
#asm
       MOV AH,6        ; direct console I/O
       MOV DL,0FFH     ; we are doing input
       INT 021H        ; call DOS
       JZ $-2          ; loop until character received
       MOV QAL2,AL     ; put the character where we can find it
#endasm
      ch = AL2;
      }
    else { /* no echo */
      ch = _dosfcall(F_DIRNOE, 0);  /* direct read with no echo */
      }
    }
  else {       /* half-baked mode */
    if(fcb[FCB_FLGS] & 8) { /* if echo */
      ch = _dosfcall(F_KBDECH, 0);  /* read with echo */
      }
    else { /* no echo */
      ch = _dosfcall(F_KBDNOE, 0);  /* read with no echo */
      }
    }
  return ch;
  }
 
 
/*
** write to a file
*/
 
_write(filedes, buffer, nbytes) int filedes, nbytes; char *buffer; {
  char *fcb, ch, *source;
  int rcode, rbytes, *ibuf, *iptr, *icnt, count;
  if((filedes >= NUMFILES) | (filedes < 0)) {
    errno = EINVAL;   /* invalid argument */
    return -1;
    }
  if(nbytes < 0) { /* zero causes extension/truncation at current position */
    errno = EINVAL;   /* invalid argument */
    return -1;
    }
  fcb = _filedes[filedes];     /* get address of DOS FCB */
  if(fcb[FCB_FLAG] == 0) {
    errno = EBADF;    /* bad file number -- file not open */
    return -1;
    }
  if((fcb[FCB_FLAG]&255) == 255) { /* if file is standard disk file */
    if((fcb[FCB_MODE]&3)==0) {    /* if not write-mode file */
      errno = EBADF;  /* bad file number -- file not write-mode */
      return -1;
      }
    if(fcb[FCB_FLGS] & 16) { /* if CR-LF translation */
      ibuf = &fcb[FCB_IBUF];   /* poor man's cast */
      iptr = &fcb[FCB_IPTR];
      icnt = &fcb[FCB_ICNT];
      if(!*ibuf) { /* if intermediate buffer is not present */
        if(!(*ibuf = *iptr = malloc(_BUFSIZE))) errno = 0;
        *icnt = _BUFSIZE;
        }
      count = nbytes;
      source = buffer;
      rcode = 0;
      while((count--) && !rcode) { /* process buffer into intermediate */
        if(*source == '\n') { /* if char is LF (NL) */
          rcode = _iwrite(fcb, '\r'); /* write a CR */
          if(!rcode) rcode = _iwrite(fcb, '\n'); /* write a LF */
          }
        else { /* char is not LF */
          rcode = _iwrite(fcb, *source); /* copy to intermediate */
          }
        ++source;
        }
      }
    else { /* no CR-LF translation */
      _dosfcall(F_SETDTA, buffer);    /* set disk transfer area */
      rcode = _dosfxcall(F_WRITE, fcb, nbytes, &rbytes); /* write the file */
      }
    if(rcode==1) {
      errno = ENOSPC; /* no room on diskette */
      return -1;
      }
    if(rcode==2) {
      errno = EFAULT; /* bad address range (segment wrap) */
      return -1;
      }
    }
  else {
    rbytes = 0;
    if(fcb[FCB_FLAG] & 64) { /* if screen */
      while(rbytes<nbytes) {
        ch = *buffer++;
        if((ch=='\n') && (fcb[FCB_FLGS]&16)) _scrnout('\r');
        _scrnout(ch);
        ++rbytes;
        }
      }
    else if(fcb[FCB_FLAG] & 32) { /* if printer */
      while(rbytes<nbytes) {
        ch = *buffer++;
        if((ch=='\n') && (fcb[FCB_FLGS]&16)) _prnout('\r');
        _prnout(ch);
        ++rbytes;
        }
      }
    else if(fcb[FCB_FLAG] & 16) { /* if async */
      ; /* output line to the async port */
      }
    }
  return nbytes;
  }
 
 
/*
** write a character to the intermediate buffer
*/
 
static _iwrite(fcb, ch) char *fcb, ch; {
  int *ibuf, *iptr, *icnt, rcode, dummy;
  ibuf = &fcb[FCB_IBUF];   /* poor man's cast */
  iptr = &fcb[FCB_IPTR];
  icnt = &fcb[FCB_ICNT];
  if(*ibuf) { /* if intermediate buffer is present */
    *(*iptr)++ = ch; /* store the character there */
    if(--(*icnt)) { /* if buffer is full */
      return _iflush(fcb); /* write the buffer */
      }
    else { /* the buffer still has space */
      return 0;
      }
    }
  else { /* no intermediate buffer */
    _dosfcall(F_SETDTA, &ch); /* buffer = source char */
    return _dosfxcall(F_WRITE, fcb, 1, &dummy); /* write one char at a time */
    }
  }
 
 
/*
** flush the intermediate buffer
*/

static _iflush(fcb) char *fcb {
int *ibuf, *iptr, *icnt, rcode, dummy;
  ibuf = &fcb[FCB_IBUF];   /* poor man's cast */
  iptr = &fcb[FCB_IPTR];
  icnt = &fcb[FCB_ICNT];
  if(*ibuf) { /* if intermediate buffer is present */
    _dosfcall(F_SETDTA, *ibuf); /* buffer = intermediate bufer */
    if(*iptr != *ibuf) /* CON can't handle zero length */
      rcode = _dosfxcall(F_WRITE, fcb, (*iptr-*ibuf), &dummy);
    *iptr = *ibuf; /* reset next char pointer */
    *icnt = _BUFSIZE; /* reset # of chars remaining */
    return rcode;
    }
  else {
    return 0;
    }
  }


/*
* write a character to the screen
*/

static _scrnout(ch) char ch; {
  _dosfcall(F_DSPOUT, ch);
  }

/*
* write a character to the pointer
*/

static _prnout(ch) char ch; {
  _dosfcall(F_PRNOUT, ch);
  }

