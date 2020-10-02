
#include <stdio.h>
#include <dos.h>
#include <mcega.h>

  WORKSPACE *gdwscore;
  WORKSPACE *gdwsswap;
  WORKSPACE *gdwschr1;
  WORKSPACE *gdwschr2;

  char *gdwspath;

void gpinitws(path)
  char *path;
{
  static WORKSPACE charset1 =
    {
    1,0xC230,0,8,3584,0,0,8,14,0
    };

  static WORKSPACE charset2 =
    {
    1,0xC316,0,8,2048,0,0,8,8,0
    };

  gdwscore = NULL;
  gdwsswap = NULL;

  gdwschr1 = &charset1;
  gdwschr2 = &charset2;

  gdwspath = path;

}

void gptermws()
{
  extern void gpdelws();

  register WORKSPACE *j;

  j = gdwscore;
  while (j != NULL)
    {
    gpdelws(j);
    j = (WORKSPACE *)j->next;
    }

  j = gdwsswap;
  while (j != NULL)
    {
    gpdelws(j);
    j = (WORKSPACE *)j->next;
    }
}

WORKSPACE *gpgenws(width,height,bits)
  int width,height,bits;
{
  extern WORKSPACE *calloc();
  extern void gpdelws();

  register i;
  register WORKSPACE *j;

  if ((j = calloc(sizeof(WORKSPACE),1)) == NULL)
    {
    gpterm();
    perror("GPGENWS: Out of local memory!\n");
    exit(1);
    }

  if (bits <= 8)
    {
    i = 8/bits;
    i = ((width + i - 1) / i * height + 15) / 16;
    }
  else
    {
    i = width * height / 8;
    };

  j->bits    = bits;
  j->segment = gpgetmem(i);
  j->para    = i;
  j->width   = width;
  j->height  = height;
  j->x       = 0;
  j->y       = 0;
  j->w       = width;
  j->h       = height;
  j->next    = (char *)gdwscore;

  gdwscore = j;

}

void gpdelws(ws)
  WORKSPACE *ws;
{
  union  REGS  inregs, otregs;
  struct SREGS segregs;

  char buf[64];

  register WORKSPACE *i;
  register WORKSPACE *j;

  j = gdwscore;
  while (j != NULL)
    {
    if (ws == j)
      {
      inregs.h.ah = 0x49;
      segregs.es  = j->segment;
      intdosx(&inregs,&otregs,&segregs);
      if (j == gdwscore)
        {
        gdwscore = (WORKSPACE *)j->next;
        }
      else
        {
        i->next = j->next;
        };
      free(j);
      return;
      }
    i = j;
    j = (WORKSPACE *)j->next;
    }

  j = gdwsswap;
  while (j != NULL)
    {
    if (ws == j)
      {
      sprintf(buf,"%s\WS_%u.$$$",gdwspath,j);
      unlink(buf);

      if (j == gdwsswap)
        {
        gdwsswap = (WORKSPACE *)j->next;
        }
      else
        {
        i->next = j->next;
        };
      free(j);
      return;
      }
    i = j;
    j = (WORKSPACE *)j->next;
    }

}

int gpgetmem(para)
  unsigned para;
{
  union  REGS  inregs, otregs;
  struct SREGS segregs;

  char buf[64];

  register i;
  register WORKSPACE *j;

  inregs.h.ah = 0x48;
  inregs.x.bx = para;

  intdosx(&inregs,&otregs,&segregs);

  if (otregs.x.cflag == 0)
    {
    return(otregs.x.ax);
    };

  gpterm();
  perror("GPGETMEM: Out of system memory!\n");
  exit(1);

}
