/* hsh -- ZCPR3 history shell                                               */
/* copyright 1985  Michael M Rubenstein                                     */

/* version 1.1  6 Oct 85                                                    */

/* Modifications                                                            */

/* 1.1      (MMR) Fixed handling of firsthist on wrap around.  Previous     */
/*          did not reset to 0, which could result in the program hanging   */
/*          if a search was done for a leading string which was not in the  */
/*          history.                                                        */

#include <sysio.h>
#include <fcb.h>

#define SHNAME          "HSH.COM"       /* default name */
#define VARFILE         "HSH.VAR"       /* history file */
#define DEFNHIST        10              /* default number of history lines */

/* some interesting characters                                              */
#define CTL             0x1f
#define NUL             0
#define CTLA            (CTL & 'A')
#define CTLC            (CTL & 'C')
#define CTLD            (CTL & 'D')
#define CTLE            (CTL & 'E')
#define CTLF            (CTL & 'F')
#define CTLG            (CTL & 'G')
#define BEL             CTLG
#define BS              (CTL & 'H')
#define CTLJ            (CTL & 'J')
#define LF              CTLJ
#define CR              (CTL & 'M')
#define CTLL            (CTL & 'L')
#define CTLP            (CTL & 'P')
#define CTLQ            (CTL & 'Q')
#define CTLR            (CTL & 'R')
#define CTLS            (CTL & 'S')
#define CTLT            (CTL & 'T')
#define CTLU            (CTL & 'U')
#define CTLV            (CTL & 'V')
#define CTLW            (CTL & 'W')
#define CTLX            (CTL & 'X')
#define CTLY            (CTL & 'Y')
#define CTLZ            (CTL & 'Z')
#define ESC             (CTL & '[')
#define DEL             0x7f

extern unsigned         z3env;          /* pointer to ZCPR3 environment */
extern FCB              dfcb_;          /* default CP/M FCB */

int                     curdir = FALSE;     /* if TRUE, search current dir */
int                     nhist = DEFNHIST;   /* number of history lines */
int                     firsthist = 0;      /* index of first history line */
int                     lasthist = 0;       /* index of one past last line */
int                     drive, user;        /* current drive/user */
int                     rdrive, ruser;      /* root drive/user */
int                     linepos;            /* position on CRT line */
int                     maxpos;             /* max pos on CRT line used */
int                     maxcols;            /* columns on CRT display */
char                    *cur;               /* pointer to current char in */
                                            /* line being edited */
char                    *line;              /* pointer to start of line */
                                            /* being edited */
char                    *maxcur;            /* end of line */

char                    *gethline();        /* get a line from the history */
char                    *puthline();        /* put a line to the history */

FCB                     *gefcb();       /* get ZCPR3 external FCB */
char                    *getcrt();      /* get ZCPR3 terminal data */

main()
{
  static FCB            *efcb;          /* FCB for shell name */
  static char           shname[13];     /* shell name */
  static char           *s;
  int                   du;
  static int            c;
  static int            i;
  static int            n = -1;         /* value of command line arg */
  char                  aline[256];

  line = aline;                         /* cheap way to allocate mem */
  maxcur = line + 255;                  /* end of line */

  /* if there is a command line argument, evaluate and save it */
  if (isdigit(dfcb_.name1.fname[0]))
  {
    n = atoi(dfcb_.name1.fname);
    if (n < 0)
      error("hsh: number of history lines too large.");
  }
  else
  if (dfcb_.name1.fname[0] != ' ')
    error("syntax:  hsh [<number of history lines to keep>]");


  if (z3env == 0)
    error("hsh: ZCPR3 required.");

  /* clear external command line.  abort if none */
  if (!clcl())
    error("hsh: ZCPR3 external command line required.");

  /* find where we are and where root dir is for later use */
  drive = curdsk();
  user = getusr();
  du = groot();
  rdrive = du >> 8;
  ruser = du & 8;

  /* if not invoked as a shell, install as shell */
  if (!qsh())
  {
    /* if no external FCB, use default name */
    if ((efcb = gefcb()) == NULL)
    {
      setfcb(SHNAME, &dfcb_);
      efcb = &dfcb_;
    }

    /* find where program is and set up name with drive/user */
    if ((du = pfnd(efcb, curdir)) == -1)
      error("hsh: Cannot find shell program.");
    shname[0] = (du >> 8) + 'A';
    du &= 0xff;
    shname[1] = du / 10 + '0';
    shname[2] = du % 10 + '0';
    shname[3] = ':';
    for (i = 0; i < 8; ++i)
    {
      if ((c = (efcb->name1.fname[i] & 0x7f)) == ' ')
        break;
      shname[4 + i] = c;
    }
    shname[4 + i] = '\0';
    if (shpsh(shname))
      error("hsh: Cannot install shell.");

    ps("HSH Version 1.0 (1 Oct 85) installed\r\n");

    /* if a command line arg was entered, set up history file with number */
    /* of lines to keep */
    if (n >= 0)
    {
      nhist = n;
    /* go to the right place */
      select(rdrive);
      setusr(ruser);
      setfcb(VARFILE, &dfcb_);
      delete(&dfcb_);
      if (make(&dfcb_) == 0xff)
        sherror("hsh: Cannot create HSH.VAR.");
      puthist();
    }

    /* exit after installing shell, so will work from submit file */
    return;
  }

  /* set ZCPR3 command status message so next program won't think it's */
  /* a shell (needed since we don't warm boot on exit) */
  pcst(0);

  maxcols = *getcrt() & 0xff;

  gethist();                            /* get history */
  getcmd();                             /* handle a command */
  puthist();                            /* save history */
}

/* get the history                                                          */
gethist()
{
  /* first record in history file keeps parameters */
  struct {              int     f_nhist, f_fhist, f_lhist;
                        char    f_fill[126];
         }              fnhist;

  /* go to the right place */
  select(rdrive);
  setusr(ruser);
  setfcb(VARFILE, &dfcb_);

  /* if there's a history file, read it in and set firsthist and lasthist */
  if (open(&dfcb_) != 0xff)
  {
    setdma(&fnhist);
    if (!rdseq(&dfcb_))
    {
      nhist = fnhist.f_nhist;
      firsthist = fnhist.f_fhist;
      lasthist = fnhist.f_lhist;
    }
  }
  else
    /* if no history file, create it */
    if (make(&dfcb_) == 0xff)
      sherror("hsh: Cannot create HSH.VAR.");
}

/* put the history paramters to file                                        */
puthist()
{
  /* first record in history file keeps parameters */
  struct {              int     f_nhist, f_fhist, f_lhist;
                        char    f_fill[126];
         }              fnhist;

  fnhist.f_nhist = nhist;
  fnhist.f_fhist = firsthist;
  fnhist.f_lhist = lasthist;

  setdma(&fnhist);
  dfcb_.rrec = 0;
  if (wrran(&dfcb_))
    sherror("hsh: Error writing HSH.VAR.");

  if (close(&dfcb_) == 0xff)
    sherror("hsh: Error closing HSH.VAR");

  /* return to where we were */
  select(drive);
  setusr(user);
}

/* get a history line.  returns pointer to line for convenience.            */
char *gethline(line, n)
  char                  *line;
  int                   n;
{
  dfcb_.rrec = 2 * n + 1;
  setdma(line);
  if (rdran(&dfcb_))
    sherror("hsh: Cannot read HSH.VAR.");

  ++dfcb_.rrec;
  setdma(line + 128);
  if (rdran(&dfcb_))
    sherror("hsh: Cannot read HSH.VAR.");
  return line;
}

/* put a history line.  returns pointer to line for convenience.            */
char *puthline(line, n)
  char                  *line;
  int                   n;
{
  dfcb_.rrec = 2 * n + 1;
  setdma(line);
  if (wrran(&dfcb_))
    sherror("hsh: Cannot write HSH.VAR.");

  ++dfcb_.rrec;
  setdma(line + 128);
  if (wrran(&dfcb_))
    sherror("hsh: Cannot write HSH.VAR.");
  return line;
}

/* get a command and pass to CCP                                            */
getcmd()
{
  static char           *cmd;
  static char           *dirname;
  static int            i;
  static char           prompt[14];
  static char           *s;

  /* set up prompt */
  s = prompt;
  *(s++) = drive + 'A';
  if (user > 10)
    *(s++) = user / 10 + '0';
  *(s++) = user % 10 + '0';
  if ((dirname = dutd((drive << 8) + user)) != NULL)
  {
    *(s++) = ':';
    for (i = 0; i < 8 && dirname[i] != ' '; ++i)
      *(s++) = dirname[i];
  }
  *(s++) = '>';
  *s = '\0';

  for (;;)
  {
    pzex(1);                            /* mark prompt for ZEX */
    getline(prompt);                    /* get a line */
    pzex(0);
    for (cmd = line; isspace(*cmd); ++cmd)      /* skip spaces at start */
      ;
    if (*cmd == '\0')
      continue;
    puthline(line, lasthist);
    if (++lasthist > nhist)          /* advance history */
      lasthist = 0;
    if (lasthist == firsthist)
      ++firsthist;
    if (firsthist > nhist)
      firsthist = 0;

    if (pcl(cmd))                     /* pass to CCP */
      return;

    ps("\r\nhsh: Command too long.");
  }
}

/* get a line with editing                                                  */
getline(prompt)
  char                  *prompt;
{
  static char           *maxcur;        /* last char possible on line */
  static char           *p, *q;
  static int            n;
  static int            i;
  static int            c;
  static int            hindex;         /* index into history for recall */
  char                  wkline[256];    /* work line for history search */

  maxpos = 0;
  setmem(line, 256, 0);                 /* clear the line */
  hindex = lasthist;                    /* point to next line for history */

  ps("\r\n");
  ps(prompt);
  cur = line;

  for (;;)
    switch (c = bconin() & 0x7f)
    {
      case CR:      /* done with line */
                    bcnout('\r');
                    i = (maxpos + maxcols - 1) / maxcols
                      - (linepos + maxcols - 1) / maxcols;
                    while (i-- > 0)
                      bcnout('\n');
                    return line;

      case CTLS:    /* back one character */
                    if (cur != line)
                      backup();
                    break;

      case CTLA:    /* back one word */
                    if (cur == line)
                      break;
                    backup();
                    while (cur != line && !isalnum(*cur))
                      backup();
                    while (cur != line && isalnum(*(cur - 1)))
                      backup();
                    break;

      case CTLQ:    /* back on command */
                    if (cur != line)
                      backup();
                    while (cur != line && *(cur - 1) != ';')
                      backup();
                    break;

      case CTLD:    /* forward one character */
                    if (*cur != '\0')
                      forward();
                    break;

      case CTLF:    /* forward one word */
                    while (isalnum(*cur))
                      forward();
                    while (*cur != '\0' && !isalnum(*cur))
                      forward();
                    break;

      case CTLR:    /* forward command */
                    while (*cur != '\0' && *cur != ';')
                      forward();
                    if (*cur == ';')
                      forward();
                    break;

      case BS:
      case DEL:     /* backspace and delete */
                    if (cur == line)
                      break;
                    backup();           /* NOTE fall through */
      case CTLG:    /* delete current char */
                    if (*cur != '\0')
                      delchr();
                    break;

      case CTLW:    /* delete word left */
                    if (cur == line)
                      break;
                    backup();
                    while (cur != line && !isalnum(*cur))
                      backup();
                    while (cur != line && isalnum(*(cur - 1)))
                      backup();
                    /* NOTE fall through */
      case CTLT:    /* delete word right */
                    while (isalnum(*cur))
                      delchr();
                    while (*cur != '\0' && !isalnum(*cur))
                      delchr();
                    break;

      case CTLZ:    /* delete command */
                    if (*cur == ';')
                      forward();
                    while (cur != line && *(cur - 1) != ';')
                      backup();
                    while (*cur != '\0' && *cur != ';')
                      delchr();
                    if (*cur == ';')
                      delchr();
                    if (*cur == '\0' && *(cur - 1) == ';')
                    {
                      backup();
                      delchr();
                    }
                    break;

      case CTLD:    /* forward one character */
                    if (*cur != '\0')
                      forward();
                    break;

      case CTLV:    /* insert char */
                    p = cur + strlen(cur);
                    if (p == maxcur)
                      *(p - 1) = '\0';
                    while (p > cur)
                    {
                      *p = *(p - 1);
                      --p;
                    }
                    *cur = ' ';
                    while (*cur != '\0')
                      forward();
                    while (cur > p)
                      backup();
                    break;

      case CTLX:    /* recall next line */
                    if (hindex != lasthist)
                    {
                      ++hindex;
                      if (hindex > nhist)
                        hindex = 0;
                    }
                    if (hindex != lasthist)
                    {
                      delln();
                      gethline(line, hindex);
                      showln();
                      break;
                    }                   /* NOTE fall through */
      case CTLU:    /* delete line */
                    hindex = lasthist;
                    delln();
                    break;

      case CTLY:    /* delete to end of line */
                    i = linepos;
                    p = cur;
                    while (*cur != '\0')
                      forward();
                    n = linepos - i;
                    while (cur > p)
                      *(--cur) = '\0';
                    for (i = 0; i < n; ++i)
                      pc('\b');
                    for (i = 0; i < n; ++i)
                      pc(' ');
                    for (i = 0; i < n; ++i)
                      pc('\b');
                    break;

      case CTLE:    /* recall previous line */
                    if (hindex == firsthist)
                    {
                      bell();
                      break;
                    }
                    delln();
                    if (--hindex < 0)
                      hindex = nhist;
                    gethline(line, hindex);
                    showln();
                    break;

      case CTLL:    /* search for previous */
                    n = FALSE;
                    for (i = hindex; i != firsthist;)
                    {
                      if (--i < 0)
                        i = nhist;

                      if (n = prefix(line, cur, gethline(wkline, i)))
                        break;
                    }
                    if (n)
                    {
                      p = cur;
                      delln();
                      gethline(line, hindex = i);
                      showln();
                      while (cur < p)
                        forward();
                    }
                    else
                      bell();
                    break;

      case CTLJ:    /* help */
                    p = cur;
                    help();
                    maxpos = 0;
                    ps(prompt);
                    cur = line;
                    while (cur < p)
                      forward();
                    showln();
                    break;

      case NUL:     /* ignore nulls */
                     break;

      case CTLP:    /* take next char as literal */
                    if ((c = bconin() & 0x7f) == NUL)
                      break;            /* NUL can't be handled */
                                        /* NOTE fall through */
      default:      if (cur == line)
                      switch (c)        /* handle special commands */
                      {
                        case ESC:   delln();
                                    ps("<exit hsh>\r\n");
                                    shpop();
                                    puthist();
                                    exit(0);

                        case CTLC:  delln();
                                    ps("^C");
                                    puthist();
                                    boot_();
                      }

                    if (cur == maxcur)
                      bell();
                    else
                    {
                      *cur = c;
                      forward();
                      showln();
                    }
    }
}

/* delete character                                                         */
delchr()
{
  static char           *p;

  for (p = cur; (*p = *(p + 1)) != '\0'; ++p)
    ;
  p = cur;
  while (*cur != '\0')
    forward();
  ps("  \b\b");
  while (cur > p)
    backup();
}

/* delete entire line                                                       */
delln()
{
  while (*cur != '\0')
    forward();
  while (cur > line)
  {
    if (*--cur < ' ' || *cur == DEL)
      ps("\b \b");
    ps("\b \b");
    *cur = '\0';
  }
  maxpos = linepos;
}

/* show line from current position                                          */
showln()
{
  static char           *p;

  if (*cur != '\0')
  {
    p = cur;
    while (*cur != '\0')
      forward();
    ps(" \b");
    while (cur != p)
      backup();
  }
}

/* back up one character                                                    */
backup()
{
  --cur;
  if (*cur < ' ' || *cur == DEL)
    pc('\b');
  pc('\b');
}

/* forward one character                                                    */
forward()
{
  pctl(*(cur++));
}

/* is s - t a prefix of u.  ignore case                                     */
prefix(s, t, u)
  register char         *s, *t, *u;
{
  while (s < t)
    if (toupper(*(s++)) != toupper(*(u++)))
      return FALSE;

  return  TRUE;
}

/* put string to console                                                    */
ps(s)
  register char         *s;
{
  while (*s != '\0')
    pc(*(s++));
}

/* output a number to console                                               */
pn(n)
  unsigned              n;
{
  if (n > 9)
    pn(n / 10);
  pc(n % 10 + '0');
}

/* put char to console                                                      */
pc(c)
  int                   c;
{
  bcnout(c);
  switch (c)
  {
    case BS:            --linepos;
                        break;

    case CR:            linepos = 0;
                        break;

    default:            if (c >= ' ' && c < DEL)
                          if (++linepos > maxpos)
                            maxpos = linepos;
  }
}

/* put character to console, converting controls to printables              */
pctl(c)
  int                   c;
{
  if (c == DEL)
  {
    ps("^?");
    return;
  }

  if (c < ' ')
  {
    pc('^');
    pc (c += '@');
    return;
  }

  pc(c);
}

/* audible alarm                                                            */
bell()
{
  bcnout(BEL);
}

/* critical error.  remove shell and exit with message                      */
sherror(s)
  char                  *s;
{
  shpop();
  error(s);
}

/* display help                                                             */
help()
{
  static char           *msg[] =
  {
    "\r\n\n\n\n          +-------+-------+-------+--------+--------+\r\n",
    "          | Back  | Fwd   | Del L | Del R  | Del All|\r\n",
    "    +-----+-------+-------+-------+--------+--------+\r\n",
    "    |Char |  ^S   |  ^D   |  BS   |  ^G    |        |\r\n",
    "    +-----+-------+-------+-------+--------+--------+\r\n",
    "    |Word |  ^A   |  ^F   |  ^W   |  ^T    |        |\r\n",
    "    +-----+-------+-------+-------+--------+--------+\r\n",
    "    |Cmd  |  ^Q   |  ^R   |       |        |  ^Z    |\r\n",
    "    +-----+-------+-------+-------+--------+--------+\r\n",
    "    |Line |       |       |       |  ^Y    |  ^U    |\r\n",
    "    +-----+-------+-------+-------+--------+--------+\r\n\n",
    "    ^E - Recall prev cmd line.\r\n",
    "    ^L - Search for match left of the cursor.\r\n",
    "    ^P - Quote next char.\r\n",
    "    ^V - Insert char.\r\n",
    "    ^X - Recall next cmd line.\r\n\n",
    NULL
  };

  static char           **p;

  for (p = msg; *p != NULL; ++p)
    ps(*p);
}

/* ZCPR3 transfer routines                                                  */
/* These routines simply convert calling sequences and return values        */
/* between C/80 and Z3LIB.                                                  */

/* clear command line                                                       */
clcl()
{
#asm
        call    clrcl##
        mov     h,a
        mov     l,a
#endasm
}

/* is this a shell?                                                         */
qsh()
{
#asm
        call    qshell##
        lxi     h,0
        rnz
        inx     h
#endasm
}

/* point to external FCB                                                    */
FCB *gefcb()
{
#asm
        call    getefcb##
        rnz
        lxi     h,0
#endasm
}

/* install shell                                                            */
shpsh()
{
#asm
        pop     d
        pop     h
        push    h
        push    d
        call    shpush##
        mvi     h,0
        mov     l,a
#endasm
}

/* get root drive/user                                                      */
groot()
{
#asm
        call    root##
        mov     h,b
        mov     l,c
#endasm
}

/* put command line                                                         */
pcl()
{
#asm
        pop     d
        pop     h
        push    h
        push    d
        call    putcl##
        mov     h,a
        mov     l,a
#endasm
}

/* convert drive/user to directory name                                     */
dutd()
{
#asm
        pop     h
        pop     b
        push    b
        push    h
        call    dutdir##
        rnz
        lxi     h,0
#endasm
}

/* find file along path                                                     */
pfnd()
{
#asm
        pop     h
        pop     b
        pop     d
        push    d
        push    b
        push    h
        mov     a,c
        call    pfind##
        mov     h,b
        mov     l,c
        rnz
        lxi     h,0
#endasm
}

/* set command status message                                               */
pcst()
{
#asm
        pop     h
        pop     b
        push    b
        push    h
        mov     a,c
        call    putcst##
#endasm
}

/* set ZEX control message                                                  */
pzex()
{
#asm
        pop     h
        pop     b
        push    b
        push    h
        mov     a,c
        call    putzex##
#endasm
}
