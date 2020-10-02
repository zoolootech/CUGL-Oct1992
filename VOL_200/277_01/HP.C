/*
 *  hp.c
 *  output driver for HP7470A plotter
 *  copyright 1988 Ronald Florence
 *
 *	revision of plotter.c (published PC Tech Journal 11/86)
 *	Tek4014 files (2/87)
 *	Accu-Weather maps (3/88)
 *	env "PLOTTER", extended status read, cpu scaling (6/88)
 */


#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termio.h>
#include <signal.h>

#define TEK4		0x1	/* option flags */
#define TEKPTS		0x2
#define SCALED		0x4
#define DEBUG		0x8
				/* 80 char buffer, xon/xoff */
#define HPONLINE	"\33.(\33.I80;;17:\33.N;19:"
#define HPOFFLINE	"SP0;PU0,0;\33.)"
#define HPSTAT		"\33.O"
#define HPABORT		"\33.K"
#define HPGLERR		"OE;"
#define HPRSERR		"\33.E"
#define	LOCK		"/tmp/LCK.."
#define ERR(a,b)	fprintf(stderr, "\rhp: "), fprintf(stderr, a, b)

char	*title, 
	*plotdev,
	lock[sizeof(LOCK) + 5] = LOCK, 
	*usage[] = {
	   "usage: hp [-t|p|T] [-l line] [-s speed] [-h title] [file]",
	   "	-t	Tek 4014 file",
	   "	-p	xmin, ymin, xmax, ymax before points",
	   "	-T	points scaled to 4096 x 3120",
	   "	-l	default stdout or environment \"PLOTTER\"",
	   "	-s	default 9600 baud",
	   "	-h	centered heading",
	   0
	};

FILE	*plr = stdin, 
	*plw = stdout,
	*fi = stdin;

int	fd = 1,
	speed = 9600,
	opt_flag,
	die(), 
	quit();

double	xm, 
	ym, 
	xscale, 
	yscale;

struct	termio	old;


main(argc, argv)
     int  argc;
     char  **argv;
{
  int	hperr, alrmint();  
  char	buf[BUFSIZ];
  FILE	*tty = NULL;
	
  scanarg(argc, argv);					
  signal(SIGQUIT, quit);	/* trap del & quit */
  signal(SIGINT, quit);		
  signal(SIGALRM, alrmint);	
  setline();			/* lock & set line */
  if (!(opt_flag & DEBUG))	/* wake up the plotter and query status */
    {
      fputs(HPONLINE, plw);
    try_again:
      alarm(5);
      do 
	  hperr = readhp(HPSTAT);
      while (hperr < 0 || hperr > 40);
      alarm(0);
      if (hperr & 0x30) 
	{
	  if (!tty)
	    tty = fopen("/dev/tty", "r");
	  if (!plotdev)
	    fputs("\33.)", plw);
	  ERR("press Return when plotter is ready ", NULL);
	  getc(tty);
	  if (!plotdev)
	    fputs(HPONLINE, plw);
	  goto try_again;
	}
    }
  fputs("IN", plw);		/* initialize */
  if (title)
    heading();	
  fputs("SP1", plw);		/* pen 1 */			
  if (opt_flag & TEK4)		/* Tek 4014 pic */
    {
      scale();
      tekdecode(fi);
    }		
  else				/* points */
    if (opt_flag & (SCALED | TEKPTS))
      rd_points();
  else				/* HP-GL instructions */		
    while (fgets(buf, sizeof buf, fi) != NULL)
      fputs(buf, plw);
  if (!(opt_flag & DEBUG))	/* check for problems */ 
    {
      alarm(60);
      if (hperr = readhp(HPGLERR))
	ERR("HP-GL error = %d\n", hperr);
      if (hperr = readhp(HPRSERR))
	ERR("rs232 error = %d\n", hperr);
      alarm(0);
      fputs(HPOFFLINE, plw);
    }
  die();
}


heading()
{
  int	tx = 5150, ty = 7350;	/* arbitrary */

  fprintf(plw, "SP2SI.30,.48PU%d,%d", tx, ty);
  fprintf(plw, "CP-%d,0", strlen(title)/2);
  fprintf(plw, "LB%s\003", title);	
}

scale()
{
  int	delta,
	px1 = 250,		/* HP7470 boundary points */
	px2 = 10250,
	py1 = 279,
	py2 = 7479;
  double	ar,
		xmin = 0.0,	/* default Tek 4014 scaling */
		ymin = 0.0, 
		xmax = 4096.0,
		ymax = 3120.0,
		dx = px2 - px1,	/* HP aspect parameters */
		dy = py2 - py1;

  if (opt_flag & SCALED)	/* read scaling points	*/
    {
      if (fscanf(fi, "%lf%lf%lf%lf", &xmin,&ymin,&xmax,&ymax) == EOF)
	quit();
      if (xmin >= xmax || ymin >= ymax)
	ERR("invalid scaling points\n", NULL), quit();
    }		
  ar = (xmax - xmin) / (ymax - ymin); /* calculate aspect */
  if (ar < dx / dy) 
    {
      delta = (dx - ar * dy) * 0.5;
      px1 += delta;
      px2 -= delta;
    }
  else 
    {
      delta = (dy - dx / ar) * 0.5;
      py1 += delta;
      py2 -= delta;
    }
	
  fprintf(plw, "IP%d,%d,%d,%d", px1, py1, px2, py2); /* set boundary */
  xm = (px2 - px1) / (xmax - xmin); /* scalars for points */
  ym = (py2 - py1) / (ymax - ymin);
  xscale = px1 - xmin * xm;
  yscale = py1 - ymin * ym;
}


rd_points()
{
  register	hx, hy;
  int	first = 1;
  double	rx, ry; 

  scale();
  while (fscanf(fi, "%lf%lf", &rx, &ry) != EOF) 
    {
      hx = xm * rx + xscale;
      hy = ym * ry + yscale;
      if (!first)
	fprintf(",%d,%d", hx, hy);
      else if (first == 1) 
	{
	  fprintf(plw, "PU%d,%d", hx, hy);
	  first++;
	}
      else if (first == 2) 
	{
	  fprintf(plw, "PD%d,%d", hx, hy);
	  first = 0;
	}
    }
  fputs("PU", plw);
}


alrmint()
{
  fputs(HPOFFLINE, plw);
  ERR("no response from plotter\n", NULL);
  die();
}


die()
{
  if (!plotdev)			/* reset the line */
    ioctl(fd, TCSETA, &old);
  else if (unlink(lock) == -1)	/* remove the lock */
     ERR("cannot unlink %s\n", lock);
  exit(0);
}


quit()
{
  fputs(HPABORT, plw);
  fputs(HPOFFLINE, plw);
  die();
}


scanarg(argc, argv)
     int  argc;
     char **argv;
{
  extern	int	optind;
  extern	char	*optarg;
  int	i;
  char	**cp, *strdup(), *getenv();

  plotdev = getenv("PLOTTER");
  while ((i = getopt(argc, argv, "s:dTl:tph:?")) != EOF)
    switch (i)  
      {
      case 's' :
	speed = atoi(optarg);
	break;
      case 'd' :
	plotdev = NULL;
	opt_flag |= DEBUG;
	break;
      case 'l' :
	plotdev = optarg;  
	break;
      case 't' :
	if (opt_flag & (SCALED | TEKPTS))
	  errx();
	opt_flag |= TEK4;
	break;
      case 'p' :
	if (opt_flag & (TEK4 | TEKPTS))
	  errx();
	opt_flag |= SCALED;
	break;
      case 'T' :
	if (opt_flag & (TEK4 | SCALED))
	  errx();
	opt_flag |= TEKPTS;
	break;
      case 'h' :
	title = strdup(optarg);
	break;
      case '?' :
	for (cp = usage; *cp; ++cp)
	  fprintf(stderr, "%s\n", *cp); 
	exit(1);
      }
  if (argc > 1 && argc != optind)
    if ((fi = fopen(argv[optind], "r")) == NULL)
      ERR("can't find %s\n", argv[optind]), exit(1);
}

errx()
{
  ERR("incompatible options\n", NULL);
  exit(1);
}


setline()
{	
  struct	termio term;
  char	dvc[5], *strchr();
  int	BAUD,
	ld;

  switch (speed) 
    {
    case 300:
      BAUD = B300;
      break;
    case 1200:
      BAUD = B1200;
      break;
    case 2400:
      BAUD = B2400;
      break;
    case 4800:
      BAUD = B4800;
      break;
    case 9600:
      BAUD = B9600;
      break;
    default:
      ERR("invalid speed", NULL);
      die();
    }
  if (plotdev)			/* parse the name of the device */ 
    {	
      if (strncmp(plotdev, "/dev/", 5))  
	{
	  strcpy(dvc, plotdev);
	  strcpy(plotdev, "/dev/");
	  strcat(plotdev, dvc);
	}
      else 
	strcpy(dvc, strchr(plotdev, 't'));

      strcat(lock, dvc);
      if (!access(lock, 0))
	ERR("%s is locked\n", dvc), exit(1);			
      if ((fd = open(plotdev, O_RDWR | O_NDELAY)) < 0)
	ERR("can't open %s\n", plotdev), exit(1);			
      if ((ld = creat(lock, 0644)) < 0)
	ERR("can't create %s\n", lock), exit(1);
      close(ld);
    }
  else 				/* save line parameters */
    ioctl(fd, TCGETA, &old);

  ioctl(fd, TCGETA, &term);
  term.c_cflag &= ~CBAUD;
  term.c_cflag |= BAUD | CLOCAL;
  term.c_lflag &= ~ECHO;
  term.c_iflag |= ICRNL | IXON | BRKINT;
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 0;
  ioctl(fd, TCSETAF, &term);
  if (plotdev) 
    {
      plr = fopen(plotdev, "r"); 
      plw = fopen(plotdev, "w");
     }
}


readhp(query)
     char  *query;
{
  int  hperr;

  fputs(query, plw);
  if (fscanf(plr, "%d", &hperr) == EOF) 
    alrmint();
  return(hperr);
}
