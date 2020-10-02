/*
** include baseio2.c -- base I/O routines part 2
*/
 
 
/*
** reposition within a file
*/
 
_seek(fildes, offset, whence) int fildes, offset, whence; {
  int temp, count;
  char *fcb, adder[4], *from, *to;
  if((whence < 0) | (whence > 5)) {
    errno = EINVAL;
    return -1;
    }
  fcb = _filedes[fildes];
  if((fcb[FCB_FLAG]&255) != 255) {
    errno = ESPIPE;
    return -1;
    }
  if(_iflush(fcb)) return -1;
  if((whence == 0) | (whence == 3)) { /* if seek from front */
    to = &fcb[FCB_RRECNO];
    count = 4;
    while(count--) *to++ = 0;
    }
  if((whence == 2) | (whence == 5)) { /* if seek from end */
    from = &fcb[FCB_FSIZE];
    to = &fcb[FCB_RRECNO];
    count = 4;
    while(count--) *to++ = *from++;
    }
  if(whence < 3) { /* if offset in bytes */
    adder[0] = offset;
    adder[1] = offset >> 8;
    adder[2] = offset >> 15;
    adder[3] = offset >> 15;
    }
  else { /* if offset in 512 byte blocks */
    adder[0] = 0;
    adder[1] = (offset << 1) & 254;
    adder[2] = offset >> 7;
    adder[3] = offset >> 15;
    }
  temp = 0;
  count = 4;
  from = &adder[0];
  to = &fcb[FCB_RRECNO];
  while(count--) { /* return current position */
    temp = (*from++ & 255) + (*to & 255) + (temp >> 8);
    *to++ = temp;
    }
  if(whence < 3) {
    temp = (fcb[FCB_RRECNO+1] << 8) + fcb[FCB_RRECNO];
    }
  else {
    temp = (fcb[FCB_RRECNO+3] << 15) + (fcb[FCB_RRECNO+2] << 7) +
        ((fcb[FCB_RRECNO+1] >> 1) & 127);
    }
  return temp;
  }
 
 
/*
** set file parameters
*/
 
_ioctl(fildes, request, argp) int fildes, request; char *argp; {
  char *fcb, *from, *to;
  int count, *ibuf;
  fcb = _filedes[fildes];
  if(request == TIOCGETP) {
    from = &fcb[FCB_SGTTYB];
    to = argp;
    }
  else if(request == TIOCSETP) {
    from = argp;
    to = &fcb[FCB_SGTTYB];
    ibuf = &fcb[FCB_IBUF];
    if(*ibuf && !(from[SG_FLGS] & 32)) { /* if changing to non-CR-LF conv */
      if(_iflush(fcb)) return -1;
      free(*ibuf);
      *ibuf = 0;
      }
    }
  else {
    errno = EINVAL; /* invalid request code */
    return -1;
    }
  count = SG_SIZE;
  while(count--) *to++ = *from++;
  return 0;
  }
 
 
/*
** determine if the specified fildes is the console
*/
 
_isatty(fildes) int fildes; {
  char *fcb;
  fcb = _filedes[fildes];
  if((fcb[FCB_FLAGS]&255) == 255) return 0;
  if(fcb[FCB_FLAGS]&'\300') return 1;
  return 0;
  }
 
 
/*
** allocate a DOS FCB and initialize it
*/
 
static _getfcb(path) char *path; {
  int i, j, drive;
  char *fcb, *ptr1, *ptr2;
  i = 0;
  while(i < NUMFILES) {
    if(_filedes[i] == 0) { /* if handle is available */
      if((fcb = malloc(FCBSIZE)) == 0) break; /* quit if we can't get space */
      ptr1 = _filedes[i] = fcb; /* record fcb address */
      j = FCBSIZE;
      while(j--) *ptr1++ = 0; /* clear the FCB */
      _parsedos(path, &fcb[FCB_DRIVE]); /* parse the file name */
      /* (relative record number is already 0) */
      /* determine if file is reserved file name */
      if     (_equal(&fcb[FCB_FILE],"kbd     ")) fcb[FCB_FLAG] = 128 + 2;
      else if(_equal(&fcb[FCB_FILE],"scrn    ")) fcb[FCB_FLAG] =  64 + 1;
      else if(_equal(&fcb[FCB_FILE],"con     ")) fcb[FCB_FLAG] = 192 + 3;
      else if(_equal(&fcb[FCB_FILE],"prn     ")) fcb[FCB_FLAG] =  32 + 1;
      else if(_equal(&fcb[FCB_FILE],"async   ")) fcb[FCB_FLAG] =  16 + 3;
      else{
        /* DOS expects upper case file names */
        j = 11;
        ptr1 = &fcb[FCB_FILE];
        while(j--) {
          if((*ptr1 >= 'a') & (*ptr1 <= 'z')) *ptr1 += ('A' - 'a');
          ++ptr1;
          }
        fcb[FCB_FLAG] =  255;
        }
      return i;
      }
    ++i;
    }
  return -1;
  }
 
 
/*
** deallocate a DOS FCB
*/
 
static _freefcb(filedes) int filedes; {
  free(_filedes[filedes]); /* free the FCB */
  _filedes[filedes] = 0; /* free the handle */
  }
 
 
/*
** parse a DOS file name (but don't upcase it yet)
*/
 
static _parsedos(path, target) char *path, *target; {
  /* target includes drive number as first byte */
  /* returns pointer to first byte beyond valid name */
  char *ptr1, *ptr2;
  int drive, j;
  ptr1 = path;
  while(*ptr1 == ' ') ++ptr1; /* scan off leading blanks */
  /* get drive number */
  drive = 0;
  if(ptr1[1] == ':') {
    if((*ptr1 >= 'a') & (*ptr1 <= 'z')) drive = *ptr1 - 'a' + 1;
    if((*ptr1 >= 'A') & (*ptr1 <= 'Z')) drive = *ptr1 - 'A' + 1;
    if(drive) ptr1 += 2;
    }
  *target = drive;
  /* blank the name */
  j = 11;
  ptr2 = target + 1;
  while(j-- > 0) *ptr2++ = ' ';
  /* get the file name */
  j = 11;
  ptr2 = target + 1;
  while(_doschar(*ptr1) & (j-- > 0)) *ptr2++ = *ptr1++;
  /* get the extension */
  if(*ptr1 == '.') {
    ++ptr1; /* skip '.' */
    j = 3;
    ptr2 = target + 9;
    while(j-- > 0) *ptr2++ = ' ';
    j = 3;
    ptr2 = target + 9;
    while(_doschar(*ptr1) & (j-- > 0)) *ptr2++ = *ptr1++;
    }
  return ptr1;
  }
 
 
/*
** compare file name to reserved name
*/
 
static _equal(name1,name2) char *name1, *name2; {
  int count;
  count = 8;
  while(count--) {if(*name1++!=*name2++) return 0;}
  return 1;
  }
