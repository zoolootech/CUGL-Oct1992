
#include <stdio.h>
#include <dos.h>
#include <mcega.h>

  WORKAREA *gdwalist;
  WORKAREA *gdwafull;

void gpinitwa()
{
  static WORKAREA area1 =
    {
    0,1,0,0,640,350,0,0,640,350,3,2
    };

  gdwalist = NULL;

  gdwafull = &area1;

}

void gptermwa()
{
  register WORKAREA *j;

  j = gdwalist;
  while (j != NULL)
    {
    free(j);
    j = (WORKAREA *)j->next;
    }
}

WORKAREA *gpgenwa(x,y,w,h)
  unsigned int  x,y,w,h;
{
  extern WORKAREA *calloc();

  register WORKAREA *j;

  if ((j = calloc(sizeof(WORKAREA),1)) == NULL)
    {
    gpterm();
    perror("GPGENWA: Out of local memory!\n");
    exit(1);
    }

  j->device  = 1;
  j->base    = 0;
  j->xorigin = x;
  j->yorigin = y;
  j->width   = w;
  j->height  = h;
  j->x       = 0;
  j->y       = 0;
  j->w       = w;
  j->h       = h;
  j->aspect1 = 3;
  j->aspect2 = 2;
  j->next    = (char *)gdwalist;

  gdwalist = j;

  return(j);
}

void gpdelwa(wa)
  WORKAREA *wa;
{
  register WORKAREA *i;
  register WORKAREA *j;

  j = gdwalist;
  while (j != NULL)
    {
    if (wa == j)
      {
      if (j == gdwalist)
        {
        gdwalist = (WORKAREA *)j->next;
        }
      else
        {
        i->next = j->next;
        };
      free(j);
      return;
      }
    i = j;
    j = (WORKAREA *)j->next;
    }
}
